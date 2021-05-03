clc

% create the time base for a sample frequency fs
fs = 48000;

%N number of samples
N = 1024;

% the time steps for the generated signals, truncated to the fft length
t = ( 0 : 1 : N-1 );

% f = [(1/2^5), (1/2^6)];
f = [ 100/fs, 500/fs, 900/fs, 1300/fs, 1700/fs, 2100/fs, 2500/fs, 2900/fs, ...
       3300/fs, 3700/fs, 4100/fs, 4500/fs, 4900/fs, 5300/fs, 5700/fs, 6100/fs];

input = 0;
for i = 1 : size(f, 2)
    input = input + sin(2 * pi * f(i) * t);
end

input = round((input*32767)/size(f,2));

if (input > 32767 | input < -32767)
    disp("YOU'RE AN IDIOT")
    return
end

%% Encode
z = 0b0u16;
k = 0;
y = 0;
reference = 0b0s16;
stepSize = 1024;
rho = 0x2u16;
MAX = 32767;
MIN = -32768;
for i = 1 : N
    if (input(i) >= reference)
        z = bitor(0b1u16, bitsll(z,1));
    else
        z = bitor(0b0u16, bitsll(z,1));
    end
    
    stepSize = runOf3(z, stepSize);
    
    steps(i) = stepSize;
    
    x = bitand(0b1u16, z);
    % Integration
    if (x == 0)
        add = reference - stepSize;
    else
        add = reference + stepSize;
    end
    
    if (add > MAX)
        reference = MAX;
    elseif (add < MIN)
        reference = MIN;
    else
        reference = add;
    end
    arr(i) = reference;
    binary(i) = uint16(x);
end

% stairs(steps);
% hold on;
% stairs(arr);
% hold on;
plot(input);
%% Decode
z = 0b0u16;
k = 0;
y = 0;
reference = 0b0s16;
stepSize = 1024;
rho = 0x2u16;
MAX = 32767;
MIN = -32768;
for i = 1 : N
    z = bitor(bitand(0x1u16, binary(i)), bitsll(z,1));
    
    stepSize = runOf3(z, stepSize);
    
    steps(i) = stepSize;
    
    x = bitand(0b1u16, z);
    % Integration
    if (x == 0)
        add = reference - stepSize;
    else
        add = reference + stepSize;
    end
    
    if (add > MAX)
        reference = MAX;
    elseif (add < MIN)
        reference = MIN;
    else
        reference = add;
    end
    output(i) = reference;
end
hold on;
plot(output);
disp(sum(abs(double(output))-abs(double(input))));

%% Functions

function step = runOf3(z, stepSize)
    step = stepSize;
    k = bitand(0b111u16, z);
    if (k == 0b0u16 || k == 0b111u16)
        step = (stepSize * 2) - (stepSize / 4);
    elseif (stepSize > 1024)
        step = (stepSize / 2) + (stepSize / 8);
    end
end