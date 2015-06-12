function [seg edge] =srm2(im, Mergelevel)

% Mergelevel = Mergelevel*10;
I1 = uint8(im);
[maps,images]=srm(im,Mergelevel);
Iedge=zeros([size(images{1},1),size(images{1},2)]);
map=reshape(maps{1},size(Iedge));
quick_I2{1} = images{1} ;
precision=numel(maps);
quick_I1 = cell(precision,1);
newseg = cell2mat(quick_I2);
newseg = newseg(:,:,1);
seg = newseg;
seg = renumberregions(seg);
edge =drawregionboundaries(seg);
