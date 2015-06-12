function varargout = OBIAmainGUI(varargin)
% OBIAMAINGUI MATLAB code for OBIAmainGUI.fig
%      OBIAMAINGUI, by itself, creates a new OBIAMAINGUI or raises the existing
%      singleton*.
%
%      H = OBIAMAINGUI returns the handle to a new OBIAMAINGUI or the handle to
%      the existing singleton*.
%
%      OBIAMAINGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in OBIAMAINGUI.M with the given input arguments.
%
%      OBIAMAINGUI('Property','Value',...) creates a new OBIAMAINGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before OBIAmainGUI_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to OBIAmainGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help OBIAmainGUI

% Last Modified by GUIDE v2.5 11-Dec-2014 19:26:18

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @OBIAmainGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @OBIAmainGUI_OutputFcn, ...
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


% --- Executes just before OBIAmainGUI is made visible.
function OBIAmainGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to OBIAmainGUI (see VARARGIN)

% Choose default command line output for OBIAmainGUI
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes OBIAmainGUI wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = OBIAmainGUI_OutputFcn(hObject, eventdata, handles) 
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
ImageStats





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
