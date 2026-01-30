classdef EPRadarD2GL < handle

    %% methods
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Hidden) % constructor, general class
        function obj = EPRadarD2GL(oRS, uEndpointID, uEndpointVer)
            obj.uEndpointID = uEndpointID;
            obj.uEndpointVer = uEndpointVer;
            obj.oRS = oRS;
        end

        function initValues(obj)
            obj.get_shield_info;
			obj.get_parameters_def;
			obj.get_parameters;
            obj.m_new_parameters = {};
            obj.m_result_data = myQueue;
            obj.m_raw_data = myQueue;
        end
        
       %  obj.m_result_data = Queue;
        
        parsePayload(obj, RX_content)
        doTransmission(obj, message, poll_response, wait)
    end

    methods (Hidden) % communication with endpoint
		param = get_parameters(obj)		
		parameters_def = get_parameters_def(obj)
		result = get_result_data(obj)
		shield_info = get_shield_info(obj)
		raw_data = get_raw_data(obj)
        frame_data = get_frame_data(obj)
        set_parameter(obj, new_parameter)
        stop_data(obj)
	end

    methods (Hidden) % parsing of incoming messages
		parseMSG_PARA_VALUES_CNF(obj, payload)    
		parseMSG_PARA_VALUES_DEF_CNF(obj, payload)
		parseMSG_RESULT_CNF(obj, payload)		 	
        parseMSG_ADV_RESULT_CNF(obj, payload)		 	
		parseMSG_GET_SHIELD_INFO_CNF(obj, payload)
        parseMSG_GET_SHIELD_INFO_CNF_V2(obj, payload)
		parseMSG_GET_RAW_DATA_CNF(obj, payload)   
    end

	%% properties
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties
       get_only_latest_raw_data = false; 
       frame_period_corrected= false;
    end
    
    properties (Hidden, Constant) % endpoint tag
        szEPTag = 'TJSF'; % ep_radar_tjsf_definition    0x544A5055  Device used TJSF
        uMinVer = 1;      % min version of this endpoint on FW supported by this host code
        uMaxVer = 1;      % max version of this endpoint on FW supported by this host code
    end

    properties (Hidden) % general class
        uEndpointID;
        uEndpointVer; 
        oRS;
    end

    properties (Hidden) % data properties
		m_read_parameters;
        m_new_parameters;
		m_parameters_def;
        m_shield_info;
		m_result_data;
		m_raw_data;
        m_last_frame_time_sec;
    end
    
    properties (Dependent)
		parameters;
		parameters_def;
        shield_info;
		result_data;
		raw_data;
        frame_data;
    
    end

    properties (Access = private, Constant)	% message ids
        MSG_PARA_VALUES_REQ 	= uint8(hex2dec('50')); 
        MSG_PARA_VALUES_CNF     = uint8(hex2dec('51')); 
		
        MSG_PARA_VALUES_DEF_REQ = uint8(hex2dec('52')); 
		MSG_PARA_VALUES_DEF_CNF = uint8(hex2dec('53')); 
		MSG_PARA_SET_VALUES_REQ = uint8(hex2dec('54')); 
		

        MSG_ADV_RESULT_REQ      = uint8(hex2dec('61'));  % actually the code for MSG_ADV_RESULT_REQ
		MSG_ADV_RESULT_CNF		= uint8(hex2dec('62')); 

        %MSG_RESULT_REQ		 	= uint8(hex2dec('55')); 
        MSG_RESULT_REQ          = uint8(hex2dec('61'));    % actually the code for MSG_ADV_RESULT_REQ
		MSG_RESULT_CNF		 	= uint8(hex2dec('56')); 

        
		MSG_GET_SHIELD_INFO_REQ = uint8(hex2dec('57')); 
		MSG_GET_SHIELD_INFO_CNF = uint8(hex2dec('58')); 
        MSG_GET_SHIELD_INFO_CNF_V2 = uint8(hex2dec('63')); 
		
		MSG_GET_RAW_DATA_REQ    = uint8(hex2dec('59')); 
		MSG_GET_RAW_DATA_CNF    = uint8(hex2dec('6A')); 
        
    end
    
    %% set/get methods
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods % set/get
        function val = get.parameters_def(obj)
            val = obj.m_parameters_def;
        end
        function val = get.parameters(obj)
            val = obj.m_new_parameters;
        end
        function val = get.shield_info(obj)
            val = obj.m_shield_info;
        end
        function val = get.result_data(obj)
            val = obj.m_result_data;
        end
    	function val = get.raw_data(obj)
            val = obj.m_raw_data;
        end
        function val = get.frame_data(obj)
            val = obj.m_raw_data;
        end
        %%
        function set.parameters(obj, param_new)
		
           % check on additional fieldnames
           test = fieldnames(rmfield(param_new,intersect(fieldnames(obj.m_read_parameters),fieldnames(param_new))));
           if( ~isempty(test))
               disp([test]);
               error('invalid parameters names are added!');
           end
           
            if isfield(param_new, 'max_distance_m')
                obj.m_new_parameters.max_distance_m = param_new.max_distance_m;
            end

            if isfield(param_new, 'min_distance_m')
                obj.m_new_parameters.min_distance_m = param_new.min_distance_m;
            end

            if isfield(param_new, 'frame_time_sec')
                obj.m_new_parameters.frame_time_sec = param_new.frame_time_sec;
            end
            if isfield(param_new, 'number_of_samples')
                obj.m_new_parameters.number_of_samples = param_new.number_of_samples;
            end
            if isfield(param_new, 'target_hold_threshold')
                obj.m_new_parameters.target_hold_threshold = param_new.target_hold_threshold;
            end

            if isfield(param_new, 'target_activation_threshold')
                obj.m_new_parameters.target_activation_threshold = param_new.target_activation_threshold;
            end

            if isfield(param_new, 'number_of_chirps_per_frame')
               obj.m_new_parameters.number_of_chirps_per_frame = param_new.number_of_chirps_per_frame;
            end

            if isfield(param_new, 'baseband_gain_stage')
                obj.m_new_parameters.baseband_gain_stage = param_new.baseband_gain_stage;
            end
            if isfield(param_new, 'pulse_repetition_time_sec')
              obj.m_new_parameters.pulse_repetition_time_sec = param_new.pulse_repetition_time_sec;
            end

            if isfield(param_new, 'activate_cfar')
                obj.m_new_parameters.activate_cfar = param_new.activate_cfar;
            end
           
           %obj.apply_parameters( val );
        end
        %%
        function [status, message] = check_parameter_limits(obj, param_new )
            
            status = 0;
            message = '';
            
            if isfield(param_new, 'max_distance_m')
                if( param_new.max_distance_m > obj.m_parameters_def.max_distance_m_upper_boundary)
                    message = [message sprintf('max_distance_m exceeds upper limit of %f\n',obj.m_parameters_def.max_distance_m_upper_boundary)];
                    status = status + 1;
                end
                if( param_new.max_distance_m < obj.m_parameters_def.max_distance_m_lower_boundary)
                    message = [message sprintf('max_distance_m exceeds lower limit of %f\n',obj.m_parameters_def.max_distance_m_lower_boundary)] ;
                    status = status +1;
                end     
            end

            if isfield(param_new, 'min_distance_m')
                if( param_new.min_distance_m > obj.m_parameters_def.min_distance_m_upper_boundary)
                    message = [message sprintf('min_distance_m exceeds upper limit of %f\n',obj.m_parameters_def.min_distance_m_upper_boundary)] ;
                    status = status +1;
                end
                if( param_new.min_distance_m < obj.m_parameters_def.min_distance_m_lower_boundary)
                    message = [message sprintf('min_distance_m exceeds lower limit of %f\n',obj.m_parameters_def.min_distance_m_lower_boundary)] ;
                    status = status +1;
                end    
            end

            if isfield(param_new, 'frame_time_sec')
                if( param_new.frame_time_sec > obj.m_parameters_def.frame_time_sec_upper_boundary)
                    message = [message sprintf('frame_time_sec exceeds upper limit of %f\n',obj.m_parameters_def.frame_time_sec_upper_boundary)] ;
                    status = status +1;
                end
                % check not needed, because the frame time is corrected by
                % firmware to minimum possible value if needed!
%                 if( param_new.frame_time_sec < obj.m_parameters_def.frame_time_sec_lower_boundary)
%                     message = [message sprintf('frame_time_sec exceeds lower limit of %f\n',obj.m_parameters_def.frame_time_sec_lower_boundary)] ;
%                     status = status +1;
%                 end    
              %  obj.m_last_frame_time_sec = param_new.frame_time_sec;
            end
            
            if isfield(param_new, 'number_of_samples')
                if( ~ismember(param_new.number_of_samples, obj.m_parameters_def.sample_list))
                    message = [message sprintf('number_of_samples can only be a value out of: [') sprintf('%d ', obj.m_parameters_def.sample_list) sprintf(']\n')];
                    status = status +1;
                end
            end
            if isfield(param_new, 'target_hold_threshold')
                if( param_new.target_hold_threshold > obj.m_parameters_def.target_hold_threshold_upper_boundary)
                    message = [message sprintf('target_hold_threshold exceeds upper limit of %f\n',obj.m_parameters_def.target_hold_threshold_upper_boundary)] ;
                    status = status +1;
                end
                if( param_new.target_hold_threshold < obj.m_parameters_def.target_hold_threshold_lower_boundary)
                    message = [message sprintf('target_hold_threshold exceeds lower limit of %f\n',obj.m_parameters_def.target_hold_threshold_lower_boundary)] ;
                    status = status +1;
                end  
            end

            if isfield(param_new, 'target_activation_threshold')
                if( param_new.target_activation_threshold > obj.m_parameters_def.target_activation_threshold_upper_boundary)
                    message = [message sprintf('target_activation_threshold exceeds upper limit of %f\n',obj.m_parameters_def.target_activation_threshold_upper_boundary)] ;
                    status = status +1;
                end
                if( param_new.target_activation_threshold < obj.m_parameters_def.target_activation_threshold_lower_boundary)
                    message = [message sprintf('target_activation_threshold exceeds lower limit of %f\n',obj.m_parameters_def.target_activation_threshold_lower_boundary)] ;
                    status = status +1;
                end                  
            end

            if isfield(param_new, 'number_of_chirps_per_frame')
                if( param_new.number_of_chirps_per_frame > obj.m_parameters_def.number_of_chirps_per_frame_upper_boundary)
                    message = [message sprintf('number_of_chirps_per_frame exceeds upper limit of %f\n',obj.m_parameters_def.number_of_chirps_per_frame_upper_boundary)] ;
                    status = status +1;
                end
                if( param_new.number_of_chirps_per_frame < obj.m_parameters_def.number_of_chirps_per_frame_lower_boundary)
                    message = [message sprintf('number_of_chirps_per_frame exceeds lower limit of %f\n',obj.m_parameters_def.number_of_chirps_per_frame_lower_boundary) ];
                    status = status +1;
                end  
                if( param_new.number_of_chirps_per_frame ~= ceil( param_new.number_of_chirps_per_frame ) | ...
                        param_new.number_of_chirps_per_frame ~= floor( param_new.number_of_chirps_per_frame ))
                    message = [message sprintf('number_of_chirps_per_frame must be integer value\n')] ;
                    status = status +1;                    
                end
            end
% 
%             if isfield(param_new, 'baseband_gain_stage')
%                  test = param_new.baseband_gain_stage;
%             end
%             
            if isfield(param_new, 'pulse_repetition_time_sec')
                if( param_new.pulse_repetition_time_sec > obj.m_parameters_def.pulse_repetition_time_sec_upper_boundary)
                    message = [message sprintf('pulse_repetition_time_sec exceeds upper limit of %f\n',obj.m_parameters_def.pulse_repetition_time_sec_upper_boundary)] ;
                    status = status +1;
                end
                if( param_new.pulse_repetition_time_sec < obj.m_parameters_def.pulse_repetition_time_sec_lower_boundary)
                    message = [message sprintf('pulse_repetition_time_sec exceeds lower limit of %f\n',obj.m_parameters_def.pulse_repetition_time_sec_lower_boundary)] ;
                    status = status +1;
                end  
            end
 
%             if isfield(param_new, 'activate_cfar')
%                test = param_new.activate_cfar;
%             end
            
            
           
        end
    end
    
end
