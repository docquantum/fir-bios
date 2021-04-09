clc

% create the time base for a sample frequency fs
fs = 48000;

% L is the number of samples (i.e. windowed number of samples)
L = 256;

%N-point FFT
N = 256;

% freqeuncy resolution df = fs/L
df = fs/L;

% computational resolution is dfk = fs/N
dfk = fs/N;

% the time steps for the generated signals, truncated to the fft length
t = ( 0 : 1 : N-1 );

% Same as fs/2^n to allow for seamless looping on the output which reduces the
% distortions that happens when actually outputting the vector as audio. When
% performing FFT, these should be at "maximum" amplitudes since they are not out
% of phase with the sampling frequency.
% Currently 1500 + 750
% f = [(1/2^5), (1/2^6)];

% Uncomment to use arbitrary frequencies
f = [ 1000/fs, 1200/fs ];

input = 0;
for i = 1 : size(f, 2)
    input = input + sin(2 * pi * f(i) * t);
end

input = round((input*32767)/size(f,2));

if (input > 32767 | input < -32767)
    disp("YOU'RE AND IDIOT")
    return
end

% 0.347556343459177
processed = ParseGenericDataFile('int16', 'fftProcessed1000and1200.dat')';
% 0.008390261844627
% processed = ParseGenericDataFile('int16', 'fftProcessed750and1500.dat')';

% processed =  round((processed ./ max(processed)) * 32767);
% figure
% set('gcf', 'color') = "white";
X = fft(input(1:L),N);
k = 0:1:N-1; % these are our frequency bins
fk = k * fs/N; % discrete frequencies of DFT or FFT result
X = abs(X)/min(L,N);
% X = round((X ./ max(X)) * 32767);
% Plot single-sided amplitude spectrum.
% plot(fk, X)
% hold on
% plot(fk, processed)

diff = X-processed;
% plot(diff);
% axis([0 256 -0.5 1.5]);
% title('Raw Difference for 750 & 1500 Hz');
% xlabel('sample');
% ylabel('power');
% saveas(gcf, 'fft_raw_diff_750_1500.png');

err1 = mean((diff).^2);
disp(err1)

% plot(fk,processed)
% hold on;
% plot_real = plot(fk,abs(X), '--');
% grid on;
% alpha(plot_real, 0.1)
% axis([min(fk) max(fk) 0 9000]);
% title('Spectrum Comparison for 750 & 1500 Hz')
% xlabel('Frequency (Hz)')
% ylabel('|X(fk)|')
% saveas(gcf, 'fft_750_1500_compare.png');