function parseMSG_GET_SHIELD_INFO_CNF_V2(obj, payload)

if (length(payload) > 1)
	offset = 1;
   
	% read string list
	shield_info.rf_frequency_khz = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
    shield_info.bandwidth_mhz = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	shield_info.major_version = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	shield_info.minor_version = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	shield_info.num_tx_antenna = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	shield_info.num_rx_antenna = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	
	[ shield_info.rf_shield_type_id, len] = protocol_read_payload_str(payload, offset);
    offset = offset + len; 
	
	[ shield_info.description, len] = protocol_read_payload_str(payload, offset);
    offset = offset + len; 
    
   	[ shield_info.modulation, len] = protocol_read_payload_str(payload, offset);
    offset = offset + len;
    if( length(payload)>= offset + 4)
        shield_info.up_chirp_time_s = protocol_read_payload_float(payload, offset);
        offset = offset + 4;
    end
    
    obj.m_shield_info = shield_info;
else
    fprintf('[%s.%s] Error! Wrong payload size.\n',mfilename('Class'),mfilename);
end
