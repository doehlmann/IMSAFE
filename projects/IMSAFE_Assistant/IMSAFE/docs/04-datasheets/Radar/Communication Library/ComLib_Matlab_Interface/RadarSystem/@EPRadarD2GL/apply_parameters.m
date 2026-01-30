function apply_parameters(obj, param_new)

%obj.frame_period_corrected = false;
temp_param = obj.m_read_parameters;
if nargin == 1
    param_new = obj.m_new_parameters;
    [status, msg] = obj.check_parameter_limits( param_new );
    if( status ~= 0)
        error(msg); 
    end
    
    obj.m_new_parameters = {};
else
    [status, msg] = obj.check_parameter_limits( param_new );
    if( status ~= 0)
        error(msg); 
    end
end




if isfield(param_new, 'max_distance_m')
   temp_param.max_distance_m = param_new.max_distance_m;
end

if isfield(param_new, 'min_distance_m')
   temp_param.min_distance_m = param_new.min_distance_m;
end

if isfield(param_new, 'frame_time_sec')
    temp_param.frame_time_sec = param_new.frame_time_sec;
end
if isfield(param_new, 'number_of_samples')
    temp_param.number_of_samples = param_new.number_of_samples;
end

if isfield(param_new, 'target_hold_threshold')
    temp_param.target_hold_threshold = param_new.target_hold_threshold;
end

if isfield(param_new, 'target_activation_threshold')
    temp_param.target_activation_threshold = param_new.target_activation_threshold;
end

if isfield(param_new, 'number_of_chirps_per_frame')
   temp_param.number_of_chirps_per_frame = param_new.number_of_chirps_per_frame;
end

if isfield(param_new, 'baseband_gain_stage')
    temp_param.baseband_gain_stage = param_new.baseband_gain_stage;
end

if isfield(param_new, 'pulse_repetition_time_sec')
   temp_param.pulse_repetition_time_sec = param_new.pulse_repetition_time_sec;
end

if isfield(param_new, 'activate_cfar')
    temp_param.activate_cfar = param_new.activate_cfar;
end

offset = 0;

cmd_message = uint8(zeros(1,31));
cmd_message = protocol_write_payload_uint8 (cmd_message, offset, obj.MSG_PARA_SET_VALUES_REQ);
offset = offset + 1;

cmd_message = protocol_write_payload_float (cmd_message, offset, temp_param.max_distance_m);
offset = offset + 4;
cmd_message = protocol_write_payload_float (cmd_message, offset, temp_param.min_distance_m);
offset = offset + 4;
cmd_message = protocol_write_payload_float (cmd_message, offset, temp_param.frame_time_sec);
offset = offset + 4;
cmd_message = protocol_write_payload_uint16(cmd_message, offset, temp_param.number_of_samples);
offset = offset + 2;
cmd_message = protocol_write_payload_uint32 (cmd_message, offset, temp_param.target_hold_threshold);
offset = offset + 4;
cmd_message = protocol_write_payload_uint32 (cmd_message, offset, temp_param.target_activation_threshold);
offset = offset + 4;
cmd_message = protocol_write_payload_uint8 (cmd_message, offset, temp_param.number_of_chirps_per_frame);
offset = offset + 1;
cmd_message = protocol_write_payload_uint8 (cmd_message, offset, 0); % uint8 parameter ignored
offset = offset + 1;
cmd_message = protocol_write_payload_uint8 (cmd_message, offset, temp_param.baseband_gain_stage);
offset = offset + 1;

if isfield(temp_param, 'pulse_repetition_time_sec') 
    cmd_message = protocol_write_payload_float (cmd_message, offset, temp_param.pulse_repetition_time_sec);    
    offset = offset + 4;
end
if isfield(temp_param,'activate_cfar')
    cmd_message = protocol_write_payload_uint8 (cmd_message, offset, temp_param.activate_cfar);    
    offset = offset + 1;
end

if( offset ~= length(cmd_message))
   error('length mismatch!'); 
end

obj.m_read_parameters = {};
obj.doTransmission(cmd_message, 2, 0.1);
obj.m_last_frame_time_sec = temp_param.frame_time_sec;
while(isempty(obj.m_read_parameters))
    obj.oRS.processResponse;
end


