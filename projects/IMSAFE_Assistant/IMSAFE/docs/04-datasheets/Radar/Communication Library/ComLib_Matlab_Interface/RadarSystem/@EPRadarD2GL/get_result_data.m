function result = get_result_data(obj)

requested_results = false;
if( obj.m_result_data.isEmpty()) 
    cmd_message = uint8(zeros(1,5));
    cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_RESULT_REQ);
    cmd_message = protocol_write_payload_uint32(cmd_message, 1, 10);
    %obj.m_result_pending = true;
    obj.doTransmission(cmd_message);
    requested_results = true;
end

obj.oRS.processResponse;
while(obj.m_result_data.isEmpty())
    obj.oRS.processResponse;
end
resultCellArray = obj.m_result_data.dequeue();
result = resultCellArray{1};

if( ~requested_results) 
    cmd_message = uint8(zeros(1,5));
    cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_RESULT_REQ);
    cmd_message = protocol_write_payload_uint32(cmd_message, 1, 10);
    %obj.m_result_pending = true;
    obj.doTransmission(cmd_message);
end