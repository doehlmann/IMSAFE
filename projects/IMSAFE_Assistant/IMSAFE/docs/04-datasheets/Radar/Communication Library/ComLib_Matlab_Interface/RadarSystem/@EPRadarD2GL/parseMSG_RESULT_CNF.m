function parseMSG_RESULT_CNF(obj, payload)

if( obj.get_only_latest_raw_data == true )
    if( obj.oRS.BytesAvailable > 5*length(payload) )
        % skip parsing, if we have already more the two frames in queue!
        return;
    end
end


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
	
    result_data.fft_size                = 256; % hardcoded, value used on the target
    obj.m_result_data.enqueue( result_data );
else
    fprintf('[%s.%s] Error! Wrong payload size.\n', mfilename('Class'), mfilename);
end
