#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

#include "../aux/readRawDUT.h"
#include "../aux/fitWaveform.hh"
#include "../aux/configClass.hh"

#include <TFile.h>
#include <TTree.h>
#include <TPad.h>
#include <TError.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TLine.h>
#include <TPaveText.h>

int Landau(const unsigned int run,
	   const char *fileNameDUTPosition,                        
	   const unsigned int nEvents = 0,
	   const unsigned int startEvent = 0,
	   const bool lxplus=true,
	   const bool positionCorrected = true){

  ////////////////////////////////
  // loading calibration constants
  ////////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": loading calibration constants" << endl;
  const unsigned int sampleID = 106;
  const double irradLevel = 0;
  double offset[4];
  double slope[4];
  double offset_error[4];
  double slope_error[4];
  if(sampleID == 106){
    if(irradLevel == 0){
      offset[0] = 5308.14;
      slope[0] = 154.127;
      offset_error[0] = 472.913;
      slope_error[0] = 40.212;
      offset[1] = 5202.92;
      slope[1] = 158.941;
      offset_error[1] = 524.474;
      slope_error[1] = 44.7061;
      offset[2] = 4839.37;
      slope[2] = 136.518;
      offset_error[2] = 427.379;
      slope_error[2] = 36.6015;
      offset[3] = 5297.99;
      slope[3] = 150.976;
      offset_error[3] = 433.193;
      slope_error[3] = 37.833;
    }
  }
  for(unsigned int i=0; i<4; i++){
    cout << __PRETTY_FUNCTION__ << ": offset[" << i << "] = " << offset[i] << endl;
    cout << __PRETTY_FUNCTION__ << ": slope[" << i << "] = " << slope[i] << endl;
    cout << __PRETTY_FUNCTION__ << ": offset_error[" << i << "] = " << offset_error[i] << endl;
    cout << __PRETTY_FUNCTION__ << ": slope_error[" << i << "] = " << slope_error[i] << endl;
  }
  
  ///////////////////////////
  // constants and parameters
  ///////////////////////////
  cout << __PRETTY_FUNCTION__ << ": fileNameDUTPosition = " << fileNameDUTPosition << endl;
  char fileNameSynced[1000];
  if(lxplus) sprintf(fileNameSynced, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/PosCorr/posCorr_%06d_000000.root", run);
  else sprintf(fileNameSynced, "../../../output/posCorr_%06d_000000.root", run);
  cout << __PRETTY_FUNCTION__ << ": fileNameSynced = " << fileNameSynced << endl;
  const char *treeNameSynced = "posCorrTree";
  cout << __PRETTY_FUNCTION__ << ": treeNameSynced = " << treeNameSynced << endl;
  char fileNameRawData[1000];
  if(lxplus) sprintf(fileNameRawData, "/eos/atlas/atlascerngroupdisk/pixel-upgrade/cmos/TowerJazz/Investigator/Testbeam/Testbeam2017/DUT_%06d.dat", run);
  else sprintf(fileNameRawData, "../../../raw/DUT_%06d.dat", run);
  cout << __PRETTY_FUNCTION__ << ": fileNameRawData = " << fileNameRawData << endl;
  char fileNameCfg[1000];
  if(lxplus) sprintf(fileNameCfg, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/Run_Configs/run_%06d.cfg", run);
  else sprintf(fileNameCfg, "../../../cfg/run_%06d.cfg", run);
  cout << __PRETTY_FUNCTION__ << ": fileNameCfg = " << fileNameCfg << endl;
  char fileNameCuts[1000];
  if(lxplus) sprintf(fileNameCuts, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/Cuts/cuts_%06d.txt", run);
  else sprintf(fileNameCuts, "../../../output/cuts_%06d.txt", run);
  cout << __PRETTY_FUNCTION__ << ": fileNameCuts = " << fileNameCuts << endl;

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

  ///////////////
  // loading cuts
  ///////////////
  cout << __PRETTY_FUNCTION__ << ": loading cuts" << endl;
  pair<double, double> T0Cut[4];
  pair<double, double> amplitudeCut[4];
  pair<double, double> riseTimeCut[4];
  pair<double, double> redChi2Cut[4];
  ifstream fileCuts(fileNameCuts);
  if(!fileCuts){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileNameCuts << endl;
    return 1;
  }
  string text = "";
  getline(fileCuts, text);
  for(unsigned int iCH=0; iCH<4; iCH++){
    getline(fileCuts, text);
    sscanf(text.c_str(), "%lf %lf", &T0Cut[iCH].first, &T0Cut[iCH].second);
    cout << __PRETTY_FUNCTION__ << ": T0Cut[" << iCH << "] = " << T0Cut[iCH].first << ", " << T0Cut[iCH].second << endl;
  }
  getline(fileCuts, text);
  for(unsigned int iCH=0; iCH<4; iCH++){
    getline(fileCuts, text);
    sscanf(text.c_str(), "%lf %lf", &riseTimeCut[iCH].first, &riseTimeCut[iCH].second);
    cout << __PRETTY_FUNCTION__ << ": riseTimeCut[" << iCH << "] = " << riseTimeCut[iCH].first << ", " << riseTimeCut[iCH].second << endl;
  }
  getline(fileCuts, text);
  for(unsigned int iCH=0; iCH<4; iCH++){
    getline(fileCuts, text);
    sscanf(text.c_str(), "%lf %lf", &amplitudeCut[iCH].first, &amplitudeCut[iCH].second);
    cout << __PRETTY_FUNCTION__ << ": amplitudeCut[" << iCH << "] = " << amplitudeCut[iCH].first << ", " << amplitudeCut[iCH].second << endl;
  }
  getline(fileCuts, text);
  for(unsigned int iCH=0; iCH<4; iCH++){
    getline(fileCuts, text);
    sscanf(text.c_str(), "%lf %lf", &redChi2Cut[iCH].first, &redChi2Cut[iCH].second);
    cout << __PRETTY_FUNCTION__ << ": redChi2Cut[" << iCH << "] = " << redChi2Cut[iCH].first << ", " << redChi2Cut[iCH].second << endl;
  }
  fileCuts.close();

  ///////////////////////
  // getting DUT position
  ///////////////////////
  cout << __PRETTY_FUNCTION__ << ": getting DUT position" << endl;
  ifstream fileDUTPosition(fileNameDUTPosition);
  if(!fileDUTPosition){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameDUTPosition << endl;
    return 1;
  }
  double xMin = 0.;
  double xMax = 0.;
  double yMin = 0.;
  double yMax = 0.;
  string line = "";
  bool found = false;
  while(getline(fileDUTPosition, line)){
    unsigned int _run = 0;
    sscanf(line.c_str(), "%d %lf %lf %lf %lf", &_run, &xMin, &xMax, &yMin, &yMax);
    if(_run == run){
      found = true;
      break;
    }
  }
  fileDUTPosition.close();
  if(!found){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - run not found in list: " << run << endl;
    return 1;
  }
  cout << __PRETTY_FUNCTION__ << ": xMin = " << xMin << endl;
  cout << __PRETTY_FUNCTION__ << ": xMax = " << xMax << endl;
  cout << __PRETTY_FUNCTION__ << ": yMin = " << yMin << endl;
  cout << __PRETTY_FUNCTION__ << ": yMax = " << yMax << endl;

  ///////////////////////////////////////
  // computing position of the DUT center
  ///////////////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": computing position of the DUT center" << endl;
  const double xSize = xMax - xMin;
  cout << __PRETTY_FUNCTION__ << ": xSize = " << xSize << endl;
  const double ySize = yMax - yMin;
  cout << __PRETTY_FUNCTION__ << ": ySize = " << ySize << endl;
  xMin = xMin + xSize/4;
  xMax = xMax - xSize/4;
  yMin = yMin + ySize/4;
  yMax = yMax - ySize/4;
  cout << __PRETTY_FUNCTION__ << ": xMin = " << xMin << endl;
  cout << __PRETTY_FUNCTION__ << ": xMax = " << xMax << endl;
  cout << __PRETTY_FUNCTION__ << ": yMin = " << yMin << endl;
  cout << __PRETTY_FUNCTION__ << ": yMax = " << yMax << endl;

  TH1F *h1Landau = new TH1F("h1Landau", "Landau fit to unselected data", 64, 0, 64000);
  TH1F *h1LandauSelected = new TH1F("h1LandauSelected", "h1LandauSelected", 64, 0, 64000);

  //////////////////////
  // opening synced file
  //////////////////////
  cout << __PRETTY_FUNCTION__ << ": opening synced file" << endl;
  TFile *fileSynced = TFile::Open(fileNameSynced);
  if(fileSynced == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileNameSynced << endl;
    return 1;
  }

  //////////////////////
  // getting synced tree
  //////////////////////
  cout << __PRETTY_FUNCTION__ << ": getting synced tree" << endl;
  TTree *tree = (TTree *) fileSynced -> Get(treeNameSynced);
  if(tree == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << treeNameSynced << endl;
    return 1;
  }
  Double_t xTrack = 0.;
  tree -> SetBranchAddress("corrXTrack", &xTrack);
  Double_t yTrack = 0.;
  tree -> SetBranchAddress("corrYTrack", &yTrack);
  Int_t nTrack = 0.;
  tree -> SetBranchAddress("nTrack", &nTrack);
  Int_t FrameNumber = 0;
  tree -> SetBranchAddress("FrameNumber", &FrameNumber);
  Float_t pulseAmplitude = 0.;
  tree -> SetBranchAddress("pulseAmplitude", &pulseAmplitude);
  Float_t pulseT0 = 0.;
  tree -> SetBranchAddress("pulseT0", &pulseT0);
  Float_t pulseRiseTime = 0.;
  tree -> SetBranchAddress("pulseRiseTime", &pulseRiseTime);
  Float_t pulseRedChi2 = 0.;
  tree -> SetBranchAddress("pulseRedChi2", &pulseRedChi2);

  ///////////////////////////////
  // building Landau distribution
  ///////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": building Landau distribution" << endl;
  const unsigned int nEntries = tree -> GetEntries();
  for(unsigned int iEntry=0; iEntry<nEntries; iEntry+=5){    
    tree -> GetEntry(iEntry);
    if(xTrack > xMin && xTrack < xMax 
       && 
       yTrack > yMin && yTrack < yMax){
      double clusterEnergy = 0.;
      bool isSelected =false;
      for(unsigned int iCH=0; iCH<5; iCH++){
	if(iCH==3) continue; // reset signal
	tree -> GetEntry(iEntry+iCH);
	unsigned int index = iCH;
	if(iCH==4) index = 3;
	const double energy = (pulseAmplitude-offset[index]) / slope[index];
	//	clusterEnergy += energy; // something is seriously wrong with the calibration
	clusterEnergy += pulseAmplitude;
	unsigned int passesT0Cut = 0;
	unsigned int passesAmplitudeCut = 0;
	unsigned int passesRiseTimeCut = 0;
	unsigned int passesRedChi2Cut = 0;
	if(pulseT0 > T0Cut[index].first && pulseT0 < T0Cut[index].second) passesT0Cut = 1;
	if(pulseRiseTime > riseTimeCut[index].first && pulseRiseTime < riseTimeCut[index].second) passesRiseTimeCut = 1;
	if(pulseAmplitude > amplitudeCut[index].first && pulseAmplitude < amplitudeCut[index].second) passesAmplitudeCut = 1;
	if(pulseRedChi2 > redChi2Cut[index].first && pulseRedChi2 < redChi2Cut[index].second) passesRedChi2Cut = 1;
	if(passesT0Cut * passesAmplitudeCut * passesRiseTimeCut * passesRedChi2Cut != 0){
	  isSelected = true;
	}
      }
      h1Landau -> Fill(clusterEnergy);
      if(isSelected) h1LandauSelected -> Fill(clusterEnergy);
    }
    else continue;
  }

  //////////////////////
  // closing synced file
  //////////////////////
  cout << __PRETTY_FUNCTION__ << ": closing synced file" << endl;
  fileSynced -> Close();
  delete fileSynced;

  gStyle -> SetOptFit(1);
  gStyle -> SetOptTitle(0);
  TCanvas *cc =  new TCanvas("cc", "cc", 0, 0, 1000, 1000);
  h1Landau -> SetLineColor(1);
  h1Landau -> SetLineWidth(6);
  h1Landau -> GetXaxis() -> SetTitle("Uncalibrated signal S #left[a.u.#right]");
  h1Landau -> GetYaxis() -> SetTitle("#frac{dN}{dS} #left[a.u.^{-1}#right]");
  h1Landau -> Draw();
  h1LandauSelected -> SetLineColor(3);
  h1LandauSelected -> SetLineWidth(2);
  h1LandauSelected -> Draw("same");
  h1LandauSelected -> Fit("landau");

  char name[1000];
  sprintf(name, "tmp/run%d.png", run);
  cc -> SaveAs(name);

  return 0;

  //////////////////
  // cleaning memory
  //////////////////
  cout << __PRETTY_FUNCTION__ << ": cleaning memory" << endl;
  delete cfg;
  delete cc;
  delete h1Landau;
  delete h1LandauSelected;

  return 0;
}
