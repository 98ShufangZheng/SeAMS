%Export Cluster images





n = max(Posimage(:));% Get the number of clusters

%folder_name = uigetdir;%Get the folder string

%This will loop through each cluster producing a seperate segment image for
%each to work on individually.

%For some reason -3 keeps coming up as a filename. 

for i = 1-n
    BW = Posimage==i;
    newseg = membership2(seg, BW);
    x=num2str(i);
    %filename =strcat(folder_name,'/',x,'.tif');
    filename =strcat('miniseg',x);
    assignin('base',filename,newseg);
    %imwrite(newseg,filename);
end
    


%This creates a list of file names in a folder- need to change a few things
%here though - off matlab site
% fileFolder = fullfile(matlabroot,'toolbox','images','imdemos');
% dirOutput = dir(fullfile(fileFolder,'AT3_1m4_*.tif'));
% fileNames = {dirOutput.name}'



