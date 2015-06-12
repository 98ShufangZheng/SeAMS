Semi Automated Mapping System (SeAMS)
==============================================================================================

Ciaran Robb, Neil Arnold, Ian Willis 


Scott Polar Research Institute, University of Cambridge, Lensfield Rd, Cambridge, UK, CB1 1ER.

Corresponding author: Ciaran Robb, cr485@cam.ac.uk


Abstract
--------

SeAMS (Semi-automated mapping system) is a series of Matlab-based tools and Graphical User Interfaces (GUIs) for classifying geomorphology using Object Based Image Analysis (OBIA). SeAMS is designed to be easy to use for those from a non-programming background through the use of GUIs. We provide a short example of how the tools may be used in a semi- automated geomorphological mapping work flow. Future development will include the introduction of training sample-based classification routines.

Keywords: segmentation, OBIA, geomorphology, SeAMS


Citation
--------

Robb. C, Arnold. N, Willis. I (2015). SeAMS: A Matlab-based semi-automated mapping system for geomorphology.
URL https://github.com/Ciaran1981/SeAMS


Installation
------------

Extract the folder from Github, save it to a desired location and add to the Matlab path.


Windows users: You will need to compile the .mex files for the mean shift and statistical region merging functions to use them in the 64 bit version of Matlab, as the windows version does not have a built in compiler. Please refer to the Matlab website for instructions. 


Dependencies
------------

Matlab image processing and statistics toolboxes.
The software is developed/tested in Linux environment with Matlab & gcc.
Some algorithms will require the mex files compiled for windows 64bit use (see above).

Sample data
-----------

The sample data folder contains the data used in the worked example in the paper, along with an example workflow script.
-Elevation_Percentile.asc
-Slope.asc
-Shade.asc
-ExampleWorkflow.m


Running the software
---------------------

The main menu may be accessed by typing SeAMS in the Matlab command prompt and pressing enter. The segmentation and statistics, visualisation and classification (SVC) GUIs may be accessed from drop down menus within the main menu bar. Alternatively, to access the segmentation GUI type Segmentation_GUI in the command prompt or SVC for the SVC GUI.

All code is commented and provides information regarding the use of each function.


License
-------

This program is released under GNU Lesser General Public License. 


Acknowledgements
----------------

Ciaran Robb acknowledges the financial assistance of the Cambridge Home and
European Scholarship Scheme and contributors to the Matlab file exchange,
from which inspiration and ideas were taken. We also thank Dr Tomas
Jóhannesson, Dr Finnur Pálsson and Professor Helgi Björnsson for providing
the lidar data used in the example work flow.

