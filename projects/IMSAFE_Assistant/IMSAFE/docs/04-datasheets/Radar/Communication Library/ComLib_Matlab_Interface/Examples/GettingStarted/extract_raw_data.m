%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Script extract_raw_data.m
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Copyright (c) 2014-2021, Infineon Technologies AG
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
% This script showcases the acquisition and live visualization of raw data.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Cleanup and init
% Before starting any kind of device the workspace must be cleared and the
% MATLAB Interface must be included into the code.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clc;
clear;
close all;

%% 1. Create radar system object
disp('******************************************************************');
addpath('..\..\RadarSystem'); % add MATLAB API
resetRS; % close and delete ports

szPort = findRSPort; % find the right COM Port
if (isempty(szPort)) 
    % try 2nd time
    szPort = findRSPort; % find the right COM Port
end
if (isempty(szPort))
   disp ('No RadarSystem found.');
   return;
end

oRS = RadarSystem(szPort); % create RadarSystem API object
oRS.oEPRadarBaseBoard.reset_parameters; % reset radar parameters
oRS.oEPRadarD2GL.get_parameters_def % display default radar parameters

%% 2. Display device information
board_info = oRS.oEPRadarBaseBoard.get_board_info;
shield_info = oRS.oEPRadarD2GL.get_shield_info;
current_info = oRS.oEPRadarBaseBoard.consumption;
current_def = oRS.oEPRadarBaseBoard.consumption_def;

disp(board_info.description);
disp(shield_info.description);
fprintf('%s: %f %s\n\n',current_def, current_info.value, current_info.unit);

%% 3. Set radar parameters
param = oRS.oEPRadarD2GL.get_parameters;

frame_time_sec = 0.1;
number_of_chirps_per_frame = 10;
pulse_repetition_time_sec = 0.005;
NTS = 64;
min_distance_m = 1;
max_distance_m = 16;
target_activation_threshold = 150;
target_hold_threshold = 75;
baseband_gain_stage = 0; % 0: high gain, 1: low gain
activate_cfar = 0;

if 0
    %%% User can directly set one individual parameter
    oRS.oEPRadarD2GL.parameters.frame_time_sec = frame_time_sec;
    oRS.oEPRadarD2GL.parameters.number_of_chirps_per_frame = number_of_chirps_per_frame;
    oRS.oEPRadarD2GL.parameters.pulse_repetition_time_sec = pulse_repetition_time_sec;
    oRS.oEPRadarD2GL.parameters.number_of_samples = NTS;
    oRS.oEPRadarD2GL.parameters.min_distance_m = min_distance_m;
    oRS.oEPRadarD2GL.parameters.max_distance_m = max_distance_m;
    oRS.oEPRadarD2GL.parameters.target_activation_threshold = target_activation_threshold;
    oRS.oEPRadarD2GL.parameters.target_hold_threshold = target_hold_threshold;
    oRS.oEPRadarD2GL.parameters.baseband_gain_stage = baseband_gain_stage;
    oRS.oEPRadarD2GL.parameters.activate_cfar = activate_cfar;
    oRS.oEPRadarD2GL.apply_parameters  % with this command the parameters are actually transfered to the device
    
    param_set = oRS.oEPRadarD2GL.get_parameters % display set parameters
else 
    %%% But it is easier to first set write all params in one struct and then
    %%% send the complete params at once
    param.frame_time_sec = frame_time_sec;
    param.number_of_chirps_per_frame = number_of_chirps_per_frame;
    param.pulse_repetition_time_sec = pulse_repetition_time_sec;
    param.number_of_samples = NTS;
    param.min_distance_m = min_distance_m;
    param.max_distance_m = max_distance_m;
    param.target_activation_threshold = target_activation_threshold;
    param.target_hold_threshold = target_hold_threshold;
    param.baseband_gain_stage = baseband_gain_stage;
    param.activate_cfar = activate_cfar;
    
    oRS.oEPRadarD2GL.apply_parameters(param); % send parameters
    
    param_set = oRS.oEPRadarD2GL.get_parameters % display set parameters
end

%% 4. Get and plot raw data
%%% Setup data recording
disp('Plot raw data...');
oRS.oEPRadarD2GL.get_only_latest_raw_data = true; % true for live streaming accepting frame losses due to MATLAB processing,
                                                  % false for data recording without frame losses utilizing buffering.

%%% Initialize figure plotting first frame
hFig = figure;

mxRawData = oRS.oEPRadarD2GL.get_raw_data;
% mxResultData = oRS.oEPRadarD2GL.get_result_data;
% [mxResultData, mxRawData] = oRS.oEPRadarD2GL.get_result_and_raw_data;
fprintf('Frame number: %d\n', mxRawData.frame_number);

subplot(2,1,1);
plot_data_I = squeeze(real(mxRawData.sample_data(:,1,:)));
hSub1 = plot(plot_data_I);
xlim([1,mxRawData.num_samples_per_chirp]);
ylim([0,1]);
xlabel('Samples');
ylabel('ADC Value (FSR)');
title('Raw Data: I');

subplot(2,1,2);
plot_data_Q = squeeze(imag(mxRawData.sample_data(:,1,:)));
hSub2 = plot(plot_data_Q);
xlim([1,mxRawData.num_samples_per_chirp]);
ylim([0,1]);
xlabel('Samples');
ylabel('ADC Value (FSR)');
title('Raw Data: Q');

drawnow;

%%% Start infinite loop to get and plot raw data
while ishandle(hFig)
    
    mxRawData = oRS.oEPRadarD2GL.get_raw_data;
    fprintf('Frame number: %d\n',mxRawData.frame_number);
    
    plot_data_I = squeeze(real(mxRawData.sample_data(:,1,:)));
    plot_data_I_cell = mat2cell(transpose(plot_data_I),ones(1,mxRawData.num_chirps));
    set(hSub1,{'YData'},plot_data_I_cell);
    
    plot_data_Q = squeeze(imag(mxRawData.sample_data(:,1,:)));
    plot_data_Q_cell = mat2cell(transpose(plot_data_Q),ones(1,mxRawData.num_chirps));
    set(hSub2,{'YData'},plot_data_Q_cell);
    
    drawnow;
    
end

%% 5. Stop sending data
disp('Stop sending data...');
oRS.oEPRadarD2GL.stop_data;

%% 6. Clear radar system object
disp('Clear radar object...');
clearSP(szPort);

%% 7. End of script
disp('Done!');

