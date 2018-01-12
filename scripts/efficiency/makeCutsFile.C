#include <iostream>
#include <fstream>
using namespace std;

#include "../aux/configClass.hh"

int makeCutsFile(const unsigned int run,
		 const bool lxplus = true){

  /////////////////////
  // setting file names
  /////////////////////
  cout << __PRETTY_FUNCTION__ << ": setting file names" << endl;
  char fileNameCuts[1000];
  if(lxplus) sprintf(fileNameCuts, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/Cuts/cuts_%06d.txt", run);
  else sprintf(fileNameCuts, "../../../output/cuts_%06d.txt", run);
  cout << __PRETTY_FUNCTION__ << ": fileNameCuts = " << fileNameCuts << endl;
  char fileNameCfg[1000];
  if(lxplus) sprintf(fileNameCfg, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/Run_Configs/run_%06d.cfg", run);
  else sprintf(fileNameCfg, "../../../cfg/run_%06d.cfg", run);
  cout << __PRETTY_FUNCTION__ << ": fileNameCfg = " << fileNameCfg << endl;

  /////////////////////////////
  // reading configuration file
  /////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": reading configuration file " << fileNameCfg << endl;
  configClass *cfg = new configClass();
  if(cfg -> load(fileNameCfg)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot load configuration" << endl;
    return 1;
  }
  cfg -> print();

  ////////////////////
  // writing cuts file
  ////////////////////
  cout << __PRETTY_FUNCTION__ << ": writing cuts file" << endl;
  ofstream file(fileNameCuts);
  if(!file){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameCuts << endl;
    return 1;
  }
  file << "# T0 cut" << endl;
  file << cfg -> _pulseT0Range[0][0].min << " " << cfg -> _pulseT0Range[0][0].max << endl;
  file << cfg -> _pulseT0Range[0][1].min << " " << cfg -> _pulseT0Range[0][1].max << endl;
  file << cfg -> _pulseT0Range[0][2].min << " " << cfg -> _pulseT0Range[0][2].max << endl;
  file << cfg -> _pulseT0Range[1][0].min << " " << cfg -> _pulseT0Range[1][0].max << endl;
  file << "# rise time cut" << endl;
  file << "0 1000000" << endl;
  file << "0 1000000" << endl;
  file << "0 1000000" << endl;
  file << "0 1000000" << endl;
  file << "# amplitude cut" << endl;
  file << "0 1000000" << endl;
  file << "0 1000000" << endl;
  file << "0 1000000" << endl;
  file << "0 1000000" << endl;
  file << "# red chi2 cut" << endl;
  file << "0 1000000" << endl;
  file << "0 1000000" << endl;
  file << "0 1000000" << endl;
  file << "0 1000000" << endl;
  file.close();

  ///////////
  delete cfg;  
  return 0;
}
