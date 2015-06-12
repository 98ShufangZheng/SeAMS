%Author: Ciaran Robb
% Segment Pixel statistics
% Acknowledgements to Image Analyst for the GetSkewAndKurtosis function

% Loops through each region/segment extracting pixel statistics and outputs
% the following:
% TextStats = 7 by n of statistical props of region pixel vals
% Each 'stat vector' can be called via 
% TextStats(1,:) = Mean 
% TextStats(2,:) = Entropy 
% TextStats(3,:) = stddev 
% TextStats(4,:)= skewness
% TextStats(5,:)= variance
% TextStats(6,:) = Kurtosis 
% the 7th value is the label value retained for export to csv for use in
% GIS

%Seg = a segmented image
%I = the image from which pixel stats are extracted

function TextStats = Ctextprops(Seg,I)

if(size(I,3)==3)
    I = rgb2gray(I);
end
   
structure= regionprops(Seg,I,'Image','BoundingBox', 'MeanIntensity',...
    'PixelValues');
n=length(structure);
TextStats = zeros(7,n);
TextStats(1,:) = [structure.MeanIntensity];
for k =1:n
    Mask = structure(k).Image;
    BBox = structure(k).BoundingBox;
    I2= I(round(structure(k).BoundingBox(2):structure(k).BoundingBox(2)+structure(k).BoundingBox(4)-1),...
    round(structure(k).BoundingBox(1):structure(k).BoundingBox(1)+structure(k).BoundingBox(3)-1));
    I2 = double(I2);
    I3 = I2.*double(Mask);
    outprops = entropy(double(I3)*100);
    [skew kurtosis meanGL varianceGL sd] = GetSkewAndKurtosis(I3);
    TextStats(2,k) = outprops';
    TextStats(3,k) = sd';
    TextStats(4,k) = skew';
    TextStats(5,k) = varianceGL';
    TextStats(6,k) = kurtosis;
    TextStats(7,k) = k;
end