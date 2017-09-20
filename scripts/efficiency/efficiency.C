#include "efficiency.hh"

int efficiency(const char *fileNameRunList,
	       const char *fileNameInFormat,
	       const char *fileNameCfgFormat){

  //////////////////////
  // getting run list //
  //////////////////////
  cout << __PRETTY_FUNCTION__ << ": getting run list" << endl;
  vector<runClass *> run;
  if(getRunList(fileNameRunList, run)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot get run list" << endl;
    return 1;
  }
  for(unsigned int iRun=0; iRun<run.size(); iRun++){
    run[iRun] -> setFileNames(fileNameInFormat, fileNameCfgFormat);
  }
  cout << __PRETTY_FUNCTION__ << ": nRuns = " << run.size() << endl;
  for(unsigned int iRun=0; iRun<run.size(); iRun++){
    run[iRun] -> print();
  }

  //////////////////////
  // allocating plots //
  //////////////////////
  cout << __PRETTY_FUNCTION__ << ": allocating plots" << endl;
  for(unsigned int iRun=0; iRun<run.size(); iRun++){
    run[iRun] -> allocateHitMaps(XMIN, XMAX, XRES, YMIN, YMAX, YRES);
  }

  //////////////////
  // loading data //
  //////////////////
  cout << __PRETTY_FUNCTION__ << ": loading data" << endl;
  for(unsigned int iRun=0; iRun<run.size(); iRun++){
    cout << __PRETTY_FUNCTION__ << ": analyzing run " << run[iRun] -> _runNumber << endl;
    if(run[iRun] -> loadData()){
      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot load data for run " << run[iRun] -> _runNumber << endl;
      return 1;
    }
  }

  gStyle -> SetOptStat(0);
  gStyle -> SetPalette(55);
  gStyle->SetNumberContours(256);
  // const double yMin = -10000;
  // const double yMax = +10000;
  // const double xMin = -10000;
  // const double xMax = +10000;
  const double yMin = -4000;
  const double yMax = -3700;
  const double xMin = -1950;
  const double xMax = -1650;
  TCanvas *c1 = new TCanvas("c1", "c1", 0, 0, 1000, 1000);
  run[0] -> _h2HitsAll -> GetXaxis() -> SetRangeUser(xMin, xMax);
  run[0] -> _h2HitsAll -> GetYaxis() -> SetRangeUser(yMin, yMax);
  run[0] -> _h2HitsAll -> Draw("colz");
  TCanvas *c2 = new TCanvas("c2", "c2", 0, 0, 1000, 1000);
  run[0] -> _h2HitsSelected -> GetXaxis() -> SetRangeUser(xMin, xMax);
  run[0] -> _h2HitsSelected -> GetYaxis() -> SetRangeUser(yMin, yMax);
  run[0] -> _h2HitsSelected -> Draw("colz");
  TCanvas *c3 = new TCanvas("c3", "c3", 0, 0, 1000, 1000);
  run[0] -> _h2Efficiency -> GetXaxis() -> SetRangeUser(xMin, xMax);
  run[0] -> _h2Efficiency -> GetYaxis() -> SetRangeUser(yMin, yMax);
  run[0] -> _h2Efficiency -> GetZaxis() -> SetRangeUser(0., 1.);
  run[0] -> _h2Efficiency -> Draw("colz");
  TCanvas **c4 = new TCanvas*[NCLUSTERS];
  for(unsigned int i=0; i<NCLUSTERS; i++){
    char name[10];
    sprintf(name, "c4_%d", i);
    c4[i] = new TCanvas(name, name, 0, 0, 1000, 1000);
    run[0] -> _h2ClusterSize[i] -> GetXaxis() -> SetRangeUser(xMin, xMax);
    run[0] -> _h2ClusterSize[i] -> GetYaxis() -> SetRangeUser(yMin, yMax);
    run[0] -> _h2ClusterSize[i] -> GetZaxis() -> SetRangeUser(0., 1.);
    run[0] -> _h2ClusterSize[i] -> Draw("colz");
  }
  return 0;
  
  /////////////////////
  // cleaning memory //
  /////////////////////
  cout << __PRETTY_FUNCTION__ << ": cleaning memory" << endl;
  for(unsigned int iRun=0; iRun<run.size(); iRun++){
    delete run[iRun];
  }
  run.clear();
  
  return 0;
}
