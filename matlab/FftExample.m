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
% f = [ 100/fs, 500/fs, 900/fs, 1300/fs, 1700/fs, 2100/fs, 2500/fs, 2900/fs, ...
%       3300/fs, 3700/fs, 4100/fs, 4500/fs, 4900/fs, 5300/fs, 5700/fs, 6100/fs];
% 
% input = 0;
% for i = 1 : size(f, 2)
%     input = input + sin(2 * pi * f(i) * t);
% end
% 
% input = round((input*32767)/size(f,2));
% 
% if (input > 32767 | input < -32767)
%     disp("YOU'RE AND IDIOT")
%     return
% end

[y, aFs] = audioread('c:/users/user/music/edwin starr - war - 48.wav', 'native');
input = y(:,1);
% y = round((normalize(y(:,1), 'range') - 0.5) * 32767 * 2);
% disp(max(y));
% disp(min(y));


k = 0:1:N-1;
fk = k * fs/N;

% fig = figure();

% maxim = 16;
totals = zeros(1,1);
% soundsc(y, aFs)
for i = 1 : L : size(input, 1)
     if i + L < size(input, 1)
        X = fft(input(i:i+L),N);
        X = floor(abs(X)/min(L,N));
%         X = myNorm(X);
        totals = cat(1, totals, X(1:32));
%         bar(fk(1:32), X(1:32))
%         
%         axis([-200 6000 0 16]);
%         saveas(gcf, sprintf('fft/%010d.png', i));
%         clf('reset')
%         disp(i);
%         if max(X) > maxim
%             maxim = max(X);
%         end
    end
end

disp(round(maxim))

disp(round(mean(totals)))

function Y = myNorm(X)
    Y = zeros(size(X));
    for i=1:size(X,1)
        Y(i) = myScale(X(i));
    end
end


function y = myScale(x)
    if x >= 2000
        y = 16;
    elseif x >= 1875
        y = 15;
    elseif x >= 1750
        y = 14;
    elseif x >= 1625
        y = 13;
    elseif x >= 1500
        y = 12;
    elseif x >= 1375
        y = 11;
    elseif x >= 1250
        y = 10;
    elseif x >= 1125
        y = 9;
    elseif x >= 1000
        y = 8;
    elseif x >= 875
        y = 7;
    elseif x >= 750
        y = 6;
    elseif x >= 625
        y = 5;
    elseif x >= 500
        y = 4;
    elseif x >= 375
        y = 3;
    elseif x >= 250
        y = 2;
    elseif x >= 125
        y = 1;
    else
        y = 0;
    end
end

% 0.347556343459177
% processed = ParseGenericDataFile('int16', 'fftProcessed1000and1200.dat')';
% 0.008390261844627
% processed = ParseGenericDataFile('int16', 'fftProcessed750and1500.dat')';

% processed =  round((processed ./ max(processed)) * 32767);
% figure
% set('gcf', 'color') = "white";
% X = fft(input(1:L),N);
% k = 0:1:N-1; % these are our frequency bins
% fk = k * fs/N; % discrete frequencies of DFT or FFT result
% X = abs(X)/min(L,N);
% % X = round((X ./ max(X)) * 32767);
% % Plot single-sided amplitude spectrum.
% plot(fk(1:32), X(1:32))
% hold on
% plot(fk, processed)

% diff = X-processed;
% plot(diff);
% axis([0 256 -0.5 1.5]);
% title('Raw Difference for 750 & 1500 Hz');
% xlabel('sample');
% ylabel('power');
% saveas(gcf, 'fft_raw_diff_750_1500.png');

% err1 = mean((diff).^2);
% disp(err1)

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