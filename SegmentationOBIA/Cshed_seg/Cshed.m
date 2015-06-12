function [edge, segcontour, seg] = Cshed(I, Thresh)

% Authour Ciaran Robb
% Segments grayscale image using the input image gradient magnitude.
% the suppresion of local minima indirectly controls the number and size of
% segments
% Inputs:
% I = input image
% Thresh the minima suppresion threshold value
% Outputs
% edge = segment edges
% segcontour = the original native matlab output
% seg = a labeled segmentation image

if(size(I,3)==3)
   I = rgb2gray(I);
end
I = double(I);
filter = fspecial('gaussian',[3 3],1);
I=imfilter(I,filter,'symmetric');
[Gmag,Gdir] = imgradient(I); 
%Make the extended minima the scale value
Gmag = imhmin(Gmag,Thresh);
Wshed = watershed(Gmag);
edge = Wshed>0;
edge = imcomplement(edge);
[seg, N] = bwlabel(Wshed);
segcontour = seg;
% removes annoying edge lines created by native matlab function
seg = imclose(seg, strel('square', 3)); 



