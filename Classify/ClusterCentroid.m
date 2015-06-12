

function[Clustered]= ClusterCentroid(seg, cluster)

% Author: Ciaran Robb
% calculate centroid and separate x & y into 2 vectors then cluster with
% kmeans++
% Inputs:
% seg = a segmented image
% cluster = the number of clusters
% Outputs:
% Clustered = an image where each labeled region is a cluster
structure= regionprops(seg,'Centroid');
n=length(structure);
Stats = zeros(2,n);
 %This can probably be vectorised for speed elimination the need for the
 %for loop
for k =1:n
    Centroid =structure(k,1).Centroid;
    Stats(1,k) = Centroid(1,1);
    Stats(2,k) = Centroid(1,2);
end

Clustered = kmeansxx(Stats, cluster);
