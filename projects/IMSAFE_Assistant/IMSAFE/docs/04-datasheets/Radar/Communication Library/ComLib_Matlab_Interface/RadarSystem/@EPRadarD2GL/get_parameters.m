function param = get_parameters(obj)

obj.m_read_parameters = {};
obj.doTransmission(obj.MSG_PARA_VALUES_REQ, 2, 0.2);
while(isempty(obj.m_read_parameters))
    obj.oRS.processResponse;
end

param = obj.m_read_parameters;