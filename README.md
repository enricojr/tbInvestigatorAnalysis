This code is meant to analyze test beam data from the TJ Investigator chip, acquired using a reference telescope.

!!!!!!! WARNING !!!!!!! A few paths are hard-coded here and there. If things go wrong, the very first thing to check (and eventually modify) are the hard-coded paths.

####### 1 - FOLDER STRUCTURE

There are three main folders:
config/ -> where the Proteus configuration files for the CERN FEI-4 telescope are stored
output/ -> where all the output files are stored by default
scripts/ -> where the codes are developed

The script/ folder further contains:
aux/ -> contains auxiliary code
cfg/ -> where all the configuration files are stored by default
telescope/ -> contains the scripts used to run the alignment and the tracking with Proteus
DUT/ -> contains the scripts used to read the binary DUT files, fit the waveforms and visualize the results
sync/ -> contains the scripts used to synchronize the DUT data with the telescope data

####### 2 - RECONSTRUCTION OF TELESCOPE DATA

The reconstruction of the telescope data is done with Proteus: https://gitlab.cern.ch/unige-fei4tel/proteus
Proteus runs on a special data format. The Proteus input files are vreated from the telescope raw data files using a special version of tbConverter, which has been modified from https://github.com/schaed/tbConverter
The configuration files for the CERN FE-I4 telescope are stored in the config/ folder
In lxplus, before running the reconstruction one must setup the environment using setup_proteus.sh from the top folder.
The reconstruction is a two-steps process: alignment + tracking
The code to execute both steps is in scripts/telescope/.

------- ALIGNMENT
To align, check the number of the alignment run and type
source telescope-align.sh <runNumber>

------- TRACKING
To track, check the number of the data run AND of the corresponding alignment run, then type
source telescope-track.sh <runNumber-data> <runNumber-align>

####### 3 - RECONSTRUCTION OF DUT DATA

The DUT data are read-out directly from the raw DRS binary files, so no need to use tbConverter anymore.
The code to analyze the DUT data is in scripts/DUT/.
The main script is convertDUT.C, which makes use of DRSDataClass.hh, fitWaveform.hh and readRawDUT.h, plus the code in the scripts/aux/ folder.
The convertDUT script is a useful tool to run the reconstruction from command line, instead of using CINT.

------- WRITING THE CONFIGURATION FILE
The DUT analysis code (and later the synchronization code) takes input from a configuration file.
The configuration files are stored in the scripts/cfg/ folder.
The configuration file can be written by hand (strongly not suggested) or it can be created using the script makeConfigFile.C as follows:

root -l
[0] .L makeConfigFile.C++
[1] makeConfigFile(<runNumber>)

This creates a dummy configuration file called run_XXXXXX.cfg, where XXXXXX = <runNumber>
In general, the numbers set in the configuration file must be edited to match the run-specific values. For this, one may use the additional parameters of the makeConfigFile() function (see later in SUGGESTED PROCEDURE).

------- RUNNING THE RECONSTRUCTION
To reconstruct the DUT data, run

./convertDUT <runNumber> <nEvents> <eventStart>

When <nEvents> is set to 0, the reconstruction runs over the full dataset.
WARNING: if <nEvents> is different from zero, make sure it is smaller than the actual number of events contained in the raw data file (check the test beam excel logbook), otherwise the code will crash when reaching the last event (this can be fixed, but lazyness wins).
When <eventStart> is different from 0, the reconstruction will start from the specified event, the previous ones being skipped. This can be used for testing, but <eventStart> should always be 0 when doing the actual analysis, otherwise it will mess up with the synchronization.

------- VISUALIZING RESULTS
The results of the DUT data reconstruction are stored in a root file. Such file contains a tree with all the fit results, plus a few plots that are useful for checking. One can either open the file by hand, or run the following script:

root -l
[0] .L draw.C
[1] draw(<runNumber>)

------- SUGGESTED PROCEDURE
The following is the suggested procedure to run the full DUT data reconstruction:

1) Read the run numbers from the excel sheet (only Data runs):
https://docs.google.com/spreadsheets/d/1aqYTKp-7KoSPRIHlO4xX8QjAHAG4_QRSNxxN7YON6nc/edit#gid=0

2) Create a virgin config file, from the folder aux/ (for example, for run 207)
root -l
[0] .L makeConfigFile.C++
[1] makeConfigFile(207)

3) Run a preliminary reconstruction, from the folder DUT/ (for example, for run 207, suggested 10000 events, be careful, some runs have fewer events)
./convertDUT 207 10000 0

4) Take a look at the output histograms, from the folder DUT/ (e.g. run 207)
root -l
[0] .L draw.C
[1] draw(207)

5) Note down the T0 positions of each of the 4 channels
5A) if 200, 200, 200, 200, no need to regenerate the config file, since these are the default values
5B) else, regenerate config file, from the folder aux/ (for example, for run 207, for which the T0 positions are 650, 650, 650, 800)
root -l
[0] .L makeConfigFile.C++
[1] makeConfigFile(207, 650, 650, 650, 800)

6) Re-run the reconstruction on 10000 events (same as 3)) and take a look again at the output histograms (same as 4))
6A) if left plots are flat and right plots contain the signals, run the full reconstruction:
./convertDUT 207 0 0
6B) else must debug

####### 4 - SYNCHRONIZATION

####### 5 - EFFICIENCY AND OTHER FINAL RESULTS

To be implemented.