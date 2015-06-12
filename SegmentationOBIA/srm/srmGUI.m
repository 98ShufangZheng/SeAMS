function varargout = srmGUI(varargin)
% SRMGUI MATLAB code for srmGUI.fig
%      SRMGUI, by itself, creates a new SRMGUI or raises the existing
%      singleton*.
%
%      H = SRMGUI returns the handle to a new SRMGUI or the handle to
%      the existing singleton*.
%
%      SRMGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SRMGUI.M with the given input arguments.
%
%      SRMGUI('Property','Value',...) creates a new SRMGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before srmGUI_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to srmGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help srmGUI

% Last Modified by GUIDE v2.5 28-Feb-2014 11:36:54

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @srmGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @srmGUI_OutputFcn, ...
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


% --- Executes just before srmGUI is made visible.
function srmGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to srmGUI (see VARARGIN)

% Choose default command line output for srmGUI
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes srmGUI wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = srmGUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --------------------------------------------------------------------
function Untitled_1_Callback(hObject, eventdata, handles)
% hObject    handle to Untitled_1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --------------------------------------------------------------------
function Untitled_2_Callback(hObject, eventdata, handles)
% hObject    handle to Untitled_2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global im im2 I1 
[path,user_cance]=imgetfile();
if user_cance
    msgbox(sprintf('Error'),'Error','Error');
    return
end
im=imread(path);
I1=im; %for backup process :)
axes(handles.axes1);
if(size(I,3)==3)
    imshow(uint8(I));
else
imshow(I1, 'DisplayRange',[]), colormap(jet);
end

% --------------------------------------------------------------------
function Untitled_4_Callback(hObject, eventdata, handles)
% hObject    handle to Untitled_4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global seg im 
im = evalin('base','MnImage');
I1 = uint8(im);
im = double(im);
axes(handles.axes1);
imshow(I1);

% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
 global seg2 im seg MergeLevel waitbarHandle;
 waitbarHandle = waitbar(0,'Merging...');
	set(waitbarHandle,...
		'Name','Merging');
I1 = uint8(im);
[maps,images]=srm(I1,MergeLevel);
Iedge=zeros([size(images{1},1),size(images{1},2)]);
map=reshape(maps{1},size(Iedge));
quick_I2{1} = images{1} ;
precision=numel(maps);
quick_I1 = cell(precision,1);
newseg = cell2mat(quick_I2);
newseg = newseg(:,:,1);
seg = newseg;
edge =drawregionboundaries(seg);
close(waitbarHandle);
imshow(imoverlay( I1, edge, [1 0 0]));

% --- Executes on slider movement.
function slider2_Callback(hObject, eventdata, handles)
% hObject    handle to slider2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider
global g_Floating;
    global g_MaxThreshold;
    global g_MinThreshold;
    global g_LastThresholdedColorBand;
    g_LastThresholdedColorBand = get(handles.listbox_DisplayColorBand,'Value') - 1;

    % Get the value of the sliders
    minSliderValue = get(hObject,'Value');
	% Round it to an integer if the image is a uint (not a floating).
	if g_Floating == 0 
	    minSliderValue = round(minSliderValue);
	end
	
    %disp(['Moved min slider.  New value = ' num2str(minSliderValue)]);

    % Set the global variable.
    if (minSliderValue <= g_MaxThreshold)
        g_MinThreshold = minSliderValue;
        % Update the label.
        set(handles.edit_Min, 'string', num2str(g_MinThreshold));
    end
    % Call guidata anytime you use the set() command or directly access
    % a variable you added to the handles structure.  Generally it's a good
    % practice to just automatically add this at the end of every function.
    guidata(hObject, handles);

    % Apply the threshold to the display so we can see its effect.
    ShowThresholdedBinaryImage(hObject, eventdata, handles);

% --- Executes during object creation, after setting all properties.
function slider2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end



function edit1_Callback(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit1 as text
%        str2double(get(hObject,'String')) returns contents of edit1 as a double
global MergeLevel
    MergeLevel= str2double(get(hObject,'String'));
    % Call guidata anytime you use the set() command or directly access
    % a variable you added to the handles structure.  Generally it's a good
    % practice to just automatically add this at the end of every function.
   
% --- Executes during object creation, after setting all properties.
function edit1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes during object creation, after setting all properties.


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
global im seg
[path,user_cance]=imgetfile();
if user_cance
    msgbox(sprintf('Error'),'Error','Error');
    return
end
seg=imread(path);
axes(handles.axes1);
I1 = uint8(im);
edge =drawregionboundaries(seg);
imshow(imoverlay( I1,edge, [1 0 0]));

% --------------------------------------------------------------------
function Untitled_7_Callback(hObject, eventdata, handles)
% hObject    handle to Untitled_7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
global seg im
seg = evalin('base','seg');
axes(handles.axes1);
I1 = uint8(im);
edge =drawregionboundaries(seg);
imshow(imoverlay( I1,edge, [1 0 0]));
