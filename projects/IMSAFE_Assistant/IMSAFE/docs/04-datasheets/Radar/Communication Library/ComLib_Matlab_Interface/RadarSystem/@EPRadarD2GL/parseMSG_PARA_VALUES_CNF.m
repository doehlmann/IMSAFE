function parseMSG_PARA_VALUES_CNF(obj, payload)

if (length(payload) > 1)
	offset = 1;
    % extract parameters from message

	% read string list
	para.max_distance_m = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para.min_distance_m = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para.frame_time_sec = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	
	para.number_of_samples = protocol_read_payload_uint16(payload, offset);
	offset = offset + 2;
	para.target_hold_threshold = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	para.target_activation_threshold = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	para.number_of_chirps_per_frame = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
    % ignore, not used  = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	
    para.baseband_gain_stage = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	if(length(payload) >= offset + 4 )
        para.pulse_repetition_time_sec = protocol_read_payload_float(payload, offset);
        offset = offset + 4;
    end
	if(length(payload) >= offset + 1 )
        para.activate_cfar = protocol_read_payload_uint8(payload, offset);
        offset = offset + 1;
    end
    if( (obj.m_last_frame_time_sec ~= 0) & abs(obj.m_last_frame_time_sec - para.frame_time_sec)> 0.001 )
        warning('parameter frame_time_sec was corrected by device! new value: %f, wanted: %f\n', para.frame_time_sec, obj.m_last_frame_time_sec);
        obj.frame_period_corrected = true;
    end
    
    obj.m_last_frame_time_sec = 0.0;
    obj.m_read_parameters = para;
else
    fprintf('[%s.%s] Error! Wrong payload size.\n',mfilename('Class'),mfilename);
end