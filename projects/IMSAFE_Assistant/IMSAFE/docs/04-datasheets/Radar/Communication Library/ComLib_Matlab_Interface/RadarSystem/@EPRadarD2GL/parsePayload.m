function parsePayload(obj, RX_content)

switch RX_content(1) % command code
	case obj.MSG_PARA_VALUES_CNF
		obj.parseMSG_PARA_VALUES_CNF(RX_content)
	case obj.MSG_PARA_VALUES_DEF_CNF
		obj.parseMSG_PARA_VALUES_DEF_CNF(RX_content)
	case obj.MSG_RESULT_CNF
		obj.parseMSG_RESULT_CNF(RX_content)
    case obj.MSG_ADV_RESULT_CNF
		obj.parseMSG_ADV_RESULT_CNF(RX_content)        
	case obj.MSG_GET_SHIELD_INFO_CNF
		obj.parseMSG_GET_SHIELD_INFO_CNF(RX_content)
    case obj.MSG_GET_SHIELD_INFO_CNF_V2
        obj.parseMSG_GET_SHIELD_INFO_CNF_V2(RX_content)
	case obj.MSG_GET_RAW_DATA_CNF
		obj.parseMSG_GET_RAW_DATA_CNF(RX_content)


    otherwise
        text = sprintf('[%s.%s] Error! Unknown Command Code: 0x%s\n',mfilename('Class'),mfilename,dec2hex(RX_content(1)));
        if( obj.oRS.throwEx_on_parse_errors == true )
             error( '%s', text );
        else
            fprintf( '%s', text );
        end 
end

