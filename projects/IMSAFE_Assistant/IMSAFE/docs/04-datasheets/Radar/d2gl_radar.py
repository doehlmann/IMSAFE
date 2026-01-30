#!/usr/bin/env python3
"""
Distance2Go L Radar Communication Library for Raspberry Pi

Protocol implementation for Infineon Distance2Go L (D2GL) FMCW radar module.
Based on reverse-engineered protocol from Infineon's C communication library.

Usage:
    python d2gl_radar.py /dev/ttyACM0
    python d2gl_radar.py /dev/ttyACM0 -n 1000 -o measurements.csv
"""

import struct
import serial
import time
import argparse
import logging
from dataclasses import dataclass
from typing import Optional, Callable, List, Tuple
from enum import IntEnum

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)


START_BYTE_DATA = 0x5A
START_BYTE_STATUS = 0x5B
END_OF_PAYLOAD = 0xE0DB

CMD_QUERY_ENDPOINT_INFO = 0x00
CMD_QUERY_FW_INFO = 0x01
CMD_FIRMWARE_RESET = 0x02

EP_TYPE_D2GL_FMCW = 0x544A5346

MSG_PARA_VALUES_REQ = 0x50
MSG_PARA_VALUES_CNF = 0x51
MSG_PARA_VALUES_DEF_REQ = 0x52
MSG_PARA_VALUES_DEF_CNF = 0x53
MSG_PARA_SET_VALUES_REQ = 0x54
MSG_RESULT_REQ = 0x55
MSG_RESULT_CNF = 0x56
MSG_GET_SHIELD_INFO_REQ = 0x57
MSG_GET_SHIELD_INFO_CNF = 0x58
MSG_GET_RAW_DATA_REQ = 0x59
MSG_GET_RAW_DATA_CNF = 0x6A
MSG_ADV_RESULT_REQ = 0x61
MSG_ADV_RESULT_CNF = 0x62


class TargetStatus(IntEnum):
    NOT_DETECTED = 0
    STATIC = 1
    TRANSIT = 2
    MOVING = 3


@dataclass
class RadarParameters:
    max_distance_m: float = 16.0
    min_distance_m: float = 1.0
    frame_time_sec: float = 0.1
    number_of_samples: int = 64
    hold_threshold: int = 75
    activation_threshold: int = 150
    chirps_per_frame: int = 10
    continuous_mode: bool = False
    baseband_gain_stage: bool = True
    pulse_repetition_rate_sec: float = 0.0
    activate_cfar: bool = False


@dataclass
class RadarResult:
    result_count: int
    frame_number: int
    range_m: float
    strength: float
    velocity_mps: float
    target_status: TargetStatus
    direction_of_movement: int
    timestamp: float = 0.0


@dataclass
class EndpointInfo:
    number: int
    type_id: int
    version: int


@dataclass
class FirmwareInfo:
    version_major: int
    version_minor: int
    version_build: int
    description: str


class D2GLRadar:
    def __init__(self, port: str, timeout: float = 2.0):
        self.port = port
        self.timeout = timeout
        self.serial: Optional[serial.Serial] = None
        self.endpoints: List[EndpointInfo] = []
        self.d2gl_endpoint: Optional[int] = None
        self._connected = False

    def connect(self) -> bool:
        try:
            self.serial = serial.Serial(
                port=self.port,
                baudrate=115200,
                timeout=self.timeout,
                write_timeout=self.timeout
            )
            self.serial.reset_input_buffer()
            self.serial.reset_output_buffer()
            time.sleep(0.1)

            if not self._query_endpoints():
                logger.error("Failed to query endpoints")
                self.disconnect()
                return False

            for ep in self.endpoints:
                if ep.type_id == EP_TYPE_D2GL_FMCW:
                    self.d2gl_endpoint = ep.number
                    logger.info(f"Found D2GL endpoint: {ep.number} (version {ep.version})")
                    break

            if self.d2gl_endpoint is None:
                logger.error("No D2GL endpoint found on device")
                self.disconnect()
                return False

            self._connected = True
            return True

        except serial.SerialException as e:
            logger.error(f"Failed to open serial port: {e}")
            return False

    def disconnect(self):
        if self.serial and self.serial.is_open:
            self.serial.close()
        self.serial = None
        self._connected = False
        self.d2gl_endpoint = None
        self.endpoints = []

    def is_connected(self) -> bool:
        return self._connected and self.serial is not None and self.serial.is_open

    def _build_message(self, endpoint: int, payload: bytes) -> bytes:
        length = len(payload)
        msg = bytearray()
        msg.append(START_BYTE_DATA)
        msg.append(endpoint)
        msg.append(length & 0xFF)
        msg.append((length >> 8) & 0xFF)
        msg.extend(payload)
        msg.append(END_OF_PAYLOAD & 0xFF)
        msg.append((END_OF_PAYLOAD >> 8) & 0xFF)
        return bytes(msg)

    def _send_message(self, endpoint: int, payload: bytes) -> bool:
        if not self.serial:
            return False
        msg = self._build_message(endpoint, payload)
        logger.debug(f"TX: {msg.hex()}")
        self.serial.write(msg)
        return True

    def _receive_message(self) -> Tuple[Optional[int], Optional[bytes], Optional[int]]:
        if not self.serial:
            return None, None, None

        header = self.serial.read(4)
        if len(header) < 4:
            logger.debug("Timeout reading message header")
            return None, None, None

        start_byte = header[0]
        endpoint = header[1]

        if start_byte == START_BYTE_DATA:
            length = header[2] | (header[3] << 8)
            payload = self.serial.read(length)
            if len(payload) < length:
                logger.warning(f"Incomplete payload: got {len(payload)}, expected {length}")
                return None, None, None

            tail = self.serial.read(2)
            if len(tail) < 2:
                logger.warning("Missing end marker")
                return None, None, None

            end_marker = tail[0] | (tail[1] << 8)
            if end_marker != END_OF_PAYLOAD:
                logger.warning(f"Bad end marker: 0x{end_marker:04X}")
                return None, None, None

            logger.debug(f"RX payload from ep{endpoint}: {payload.hex()}")
            return endpoint, payload, None

        elif start_byte == START_BYTE_STATUS:
            status_code = header[2] | (header[3] << 8)
            logger.debug(f"RX status from ep{endpoint}: 0x{status_code:04X}")
            return endpoint, None, status_code

        else:
            logger.warning(f"Unknown start byte: 0x{start_byte:02X}")
            self.serial.reset_input_buffer()
            return None, None, None

    def _send_and_receive(self, endpoint: int, payload: bytes) -> Tuple[List[bytes], int]:
        self._send_message(endpoint, payload)
        payloads = []
        while True:
            ep, data, status = self._receive_message()
            if ep is None:
                return payloads, -1
            if data is not None:
                payloads.append(data)
            if status is not None:
                return payloads, status

    def _query_endpoints(self) -> bool:
        payload = bytes([CMD_QUERY_ENDPOINT_INFO])
        responses, status = self._send_and_receive(0, payload)

        if status != 0 or not responses:
            logger.error(f"Endpoint query failed with status {status}")
            return False

        data = responses[0]
        if len(data) < 2 or data[0] != CMD_QUERY_ENDPOINT_INFO:
            logger.error("Invalid endpoint info response")
            return False

        num_endpoints = data[1]
        expected_size = 2 + (num_endpoints * 6)

        if len(data) < expected_size:
            logger.error(f"Endpoint response too short: {len(data)} < {expected_size}")
            return False

        self.endpoints = []
        for i in range(num_endpoints):
            offset = 2 + (i * 6)
            type_id = struct.unpack_from('<I', data, offset)[0]
            version = struct.unpack_from('<H', data, offset + 4)[0]
            ep = EndpointInfo(number=i + 1, type_id=type_id, version=version)
            self.endpoints.append(ep)
            logger.debug(f"Endpoint {ep.number}: type=0x{ep.type_id:08X}, version={ep.version}")

        logger.info(f"Found {num_endpoints} endpoints")
        return True

    def get_firmware_info(self) -> Optional[FirmwareInfo]:
        if not self.is_connected():
            return None

        payload = bytes([CMD_QUERY_FW_INFO])
        responses, status = self._send_and_receive(0, payload)

        if status != 0 or not responses:
            return None

        data = responses[0]
        if len(data) < 7 or data[0] != CMD_QUERY_FW_INFO:
            return None

        major = struct.unpack_from('<H', data, 1)[0]
        minor = struct.unpack_from('<H', data, 3)[0]
        build = struct.unpack_from('<H', data, 5)[0]
        description = ""
        if len(data) > 7:
            description = data[7:].decode('utf-8', errors='ignore').rstrip('\x00')

        return FirmwareInfo(major, minor, build, description)

    def get_parameters(self) -> Optional[RadarParameters]:
        if not self.is_connected() or self.d2gl_endpoint is None:
            return None

        payload = bytes([MSG_PARA_VALUES_REQ])
        responses, status = self._send_and_receive(self.d2gl_endpoint, payload)

        if status != 0 or not responses:
            logger.error(f"Get parameters failed: status={status}")
            return None

        data = responses[0]
        if len(data) < 26 or data[0] != MSG_PARA_VALUES_CNF:
            logger.error("Invalid parameter response")
            return None

        params = RadarParameters()
        params.max_distance_m = struct.unpack_from('<f', data, 1)[0]
        params.min_distance_m = struct.unpack_from('<f', data, 5)[0]
        params.frame_time_sec = struct.unpack_from('<f', data, 9)[0]
        params.number_of_samples = struct.unpack_from('<H', data, 13)[0]
        params.hold_threshold = struct.unpack_from('<I', data, 15)[0]
        params.activation_threshold = struct.unpack_from('<I', data, 19)[0]
        params.chirps_per_frame = data[23]
        params.continuous_mode = bool(data[24])
        params.baseband_gain_stage = bool(data[25])

        if len(data) >= 30:
            params.pulse_repetition_rate_sec = struct.unpack_from('<f', data, 26)[0]
        if len(data) >= 31:
            params.activate_cfar = bool(data[30])

        return params

    def set_parameters(self, params: RadarParameters) -> bool:
        if not self.is_connected() or self.d2gl_endpoint is None:
            return False

        payload = bytearray()
        payload.append(MSG_PARA_SET_VALUES_REQ)
        payload.extend(struct.pack('<f', params.max_distance_m))
        payload.extend(struct.pack('<f', params.min_distance_m))
        payload.extend(struct.pack('<f', params.frame_time_sec))
        payload.extend(struct.pack('<H', params.number_of_samples))
        payload.extend(struct.pack('<I', params.hold_threshold))
        payload.extend(struct.pack('<I', params.activation_threshold))
        payload.append(params.chirps_per_frame)
        payload.append(1 if params.continuous_mode else 0)
        payload.append(1 if params.baseband_gain_stage else 0)
        payload.extend(struct.pack('<f', params.pulse_repetition_rate_sec))
        payload.append(1 if params.activate_cfar else 0)

        responses, status = self._send_and_receive(self.d2gl_endpoint, bytes(payload))

        if status != 0:
            logger.error(f"Set parameters failed: status={status}")
            return False

        return True

    def get_result(self) -> Optional[RadarResult]:
        if not self.is_connected() or self.d2gl_endpoint is None:
            return None

        payload = bytearray()
        payload.append(MSG_RESULT_REQ)
        payload.extend(struct.pack('<I', 1))

        responses, status = self._send_and_receive(self.d2gl_endpoint, bytes(payload))

        if status != 0 or not responses:
            logger.error(f"Get result failed: status={status}")
            return None

        return self._parse_result(responses[0])

    def _parse_result(self, data: bytes) -> Optional[RadarResult]:
        if len(data) < 23 or data[0] != MSG_RESULT_CNF:
            return None

        result = RadarResult(
            result_count=struct.unpack_from('<I', data, 1)[0],
            frame_number=struct.unpack_from('<I', data, 5)[0],
            range_m=struct.unpack_from('<f', data, 9)[0],
            strength=struct.unpack_from('<f', data, 13)[0],
            velocity_mps=struct.unpack_from('<f', data, 17)[0],
            target_status=TargetStatus(data[21]),
            direction_of_movement=data[22],
            timestamp=time.time()
        )
        return result

    def start_continuous_results(self) -> bool:
        if not self.is_connected() or self.d2gl_endpoint is None:
            return False

        payload = bytearray()
        payload.append(MSG_RESULT_REQ)
        payload.extend(struct.pack('<I', 0xFFFFFFFF))

        return self._send_message(self.d2gl_endpoint, bytes(payload))

    def stop_continuous_results(self) -> bool:
        if not self.is_connected() or self.d2gl_endpoint is None:
            return False

        payload = bytearray()
        payload.append(MSG_RESULT_REQ)
        payload.extend(struct.pack('<I', 0))

        responses, status = self._send_and_receive(self.d2gl_endpoint, bytes(payload))
        return status == 0

    def receive_result(self) -> Optional[RadarResult]:
        if not self.serial:
            return None

        ep, data, status = self._receive_message()

        if data is not None and len(data) >= 23 and data[0] == MSG_RESULT_CNF:
            return self._parse_result(data)

        return None

    def stream_results(self, callback: Callable[[RadarResult], bool], max_samples: int = 0) -> int:
        if not self.start_continuous_results():
            return 0

        count = 0
        try:
            while True:
                result = self.receive_result()
                if result:
                    count += 1
                    if not callback(result):
                        break
                    if max_samples > 0 and count >= max_samples:
                        break
        finally:
            self.stop_continuous_results()

        return count


def main():
    parser = argparse.ArgumentParser(description='Distance2Go L Radar Interface')
    parser.add_argument('port', help='Serial port (e.g., /dev/ttyACM0 or COM3)')
    parser.add_argument('-n', '--samples', type=int, default=100,
                        help='Number of samples to collect (default: 100)')
    parser.add_argument('-o', '--output', type=str, default=None,
                        help='Output CSV file for logging')
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Enable verbose logging')
    args = parser.parse_args()

    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)

    radar = D2GLRadar(args.port)

    print(f"Connecting to {args.port}...")
    if not radar.connect():
        print("Failed to connect!")
        return 1

    print("Connected!")

    fw = radar.get_firmware_info()
    if fw:
        print(f"Firmware: v{fw.version_major}.{fw.version_minor}.{fw.version_build}")
        if fw.description:
            print(f"  {fw.description}")

    params = radar.get_parameters()
    if params:
        print(f"\nRadar Parameters:")
        print(f"  Distance range: {params.min_distance_m}m - {params.max_distance_m}m")
        print(f"  Frame time: {params.frame_time_sec}s ({1/params.frame_time_sec:.1f} Hz)")
        print(f"  Samples/chirp: {params.number_of_samples}")
        print(f"  Chirps/frame: {params.chirps_per_frame}")
        print(f"  Thresholds: activation={params.activation_threshold}, hold={params.hold_threshold}")

    outfile = None
    if args.output:
        outfile = open(args.output, 'w')
        outfile.write("timestamp,frame,range_m,strength,velocity_mps,status,direction\n")

    print(f"\nCollecting {args.samples} samples...")
    print("-" * 60)

    def handle_result(result: RadarResult) -> bool:
        status_names = ['none', 'static', 'transit', 'moving']
        status_str = status_names[result.target_status] if result.target_status < 4 else '?'

        print(f"Frame {result.frame_number:5d}: "
              f"range={result.range_m:6.3f}m, "
              f"strength={result.strength:8.1f}, "
              f"velocity={result.velocity_mps:+6.3f}m/s, "
              f"status={status_str}")

        if outfile:
            outfile.write(f"{result.timestamp:.3f},{result.frame_number},"
                         f"{result.range_m:.4f},{result.strength:.2f},"
                         f"{result.velocity_mps:.4f},{result.target_status},"
                         f"{result.direction_of_movement}\n")

        return True

    try:
        count = radar.stream_results(handle_result, max_samples=args.samples)
        print("-" * 60)
        print(f"Collected {count} samples")
    except KeyboardInterrupt:
        print("\nInterrupted by user")
    finally:
        if outfile:
            outfile.close()
        radar.disconnect()

    return 0


if __name__ == '__main__':
    exit(main())
