function parseMSG_ADV_RESULT_CNF(obj, payload)

if (length(payload) > 1)
	offset = 1;
	% read string list
	result_data.result_cnt              = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	result_data.frame_number            = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	result_data.range_m                 = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	result_data.strength                = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	result_data.velocity_mps            = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	result_data.target_status           = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	result_data.direction_of_movement   = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
    
    num_activation_data     = protocol_read_payload_uint16(payload, offset);
    offset = offset + 2;
    for i = 1:num_activation_data
        result_data.activation_data(i)  = protocol_read_payload_float(payload, offset);
        offset = offset + 4;
    end
    num_hold_data     = protocol_read_payload_uint16(payload, offset);
    offset = offset + 2;
    for i = 1:num_hold_data
        result_data.hold_data(i)  = protocol_read_payload_float(payload, offset);
        offset = offset + 4;
    end
	result_data.fft_size                = 256; % hardcoded, value used on the target
    obj.m_result_data.enqueue( result_data );
else
    fprintf('[%s.%s] Error! Wrong payload size.\n', mfilename('Class'), mfilename);
end
