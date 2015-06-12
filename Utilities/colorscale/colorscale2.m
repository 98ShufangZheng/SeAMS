%Allows visualisation of labeled object properties (such as a kmean
%value or slope value)via a colormap
%
%output results are superimposed on a background image such as a dem

function colorscale2(Scale,Seg,hillshd)
%Scale = array with each object value eg mean slope or kmean value
%Props = the image objects - ie regions labeled with an integer
%Max = the maximum value of image object
%hillshd = a hillshade grid to superimpose results on

%Neils idea - what is colorvalue??
%PCAseg  
%for k=1:(size(Seg,1))
     %
 %    L(Seg(1).PixelList(:,1),Seg(1).PixelList(:,2))=idx(k);
     %
%end

mal = transpose(Scale);
L = zeros(size(Seg)); %preallocate
for k=1:(size(Seg,1))
     %
     L(Seg(1).PixelList(:,1),Seg(1).PixelList(:,2))=mal(k);
     %
end
cmap = hsv(max(mal);   %a colormap(hsv works better than jet)
Lrgb = label2rgb(L,cmap); %build rgb image
figure, imshow(hillshd), hold on
himage = imshow(Lrgb); 
set(himage, 'AlphaData', 0.2);
title('Stats Property ranked by colormap')
figure, imshow(Lrgb);