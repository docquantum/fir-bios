function CopyToC(data, name)
% Copyright Nebraska Embedded Systems Group 2014
%
switch nargin
    case 0
        error('not enough arguments, data, name');
    case 1
        name = 'data';
    case 2
    otherwise
        error('expected only data and name of vector')
end

str = sprintf('int16_t %s[] = \n{', name);

for i = 1:16:length(data)
    strln = sprintf('%6d, ', data(i:min(i+15,length(data))));
    str = [str sprintf('\n    ') strln];
end

str = [str(1:end-1) sprintf('\n};')];
clipboard('copy', str);
disp('Coppied to clipboard');
disp(str);
end