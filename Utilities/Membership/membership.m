%Membership function
%Outputs a binary image & int32 label image

%Seg = segmented image labeled with integers
%label_Vector = vector with segment properties(eg cluster val, slope val)
%statement = boolean rule e.g KMEAN ==3 or >3 or whatevs

function [Seg2]  = membership(Seg,statement)

Seg2 = Seg;

Allow = statement;
Kp = find(Allow);%keep
Mem_Class_BW = ismember(Seg, Kp);
Seg2(BW == 0) = 0;
Seg2 = renumberregions(Seg2);
%LMP2 = bwlabel(Mem_lumps2);
RGB = label2rgb(Seg2, 'hsv',[1 1 1],'shuffle');
figure, imshow(RGB), hold on % figure with transparency




