#include "fit.hh"

int fit(const char *fileNameCfg = "/home/enricojr/cernbox/CMOS/testBeam/2017/calibration/config/fit.cfg",
	const string outputFolder = "/home/enricojr/cernbox/CMOS/testBeam/2017/calibration/plots"){
  
  /////////////////////////////
  // loading configuration file
  /////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": loading configuration file " << fileNameCfg << endl;
  fitClass *ffit = new fitClass();
  if(ffit -> loadConfig(fileNameCfg)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot load configuration file" << endl;
    return 1;
  }
  ffit -> print();
  
  //////////////////
  // fitting spectra
  //////////////////
  cout << __PRETTY_FUNCTION__ << ": fitting spectra" << endl;
  if(ffit -> fitSpectra(outputFolder)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot fit spectra" << endl;
    return 1;
  }

  //////////////////////////////
  // building calibration curves
  //////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": building calibration curves" << endl;
  if(ffit -> buildCalibrationCurves(outputFolder)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot build calibration curves" << endl;
    return 1;
  }

  /////////////////////
  // saving fit results
  /////////////////////
  cout << __PRETTY_FUNCTION__ << ": saving fit results" << endl;
  if(ffit -> saveResults(outputFolder)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot save fit results" << endl;
    return 1;
  }
  
  /////////
  // ending
  /////////
  cout << __PRETTY_FUNCTION__ << ": ending" << endl;
  delete ffit;
  return 0;
}
