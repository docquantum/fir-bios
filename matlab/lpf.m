function Hd = lpf
%LPF Returns a discrete-time filter object.

% MATLAB Code
% Generated by MATLAB(R) 9.9 and DSP System Toolbox 9.11.
% Generated on: 13-Mar-2021 23:33:29

% Equiripple Lowpass filter designed using the FIRPM function.

% All frequency values are in Hz.
Fs = 48000;  % Sampling Frequency

N     = 200;   % Order
Fpass = 800;   % Passband Frequency
Fstop = 1200;  % Stopband Frequency
Wpass = 1;     % Passband Weight
Wstop = 1;     % Stopband Weight
dens  = 20;    % Density Factor

% Calculate the coefficients using the FIRPM function.
b  = firpm(N, [0 Fpass Fstop Fs/2]/(Fs/2), [1 1 0 0], [Wpass Wstop], ...
           {dens});
Hd = dfilt.dffir(b);
set(Hd, 'Arithmetic', 'fixed', ...
    'CoeffWordLength', 16, ...
    'CoeffAutoScale', true, ...
    'Signed',         true, ...
    'InputWordLength', 16, ...
    'inputFracLength', 15, ...
    'FilterInternals',  'FullPrecision');
denormalize(Hd);

% [EOF]