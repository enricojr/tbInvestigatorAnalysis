// from the output root file, noise can be accessed as
// h1Noise_DRS_0_CH_0 -> GetRMS()

#include "../aux/readRawDUT.h"
#include "../aux/fitWaveform.hh"

#include <TFile.h>
#include <TTree.h>
#include <TPad.h>
#include <TError.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TLine.h>
#include <vector>
#include <sstream>

#define TIMECONVERSION 0.001 // to convert timestamp into seconds

// parameters for noise plot
#define PLOTNOISENBINS 2001
#define PLOTNOISEMIN -1000
#define PLOTNOISEMAX 1000

// parameters for reduced chi2 plot
#define PLOTNBINSREDCHI2 1000
#define PLOTMINREDCHI2 0. 
#define PLOTMAXREDCHI2 10.

// parameters for waveform plot
#define PLOTWAVEFORMMINAMPLITUDE -2000
#define PLOTWAVEFORMMAXAMPLITUDE 65535 // 2 bytes
#define PLOTWAVEFORMNBINSAMPLITUDE 1000

class eventClass{
public:
  unsigned int frame;
  unsigned int DRS;
  unsigned int CH;
};

bool isSelected(const unsigned int frame,
		const unsigned int DRS,
		const unsigned int CH,
		vector<eventClass> &event){

  for(unsigned int i=0; i<event.size(); i++){
    if(event[i].frame == frame
       &&
       event[i].DRS == DRS
       &&
       event[i].CH == CH){
      event.erase(event.begin()+i);
      return true;
    }
    else continue;
  }
  
  return false;
}

/*****************/
/*****************/
/* main function */
/*****************/
/*****************/

int showSelection(const unsigned int run,
		  const bool lxplus = true){
  
  gStyle -> SetOptFit(1);

  const int shift = 2;
  
  const unsigned int nEvents = 0;
  const unsigned int startEvent = 0;
  Int_t eventNumber = 0;
  Int_t DRSNumber = 0;
  Int_t CHNumber = 0;

  char fileNameIn[2000];
  char fileNameList[2000];
  char fileNameCfg[2000];

  if(lxplus){
    sprintf(fileNameList, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/tbInvestigatorAnalysis/scripts/efficiency2/incWF_%d.txt", run);
    sprintf(fileNameCfg, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/Run_Configs/run_%06d.cfg", run);
    sprintf(fileNameIn, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/DUT_converted/DUT_%06d.dat", run);
  }
  else{
    sprintf(fileNameList, "../../../output/incWF_%d.txt", run);
    sprintf(fileNameCfg, "../../../cfg/run_%06d.cfg", run);
    sprintf(fileNameIn, "../../../raw/DUT_%06d.dat", run);
  }

  TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 2000, 1000);
  cc -> Divide(4,4);
  
  ///////////////////////
  // reading file list //
  ///////////////////////
  vector<eventClass> event;
  ifstream file(fileNameList);
  if(!file){
    cout  << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameList << endl;
    return 1;
  }
  string LINE = "";
  while(getline(file, LINE)){
    eventClass newEvent;
    sscanf(LINE.c_str(), "%d %d %d", &newEvent.frame, &newEvent.DRS, &newEvent.CH);
    event.push_back(newEvent);
  }
  file.close();

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

  /////////////////////
  // initializing plots
  /////////////////////

  // timeline
  TGraph *grTimeline = new TGraph();
  grTimeline -> SetName("grTimeline");
  grTimeline -> SetTitle("timeline");

  // noise
  TH1F ***h1Noise = new TH1F**[cfg -> _nDRS];
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    h1Noise[iDRS] = new TH1F*[cfg -> _nCH[iDRS]];
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){
      char name[1000];
      char title[1000];
      sprintf(name, "h1Noise_DRS_%d_CH_%d", iDRS, iCH);
      if(nEvents == 0) sprintf(title, "noise distribution DRS %d CH %d, full statistics", iDRS, iCH);
      else sprintf(title, "noise distribution DRS %d CH %d, %d events", iDRS, iCH, nEvents);
      h1Noise[iDRS][iCH] = new TH1F(name, title, PLOTNOISENBINS, PLOTNOISEMIN, PLOTNOISEMAX);
    }
  }

  
  // reset selection
  TH2F *h2ResetWaveforms = new TH2F("h2ResetWaveforms", "Reset waveforms", NDRSSAMPLES, 0, NDRSSAMPLES, PLOTWAVEFORMNBINSAMPLITUDE, PLOTWAVEFORMMINAMPLITUDE, PLOTWAVEFORMMAXAMPLITUDE); 

  // linear fit selection
  TH1F *h1LinearRedChi2 = new TH1F("h1LinearRedChi2", "Reduced Chi2", PLOTNBINSREDCHI2, PLOTMINREDCHI2, PLOTMAXREDCHI2);
  TH1F *h1LinearRedChi2Selected = new TH1F("h1LinearRedChi2Selected", "Reduced Chi2 of selected waveforms", PLOTNBINSREDCHI2, PLOTMINREDCHI2, PLOTMAXREDCHI2);  
  TH2F ***h2LinearExcludedWaveforms = new TH2F**[cfg -> _nDRS];
  TH2F ***h2LinearSelectedWaveforms = new TH2F**[cfg -> _nDRS];
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    h2LinearExcludedWaveforms[iDRS] = new TH2F*[cfg -> _nCH[iDRS]];
    h2LinearSelectedWaveforms[iDRS] = new TH2F*[cfg -> _nCH[iDRS]];
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){
      char name[1000];
      char title[1000];
      sprintf(name, "h2LinearExcludedWaveforms_DRS_%d_CH_%d", iDRS, iCH);
      if(nEvents == 0) sprintf(title, "linear fit excluded waveforms DRS %d CH %d, full statistics", iDRS, iCH);
      else sprintf(title, "linear fit excluded waveforms DRS %d CH %d, %d events", iDRS, iCH, nEvents);
      h2LinearExcludedWaveforms[iDRS][iCH] = new TH2F(name, title, NDRSSAMPLES, 0, NDRSSAMPLES, PLOTWAVEFORMNBINSAMPLITUDE, PLOTWAVEFORMMINAMPLITUDE, PLOTWAVEFORMMAXAMPLITUDE); 
      sprintf(name, "h2LinearSelectedWaveforms_DRS_%d_CH_%d", iDRS, iCH);
      if(nEvents == 0) sprintf(title, "linear fit selected waveforms DRS %d CH %d, full statistics", iDRS, iCH);
      else sprintf(title, "linear fit selected waveforms DRS %d CH %d, %d events", iDRS, iCH, nEvents);
      h2LinearSelectedWaveforms[iDRS][iCH] = new TH2F(name, title, NDRSSAMPLES, 0, NDRSSAMPLES, PLOTWAVEFORMNBINSAMPLITUDE, PLOTWAVEFORMMINAMPLITUDE, PLOTWAVEFORMMAXAMPLITUDE); 
    }
  }

  // pulse fit selection
  TH2F ***h2PulseExcludedWaveforms = new TH2F**[cfg -> _nDRS];
  TH2F ***h2PulseSelectedWaveforms = new TH2F**[cfg -> _nDRS];
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    h2PulseExcludedWaveforms[iDRS] = new TH2F*[cfg -> _nCH[iDRS]];
    h2PulseSelectedWaveforms[iDRS] = new TH2F*[cfg -> _nCH[iDRS]];
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){
      char name[1000];
      char title[1000];
      sprintf(name, "h2PulseExcludedWaveforms_DRS_%d_CH_%d", iDRS, iCH);
      if(nEvents == 0) sprintf(title, "pulse fit excluded waveforms DRS %d CH %d, full statistics", iDRS, iCH);
      else sprintf(title, "pulse fit excluded waveforms DRS %d CH %d, %d events", iDRS, iCH, nEvents);
      h2PulseExcludedWaveforms[iDRS][iCH] = new TH2F(name, title, NDRSSAMPLES, 0, NDRSSAMPLES, PLOTWAVEFORMNBINSAMPLITUDE, PLOTWAVEFORMMINAMPLITUDE, PLOTWAVEFORMMAXAMPLITUDE); 
      sprintf(name, "h2PulseSelectedWaveforms_DRS_%d_CH_%d", iDRS, iCH);
      if(nEvents == 0) sprintf(title, "pulse fit selected waveforms DRS %d CH %d, full statistics", iDRS, iCH);
      else sprintf(title, "pulse fit selected waveforms DRS %d CH %d, %d events", iDRS, iCH, nEvents);
      h2PulseSelectedWaveforms[iDRS][iCH] = new TH2F(name, title, NDRSSAMPLES, 0, NDRSSAMPLES, PLOTWAVEFORMNBINSAMPLITUDE, PLOTWAVEFORMMINAMPLITUDE, PLOTWAVEFORMMAXAMPLITUDE); 
    }
  }

  ///////////////
  // reading data
  ///////////////
  cout << __PRETTY_FUNCTION__ << ": converting DUT data" << endl;    
  cout << __PRETTY_FUNCTION__ << ": reading data from " << fileNameIn << endl;  
  DRSDataClass *DRSData = new DRSDataClass();
  ifstream DUTFile(fileNameIn, ios::in | ios::binary);
  if(!DUTFile){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameIn << endl;
    return 1;
  }
  double t0 = 0.;
  bool endOfFile = false;
  eventNumber = 0;
  while(!endOfFile){

    if(DUTFile.eof()) break;
    
    endOfFile = goToEventHeader(DUTFile, cfg -> _debugLevel);
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
      readHeaderInfo(DUTFile, DRSData, cfg -> _debugLevel);      
      t0 = DRSData -> computeTimestamp();
      cout << __PRETTY_FUNCTION__ << ": t0 = " << t0 << endl;
    }

    // timestamp
    if(eventNumber != 0){
      readHeaderInfo(DUTFile, DRSData, cfg -> _debugLevel);
      //      time = (DRSData -> computeTimestamp() - t0) * TIMECONVERSION;
    }

    // timeline
    //    grTimeline -> SetPoint(grTimeline -> GetN(), eventNumber, time);
    
    // loop on DRSs
    unsigned int channelNumber = 0;
    for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){

      DRSNumber = iDRS;
      
      if(cfg -> _debugLevel >= 3){
	cout << __PRETTY_FUNCTION__ << ": reading DRS " << iDRS << endl;
	cout << __PRETTY_FUNCTION__ << ": cfg -> _nCH[" << iDRS << "] = " << cfg -> _nCH[iDRS] << endl;
      }

      if(iDRS != 0){ // maybe this happens only when the second DRS reads out 1 channel only
	DRSData -> boardNumber = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
	DRSData -> boardNumber = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
	DRSData -> boardNumber = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
	DRSData -> boardNumber = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
	DRSData -> boardNumber = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
	DRSData -> boardNumber = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
      }

      DRSData -> boardNumber = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
      DRSData -> boardNumber = readTwoBytes(DUTFile, false, cfg -> _debugLevel);      
      if(cfg -> _debugLevel >= 3) cout << __PRETTY_FUNCTION__  << ": boardNumber = " << DRSData -> boardNumber << endl;
      if(cfg -> _nCH[iDRS] != 1){ // maybe this happens only when the DRS reads out 1 channel only
	DRSData -> triggerCell = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
	DRSData -> triggerCell = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
	DRSData -> triggerCell = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
	DRSData -> triggerCell = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
	if(cfg -> _debugLevel >= 3) cout << __PRETTY_FUNCTION__  << ": triggerCell = " << DRSData -> triggerCell << endl;
      }
      
      // loop on channels
      for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){

	CHNumber = iCH;
	
	if(cfg -> _debugLevel >= 3) cout << __PRETTY_FUNCTION__ << ": channel " << iCH << endl;

	if(cfg -> _nCH[iDRS] != 1){ // maybe this happens only when the DRS reads out 1 channel only
	  DRSData -> scaler = readTwoBytes(DUTFile, true, cfg -> _debugLevel);
	  DRSData -> scaler = readTwoBytes(DUTFile, true, cfg -> _debugLevel);
	  DRSData -> scaler = readFourBytes(DUTFile, true, cfg -> _debugLevel);
	}
	
	// reading data
	if(cfg -> _debugLevel >= 3) cout << __PRETTY_FUNCTION__ << ": start waveform " << endl;
	double waveform[NDRSSAMPLES];
	for(unsigned int iSample=0; iSample<NDRSSAMPLES; iSample++){
          waveform[iSample] = readTwoBytes(DUTFile, false, cfg -> _debugLevel);
	}

	// here
	if(isSelected(eventNumber+shift, DRSNumber, CHNumber, event)){
	  for(unsigned int iSample=0; iSample<NDRSSAMPLES; iSample++){
	    h2PulseSelectedWaveforms[DRSNumber][CHNumber] -> Fill(iSample, waveform[iSample]);
	  }
	}
	else{
	  for(unsigned int iSample=0; iSample<NDRSSAMPLES; iSample++){
	    h2PulseExcludedWaveforms[DRSNumber][CHNumber] -> Fill(iSample, waveform[iSample]);
	  }
	}

      }
      
    }

    eventNumber++;
  }

  TLine *line = NULL;
  
  cc -> cd(1);
  cc -> cd(1) -> SetGridx();
  h2LinearExcludedWaveforms[0][0] -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms[0][0] -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms[0][0] -> Draw("colz");  
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][0].min, gPad->GetUymin(),
		   cfg -> _pulse[0][0].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][0].max, gPad->GetUymin(),
		   cfg -> _pulse[0][0].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(2);
  cc -> cd(2) -> SetGridx();
  h2LinearSelectedWaveforms[0][0] -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms[0][0] -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms[0][0] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][0].min, gPad->GetUymin(),
		   cfg -> _pulse[0][0].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][0].max, gPad->GetUymin(),
		   cfg -> _pulse[0][0].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(5);
  cc -> cd(5) -> SetGridx();
  h2LinearExcludedWaveforms[0][1] -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms[0][1] -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms[0][1] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][1].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][1].min, gPad->GetUymin(),
		   cfg -> _pulse[0][1].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][1].max, gPad->GetUymin(),
		   cfg -> _pulse[0][1].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(6);
  cc -> cd(6) -> SetGridx();
  h2LinearSelectedWaveforms[0][1] -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms[0][1] -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms[0][1] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][1].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();  
  line = new TLine(cfg -> _pulseT0[0][1].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][1].min, gPad->GetUymin(),
		   cfg -> _pulse[0][1].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][1].max, gPad->GetUymin(),
		   cfg -> _pulse[0][1].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(9);
  cc -> cd(9) -> SetGridx();
  h2LinearExcludedWaveforms[0][2] -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms[0][2] -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms[0][2] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][2].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();  
  line = new TLine(cfg -> _pulseT0[0][2].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][2].min, gPad->GetUymin(),
		   cfg -> _pulse[0][2].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][2].max, gPad->GetUymin(),
		   cfg -> _pulse[0][2].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(10);
  cc -> cd(10) -> SetGridx();
  h2LinearSelectedWaveforms[0][2] -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms[0][2] -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms[0][2] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][2].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][2].min, gPad->GetUymin(),
		   cfg -> _pulse[0][2].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][2].max, gPad->GetUymin(),
		   cfg -> _pulse[0][2].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(13);
  cc -> cd(13) -> SetGridx();
  h2LinearExcludedWaveforms[1][0] -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms[1][0] -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms[1][0] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[1][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[1][0].min, gPad->GetUymin(),
		   cfg -> _pulse[1][0].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[1][0].max, gPad->GetUymin(),
		   cfg -> _pulse[1][0].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(14);
  cc -> cd(14) -> SetGridx();
  h2LinearSelectedWaveforms[1][0] -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms[1][0] -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms[1][0] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[1][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[1][0].min, gPad->GetUymin(),
		   cfg -> _pulse[1][0].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[1][0].max, gPad->GetUymin(),
		   cfg -> _pulse[1][0].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(3);
  cc -> cd(3) -> SetGridx();
  h2PulseExcludedWaveforms[0][0] -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms[0][0] -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms[0][0] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();  
  line = new TLine(cfg -> _pulseT0[0][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][0].min, gPad->GetUymin(),
		   cfg -> _pulse[0][0].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][0].max, gPad->GetUymin(),
		   cfg -> _pulse[0][0].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(4);
  cc -> cd(4) -> SetGridx();
  h2PulseSelectedWaveforms[0][0] -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms[0][0] -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms[0][0] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();  
  line = new TLine(cfg -> _pulseT0[0][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][0].min, gPad->GetUymin(),
		   cfg -> _pulse[0][0].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][0].max, gPad->GetUymin(),
		   cfg -> _pulse[0][0].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(7);
  cc -> cd(7) -> SetGridx();
  h2PulseExcludedWaveforms[0][1] -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms[0][1] -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms[0][1] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][1].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();  
  line = new TLine(cfg -> _pulseT0[0][1].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][1].min, gPad->GetUymin(),
		   cfg -> _pulse[0][1].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][1].max, gPad->GetUymin(),
		   cfg -> _pulse[0][1].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(8);
  cc -> cd(8) -> SetGridx();
  h2PulseSelectedWaveforms[0][1] -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms[0][1] -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms[0][1] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][1].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][1].min, gPad->GetUymin(),
		   cfg -> _pulse[0][1].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][1].max, gPad->GetUymin(),
		   cfg -> _pulse[0][1].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(11);
  cc -> cd(11) -> SetGridx();
  h2PulseExcludedWaveforms[0][2] -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms[0][2] -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms[0][2] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][2].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][2].min, gPad->GetUymin(),
		   cfg -> _pulse[0][2].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][2].max, gPad->GetUymin(),
		   cfg -> _pulse[0][2].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(12);
  cc -> cd(12) -> SetGridx();
  h2PulseSelectedWaveforms[0][2] -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms[0][2] -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms[0][2] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][2].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][2].min, gPad->GetUymin(),
		   cfg -> _pulse[0][2].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[0][2].max, gPad->GetUymin(),
		   cfg -> _pulse[0][2].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(15);
  cc -> cd(15) -> SetGridx();
  h2PulseExcludedWaveforms[1][0] -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms[1][0] -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms[1][0] -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[1][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[1][0].min, gPad->GetUymin(),
		   cfg -> _pulse[1][0].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[1][0].max, gPad->GetUymin(),
		   cfg -> _pulse[1][0].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(16);
  cc -> cd(16) -> SetGridx();
  h2PulseSelectedWaveforms[1][0] -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms[1][0] -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms[1][0] -> Draw("colz");  
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[1][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[1][0].min, gPad->GetUymin(),
		   cfg -> _pulse[1][0].min, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulse[1][0].max, gPad->GetUymin(),
		   cfg -> _pulse[1][0].max, gPad->GetUymax());
  line -> SetLineColor(4);
  line -> SetLineWidth(2);
  line -> Draw();

  //////////////////
  // cleaning memory
  //////////////////
  cout << __PRETTY_FUNCTION__ << ": cleaning memory" << endl;
  
  // objects / files
  DUTFile.close();
  delete DRSData;
  delete cfg;
  event.clear();

  return 0;
}
