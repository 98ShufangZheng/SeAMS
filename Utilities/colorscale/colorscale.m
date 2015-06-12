function [StatIm] = colorscale(seg,Props)

%StatIm = zeros(size(seg));
Clm = Props';
% replace labels with prop values
%
Clm = [0;Clm]; % so the first 
%Due to the properties of a labeled raster (which labels column wise based
%on connected components) by using the row numbers of our column we can 
%replace values with props values without using a loop 
%
StatIm = Clm(seg+1);




