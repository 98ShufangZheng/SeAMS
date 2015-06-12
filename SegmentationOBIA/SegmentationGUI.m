% Segmentation GUI
% Authour: Ciaran Robb
% Written with guide, this GUI provides three means of segmentating an
% image and visualising the results
% Acknowledgements for adaptation of file exchange/publically availbale: 
% Sean Lankton & Shai Bagon for the wrapping of EDISON C++ code
% Sylvan Boltz for the SRM function
% Brett Shoelson for the uigetvariables function
% Giuliano Langella for the Import and SaveAsciiRaster functions


% Refs
% Comaniciu, D., Meer, P., 2002. Mean shift: A robust approach toward feature
% space analysis. Pattern Analysis and Machine Intelligence, IEEE Transac-
% -tions on 24 (5), 603–619.
% Meer, P., Georgescu, B., 2001. Edge detection with embedded confidence.
% IEEE Transactions on Pattern Analysis and Machine Intelligence 23 (12),
% 1351–1365.
% Nock, R., Nielsen, F., 2004. Statistical region merging. Pattern Analysis and
% Machine Intelligence, IEEE Transactions on 26 (11), 1452–1458.
% Meyer, F., Jul. 1994. Topographic distance and watershed lines. Signal
% Processing 38 (1), 113–125.

% Code is ordered as follows:

% INPUT/OUTPUT CALLBACKS
% SEGMENTATION CALLBACKS
% MENU HEADING CALLBACKS
% SEGMENTATION RESULTS VIEW CALLBACKS


function varargout = SegmentationGUI(varargin)
% SEGMENTATIONGUI MATLAB code for SegmentationGUI.fig
%      SEGMENTATIONGUI, by itself, creates a new SEGMENTATIONGUI or raises the existing
%      singleton*.
%
%      H = SEGMENTATIONGUI returns the handle to a new SEGMENTATIONGUI or the handle to
%      the existing singleton*.
%
%      SEGMENTATIONGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SEGMENTATIONGUI.M with the given input arguments.
%
%      SEGMENTATIONGUI('Property','Value',...) creates a new SEGMENTATIONGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before SegmentationGUI_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to SegmentationGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help SegmentationGUI

% Last Modified by GUIDE v2.5 25-Jan-2015 17:56:53

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @SegmentationGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @SegmentationGUI_OutputFcn, ...
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


% --- Executes just before SegmentationGUI is made visible.
function SegmentationGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to SegmentationGUI (see VARARGIN)

% Choose default command line output for SegmentationGUI
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes SegmentationGUI wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = SegmentationGUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;

% INPUT/OUTPUT CALLBACKS--------------------------------------------------
% --------------------------------------------------------------------
% --------------------------------------------------------------------
% --------------------------------------------------------------------
function Open_asc_im_Callback(hObject, eventdata, handles)
% Open an arcgrid image
global im h I1
[im, h] = ImportAsciiRaster(0);
I1=uint8(im);
if(size(im,3)==3)
    imshow(I1);
else 
axes(handles.axes1);
imshow(im, 'DisplayRange',[]), colormap(jet);
end

function save_asc_ms_Callback(hObject, eventdata, handles)
% Save mena shift image to arcgrid format
global h ms
SaveAsciiRaster(ms, h);

% --------------------------------------------------------------------
function Save_asc_seg_Callback(hObject, eventdata, handles)
% Save segmentation to arcgrid format
global h seg
SaveAsciiRaster(seg, h);



function MS_save_to_file_Callback(hObject, eventdata, handles)
% Save mean-shift image to non geo reffed tif
global ms
[FileName, PathName] = uiputfile('*.tif', 'Save As'); %# <-- dot
Name = fullfile(PathName,FileName);  %# <--- reverse the order of arguments
imwrite(ms, Name, 'tif');

% --------------------------------------------------------------------
function Workspace_Save_Callback(hObject, eventdata, handles)
% save mean shift and segmentation to workspace
global ms  seg 
assignin('base','seg',seg)
assignin('base','ms',ms)
% --------------------------------------------------------------------
function Save_to_file_Callback(hObject, eventdata, handles)
% Save segmentation image to non geo reffed tif
global seg
seg1 = uint16(seg);
[FileName, PathName] = uiputfile('*.tif', 'Save As'); %# <-- dot
Name = fullfile(PathName,FileName);  %# <--- reverse the order of arguments
imwrite(seg1, Name, 'tif');

% --------------------------------------------------------------------
function FromFile_Callback(hObject, eventdata, handles)
% Open image from tif format (no geo ref)
global im  I1 
[path,user_cance]=imgetfile();
if user_cance
    msgbox(sprintf('Error'),'Error','Error');
    return
end
im=double(imread(path));
I1=uint8(im);
%im2=im; %for backup process 
axes(handles.axes1);
imshow(I1);

% --------------------------------------------------------------------
function WorkSpace_menu_Callback(hObject, eventdata, handles)
% Open image from workspace
global seg im I1
im = uigetvariables({'Image'});
im=im{1,1};
im = double(im);
axes(handles.axes1);
if(size(im,3)==3)
    imshow(im)    
else 
imshow(im,'DisplayRange',[]), colormap(jet);
end
clearvars -except seg im edge I1 h
% --------------------------------------------------------------------

% SEGMENTATION CALLBACKS----------------------------------------------
% --------------------------------------------------------------------
function Watershed_Menu_Callback(hObject, eventdata, handles)
% watershed segmentation
global im seg edge I1
x = inputdlg('Enter pixel gradient threshold:',...
             'Watershed Segmentation',[1 50]);
h = waitbar(0.2,'Segmenting...');
kinput = str2num(x{:});
[edge, segcontour, seg] = Cshed(im, kinput);
assignin('base','seg',seg)
waitbar(0.5, h, 'Segmenting...');
axes(handles.axes1);
edge=drawregionboundaries(seg);
assignin('base','edge',edge)
if(size(I1,3)==3)
    rgb = I1;
    imshow(imoverlay(rgb,edge, [0 0 0]));
else 
waitbar(0.8, h, 'Segmenting...');
Max = max(I1(:));
Max = double(Max);
[X, map] = gray2ind(I1,Max);
rgb = ind2rgb(X, jet(Max));
waitbar(1, h, 'Done!');
imshow(imoverlay(rgb,edge, [1 1 1]));
close(h)
clearvars -except seg im edge I1 h
end

% --------------------------------------------------------------------
function MeanShift_Menu_Callback(hObject, eventdata, handles)
% Mean shift segmentation
global im seg ms I1 edge
x = inputdlg('Enter range,radius,minimum segment size:',...
             'Mean Shift Segmentation',[1 50]);
h = waitbar(0.2,'Segmenting...');
kinput = str2num(x{:});
radius = kinput(:,1);
range =kinput(:,2);
MinSize =kinput(:,3);
waitbar(0.5,'Segmenting...');
[ms seg grad conf] = msseg(im, radius, range, MinSize);
clearvars im2
ms=ms*100;
waitbar(0.7,'Segmenting...');
assignin('base','seg',seg)
assignin('base','ms',ms)
axes(handles.axes1);
edge =drawregionboundaries(seg);
if(size(I1,3)==3)
   imshow(imoverlay(I1,edge, [0 0 0]));
else    
edge = drawregionboundaries(seg);    
Max = max(I1(:));
Max = double(Max);
[X, map] = gray2ind(I1,Max);
rgb = ind2rgb(X, jet(Max));
imshow(imoverlay(rgb,edge, [1 1 1]));
clearvars -except ms seg edge im I1 h
waitbar(1,'Done!');
end
close(h);

% --------------------------------------------------------------------
function SRM_Callback(hObject, eventdata, handles)
% Statistical region merging callbacks
global seg im edge I1 mn
x = inputdlg('Enter merging threshold (Approx number of segments):',...
             'Statistical Region Merging',[1 50]);
h = waitbar(0.2,'Segmenting...');
kinput = str2num(x{:});
[seg edge] = srm2(im, kinput);
waitbar(0.5,h,'Segmenting...');
assignin('base','seg',seg)
axes(handles.axes1);
edge=drawregionboundaries(seg);
waitbar(0.8,h,'Segmenting...');
assignin('base','edge',edge)
if(size(I1,3)==3)
    rgb = I1;
    imshow(imoverlay(rgb,edge, [0 0 0]));
else 
edge = drawregionboundaries(seg);    
Max = max(I1(:));
Max = double(Max);
[X, map] = gray2ind(I1,Max);
rgb = ind2rgb(X, jet(Max));
imshow(imoverlay(rgb,edge, [1 1 1]));
clearvars -except seg im edge I1 h
waitbar(1,h,'Done!');
end
close(h);
% -MENU HEADING CALLBACKS----------------------------------------------
% ----------------------------------------------------------------------
function Open_menu_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function Save_menu_Callback(hObject, eventdata, handles)
% --------------------------------------------------------------------
function Save_MS_Callback(hObject, eventdata, handles)
% --------------------------------------------------------------------
function MS_save_Workspace_Callback(hObject, eventdata, handles)
% --------------------------------------------------------------------
function File_menu_Callback(hObject, eventdata, handles)
% --------------------------------------------------------------------
function View_Callback(hObject, eventdata, handles)
% --------------------------------------------------------------------
function SegmentationMenu_Callback(hObject, eventdata, handles)

% SEGMENTATION RESULTS VIEW CALLBACKS--------------------------------
%-------------------------------------------------------------------------
function Seg_Outlines_Callback(hObject, eventdata, handles)
% 
global I1 edge
if(size(I1,3)==3)
    rgb = I1;
   imshow(imoverlay(rgb,edge, [0 0 0]));
else    
assignin('base','edge',edge)
Max = max(I1(:));
Max = double(Max);
[X, map] = gray2ind(I1,Max);
rgb = ind2rgb(X, jet(Max));
imshow(imoverlay(rgb,edge, [0 0 0]));
clearvars -except seg im edge I1 h
end
% --------------------------------------------------------------------
function Mean_Image_view_Callback(hObject, eventdata, handles)
% 
global ms
if(size(ms,3)==3)
   ms =uint8(ms);
   imshow(ms);
else    
imshow(ms, 'DisplayRange',[])
end

% --------------------------------------------------------------------
function Outlines_Mean_Image_Callback(hObject, eventdata, handles)
% Displays outlines and mean image - only works with mean shift
global ms seg
if(size(ms,3)==3)
    
   imshow(imoverlay(rgb,ms, [0 0 0]));
else    
Max = max(ms(:));

imshow(drawregionboundaries(seg,uint8(ms)));
clearvars -except seg im edge I1 h
end






