#include "fit.hh"

int fit(const char *fileNameCfg){
  
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
  if(ffit -> fitSpectra()){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot fit spectra" << endl;
    return 1;
  }

  /////////
  // ending
  /////////
  cout << __PRETTY_FUNCTION__ << ": ending" << endl;
  delete ffit;
  return 0;
}
