function parameters_def = get_parameters_def(obj)

obj.doTransmission(obj.MSG_PARA_VALUES_DEF_REQ);

parameters_def  = obj.m_parameters_def;

