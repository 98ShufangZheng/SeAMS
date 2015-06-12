%Graycoprops loop
%Author: Ciaran Robb
%This function allows texture propeties based on relationship 'in space'
%rather than simply the DN vals

%Takes the binary mask from regionprops('Image') and calls masked pixel
%values from an image
%struct = regionprops.Image (Boundingbox of object with mask pixels==1
%see regionprops)
%Seg -labeled blobs/segmented image
%I = Image to be masked

function Stats = cotextureprops(Seg,I,offset1, offset2)

structure= regionprops(Seg,I,'Image','BoundingBox');
n=length(structure);
Stats = zeros(5,n);

for k =1:n
    Mask = structure(k).Image;
    BBox = structure(k).BoundingBox;
    I2= I(round(structure(k).BoundingBox(2):structure(k).BoundingBox(2)+structure(k).BoundingBox(4)-1),...
    round(structure(k).BoundingBox(1):structure(k).BoundingBox(1)+structure(k).BoundingBox(3)-1));
    I2 = double(I2);
    I3 = I2.*double(Mask);
    glcm = graycomatrix(I3, 'offset', [offset1 offset2],'Symmetric', true);
    outprops = graycoprops(glcm, 'all');     
    Stats(1,k) = outprops.Contrast;
    Stats(2,k) = outprops.Correlation;
    Stats(3,k) = outprops.Energy;
    Stats(4,k) = outprops.Homogeneity;
    Stats(5,k) = k;
end


    