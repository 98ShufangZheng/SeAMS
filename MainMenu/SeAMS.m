function varargout = SeAMS(varargin)
% SEAMS MATLAB code for SeAMS.fig
%      SEAMS, by itself, creates a new SEAMS or raises the existing
%      singleton*.
%
%      H = SEAMS returns the handle to a new SEAMS or the handle to
%      the existing singleton*.
%
%      SEAMS('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SEAMS.M with the given input arguments.
%
%      SEAMS('Property','Value',...) creates a new SEAMS or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before SeAMS_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to SeAMS_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help SeAMS

% Last Modified by GUIDE v2.5 12-Jun-2015 14:41:45

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @SeAMS_OpeningFcn, ...
                   'gui_OutputFcn',  @SeAMS_OutputFcn, ...
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


% --- Executes just before SeAMS is made visible.
function SeAMS_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to SeAMS (see VARARGIN)

% Choose default command line output for SeAMS
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes SeAMS wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = SeAMS_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --------------------------------------------------------------------
function Segmentation_Callback(hObject, eventdata, handles)
% hObject    handle to Segmentation (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
SegmentationGUI

% --------------------------------------------------------------------
function Untitled_5_Callback(hObject, eventdata, handles)
% hObject    handle to Untitled_5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function Untitled_6_Callback(hObject, eventdata, handles)
% hObject    handle to Untitled_6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
SVC





% --------------------------------------------------------------------
function Workspace_Callback(hObject, eventdata, handles)
% hObject    handle to Workspace (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function External1_Callback(hObject, eventdata, handles)
% hObject    handle to External1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global im 
[path,user_cance]=imgetfile();
if user_cance
    msgbox(sprintf('Error'),'Error','Error');
    return
end
im = double(imread(path));
%axes(handles.axes1);
I1 = uint8(im);
figure, imshow(I1,'DisplayRange',[]), colormap(jet);
assignin('base','I', im);

% --------------------------------------------------------------------
function Workspace1_Callback(hObject, eventdata, handles)
% hObject    handle to Workspace1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function Thresholding_Clustering_Callback(hObject, eventdata, handles)
% hObject    handle to Thresholding_Clustering (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function MultiThresh_Callback(hObject, eventdata, handles)
% hObject    handle to MultiThresh (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global im
im(im<0)=0;
[level,level2,bw] = manual_thresh(im,jet);
assignin('base','BW',bw);
