function stop_data(obj)

cmd_message = uint8(zeros(1,5));
cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_RESULT_REQ);
cmd_message = protocol_write_payload_uint32(cmd_message, 1, 0);
obj.doTransmission(cmd_message);
%obj.oRS.processResponse;

cmd_message = uint8(zeros(1,5));
cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_GET_RAW_DATA_REQ);
cmd_message = protocol_write_payload_uint32(cmd_message, 1, 0);
obj.doTransmission(cmd_message);
%obj.oRS.processResponse;
