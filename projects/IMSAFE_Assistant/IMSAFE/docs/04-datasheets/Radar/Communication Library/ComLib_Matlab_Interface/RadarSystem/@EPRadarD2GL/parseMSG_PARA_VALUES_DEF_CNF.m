function parseMSG_PARA_VALUES_DEF_CNF(obj, payload)

if (length(payload) > 1)
	offset = 1;
    % extract parameters from message
	msgsize = protocol_read_payload_uint16(payload, offset);
    offset = offset + 2;

	% read string list
	para_def.max_distance_m_lower_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.max_distance_m_upper_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	%skip decoding of max_distance_m_step_size
	offset = offset + 4;
	para_def.min_distance_m_lower_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.min_distance_m_upper_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	%skip decoding of min_distance_m_step_size
	offset = offset + 4;
    
    para_def.frame_time_sec_lower_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.frame_time_sec_upper_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	%skip decoding of frame_time_sec_step_size
	offset = offset + 4;

	% read samples list
	num_elem = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	
	samples = [];
	
	for index = 1:num_elem
		elem = protocol_read_payload_uint16(payload, offset);
		offset = offset + 2;
		samples = [samples, elem];
    end 

	para_def.sample_list = samples;
	
	para_def.target_hold_threshold_lower_boundary = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	para_def.target_hold_threshold_upper_boundary = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
    %skip decoding of target_hold_threshold_step_size
	offset = offset + 4;
	para_def.target_activation_threshold_lower_boundary = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	para_def.target_activation_threshold_upper_boundary = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	%skip decoding of target_activation_threshold_step_size
	offset = offset + 4;
    
    para_def.number_of_chirps_per_frame_lower_boundary = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	para_def.number_of_chirps_per_frame_upper_boundary = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	para_def.number_of_chirps_per_frame_step_size = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	if( length(payload) >= offset +4) 
        para_def.pulse_repetition_time_sec_lower_boundary = protocol_read_payload_float(payload, offset);
        offset = offset + 4;
    end
	if( length(payload) >= offset +4) 
        para_def.pulse_repetition_time_sec_upper_boundary = protocol_read_payload_float(payload, offset);
        offset = offset + 4;
    end    
	if( length(payload) >= offset +4) 
        %skip decoding ofpulse_repetition_time_sec_step_size
        offset = offset + 4;
    end    
  
    obj.m_parameters_def = para_def;
else
    fprintf('[%s.%s] Error! Wrong payload size.\n',mfilename('Class'),mfilename);
end
