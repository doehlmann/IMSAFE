function raw_data = get_raw_data(obj)

requested_rawData = false;
if( obj.m_raw_data.isEmpty()) 
    cmd_message = uint8(zeros(1,5));
    cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_GET_RAW_DATA_REQ);
    cmd_message = protocol_write_payload_uint32(cmd_message, 1, 10);
    
    obj.doTransmission(cmd_message);
    requested_rawData = true;
end

obj.oRS.processResponse;
while(obj.m_raw_data.isEmpty())
    obj.oRS.processResponse;
end

resultCellArray = obj.m_raw_data.dequeue();
raw_data = resultCellArray{1};

if( ~requested_rawData) 
    cmd_message = uint8(zeros(1,5));
    cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_GET_RAW_DATA_REQ);
    cmd_message = protocol_write_payload_uint32(cmd_message, 1, 10);
    
    obj.doTransmission(cmd_message);
end