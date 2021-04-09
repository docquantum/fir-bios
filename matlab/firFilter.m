% FIR filter

t = 0:1/48000:0.03;
t = t(1:1024);
f1 = 1000;
f2 = 4000;
testVector = (0.25 * sin(2*pi*f1.*t)) + (0.25 * sin(2*pi*f2.*t));
tvfp = round(testVector * 32768);

[h,w] = freqz(hpf(), 48000);
plot(w/pi*24000,20*log10(abs(h)));
axis([1000 3000 -100 10]);
title('Frequency Response');
xlabel('Frequency (Hz)');
ylabel('Magnitude (dB)');
saveas(gcf, 'hpf_freq.png');

figure()
[h,w] = freqz(lpf(), 48000);
plot(w/pi*24000,20*log10(abs(h)));
axis([0 2000 -100 10]);
title('Frequency Response');
xlabel('Frequency (Hz)');
ylabel('Magnitude (dB)');
saveas(gcf, 'lpf_freq.png');
return

hfp = round(hpf * 32768);
h2 = hfp / 32768;



y = filter(h2, 1, testVector);

yfp = round(y * 32768);

output = ParseGenericDataFile('int16', 'output.dat')';

diff = yfp - output;

plot(diff);
axis([20 120 -1.5 1.5]);
title('Raw Difference Between Expected and Real Output');
xlabel('sample');
ylabel('amplitude');
saveas(gcf, 'raw_diff.png');

err = mean((diff).^2)

subplot(2, 1, 1);
plot(tvfp);
axis([20 120 min(tvfp)-1024 max(tvfp)+1024]);
title('Original 1000Hz + 4000Hz Signal');
xlabel('sample');
ylabel('amplitude');
subplot(2, 1, 2);
plot(yfp);
hold on;
plot(output);
alpha(0.2)
axis([20+30 120+30 min(tvfp)-1024 max(tvfp)+1024]);
title('Expected Filtered Output Overlayed with Real');
xlabel('sample');
ylabel('amplitude');
saveas(gcf, 'filtered.png');