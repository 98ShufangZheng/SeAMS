% Statistics, visualisation and classification
% Author Ciaran Robb
% Written with GUIDE, this GUI provides functions for the calculation and
% display and classification of segment statistics.

% Acknowledgements for material adapted form the Matlab file exchange:
% Anton Semechko for the adaptation of his fuzzy cmeans clustering
% function
% Laurent. S, for the adaptation of their kmeans++ function 
% Brett Shoelson for his uigetvariables function
% Yist Tauber for the manual_thresh function/gui
% Giuliano Langella for the Import and SaveAsciiRaster functions
% % Acknowledgement: Peter Kovesi for his draw region boundary function


% The code is organised into callbacks that are related to each other and
% are ordered as follows:
% PIXEL, REGION PROPERTYS AND THEIR DISPLAY IN THE GUI
% MENU HEADING CALLBACKS
% CLASSIFICATION CALLBACKS
% INPUT AND OUTPUT OF FILES
% EXPORT OF DATA
% MISCELLANEOUS

function varargout = ImageStats(varargin)
% IMAGESTATS MATLAB code for ImageStats.fig
%      IMAGESTATS, by itself, creates a new IMAGESTATS or raises the existing
%      singleton*.
%
%      H = IMAGESTATS returns the handle to a new IMAGESTATS or the handle to
%      the existing singleton*.
%
%      IMAGESTATS('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in IMAGESTATS.M with the given input arguments.
%
%      IMAGESTATS('Property','Value',...) creates a new IMAGESTATS or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before ImageStats_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to ImageStats_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help ImageStats

% Last Modified by GUIDE v2.5 08-Jun-2015 16:31:36

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @ImageStats_OpeningFcn, ...
                   'gui_OutputFcn',  @ImageStats_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before ImageStats is made visible.
function ImageStats_OpeningFcn(hObject, ~, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to ImageStats (see VARARGIN)

% Choose default command line output for ImageStats
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes ImageStats wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = ImageStats_OutputFcn(~, ~, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


%PIXEL AND REGION PROPERTY CALCULATION CALLBACKS
% --------------------------------------------------------------------
function Process_Callback(~,~, handles)
% Processing menu callback

% --------------------------------------------------------------------
function Process_Harlick_Texture_Callback(~,~, handles)
% This function is the callback for calculating Harlick texture features

global im seg CoStats waitbarHandle offset1 offset2;
x = inputdlg('Enter offsets:',...
             'GCLM offsets', [1 50]);
h = waitbar(0,'Calculating texture...');
kinput = str2num(x{:});
offset1 = kinput(:,1);
offset2 =kinput(:,2);

I = im;
if(size(im,3)==3)
    im = rgb2gray(im);
end
waitbar(0.5,h,'Calculating texture...');
CoStats = cotextureprops(seg,im, offset1, offset2);
waitbar(0.8,h,'Calculating texture...');
assignin('base','CoTextprops',CoStats);
waitbar(1,h,'Done!');
close(h);

% --------------------------------------------------------------------
function Process_ImStats_Callback(~, ~, ~)
% This function is the callback for calculating image statistics and region
% properties

global im seg Stats StatsR StatsG StatsB props ;
h = waitbar(0,'Calculating stats...');
I = im;
if(size(I,3)==3)
    R = im(:,:,1);
    G = im(:,:,2);
    B = im(:,:,3);
    [StatsR] = Ctextprops(seg,R);
    [StatsG] = Ctextprops(seg,G);
    [StatsB] = Ctextprops(seg,B);
    ClrStats= [StatsR(1,:); StatsG(1,:); StatsB(1,:)];
    assignin('base','ClrStats',ClrStats);
    clearvars ClrStats
end
waitbar(0.2,h,'Calculating stats...')
Stats = Ctextprops(seg,I);
waitbar(0.5,h,'Image stats done, calculating region props...')
props = regionprops(seg, 'Area', 'Eccentricity',...
'Extent', 'MajorAxisLength', 'MinorAxisLength', 'Orientation','Solidity');
waitbar(0.8,h,'Calculating stats...')
assignin('base','ImStats',Stats)
assignin('base','Props',props)
waitbar(1,h,'Done!');
close(h);

%REGION PROPERTY CALLBACKS---------------------------------------------

% --------------------------------------------------------------------
function Area_Callback(~,~, handles)
%The call back for displaying the area property
global  seg props  BW2 edge
[BW2] = colorscale(seg,[props.Area]);
BW2b = BW2;
BW2b(edge)=0; 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');


% --------------------------------------------------------------------
function Major_Axis_Callback(~,~, handles)
% The call back for displaying the major axis length property
global im seg props  BW2 edge
[BW2] = colorscale(seg,[props.MajorAxisLength]);
BW2b = BW2;
BW2b(edge==1)=0;
%axes(handles.axes1); 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');

% --------------------------------------------------------------------
function Minor_Axis_Callback(~,~, handles)
%The call back for displaying the minor axis length property
global im seg props  BW2 edge
[BW2] = colorscale(seg,[props.MinorAxisLength]);
BW2b = BW2;
BW2b(edge)=0;
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');


% --------------------------------------------------------------------
function Eccentricity_Callback(~,~, handles)
%The call back for displaying the eccentrcity property
global im seg props  BW2 edge
[BW2] = colorscale(seg,[props.Eccentricity]*100);
BW2b = BW2;
BW2b(edge)=0;
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');
%---------------------------------------------------------------------
% --------------------------------------------------------------------
function Orientation_Callback(~,~, handles)
% The call back to display orientation

global im seg props  BW2 edge
Orient = [props.Orientation];
[BW2] = colorscale(seg,Orient);
BW2b = BW2;
BW2b(edge)=0;
%axes(handles.axes1); 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');
Orient = [props.Orientation]+180;
[BW2] =colorscale(seg,Orient); 


% --------------------------------------------------------------------
function Extent_Callback(~,~, handles)
%The call back for displaying the extent property
global im seg props  BW2 edge
[BW2] = colorscale(seg,[props.Extent]*100);
BW2b = BW2;
BW2b(edge)=0;
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');


% --------------------------------------------------------------------
function Solidity_Callback(~,~, handles)
%The call back for displaying the solidity property
global im seg props  BW2 edge
[BW2] = colorscale(seg,[props.Solidity]*100);
BW2b = BW2;
BW2b(edge)=0;
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');


% --------------------------------------------------------------------
function Image_Stats_Callback(~,~, handles)
% hObject    handle to Image_Stats (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

%PIXEL PROPERTY CALLBACKS---------------------------------------------
% --------------------------------------------------------------------
function Mean_Callback(~,~, handles)
%The call back for displaying the mean pixel value property

global im seg Stats BW2 I1 StatsR StatsG StatsB edge 
if(size(im,3)==3)
    [BWR] = colorscale(seg,StatsR(1,:));
    [BWG] = colorscale(seg,StatsG(1,:));
    [BWB] = colorscale(seg,StatsB(1,:));
    BW2 = uint8(cat(3, BWR, BWG, BWB));
    BW2(BW2<0)=0;
    %axes(handles.axes1); 
    imshow(imoverlay(BW2, edge, [0 0 0]));
else    
[BW2] = colorscale(seg,Stats(1,:));
BW2(BW2<0)=0;
BW2b = BW2;
BW2b(edge)=0;
%axes(handles.axes1); 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...

    colorbar('location','eastoutside');
end


% --------------------------------------------------------------------
function Stdev_Callback(~,~, handles)
%The call back for displaying the stdev pixel value property

global im seg Stats StatsR BW2 edge
[BW2] = colorscale(seg,Stats(3,:)*100);
BW2b = BW2;
BW2b(edge)=0;
%axes(handles.axes1); 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');


% --------------------------------------------------------------------
function Skew_Callback(~,~, handles)
%The call back for displaying the pixel skewness property

global im seg Stats StatsR BW2 edge
[BW2] = colorscale(seg,Stats(4,:));
BW2b = BW2;
BW2b(edge)=0;
%axes(handles.axes1); 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');

% --------------------------------------------------------------------
function Variance_Callback(~,~, handles)
%The call back for displaying the pixel variance

global im seg Stats StatsR BW2 edge
[BW2] = colorscale(seg,Stats(5,:)*100);
BW2b = BW2;
BW2b = BW2;
BW2b(edge)=0;
%axes(handles.axes1); 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');

% --------------------------------------------------------------------
function Kurtosis_Callback(~,~, handles)
%The call back for displaying the pixel kurtosis
global im seg Stats StatsR BW2 edge
[BW2] = colorscale(seg,Stats(6,:));
BW2b = BW2;
BW2b = BW2;
BW2b(edge)=0;
%axes(handles.axes1); 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');


function Entropy_Callback(~,~, handles)
global  seg Stats BW2 edge
[BW2] = colorscale(seg,Stats(2,:)*100);
BW2b = BW2;
BW2b(edge)=0;
%axes(handles.axes1); 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');

% HARLICK TEXTURE DISPLAY CALLBACKS---------------------------------------
function Display_Texture_Harlick_Callback(~,~, handles)
% hObject    handle to Display_Texture_Harlick (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function Contrast_Callback(~,~, handles)
% hObject    handle to Contrast (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global im seg CoStats BW2 edge
[BW2] = colorscale(seg,CoStats(1,:)*10);
BW2b = BW2;
BW2b(edge)=0; 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...  
    colorbar('location','eastoutside');

% --------------------------------------------------------------------
function Correlation_Callback(~,~, handles)
% hObject    handle to Correlation (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global im seg CoStats BW2 edge
[BW2] = colorscale(seg,CoStats(2,:)*10);
BW2b = BW2;
BW2b(edge)=0; 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...  
    colorbar('location','eastoutside');

% --------------------------------------------------------------------
function Energy_Callback(~,~, handles)
% hObject    handle to Energy (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global im seg CoStats BW2 edge
[BW2] = colorscale(seg,CoStats(3,:)*100);
BW2b = BW2;
BW2b(edge)=0; 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...  
    colorbar('location','eastoutside');

% --------------------------------------------------------------------
function Homogeneity_Callback(~,~, handles)
% hObject    handle to Homogeneity (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global im seg CoStats BW2 edge
[BW2] = colorscale(seg,CoStats(4,:)*100);
BW2b = BW2; 
BW2b(edge)=0; 
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...  
    colorbar('location','eastoutside');



% --- Executes during object creation, after setting all properties.
function edit5_CreateFcn(~,~, handles)
% hObject    handle to edit5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



% --- Executes on button press in pushbutton26.
function pushbutton26_Callback(~,~, handles)
% hObject    handle to pushbutton26 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global BW2
[lowThreshold highThreshold lastThresholdedBand] = ...
        threshold(jetthresh(BW2), ...
        max(BW2(:)), BW2);
BW3 = BW2 >=lowThreshold & BW2 <=highThreshold;
assignin('base','BW',bw);


%MENU HEADING CALLBACKS-----------------------------------------------

% --------------------------------------------------------------------
function Classify_Callback(~,~, handles)
% hObject    handle to Classify (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% --------------------------------------------------------------------
function Untitled_18_Callback(~,~, handles)
% hObject    handle to Untitled_18 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function Reset_Callback(~,~, handles)
% hObject    handle to Reset (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function Export_Callback(~,~, handles)
% hObject    handle to Export (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function Save_Seg_Callback(~,~, handles)
% hObject    handle to Save_Seg (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


%CLASSIFICATION CALLBACKS--------------------------------------------

% --------------------------------------------------------------------
function Fuzzy_Cmeans_Callback(~,~, handles)
% hObject    handle to Fuzzy_Cmeans (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

global BW2  edge 
x = inputdlg('Enter number of clusters, fuzzy weighting:',...
             'Fuzzy Cmeans', [1 50]);
kinput = str2num(x{:});
k = kinput(:,1);
fz =kinput(:,2);
[L,C,U,LUT,H]=FastFCMeans(uint16(BW2),k,fz);
BW2=L;
BW2b=BW2;
BW2b(edge)=0;
imshow(BW2b,'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');
assignin('base','FzCmeans',L);
clearvars BW2b


% --------------------------------------------------------------------
function Cluster_Position_Callback(~,~, handles)
% hObject    handle to Cluster_Position (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global BW2  seg edge

x = inputdlg('Enter no of clusters:',...
             'Cluster Position', [1 50]);
clusters = str2num(x{:});

[Clustered]= ClusterCentroid(seg, clusters);
[BW2] = colorscale(seg,Clustered);
assignin('base', 'Posimage', BW2);
BW2b = BW2;
BW2b = BW2;
BW2b(edge)=0;
imshow(BW2b, 'DisplayRange',[]), colormap(jet),...
    colorbar('location','eastoutside');

% --------------------------------------------------------------------
function Kmeans_xx_Callback(~,~, handles)
% hObject    handle to Kmeans_xx (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function Multi_Thresholding_Callback(~,~, handles)
% hObject    handle to Multi_Thresholding (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global BW2 seg
[level,level2,bw] = manual_thresh(BW2,jet);
newseg=membership2(seg,bw);
x = inputdlg('Enter class name:',...
             'Re-label & export', [1 50]);
kinput=x{:};
assignin('base',kinput,newseg);


%INPUT AND OUTPUT CALLBACKS-------------------------------------------

% --------------------------------------------------------------------
function Image_Callback(~,~, handles)
% hObject    handle to Image (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function Seg_Callback(~,~, handles)
% hObject    handle to Seg (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function Seg_From_File_Callback(~,~, handles)
% Open a non geo-referenced segmentation image
global im seg 
[path,user_cance]=imgetfile();
if user_cance
    msgbox(sprintf('You will need a file!'));
    return
end
seg=imread(path);
%axes(handles.axes1);
I1 = uint8(im);
edge =drawregionboundaries(seg);
Max = max(I1(:));
Max = double(Max);
[X, map] = gray2ind(I1,Max);
rgb = ind2rgb(X, jet(Max));
imshow(imoverlay(rgb,edge, [1 1 1]));

% --------------------------------------------------------------------
function Seg_From_Workspace_Callback(~,~, handles)
% Open a segmentation raster from the workspace
global seg im edge
x = inputdlg('Enter variable name:',...
             'Image from workspace', [1 50]);
kinput=x{:};      
seg = evalin('base',kinput);
%axes(handles.axes1);
I1 = uint8(im);
edge =drawregionboundaries(seg);
if(size(I1,3)==3)
    rgb = I1;
   imshow(imoverlay(rgb,edge, [0 0 0]));
else 

Max = max(I1(:));
Max = double(Max);
[X, map] = gray2ind(I1,Max);
rgb = ind2rgb(X, jet(Max));
imshow(imoverlay(rgb,edge, [1 1 1]));
%axes(handles.axes1);
%imshow(imoverlay(I1,edge, [1 1 1]));
end

% --------------------------------------------------------------------
function IM_From_File_Callback(~,~, handles)
%Open a non-geo-reffed tif image

global im im2
[path,user_cance]=imgetfile();
if user_cance
     msgbox(sprintf('You will need a file!'));
    return
end
im=imread(path);
I = uint8(im);
%im=im2double(im); %converts to double
im=double(im); %for backup process :)
if(size(im,3)==3)
    imshow(I);
else
%axes(handles.axes1);
imshow(im,'DisplayRange',[]), colormap(jet);
end


% --------------------------------------------------------------------
function Im_From_Workspace_Callback(~,~, handles)
% Open image from workspace
global im 
% = uigetvariables({'Please select any variable','And another'});
im = double(im);
%axes(handles.axes1);
imshow(im,'DisplayRange',[]), colormap(jet);


% --------------------------------------------------------------------
function Seg_To_File_Callback(~,~, handles)
% hObject    handle to Seg_To_File (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% --------------------------------------------------------------------
function View_Callback(hObject, eventdata, handles)
% MENU ITEM


% --------------------------------------------------------------------
function Prop_Bck_Callback(hObject, eventdata, handles)
% Display a property overlayed on a background image
global BW2 seg im BckIm edge
rgb = label2rgb(uint16(BW2), 'jet', [1 1 1]);
 % figure with transparency
figure,imshow(BckIm), hold on
himage = imshow(rgb);
set(himage, 'AlphaData', 0.3);
clearvars rgb 
% --------------------------------------------------------------------
function Back_Im_file_Callback(hObject, eventdata, handles)
% Background menu item


% --------------------------------------------------------------------
function Bck_File_Callback(hObject, eventdata, handles)
%Open a non-geo reffed background image - eg hillshade
global BckIm im
[path,user_cance]=imgetfile();
if user_cance
    msgbox(sprintf('Error'),'Error','Error');
    return
end
BckIm=imread(path);
BckIm=uint8(BckIm);
% if(size(im,3)==3)
%     BckIm=rgb2gray(BckIm);
% end


% --------------------------------------------------------------------
function Bck_Wrk_Callback(hObject, eventdata, handles)
% Open a non-geo reffed background image - eg hillshade from the workspace
global BckIm  
BckIm = uigetvariables({'Image'});
BckIm=BckIM{1,1};
BckIm = uint8(BckIm);
% if(size(im,3)==3)
%     BckIm=rgb2gray(BckIm);
% end


% --------------------------------------------------------------------
function Open_asc_Callback(hObject, eventdata, handles)
% % Open a geo reffed image from file
global im h seg
[im, h] = ImportAsciiRaster;
imshow(im, 'DisplayRange', []), colormap('jet');


% --------------------------------------------------------------------
function seg_asc_open_Callback(hObject, eventdata, handles)
% % Open a geo reffed segmentation from file
global seg h I1 
[seg, h] = ImportAsciiRaster;
edge =drawregionboundaries(seg);
if(size(I1,3)==3)
    rgb = I1;
   imshow(imoverlay(rgb,edge, [0 0 0]));
else 

Max = max(I1(:));
Max = double(Max);
[X, map] = gray2ind(I1,Max);
rgb = ind2rgb(X, jet(Max));
imshow(imoverlay(rgb,edge, [1 1 1]));
%imshow(imoverlay(I1,edge, [0 0 0]));
end

% --------------------------------------------------------------------
function Save_asc_seg_Callback(hObject, eventdata, handles)
% save a segmentation to arcgrid format
global h seg
x = inputdlg('Enter variable name:',...
             'Save new class', [1 50]);
kinput=x{:};      
newseg = evalin('base',kinput);
SaveAsciiRaster(newseg, h);


% --------------------------------------------------------------------
function Workspace_import_Callback(hObject, eventdata, handles)
% Open both image and segmentation from the workspace
global seg im edge I1
vars = uigetvariables({'Image','Segmentation'});
im = vars{1,1};
seg = vars{1,2};
I1 = uint8(im);
edge =drawregionboundaries(seg);
if(size(I1,3)==3)
    rgb = I1;
   imshow(imoverlay(rgb,edge, [0 0 0]));
else 

Max = max(I1(:));
Max = double(Max);
[X, map] = gray2ind(I1,Max);
rgb = ind2rgb(X, jet(Max));
imshow(imoverlay(rgb,edge, [1 1 1]));
%imshow(imoverlay(I1,edge, [1 1 1]));
end
clearvars -except seg im edge I1 


% --------------------------------------------------------------------
function Workspace_menu_import_Callback(hObject, eventdata, handles)
% workspace import menu item


% --------------------------------------------------------------------
function Image_wrkspace_import_Callback(hObject, eventdata, handles)
% Import an image from the workspace
global seg im edge I1
vars = uigetvariables({'Image'});
im = vars{1,1};
I1 = uint8(im);
edge =drawregionboundaries(seg);
if(size(I1,3)==3)
    rgb = I1;
   imshow(rgb);
else 

Max = max(I1(:));
Max = double(Max);
[X, map] = gray2ind(I1,Max);
rgb = ind2rgb(X, jet(Max));
imshow(imoverlay(rgb,edge, [1 1 1]));
%mshow(im, 'DisplayRange',[]), colormap(jet);
end
clearvars -except seg im edge I1 

% --------------------------------------------------------------------
function Seg_wrkspace_import_Callback(hObject, eventdata, handles)
% import a segmentation from the workspace
global seg im edge I1
vars = uigetvariables({'Segmentation raster'});
seg= vars{1,1};
I1 = uint8(im);
edge =drawregionboundaries(seg);
if(size(I1,3)==3)
    rgb = I1;
   imshow(imoverlay(rgb,edge, [0 0 0]));
else 

Max = max(I1(:));
Max = double(Max);
[X, map] = gray2ind(I1,Max);
rgb = ind2rgb(X, jet(Max));
imshow(imoverlay(rgb,edge, [1 1 1]));
%imshow(imoverlay(I1,edge, [1 1 1]));
end
clearvars -except seg im edge I1 


% --------------------------------------------------------------------
function BckAsc_Callback(hObject, eventdata, handles)
% Open a background image in arcgrid format from fiel
global BckIm
[BckIm, h] = ImportAsciiRaster;
BckIm = uint8(BckIm);

% EXPORT PROPS/DATA CALLBACKS----------------------------------------------
function Save_Texture_Harlick_Callback(~,~, handles)
%Save to csv for use with GIS - eg join to a segmentation shapefile
global CoStats
Stats2=CoStats;
%Rounding and multiplication for visualisation in GIS
Stats2 = round(Stats2/0.01)*0.01;
Contrast = round(Stats2(1,:).*10);
Correlation = Stats2(2,:).*100;
Energy = Stats2(3,:).*100;
Homogeneity = Stats2(4,:).*100;
Label = Stats2(5,:).*100;
Table = table(Contrast, Correlation, Energy, Homogeneity, Label); 
[filename, pathname] = uiputfile('*.csv', 'Choose a file name');
outname = fullfile(pathname, filename);
writetable(Table, filename);
clearvars  Table Contrast Correlation Energy Homogeneity Label

function Save_Stats_2_XLS_Callback(~,~, handles)
% Save pixel stats and region props to csv for use with GIS
global Stats props
Stats2=Stats';
Stats2 = round(Stats2/0.01)*0.01;
Mean = Stats2(:,1);
Stdev = Stats2(:,2);
Skewness = Stats2(:,3);
Variance = Stats2(:,4);
Kurtosis = Stats2(:,5);
Entropy = Stats2(:,6);
Label = Stats2(:,7);
Area  = [props.Area]';
MajorAxisLength = [props.MajorAxisLength]';
MinorAxisLength  = [props.MinorAxisLength]';
Eccentricity = [props.Eccentricity]';
Orientation = [props.Orientation]';
Solidity = [props.Solidity]';
Extent =[props.Extent]';
Table = table(Mean, Stdev, Skewness, Variance, Kurtosis, Entropy, Label,...
    MajorAxisLength, MinorAxisLength, Eccentricity, Solidity, Extent);
[filename, pathname] = uiputfile('*.csv', 'Choose a file name');
outname = fullfile(pathname, filename);
writetable(Table, filename);
clearvars Table Mean Stdev Skewness Variance Kurtosis Entropy Label...
    MajorAxisLength MinorAxisLength Eccentricity Solidity Extent


%   Miscellaneous----------------------------------------------------------
function Display_Callback(~,~, handles)
% Display menu item


% --------------------------------------------------------------------
function Export_Cluster_Callback(~,~, handles)
% Export clustering results and re-label to the workspace for further analysis
global BW2 seg
x = inputdlg('Enter cluster number:',...
             'Choose cluster & re-label', [1 50]);
kinput = str2num(x{:});
h = waitbar(0,'Re-labeling...');
newseg=membership2(seg,BW2==kinput);
waitbar(1,h,'Re-labeling done');
close(h);
%seg = newseg;
x = inputdlg('Enter class name:',...
             'Choose cluster & re-label', [1 50]);
kinput=x{:};
assignin('base',kinput,newseg);


% --------------------------------------------------------------------
function Colour_Thresholder_Callback(~,~, handles)
% Opens the matlab color thresholder with the currently displayed image
global BW2
colorThresholder(BW2);

% --------------------------------------------------------------------



