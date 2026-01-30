function shield_info = get_shield_info(obj)

obj.doTransmission(obj.MSG_GET_SHIELD_INFO_REQ);

shield_info  = obj.m_shield_info;

