% Drumlins and Moraines classification workflow carried out in the SeAMS
% paper

% Author (code): Ciaran Robb

% Robb. C, Arnold. N, Willis. I (2015). SeAMS: A Matlab-based semi-automated mapping system for geomorphology.
% URL https://github.com/Ciaran1981/SeAMS

% Change the file paths as appropriate to your system
% The input files are in SeAMS\Sample_Data

clearvars
%Elevation Percentile (relative elevation)
%We are looking for ridges as both these landforms are in essence ridges
[I,h] = ImportAsciiRaster();
% For background image to display results
[Shd,h] = ImportAsciiRaster();

%Segment it
[edge, segcontour, seg] = Cshed(double(I), 12);
%Get the stats
ImStats = Ctextprops(seg,I);
%Convert the stats to an image for clustering/thresholding
MnImage = colorscale(seg, ImStats(1,:));
%Cluster for ridges
[L,C,U,LUT,H]=FastFCMeans(uint8(MnImage),3);
%Mask out ridges
%a binary mask - but we must retain the original segment boundaries so we
%use the function renumber regions (Peter Kosevski)
Ridges  = membership2(seg,L==3);
clearvars -except *seg *I *edge *BW *Ridges *Shade *h


% Now we have a ridge class on which further analysis can be done
Props = regionprops(Ridges, 'Orientation');
MnImage = colorscale(Ridges,[Props.Orientation]+90); % to ensure pos integers
[L,C,U,LUT,H]=FastFCMeans(uint8(MnImage),3);
MorainesBW= L==3;
FlutesBW= L==2;
DrumlinCandidates  = membership2(seg,FlutesBW);
clearvars -except *seg *I *MorainesBW *Ridges *DrumlinCandidates *Shade *h

% We have moraines done....
%Just need to tidy up drumlins

Props = regionprops(DrumlinCandidates, 'Eccentricity');
MnImage =colorscale(DrumlinCandidates, [Props.Eccentricity]);
Drumlins = MnImage>0.92;
Drumlins = bwareaopen(Drumlins,4);
Drumlins_refined = activecontour(I, Drumlins, 5);
Moraine_refined = activecontour(I, MorainesBW, 5);

%clean up workspace....
clearvars -except *seg *I *Ridges...
    *DrumlinCandidates *Drumlins_refined *Moraine_refined *Shade *h


%MorainesBW = bwlabel(MorainesBW);
[B,L] = bwboundaries(Moraine_refined,'noholes');
[B1,L1] = bwboundaries(Drumlins_refined ,'noholes');
figure, imshow(uint8(Shd))
hold on

for k = 1:length(B)
    boundary = B{k};
    plot(boundary(:,2), boundary(:,1), 'g', 'LineWidth', 1)
end
for k = 1:length(B1)
    boundary = B1{k};
    plot(boundary(:,2), boundary(:,1), 'b', 'LineWidth', 1)
end
% Combining results
FinalMap = double(Moraine_refined);
FinalMap(Drumlins_refined ==1) = 2;
% Save to geo-reffed raster where the variable h is the .asc header
SaveAsciiRaster(FinalMap, h);

