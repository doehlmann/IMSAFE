function frame_data = get_frame_data(obj)

cmd_message = uint8(zeros(1,5));

cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_GET_RAW_DATA_REQ);
cmd_message = protocol_write_payload_uint32(cmd_message, 1, 1);

obj.m_raw_data_pending = false;
obj.doTransmission(cmd_message);

while(~(obj.m_raw_data_pending | obj.m_result_pending) ) 
	obj.oRS.processResponse; 
end 

frame_data = obj.m_raw_data;
