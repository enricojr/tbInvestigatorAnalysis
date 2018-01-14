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

class eventClass{
public:
  unsigned int eventNumber;
  double xTrack;
  double yTrack;
  unsigned int nTrack;
};

bool eventIsInList(const unsigned int eventNumber, 
		   vector<eventClass> event,
		   unsigned int &numberOfTracks){
  for(unsigned int i=0; i<event.size(); i++){
    if(event[i].eventNumber == eventNumber){
      numberOfTracks = event[i].nTrack;
      return true;
    }
    else continue;
  }
  return false;
}

int artificialIntelligence(const unsigned int run,
			   const char *fileNameDUTPosition,			   
			   const unsigned int nEvents = 0,
			   const unsigned int startEvent = 0,
			   const bool lxplus=true,
			   const bool positionCorrected = true){

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

  ////////////////////
  // building hit list
  ////////////////////
  cout << __PRETTY_FUNCTION__ << ": building hit list" << endl;
  vector<eventClass> event;
  const unsigned int nEntries = tree -> GetEntries();
  for(unsigned int iEntry=0; iEntry<nEntries; iEntry+=5){    
    tree -> GetEntry(iEntry);
    if(xTrack > xMin && xTrack < xMax 
       && 
       yTrack > yMin && yTrack < yMax){
      eventClass newEvent;
      newEvent.eventNumber = FrameNumber;
      newEvent.xTrack = xTrack;
      newEvent.yTrack = yTrack;
      newEvent.nTrack = nTrack;
      event.push_back(newEvent);
    }
    else continue;
  }

  //////////////////////
  // closing synced file
  //////////////////////
  cout << __PRETTY_FUNCTION__ << ": closing synced file" << endl;
  fileSynced -> Close();
  delete fileSynced;

  ///////////////////////////////
  // initializing waveform fitter
  ///////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": initializing waveform fitter" << endl;  
  fitWaveformClass *fitWaveform = new fitWaveformClass(NDRSSAMPLES); 

  // dummy histogram
  TH1F ***h1Noise = new TH1F**[cfg -> _nDRS];
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    h1Noise[iDRS] = new TH1F*[cfg -> _nCH[iDRS]];
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){
      char name[1000];
      char title[1000];
      sprintf(name, "h1Noise_DRS_%d_CH_%d", iDRS, iCH);
      if(nEvents == 0) sprintf(title, "noise distribution DRS %d CH %d, full statistics", iDRS, iCH);
      else sprintf(title, "noise distribution DRS %d CH %d, %d events", iDRS, iCH, nEvents);
      h1Noise[iDRS][iCH] = new TH1F(name, title, 1,0,1);
    }
  }

  ////////////////////
  // reading raw  data
  ////////////////////
  cout << __PRETTY_FUNCTION__ << ": reading raw data" << endl;  
  DRSDataClass *DRSData = new DRSDataClass();
  ifstream fileRawData(fileNameRawData, ios::in | ios::binary);
  if(!fileRawData){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameRawData << endl;
    return 1;
  }
  double t0 = 0.;
  bool endOfFile = false;
  unsigned int eventNumber = 0;
  Int_t DRSNumber = 0;
  Int_t CHNumber = 0;
  unsigned int countEvents = 1;
  while(!endOfFile){

    if(fileRawData.eof()) break;
    
    endOfFile = goToEventHeader(fileRawData, cfg -> _debugLevel);
    if(endOfFile) break;

    if(nEvents != 0 && eventNumber >= (Int_t) nEvents) break;

    // startEventNumber
    if(eventNumber < (Int_t) startEvent){
      eventNumber++;
      continue;
    }
        
    if(eventNumber%1000 == 0) cout << __PRETTY_FUNCTION__ << ": Processed " << eventNumber << " events" << endl;
      
    // setting t0
    if(eventNumber == 0){
      readHeaderInfo(fileRawData, DRSData, cfg -> _debugLevel);      
      t0 = DRSData -> computeTimestamp();
      cout << __PRETTY_FUNCTION__ << ": t0 = " << t0 << endl;
    }

    // timestamp
    if(eventNumber != 0){
      readHeaderInfo(fileRawData, DRSData, cfg -> _debugLevel);
      //      time = (DRSData -> computeTimestamp() - t0) * TIMECONVERSION;
    }

    // loop on DRSs
    unsigned int channelNumber = 0;
    double WAVEFORM[5][1024];
    TF1 **f1 = new TF1*[5];
    TH1F **h1 = new TH1F*[5];
    bool selected[5];
    unsigned int passesT0Cut[5];
    unsigned int passesAmplitudeCut[5];
    unsigned int passesRiseTimeCut[5];
    unsigned int passesRedChi2Cut[5];
    double amplitudeMin[5];
    double amplitudeMax[5];
    double pulseT0[5];
    double pulseRiseTime[5];
    double pulseAmplitude[5];
    double pulseRedChi2[5];
    for(unsigned int i=0; i<5; i++){
      selected[i] = false;
      passesT0Cut[i] = 0;
      passesAmplitudeCut[i] = 0;
      passesRiseTimeCut[i] = 0;
      passesRedChi2Cut[i] = 0;
      amplitudeMin[i] = 100000000.;
      amplitudeMax[i] = -100000000.;
      pulseT0[i] = 0;
      pulseRiseTime[i] = 0;
      pulseAmplitude[i] = 0;
      pulseRedChi2[i] = 0;
    }
    for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){

      DRSNumber = iDRS;
      
      if(cfg -> _debugLevel >= 3){
        cout << __PRETTY_FUNCTION__ << ": reading DRS " << iDRS << endl;
        cout << __PRETTY_FUNCTION__ << ": cfg -> _nCH[" << iDRS << "] = " << cfg -> _nCH[iDRS] << endl;
      }

      if(iDRS != 0){ // maybe this happens only when the second DRS reads out 1 channel only
        DRSData -> boardNumber = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
        DRSData -> boardNumber = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
        DRSData -> boardNumber = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
        DRSData -> boardNumber = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
        DRSData -> boardNumber = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
        DRSData -> boardNumber = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
      }

      DRSData -> boardNumber = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
      DRSData -> boardNumber = readTwoBytes(fileRawData, false, cfg -> _debugLevel);      
      if(cfg -> _debugLevel >= 3) cout << __PRETTY_FUNCTION__  << ": boardNumber = " << DRSData -> boardNumber << endl;
      if(cfg -> _nCH[iDRS] != 1){ // maybe this happens only when the DRS reads out 1 channel only
        DRSData -> triggerCell = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
        DRSData -> triggerCell = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
        DRSData -> triggerCell = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
        DRSData -> triggerCell = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
        if(cfg -> _debugLevel >= 3) cout << __PRETTY_FUNCTION__  << ": triggerCell = " << DRSData -> triggerCell << endl;
      }
      
      // loop on channels
      for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){

        CHNumber = iCH;
        
        if(cfg -> _debugLevel >= 3) cout << __PRETTY_FUNCTION__ << ": channel " << iCH << endl;

        if(cfg -> _nCH[iDRS] != 1){ // maybe this happens only when the DRS reads out 1 channel only
          DRSData -> scaler = readTwoBytes(fileRawData, true, cfg -> _debugLevel);
          DRSData -> scaler = readTwoBytes(fileRawData, true, cfg -> _debugLevel);
          DRSData -> scaler = readFourBytes(fileRawData, true, cfg -> _debugLevel);
        }
        
        // reading data
        if(cfg -> _debugLevel >= 3) cout << __PRETTY_FUNCTION__ << ": start waveform " << endl;
        double waveform[NDRSSAMPLES];
	unsigned int index = iCH;
        if(iDRS==1) index = 4;
	char name[10];
	sprintf(name, "f%d", index);
	f1[index] = new TF1(name, "[0]+[1]*x + 0.5*[2]*(1+TMath::Erf(100*(x-[3])))*(1-exp(-(x-[3])/[4]))*(1.+[5]*(x-[3]))", cfg -> _pulse[iDRS][iCH].min, cfg -> _pulse[iDRS][iCH].max);
	sprintf(name, "h1%d", index);
	h1[index] = new TH1F(name, name, 1024, 0, 1024);
	for(unsigned int iSample=0; iSample<NDRSSAMPLES; iSample++){
          waveform[iSample] = readTwoBytes(fileRawData, false, cfg -> _debugLevel);
	  WAVEFORM[index][iSample] = waveform[iSample];
	  if(waveform[iSample] < amplitudeMin[index]) amplitudeMin[index] = waveform[iSample];
	  if(waveform[iSample] > amplitudeMax[index]) amplitudeMax[index] = waveform[iSample];
        }

	// fit
	unsigned int numberOfTracks = 0;
	if(eventIsInList(eventNumber, event, numberOfTracks)){
	  //	  cout << "+++++++++++++++++++++++++++++ in list: " << eventNumber << endl;
	  fitWaveform -> fit(waveform, cfg, eventNumber, iDRS, iCH, h1Noise);
	  f1[index] -> SetParameter(0, fitWaveform -> getPulseOffset());
	  f1[index] -> SetParameter(1, fitWaveform -> getPulseSlope());
	  f1[index] -> SetParameter(2, fitWaveform -> getPulseAmplitude());
	  f1[index] -> SetParameter(3, fitWaveform -> getPulseT0());
	  f1[index] -> SetParameter(4, fitWaveform -> getPulseRiseTime());
	  f1[index] -> SetParameter(5, fitWaveform -> getPulseDecay());
	  pulseT0[index] = fitWaveform -> getPulseT0();
	  pulseRiseTime[index] = fitWaveform -> getPulseRiseTime();
	  pulseAmplitude[index] = fitWaveform -> getPulseAmplitude();
	  pulseRedChi2[index] = fitWaveform -> getPulseRedChi2();
	  f1[index] -> SetLineColor(2);
	  f1[index] -> SetLineWidth(2);
	  for(unsigned int iSample=0; iSample<NDRSSAMPLES; iSample++){
	    h1[index] -> SetBinContent(iSample+1, iSample, f1[index] -> Eval(iSample));
	  }
	  h1[index] -> SetLineColor(2);
	  h1[index] -> SetLineWidth(2);

	  unsigned int INDEX = 0;
	  if(iDRS==0 && iCH==0) INDEX=0;
	  else if(iDRS==0 && iCH==1) INDEX=1;
	  else if(iDRS==0 && iCH==2) INDEX=2;
	  else if(iDRS==1 && iCH==0) INDEX=3;

	  if(fitWaveform -> getPulseT0() > T0Cut[INDEX].first && fitWaveform -> getPulseT0() < T0Cut[INDEX].second) passesT0Cut[index] = 1;
	  if(fitWaveform -> getPulseRiseTime() > riseTimeCut[INDEX].first && fitWaveform -> getPulseRiseTime() < riseTimeCut[INDEX].second) passesRiseTimeCut[index] = 1;
	  if(fitWaveform -> getPulseAmplitude() > amplitudeCut[INDEX].first && fitWaveform -> getPulseAmplitude() < amplitudeCut[INDEX].second) passesAmplitudeCut[index] = 1;
	  if(fitWaveform -> getPulseRedChi2() > redChi2Cut[INDEX].first && fitWaveform -> getPulseRedChi2() < redChi2Cut[INDEX].second) passesRedChi2Cut[index] = 1;
	  if(passesT0Cut[index] * passesAmplitudeCut[index] * passesRiseTimeCut[index] * passesRedChi2Cut[index] != 0){
	    selected[index] = true;
	  }
	  else selected[index] = false;
	  fitWaveform -> reset();

	}
      }
      
    }

    unsigned int numberOfTracks = 0;
    if(eventIsInList(eventNumber, event, numberOfTracks)){
      TH1F **h1Wiskey = new TH1F*[5];
      for(unsigned int i=0; i<5; i++){
        char h1Name[1000];
        sprintf(h1Name, "CH%d.png", i);
        h1Wiskey[i] = new TH1F(h1Name, h1Name, 1024, 0, 1024);
        for(unsigned int j=0; j<1024; j++){
          h1Wiskey[i] -> Fill(j, WAVEFORM[i][j]);
        }
      }
      TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 1000, 1000);
      cc -> Divide(2,2);
      TPaveText **pt = new TPaveText*[4];
      for(unsigned int i=0; i<4; i++){
	unsigned int index = i;
	if(i==3) index = 4;
	if(selected[index]) cc -> cd(i+1) -> SetFrameFillColor(kGreen-10);
	else cc -> cd(i+1) -> SetFrameFillColor(kRed-10);
	cc -> cd(i+1) -> SetFillColor(numberOfTracks+1);
	h1Wiskey[index] -> SetLineColor(1);
	h1Wiskey[index] -> SetLineWidth(2);
	h1Wiskey[index] -> GetYaxis() -> SetRangeUser(amplitudeMin[index], amplitudeMax[index]);
	h1Wiskey[index] -> Draw();
	h1[index] -> Draw("same");
	const double yRange = amplitudeMax[index] - amplitudeMin[index];
	const double xInf = 1024/2;
	const double yInf = amplitudeMin[index]+0.05*yRange;
	const double xSup = 1000;
	const double ySup = amplitudeMin[index]+0.5*yRange;
	pt[i] = new TPaveText(xInf, yInf, xSup, ySup);
	char text[100];
	sprintf(text, "T0 = %lf", pulseT0[index]);
	pt[i] -> AddText(text);
	sprintf(text, "RT = %lf", pulseRiseTime[index]);
	pt[i] -> AddText(text);
	sprintf(text, "AM = %lf", pulseAmplitude[index]);
	pt[i] -> AddText(text);
	sprintf(text, "X2 = %lf", pulseRedChi2[index]);
	pt[i] -> AddText(text);
	sprintf(text, "%d%d%d%d", passesT0Cut[index], passesRiseTimeCut[index], passesAmplitudeCut[index], passesRedChi2Cut[index]);
	pt[i] -> AddText(text);
	pt[i] -> Draw();
      }
      char ccName[1000];
      sprintf(ccName, "run%d/evt%08d-%08d.png", run, eventNumber, countEvents);
      cc -> SaveAs(ccName);
      for(unsigned int i=0; i<4; i++){
	delete pt[i];
      }
      delete[] pt;
      for(unsigned int i=0; i<5; i++){
        delete h1Wiskey[i];
      }
      delete[] h1Wiskey;
      delete cc;
      countEvents++;
    }

    for(unsigned int i=0; i<5; i++){
      delete f1[i];
      delete h1[i];
    }
    delete[] f1;
    delete[] h1;
    eventNumber++;
    if(countEvents == 101) break;
  }
  fileRawData.close();

  //////////////////
  // cleaning memory
  //////////////////
  cout << __PRETTY_FUNCTION__ << ": cleaning memory" << endl;
  delete cfg;
  event.clear();
  delete fitWaveform;
  return 0;
}
