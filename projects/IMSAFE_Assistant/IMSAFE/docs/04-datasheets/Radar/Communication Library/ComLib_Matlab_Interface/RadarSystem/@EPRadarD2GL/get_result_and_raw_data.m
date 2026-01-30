function [result, raw_data] = get_result_and_raw_data(obj)

requested_results = false;
requested_raw_data = false;

if( obj.m_raw_data.isEmpty())     
    cmd_message = uint8(zeros(1,5));
    cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_GET_RAW_DATA_REQ);
    cmd_message = protocol_write_payload_uint32(cmd_message, 1, 10);
    obj.doTransmission(cmd_message);
    requested_raw_data = true;
end

if( obj.m_result_data.isEmpty())     
    cmd_message = uint8(zeros(1,5));
    cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_RESULT_REQ);
    cmd_message = protocol_write_payload_uint32(cmd_message, 1, 10);
    obj.doTransmission(cmd_message);
    requested_results = true;
end

obj.oRS.processResponse;
while(obj.m_result_data.isEmpty() || obj.m_raw_data.isEmpty())
    obj.oRS.processResponse;
end

resultCellArray = obj.m_result_data.dequeue();
result = resultCellArray{1};

resultCellArray = obj.m_raw_data.dequeue();
raw_data = resultCellArray{1};

if( ~requested_results) 
    cmd_message = uint8(zeros(1,5));
    cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_RESULT_REQ);
    cmd_message = protocol_write_payload_uint32(cmd_message, 1, 10);
    obj.doTransmission(cmd_message);
end

if( ~requested_raw_data)
    cmd_message = uint8(zeros(1,5));
    cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_GET_RAW_DATA_REQ);
    cmd_message = protocol_write_payload_uint32(cmd_message, 1, 10);
    obj.doTransmission(cmd_message);
end

