# Lidar_CBME80_IO
## Cloud Lidar model CBM08 Data logger, Matlab input and output interface

A set of C/C++ programs to work with the Cloud Lidar (ceilometer) model CBME80 by the Sweeden Manufacturer _Björn Eliasson Ingenjörfirma AB_ ([web site](http://www.eliasson.com)).

The CBME80 is a compact light weight system Lidar operating at 905.2 nm,  50W and 2.8mrad FOV. These codes have been used for the system mounted at the scanning pedestal of the radiometer ADMIRARI ([web site](http://www2.meteo.uni-bonn.de/admirari)), therefore the CBME80 was mainly measuring at an unconvontional slant angles -mainly 30°- or scanning as well.
Unfortunately the CBME80 ceilometer does not report a backscattering factor but only a normalized factor (from 0x000 to 0xFFF hexadecimal) 

Two main programs belong to his repository:
* **Data Logger**: ```CBM80logger.cpp```

Program under __Linux__ to log the data from the cloud Lidar CBME80 telegram data streamflow.
The logger open the COM port for Modem and get the telegram sending by the instrument continuously. Then add a time-stamp based on the PC internal clock and storage in a ASCII file. By default the program uses the COM PORT 2 (under my linux that is /dev/ttyS4, but might be different in other machines), then it saves the telegram using a file named under the following format: ```./data/CEI/CEIyyyymmdd.dat``` where __yyyy__ is the year, __mm__ the month and __dd__ the day. In case those default values wanted to be changed, the source code needs to be edited.

     > g++ CBM80logger.cpp -o CBM80logger

* **MATLAB interface**: ```CEIdataRead.cpp``` and ```CEIdataRead.mexa64```

Source code for an MATLAB interface as MEX file under __Linux__  to read the ceilometer CBME80 data under the format recorded by the data logger ```CBM80logger```. The MEX compiled file is run at MATLAB's workspace directly and a file browser will be open to select the data file to open. Optionaly the data file can be directly indicated as argument for the MEX file in string format. The data file does not include the observation angle at which the measurements were done.

    >> % compiling within MATLAB, output mex file is CEIdataRead.mexa64 under Linux
    >> mex CEIdataRead.cpp
    >> % reading the CBM80 data file named 'CEI20120509.dat'
    >> data = CEIdataRead('./data/CEI/CEI20120509.dat');
    
the output variable ```data``` is a structure with the following fields:
    
    >> data.Time_hr   % 1D vector: with time [UTC hours] seriar at which the profile has beed acquired
    >> data.Ran       % 1D vector: with range [m] of the profile
    >> data.BSp       % 2D matrix {time x range}: uncalibrated [unitless] backscattering factor
    >> data.CB1       % 1D vector: first cloud base altitude [m]
    >> data.CB2       % 1D vector: second cloud base altitude [m]
    >> data.CD1       % 1D vector: first cloud thickness [m]
    >> data.CD2       % 1D vector: second cloud thickness [m]

Information about the application of the CBME80 to atmospheric studies and the mapping the backscattering factor from dimentionless to backscattering coefficient at sr^{-1}km^{-1} based on an intercalibration with a VAISALA CTK25 ceilometer and more can be found at the following reference [1]:

[1] *"Retrieval of cloud and rainwater from ground-based passive microwave observations with the multi-frequency dual-polarized  radiometer ADMIRARI"*, 2014, Dissertation by Pablo A. Saavedra Garfias, [Chapter 3, Figure 3.18](http://hss.ulb.uni-bonn.de/2015/3941/3941.htm).

More Information: pablosaa@uni-bonn.de

Author: Pablo Saavedra Garfias, 
(c) 2010
