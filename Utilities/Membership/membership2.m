

function [Seg2]  = membership2(Seg,Mem_Class_BW)
% Author: Ciaran Robb

% Membership function

% Acknowledgement: Peter Kovesi for his renumberregions function

% Outputs: a int32 labeled image from an arbitrarily labeled one
% Seg = segmented image labeled with integers
% Inputs
% Mem_Class_BW = either a boolean statement related to and image (e.g
% mean_image>10) or a binary image.

Seg2 = Seg;
Seg2(Mem_Class_BW == 0) = 0;
Seg2 = renumberregions(Seg2);





