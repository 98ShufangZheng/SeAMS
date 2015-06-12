function Histandcolorbar(BW2,jet,Props)

% BW2b = BW2;
% BW2b(edge==1)=0;
imshow(BW2, 'DisplayRange',[]), colormap(jet)



% bottom - color bar
cbar=colorbar('location','southoutside');
set(cbar,'unit','norm','Position',[0.05 0.05 0.9 0.05]);
set(cbar,'xlim',[0 color_range(2)],'xtick',[0 color_range(2)])

%next to bottom - intensity distribution
h_hist = axes('unit','norm','pos',[0.05 0.1 0.9 0.1]);
n = hist(ImStats(1,:),bin_ctrs);
bar(bin_ctrs,n)

axis([[0 color_range(2)+1] limits(n(2:end-1))]) %ignore saturated end scaling
set(h_hist,'xtick',[],'ytick',[])
title('Intensity Distribution')