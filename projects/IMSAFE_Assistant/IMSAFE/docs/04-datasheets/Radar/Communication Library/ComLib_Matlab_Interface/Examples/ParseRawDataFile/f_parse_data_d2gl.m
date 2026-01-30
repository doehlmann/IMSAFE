%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Copyright (c) 2018-2021, Infineon Technologies AG
% All rights reserved.
%
% Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
% following conditions are met:
%
% Redistributions of source code must retain the above copyright notice, this list of conditions and the following
% disclaimer.
%
% Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
% disclaimer in the documentation and/or other materials provided with the distribution.
%
% Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
% products derived from this software without specific prior written permission.
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
% INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
% DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
% SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
% SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
% WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
% OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% DESCRIPTION:
% Parse data file to extract raw data for further processing
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [frame, frame_count, results, sXML, Header] = f_parse_data_d2gl(sFile)

fIDRaw = fopen([sFile '.raw.bin'], 'r', 'b'); % open file handle, big-endian style

if( isequal(fIDRaw, -1) )
    error("file not found, or format not supported!\n"); 
end

Header.file_type_id = fread(fIDRaw, 4, 'char');
if( ~isequal(char(Header.file_type_id'),'IFRB') )
   error("File type not supported!");    
end

Header.file_format_version = fread(fIDRaw, 1, 'float');
if( Header.file_format_version ~= 1 )
   error("File type version not supported!");    
end

sXML = xml2struct([sFile '.xml']); % parese xml file to struct

if(~isstruct(sXML.Device.D2GL))
    error("Device not supported, missing D2GL Endpoint in xml!");
end

Header.Header_Size = fread(fIDRaw, 1, 'int32');

Header.Num_Tx_Antennas = fread(fIDRaw, 1, 'int32');
Header.Num_Rx_Antennas = fread(fIDRaw, 1, 'int32');
Header.Mask_Tx_Antennas = fread(fIDRaw, 1, 'int32');
Header.Mask_Rx_Antennas = fread(fIDRaw, 1, 'int32');
Header.Are_Rx_Antennas_Interleaved = fread(fIDRaw, 1, 'int32'); % If this is 0, the radar data of multiple RX antennas is stored in consecutive data blocks, where each block holds data of one antenna. If this is non-zero, the radar data of multiple RX antennas is stored in one data block, where for each point in time the samples from all RX antennas are stored consecutively before the data of the next point in time follows.
Header.Modulation_Type_Enum = fread(fIDRaw, 1, 'int32');
Header.Chirp_Shape_Enum = fread(fIDRaw, 1, 'int32');

Header.Lower_RF_Frequency_kHz = fread(fIDRaw, 1, 'float');
Header.Upper_RF_Frequency_kHz = fread(fIDRaw, 1, 'float');
Header.Sampling_Frequency_kHz = fread(fIDRaw, 1, 'float');
Header.ADC_Resolution_Bits = fread(fIDRaw, 1, 'int32'); % The ADC resolution of the data in sample_data.
Header.Are_ADC_Samples_Normalized = fread(fIDRaw, 1, 'int32');
Header.Data_Format_Enum = fread(fIDRaw, 1, 'int32'); % This indicates if the data is pDataBuffer is real or complex, and if complex data is interleaved. 0: real, 1: complex, 2: complex interleaved
Header.Chirps_per_Frame = fread(fIDRaw, 1, 'int32'); % The number of chirps in this frame.
Header.Samples_per_Chirp = fread(fIDRaw, 1, 'int32'); % The number of samples acquired in each chirp for each enabled RX antenna.
Header.Samples_per_Frame = fread(fIDRaw, 1, 'int32'); % The number of samples acquired in each frame for each enabled RX antenna.
Header.Chirp_Time_sec = fread(fIDRaw, 1, 'float');
Header.Pulse_Repetition_Time_sec = fread(fIDRaw, 1, 'float');
Header.Frame_period_sec = fread(fIDRaw, 1, 'float');

Header.Range_FFT_Size = 256; % Hardcoded in device firmware

%% read in relevant header information from parsed xml fields

switch (sXML.Device.D2GL.FrameFormat.signalPart.Text)
    case{'RADAR_SIGNAL_ONLY_I' 'RADAR_SIGNAL_ONLY_Q'}
        Header.SignalPart = 1; % Only one signal (I or Q) is captured during radar data frame acquisition.
    case('RADAR_SIGNAL_I_AND_Q')
        Header.SignalPart = 2; % Both, I and Q signal are captured as a complex signal during radar data frame acquisition.
end

NumChirpData = Header.Samples_per_Chirp * Header.Num_Rx_Antennas * Header.SignalPart; % Number of sample values per chirp

Header.NumData = Header.Chirps_per_Frame * NumChirpData; % Number of sample values per frame



%% read in each frame
n = 1;

fread(fIDRaw, 1, 'uint8'); % peek into the next frame data block if there is any data available
while(~feof(fIDRaw))
    fseek(fIDRaw, -1, 'cof');
    
    frame(n).Frame_Number = fread(fIDRaw, 1, 'int32'); % The running number of the data frame. The frame counter is, reset every time ep_radar_base_set_automatic_frame_trigger is called. If automatic frame trigger is not active, the frame counter may not work, and this could be 0.
    frame(n).RawData = fread(fIDRaw, Header.NumData, '*single'); %#ok<*SAGROW> The buffer containing the radar data
    
    if( length(frame(n).RawData) == Header.NumData)
        % dispatch data
        Chirp = zeros(Header.Samples_per_Chirp, Header.Chirps_per_Frame, Header.Num_Rx_Antennas);
        % SamplesPerChirp = Header.Samples_per_Chirp;

        sn = 0:Header.Samples_per_Chirp-1; % zero based sample number

        if Header.Are_Rx_Antennas_Interleaved % interleaved antenna data
            switch Header.Data_Format_Enum
                case 0 % EP_RADAR_BASE_RX_DATA_REAL: The frame data contains only I or Q signal
                    % data_value = pFrameStart[SAMPLE_NUMBER * num_rx_antennas + ANTENNA_NUMBER];
                    for nc = 0:Header.Chirps_per_Frame-1
                        for na = 0:Header.Num_Rx_Antennas-1
                            IData = frame(n).RawData(1+ sn*frame(n).Header.Num_Rx_Antennas + na + NumChirpData*nc); % real
                            % QData = []; % imag
                            Chirp(:,nc+1,na+1) = IData;
                        end
                    end

                case 1 % EP_RADAR_BASE_RX_DATA_COMPLEX: The frame data contains I and Q signals in separate data blocks
                    % data_value_real = frame_start[SAMPLE_NUMBER * num_rx_antennas + ANTENNA_NUMBER];
                    % data_value_imag = frame_start[(num_samples_per_chirp + SAMPLE_NUMBER) * num_rx_antennas + ANTENNA_NUMBER];
                    for nc = 0:Header.Chirps_per_Frame-1
                        for na = 0:Header.Num_Rx_Antennas-1
                            IData = frame(n).RawData(1+ sn *Header.Num_Rx_Antennas + na + NumChirpData*nc); % real
                            QData = frame(n).RawData(1+ (Header.Samples_per_Chirp + sn)*Header.Num_Rx_Antennas + na + NumChirpData*nc); % imag
                            Chirp(:,nc+1,na+1) = IData + 1i*QData;
                        end
                    end

                case 2 % EP_RADAR_BASE_RX_DATA_COMPLEX_INTERLEAVED: The frame data contains I and Q signals in one interleaved data block
                    % data_value_real = frame_start[2 * SAMPLE_NUMBER * num_rx_antennas + ANTENNA_NUMBER];
                    % data_value_imag = frame_start[2 * SAMPLE_NUMBER * num_rx_antennas + ANTENNA_NUMBER + 1];
                    for nc = 0:Header.Chirps_per_Frame-1
                        for na = 0:Header.Num_Rx_Antennas-1
                            IData = frame(n).RawData(1+ 2*sn*Header.Num_Rx_Antennas + na     + NumChirpData*nc); % real
                            QData = frame(n).RawData(1+ 2*sn*Header.Num_Rx_Antennas + na + 1 + NumChirpData*nc); % imag
                            Chirp(:,nc+1,na+1) = IData + 1i*QData;
                        end
                    end
            end

        else % non interleaved antenna data
            switch Header.Data_Format_Enum
                case 0 % EP_RADAR_BASE_RX_DATA_REAL: The frame data contains only I or Q signal
                    % data_value = frame_start[ANTENNA_NUMBER * num_samples_per_chirp + SAMPLE_NUMBER];
                    for nc = 0:Header.Chirps_per_Frame-1
                        for na = 0:Header.Num_Rx_Antennas-1
                            IData = frame(n).RawData(1+ na*Header.Samples_per_Chirp + sn + NumChirpData*nc); % real
                            % QData = []; % imag
                            Chirp(:,nc+1,na+1) = IData;
                        end
                    end

                case 1 % EP_RADAR_BASE_RX_DATA_COMPLEX: The frame data contains I and Q signals in separate data blocks
                    % data_value_real = frame_start[(2 * ANTENNA_NUMBER    ) * num_samples_per_chirp + SAMPLE_NUMBER];
                    % data_value_imag = frame_start[(2 * ANTENNA_NUMBER + 1) * num_samples_per_chirp + SAMPLE_NUMBER];
                    for nc = 0:Header.Chirps_per_Frame-1
                        for na = 0:Header.Num_Rx_Antennas-1
                            IData = frame(n).RawData(1+  2*na   *Header.Samples_per_Chirp + sn + NumChirpData*nc); % real
                            QData = frame(n).RawData(1+ (2*na+1)*Header.Samples_per_Chirp + sn + NumChirpData*nc); % imag
                            Chirp(:,nc+1,na+1) = IData + 1i*QData;
                        end
                    end

                case 2 % EP_RADAR_BASE_RX_DATA_COMPLEX_INTERLEAVED: The frame data contains I and Q signals in one interleaved data block
                    % data_value_real = frame_start[2 * ANTENNA_NUMBER * num_samples_per_chirp + 2*SAMPLE_NUMBER];
                    % data_value_imag = frame_start[2 * ANTENNA_NUMBER * num_samples_per_chirp + 2*SAMPLE_NUMBER + 1];
                    for nc = 0:Header.Chirps_per_Frame-1
                        for na = 0:Header.Num_Rx_Antennas-1
                            IData = frame(n).RawData(1+ 2*na*Header.Samples_per_Chirp + 2*sn     + NumChirpData*nc); % real
                            QData = frame(n).RawData(1+ 2*na*Header.Samples_per_Chirp + 2*sn + 1 + NumChirpData*nc); % imag
                            Chirp(:,nc+1,na+1) = IData + 1i*QData;
                        end
                    end
            end
        end

        frame(n).Chirp = Chirp;
    else
        n = n - 1;
    end
    fread(fIDRaw, 1, 'uint8'); % peek into the next frame data block if there is any data available
    n = n+1;
end
frame_count = n - 1;

%% close file
fclose(fIDRaw);

results = { };

%read application data
fIApp = fopen([sFile '.tjsfdata'], 'r', 'b'); % open file handle, big-endian style

if( isequal(fIApp, -1) )
    error("application data file not found!\n"); 
end

Header.file_type_id = fread(fIDRaw, 4, 'char');
if( ~isequal(char(Header.file_type_id'),'IFAT') )
   error("File type not supported!");    
end


% read header
n = 0;
tline = fgetl(fIApp);
while ischar(tline)
    %disp(tline)
    tline = fgetl(fIApp);
    n = n + 1;
    if( n == 17 )
        break;
    end
end

count = 1;
while ischar(tline)
   
    tline = strrep(tline,'<','');
    tline = strrep(tline,'>','');
    tline = strrep(tline,' ',''); % trim whitespaces
    values_str = split(tline,",");
    
    values(count).frame_count = str2double(values_str{2});
    values(count).velocity_mps = str2double(values_str{3});
    values(count).strength = str2double(values_str{4});
    values(count).direction_of_movement = str2double(values_str{5});
    values(count).target_status = str2double(values_str{6});
    values(count).range_m = str2double(values_str{7});
   
    tline = fgetl(fIApp);
    count = count + 1;
end

results.data = values;

results.max_distance_m = str2double(sXML.Device.D2GL.FmcwParaValues.max_distance_m.Text);
results.min_distance_m = str2double(sXML.Device.D2GL.FmcwParaValues.min_distance_m.Text);
results.frame_time_sec = str2double(sXML.Device.D2GL.FmcwParaValues.frame_time_sec.Text);
results.number_of_samples = str2double(sXML.Device.D2GL.FmcwParaValues.number_of_samples_nu.Text);
results.hold_threshold = str2double(sXML.Device.D2GL.FmcwParaValues.target_hold_threshold.Text);
results.activation_threshold = str2double(sXML.Device.D2GL.FmcwParaValues.target_activation_threshold.Text);
results.number_of_chirps_per_frame = str2double(sXML.Device.D2GL.FmcwParaValues.number_of_chirps_per_frame.Text);
results.baseband_gain_stage = str2double(sXML.Device.D2GL.FmcwParaValues.baseband_gain_stage_bool.Text);
results.pulse_repetition_rate_sec = str2double(sXML.Device.D2GL.FmcwParaValues.pulse_repetition_rate_sec.Text);

results.fft_size = 256; % Hardcoded in device firmware

%% close file
fclose(fIApp);

%read activation & hold data
fIDadvancedApp = fopen([sFile '.tjsfadvanceddata'], 'r', 'b'); % open file handle, big-endian style

if( isequal(fIDadvancedApp, -1) )
    warning("activation and hold data file not found!\n"); 
    return;
end



num_activationData = fread(fIDadvancedApp, 1, 'uint32');
num_holdData = fread(fIDadvancedApp, 1, 'uint32');

if(num_activationData < 32 || num_activationData > 256)
    error("size not consitant!"); 
end

if(num_holdData < 32 || num_holdData > 256)
    error("size not consitant!"); 
end

n = 1;
while(~feof(fIDadvancedApp))
    temp = fread(fIDadvancedApp, num_activationData , '*single');
    if ~isempty(temp)
        results.data(n).activation_data = temp;
        results.data(n).hold_data       = fread(fIDadvancedApp, num_holdData , '*single');
        n = n + 1;
    end
end
%results.data = data;
results.Frame_count = n - 1;

%% close file
fclose(fIDadvancedApp);


