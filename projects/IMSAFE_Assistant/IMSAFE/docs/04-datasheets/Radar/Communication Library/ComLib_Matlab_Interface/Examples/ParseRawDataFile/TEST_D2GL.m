%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Script TEST_D2GL.m
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
% This script parses recorded RadarGUI data and plots the algorithm results.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Cleanup and init
% Before starting any kind of device the workspace must be cleared and the
% MATLAB Interface must be included into the code.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clc;
clear;
close all;

%% XML2STRUCT
%  to parse the XML file, the package XML2STRUCT is requried.
%  Please download the package from
%  https://de.mathworks.com/matlabcentral/fileexchange/28518-xml2struct
%  unzip it and copy the files into this folder
%  the function f_parse_data is not compatible with the build-in matlab
%  function!
%
if not(isfile("xml2struct.m"))
   error("Please install xml2struct.m, please see comments in the source file above!") 
end

%% 1. Load RadarGUI data
fdata = 'data_D2GL';

[frame, frame_count, results, sXML, Header] = f_parse_data_d2gl(fdata); % Data Parser

%% 2. Extract radar settings
NTS = double(Header.Samples_per_Chirp);
RANGE_FFT_SIZE = double(Header.Range_FFT_Size); % fixed in firmware
BW  = double(Header.Upper_RF_Frequency_kHz - Header.Lower_RF_Frequency_kHz) * 1e3;
frame_time_sec = Header.Frame_period_sec;

min_distance_m = results.min_distance_m;
max_distance_m = results.max_distance_m;
target_activation_threshold = results.activation_threshold;
target_hold_threshold = results.hold_threshold;

%% 3. Plot Target Activation Data and Target Hold Data of all frames
%%% Initialize figure plotting first frame
hFig = figure;

c0 = 3e8; % speed of light
MAX_AMP = 1000;

mxResultData = results.data(1);
fprintf('Frame number: %d\n', 1);

num_bins = length(mxResultData.activation_data);
R_max = NTS*c0/(2*BW);
DistPerBin = R_max/RANGE_FFT_SIZE; % distance per frequency bin
ArrayBinRange_plot = 0:DistPerBin:((num_bins-1)*DistPerBin);

subplot(2,1,1);
hSub1 = plot(ArrayBinRange_plot,mxResultData.activation_data);
hold on;
plot([0,100],[target_activation_threshold,target_activation_threshold],'r');
plot([min_distance_m,min_distance_m],[0,MAX_AMP],'g');
plot([max_distance_m,max_distance_m],[0,MAX_AMP],'g');
hold off;
xlim([0,ArrayBinRange_plot(end)]);
ylim([0,MAX_AMP]);
xlabel('Range (m)');
ylabel('Amplitude');
title('Target Activation Data');

subplot(2,1,2);
hSub2 = plot(ArrayBinRange_plot,mxResultData.hold_data);
hold on;
plot([0,100],[target_hold_threshold,target_hold_threshold],'r');
plot([min_distance_m,min_distance_m],[0,MAX_AMP],'g');
plot([max_distance_m,max_distance_m],[0,MAX_AMP],'g');
hold off;
xlim([0,ArrayBinRange_plot(end)]);
ylim([0,1000]);
xlabel('Range (m)');
ylabel('Amplitude');
title('Target Hold Data');

drawnow;

%%% Plot result data for all frames
len_data = frame_count;
for i = 2:len_data
    
    mxResultData = results.data(i);
    fprintf('Frame number: %d\n',i);
    
    set(hSub1,'YData',mxResultData.activation_data);
    set(hSub2,'YData',mxResultData.hold_data);
    
    drawnow;
    pause(frame_time_sec);
    
end

data = results.data;

%% 4. Plot algorithm results for all collected frames
figure;
leg = [];

ax1 = subplot(5,1,1);
plot([data.strength]);
% xlabel('Frames');
ylabel('Amplitude');

ax2 = subplot(5,1,2);
plot([data.range_m]);
% xlabel('Frames');
ylabel('Range (m)');

ax3 = subplot(5,1,3);
plot([data.velocity_mps]);
% xlabel('Frames')
ylabel('Velocity (m/s)');

ax4 = subplot(5,1,4);
plot([data.target_status]);
% xlabel('Frames')
ylabel('Status');
ylim([0,3]);

text(0,0.1,'No Target');
text(0,1.1,'Static Target');
text(0,2.1,'Transit State');
text(0,3.1,'Moving Target');

ax5 = subplot(5,1,5);
plot([data.direction_of_movement]);
xlabel('Frames')
ylabel('Direction of Movement');
ylim([0,3]);

text(0,0.1,'No Target');
text(0,1.1,'Target Not Moving');
text(0,2.1,'Approaching');
text(0,3.1,'Departing');

linkaxes([ax1,ax2,ax3,ax4,ax5],'x');
xlim([0 len_data]);

%% 5. 3D visualization of Target Activation Data and Target Hold Data
activation_data_plot = zeros(num_bins,len_data);
hold_data_plot = zeros(num_bins,len_data);
for i = 1:len_data
    activation_data_plot(:,i) = data(i).activation_data;
    hold_data_plot(:,i) = data(i).hold_data;
end

figure;

subplot(2,1,1);
y = 1:1:len_data;
x = ArrayBinRange_plot;
[xx,yy] = meshgrid(x,y);
mesh(xx,yy,activation_data_plot')
set(gca,'YDir','reverse')
title('Target Activation Data');
xlabel('Range (m)');
zlabel('Amplitude');
ylabel('Frames');
xlim([min_distance_m, max_distance_m]);
ylim([0,len_data]);
zlim([0,max(max(activation_data_plot))]);

subplot(2,1,2);
x = ArrayBinRange_plot;
[xx,yy] = meshgrid(x,y);
mesh(xx,yy,hold_data_plot')
set(gca,'YDir','reverse')
title('Target Hold Data');
xlabel('Range (m)');
zlabel('Amplitude');
ylabel('Frames');
xlim([min_distance_m, max_distance_m]);
ylim([0,len_data]);
zlim([0,max(max(hold_data_plot))]);

%% 6. End of script
disp('Done!');

%% Information on the exemplary data set
% Description of the data set "data_D2GL" for all frames:
%   0 -  29: No human or moving target in the scenario
%  30 -  37: Human target enters the scenario next to the radar system
%  38 - 143: Human target departs to almost 12m and turns around
% 144 - 169: Human target approaches to 10m 
% 170 - 196: Human target stands still at 10m
% 197 - 272: Human target approaches to 5m
% 273 - 306: Human target stands still at 5m
% 307 - 348: Human target approaches to 3m
% 349 - 400: Human target stands still at 3m
% 401 - 427: Human target approaches radar system
% 428 - 464: Human target passes radar system, which clears the track
%
% NOTE on the ghost targets at the beginning and end of the scenario:
% If a person is close to the radar system, static targets in the
% background can also "move/vibrate" due to shadowing effects. This can
% lead to temporary ghost targets, when a person enters and leaves the
% scenario close to the radar system like in the shown example.
%
% NOTE on the peak in Target Activation Data at 13m:
% Distance2GoL can detect very small movements and vibrations. In the shown
% measurement scenario an exterior wall was located at 13m, which is also
% slightly vibrating and thus leading to a peak in the provided data. To
% avoid a permanent ghost target, the maximum range was set to 12m.
%
% NOTE on the target velocity:
% The target velocity is calculated by the derivative of the latest range
% values of the corresponding track. In case of an internal track handover,
% this can lead to temporarily false velocity values like in frame 73.

