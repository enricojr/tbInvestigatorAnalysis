#ifndef  EFFICIENCY_HH
#define  EFFICIENCY_HH

#include <iostream>
#include <fstream>
using namespace std;

#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TStyle.h>

#include "../aux/progressBar.h"
#include "../aux/configClass.hh"

#define XRES 9.
#define XMIN -10000
#define XMAX 10000
#define YRES 9.
#define YMIN -10000
#define YMAX 10000
#define NCLUSTERS 4

class runClass{
public:
  
  runClass(){
    init();
  };
  
  ~runClass(){
    if(_h2HitsAll != NULL) delete _h2HitsAll;
    if(_h2HitsSelected != NULL) delete _h2HitsSelected;
    if(_h2Efficiency != NULL) delete _h2Efficiency;
    if(_h2ClusterSize != NULL){
      for(unsigned int i=0; i<NCLUSTERS; i++){
	delete _h2ClusterSize[i];
      }
      delete[] _h2ClusterSize;
    }
  };
  
  void init(){
    _runNumber = 0;
    _fileNameIn = "";
    _fileNameCfg = "";
    _nBinsX = 0;
    _nBinsY = 0;
    _h2HitsAll = NULL;
    _h2HitsSelected = NULL;
    _h2Efficiency = NULL;
    return ;
  }
  
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": runNumber = " << _runNumber << endl;
    cout << __PRETTY_FUNCTION__ << ": fileNameIn = " << _fileNameIn << endl;
    cout << __PRETTY_FUNCTION__ << ": fileNameCfg = " << _fileNameCfg << endl;
  }
  
  void setFileNames(const char *fileNameInFormat,
		    const char *fileNameCfgFormat){
    char fileNameIn[1000];
    sprintf(fileNameIn, fileNameInFormat, _runNumber);
    _fileNameIn = fileNameIn;
    char fileNameCfg[1000];
    sprintf(fileNameCfg, fileNameCfgFormat, _runNumber);
    _fileNameCfg = fileNameCfg;
    return ;
  }

  void allocateHitMaps(const double xMin,
		       const double xMax,
		       const double dx,
		       const double yMin,
		       const double yMax,
		       const double dy){
    _nBinsX = (xMax-xMin) / dx;
    _nBinsY = (yMax-yMin) / dy;    
    char name[1000];
    sprintf(name, "h2HitsAll_%06d", _runNumber);
    _h2HitsAll = new TH2F(name, name, _nBinsX, xMin, xMax, _nBinsY, yMin, yMax);
    sprintf(name, "h2HitsSelected_%06d", _runNumber);
    _h2HitsSelected = new TH2F(name, name, _nBinsX, xMin, xMax, _nBinsY, yMin, yMax);
    sprintf(name, "h2Efficiency_%06d", _runNumber);
    _h2Efficiency = new TH2F(name, name, _nBinsX, xMin, xMax, _nBinsY, yMin, yMax);
    _h2ClusterSize = new TH2F*[NCLUSTERS];
    for(unsigned int i=0; i<NCLUSTERS; i++){
      sprintf(name, "h2ClusterSize_%06d_%d", _runNumber, i);
      _h2ClusterSize[i] = new TH2F(name, name, _nBinsX, xMin, xMax, _nBinsY, yMin, yMax);
    }
    return ;
  }

  int loadData(){
    
    // opening configuration file
    configClass *cfg = new configClass();
    if(cfg -> load(_fileNameCfg.c_str())){
      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot load configuration" << endl;
      return 1;
    }
    cfg -> print();

    // opening root file
    TFile *file = TFile::Open(_fileNameIn.c_str());
    if(file == NULL){
      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << _fileNameIn << endl;
      return 1;
    }
    TTree *tree = (TTree *) file -> Get("syncedData");
    if(tree == NULL){
      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot load input tree" << endl;
      return 1;
    }
    
    Double_t xTrack = 0.;
    tree -> SetBranchAddress("xTrack",&xTrack);
    Double_t yTrack = 0.;
    tree -> SetBranchAddress("yTrack",&yTrack);
    Double_t timeTelescope = 0.;
    tree -> SetBranchAddress("timeTelescope",&timeTelescope);
    ULong64_t FrameNumber = 0;
    tree -> SetBranchAddress("FrameNumber", &FrameNumber);
    Double_t redChi2 = 0.;
    tree->SetBranchAddress("redChi2Telescope",&redChi2);
    Int_t nTrack = 0;
    tree->SetBranchAddress("nTrack",&nTrack);
    Double_t timeDUT = 0.;
    tree->SetBranchAddress("timeDUT",&timeDUT);
    Float_t linearOffset = 0.;
    tree->SetBranchAddress("linearOffset",&linearOffset);
    Float_t linearSlope = 0.;
    tree->SetBranchAddress("linearSlope",&linearSlope);
    Float_t linearRedChi2 = 0.;
    tree->SetBranchAddress("linearRedChi2",&linearRedChi2);
    Float_t pulseOffset = 0.;
    tree->SetBranchAddress("pulseOffset",&pulseOffset);
    Float_t pulseSlope = 0.;
    tree->SetBranchAddress("pulseSlope",&pulseSlope);
    Float_t pulseAmplitude = 0.;
    tree->SetBranchAddress("pulseAmplitude",&pulseAmplitude);
    Float_t pulseT0 = 0.;
    tree->SetBranchAddress("pulseT0",&pulseT0);
    Float_t pulseRiseTime = 0.;
    tree->SetBranchAddress("pulseRiseTime",&pulseRiseTime);
    Float_t pulseDecay = 0.;
    tree->SetBranchAddress("pulseDecay",&pulseDecay);
    Float_t pulseCharge = 0.;
    tree->SetBranchAddress("pulseCharge",&pulseCharge);
    Float_t pulseRedChi2 = 0.;
    tree->SetBranchAddress("pulseRedChi2",&pulseRedChi2);
    Int_t eventIDDUT = 0;
    tree->SetBranchAddress("eventIDDUT",&eventIDDUT);
    Int_t DRSNumber = 0;
    tree->SetBranchAddress("DRSNumber",&DRSNumber);
    Int_t CHNumber = 0;
    tree->SetBranchAddress("CHNumber",&CHNumber);

    const unsigned int nEntries = tree -> GetEntries();
    for(unsigned int iEntry=0; iEntry<nEntries; iEntry+=cfg -> _nChannels){
      loadBar(iEntry, nEntries);
      tree -> GetEntry(iEntry);
      if(nTrack != 1) continue;
      else{
	unsigned int clusterSize = 0;
	for(unsigned int iHit=iEntry; iHit<iEntry+cfg -> _nChannels; iHit++){
	  tree -> GetEntry(iHit);
	  if(pulseT0 > cfg -> _pulseT0Range[DRSNumber][CHNumber].min && pulseT0 < cfg -> _pulseT0Range[DRSNumber][CHNumber].max // selection from pulse fit
	     &&
	     pulseRiseTime > cfg -> _pulseRiseTimeRange.min && pulseRiseTime < cfg -> _pulseRiseTimeRange.max
	     &&
	     pulseRedChi2 < cfg -> _pulseRedChi2Threshold){
	    //	    if(pulseT0 > 0.){
	    clusterSize++;
	  }
	}
	_h2HitsAll -> Fill(xTrack, yTrack);
	if(clusterSize != 0){
	  _h2HitsSelected -> Fill(xTrack, yTrack);
	  _h2ClusterSize[clusterSize-1] -> Fill(xTrack, yTrack);
	}
      }
    }
    
    // closing root file
    file -> Close();
    delete file;

    // computing efficiency and cluster maps
    for(unsigned int iX=0; iX<_nBinsX; iX++){
      for(unsigned int iY=0; iY<_nBinsY; iY++){	
	const double num = _h2HitsSelected -> GetBinContent(iX+1, iY+1);
	_h2Efficiency -> SetBinContent(iX+1, iY+1, num);
	for(unsigned int i=0; i<NCLUSTERS; i++){
	  const double num = _h2ClusterSize[i] -> GetBinContent(iX+1, iY+1);
	  _h2ClusterSize[i] -> SetBinContent(iX+1, iY+1, num);  
	}	
      }
    }
    _h2Efficiency -> Divide(_h2HitsAll);
    for(unsigned int i=0; i<NCLUSTERS; i++){
      _h2ClusterSize[i] -> Divide(_h2HitsAll);
    }
    
    // cleaning memory
    delete cfg;
    
    return 0;
  }
  
  unsigned int _runNumber;
  string _fileNameIn;
  string _fileNameCfg;
  unsigned int _nBinsX;
  unsigned int _nBinsY;
  TH2F *_h2HitsAll;
  TH2F *_h2HitsSelected;
  TH2F *_h2Efficiency;
  TH2F **_h2ClusterSize;
};

int getRunList(const char *fileNameRunList,
	       vector<runClass *> &run){

  ifstream file(fileNameRunList);
  if(!file){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open run list file " << fileNameRunList << endl;
    return 1;
  }

  string line = "";
  while(getline(file, line)){
    if(line[0] == '#') continue;
    else{
      runClass *newRun = new runClass();
      sscanf(line.c_str(), "%d", &newRun -> _runNumber);
      run.push_back(newRun);
    }
  }
  
  file.close();
  
  return 0;
}

#endif
