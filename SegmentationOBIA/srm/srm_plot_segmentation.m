function srm_plot_segmentation(images,maps)

precision=numel(maps);
Iedge=zeros([size(images{1},1),size(images{1},2)]);

quick_I1 = cell(precision,1);
quick_I2 = cell(precision,1);

for k=1:precision
    map=reshape(maps{k},size(Iedge));
    quick_I1{k} = srm_randimseg(map) ;
    quick_I2{k} = images{k} ;
    figure(101);vl_tightsubplot(precision, k) ;
    imagesc(quick_I1{k});axis off;
    figure(102);vl_tightsubplot(precision, k) ;
    imagesc(uint8(quick_I2{k}));axis off;
    borders = srm_getborders(map);
    Iedge(borders) = Iedge(borders) + 1;
end
  

figure(103); 
Iedge=precision-Iedge;
imshow(Iedge,[0 precision]);