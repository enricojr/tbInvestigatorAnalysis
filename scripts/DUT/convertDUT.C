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

/*****************/
/*****************/
/* main function */
/*****************/
/*****************/

int convertDUT(const char *fileNameIn,
	       const char *fileNameOut,
	       const char *fileNameCfg,
	       const unsigned int nEvents,
	       const unsigned int startEvent = 0){

  gStyle -> SetOptFit(1);
  
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

  ///////////////////////
  // creating output tree
  ///////////////////////
  cout << __PRETTY_FUNCTION__ << ": creating output tree" << endl;
  TFile *fileOut = TFile::Open(fileNameOut, "RECREATE");
  if(!fileOut){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameOut << endl;
    return 1;
  }
  TTree *tree = new TTree("DUTTree", "DUTTree");
  Int_t eventNumber = 0;
  tree -> Branch("eventNumber", &eventNumber);
  Double_t time = 0;
  tree -> Branch("time", &time);
  Int_t DRSNumber = 0;
  tree -> Branch("DRSNumber", &DRSNumber);
  Int_t CHNumber = 0;
  tree -> Branch("CHNumber", &CHNumber);
  Float_t linearOffset = 0.;
  tree -> Branch("linearOffset", &linearOffset);
  Float_t linearSlope = 0.;
  tree -> Branch("linearSlope", &linearSlope);
  Float_t linearRedChi2 = 0.;
  tree -> Branch("linearRedChi2", &linearRedChi2);
  Float_t pulseOffset = 0.;
  tree -> Branch("pulseOffset", &pulseOffset);
  Float_t pulseSlope = 0.;
  tree -> Branch("pulseSlope", &pulseSlope);
  Float_t pulseAmplitude = 0.;
  tree -> Branch("pulseAmplitude", &pulseAmplitude);
  Float_t pulseT0 = 0.;
  tree -> Branch("pulseT0", &pulseT0);
  Float_t pulseRiseTime = 0.;
  tree -> Branch("pulseRiseTime", &pulseRiseTime);
  Float_t pulseDecay = 0.;
  tree -> Branch("pulseDecay", &pulseDecay);
  Float_t pulseCharge = 0.;
  tree -> Branch("pulseCharge", &pulseCharge);
  Float_t pulseRedChi2 = 0.;
  tree -> Branch("pulseRedChi2", &pulseRedChi2);  

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

  ///////////////////////////////
  // initializing waveform fitter
  ///////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": initializing waveform fitter" << endl;  
  fitWaveformClass *fitWaveform = new fitWaveformClass(NDRSSAMPLES); 
  
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
      time = (DRSData -> computeTimestamp() - t0) * TIMECONVERSION;
    }

    // timeline
    grTimeline -> SetPoint(grTimeline -> GetN(), eventNumber, time);
    
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

	// // converting waveform to mV
	// for(unsigned int iSample=0; iSample<NDRSSAMPLES; iSample++){
	//   waveform[iSample] = (DRSData -> rangeCenter - 500.) + waveform[iSample] * 500./32768.;
	// }

	// fitting waveform
	if(iDRS == cfg -> _resetCH.first && iCH == cfg -> _resetCH.second){
	  fitWaveform -> reset();
	}
	else{
	  fitWaveform -> fit(waveform, cfg, eventNumber, iDRS, iCH, h1Noise);
	  linearSlope = fitWaveform -> getLinearSlope();
	  linearOffset = fitWaveform -> getLinearOffset();
	  linearRedChi2 = fitWaveform -> getLinearRedChi2();
	  pulseSlope = fitWaveform -> getPulseSlope();
	  pulseOffset = fitWaveform -> getPulseOffset();
	  pulseAmplitude = fitWaveform -> getPulseAmplitude();
	  pulseT0 = fitWaveform -> getPulseT0();
	  pulseRiseTime = fitWaveform -> getPulseRiseTime();
	  pulseDecay = fitWaveform -> getPulseDecay();
	  pulseCharge = fitWaveform -> getPulseCharge();
	  pulseRedChi2 = fitWaveform -> getPulseRedChi2();
	  fitWaveform -> reset();	  
	}

	// writing data, filling plots
	if(iDRS == cfg -> _resetCH.first && iCH == cfg -> _resetCH.second){
	  // reset waveforms
	  for(unsigned int iSample=0; iSample<NDRSSAMPLES; iSample++){
	    h2ResetWaveforms -> Fill(iSample, waveform[iSample]);
	  }
	}
	else{
	  h1LinearRedChi2 -> Fill(linearRedChi2);
	  if(linearRedChi2 > cfg -> _linearRedChi2Threshold){ // selection from linear fit
	    h1LinearRedChi2Selected -> Fill(linearRedChi2);
	    for(unsigned int iSample=0; iSample<NDRSSAMPLES; iSample++){
	      h2LinearSelectedWaveforms[iDRS][iCH] -> Fill(iSample, waveform[iSample]);
	    }
	    if(pulseT0 > cfg -> _pulseT0Range[iDRS][iCH].min && pulseT0 < cfg -> _pulseT0Range[iDRS][iCH].max // selection from pulse fit
	       &&
	       pulseRiseTime > cfg -> _pulseRiseTimeRange.min && pulseRiseTime < cfg -> _pulseRiseTimeRange.max
	       &&
	       pulseRedChi2 < cfg -> _pulseRedChi2Threshold){
	      for(unsigned int iSample=0; iSample<NDRSSAMPLES; iSample++){
		h2PulseSelectedWaveforms[iDRS][iCH] -> Fill(iSample, waveform[iSample]);
	      }
	    }
	    else{
	      for(unsigned int iSample=0; iSample<NDRSSAMPLES; iSample++){
		h2PulseExcludedWaveforms[iDRS][iCH] -> Fill(iSample, waveform[iSample]);
	      }
	    }
	  }
	  else{
	    for(unsigned int iSample=0; iSample<NDRSSAMPLES; iSample++){
	      h2LinearExcludedWaveforms[iDRS][iCH] -> Fill(iSample, waveform[iSample]);
	    }
	  }
	}

	// filling tree
	tree -> Fill();	
	channelNumber++;

	// resetting parameters
	linearSlope = 0.;
	linearOffset = 0.;
	linearRedChi2 = 0.;
	pulseSlope = 0.;
	pulseOffset = 0.;
	pulseAmplitude = 0.;
	pulseT0 = 0.;
	pulseRiseTime = 0.;
	pulseDecay = 0.;
	pulseCharge = 0.;
	pulseRedChi2 = 0.;
      }
      
    }

    eventNumber++;
  }

  //////////////////
  // computing noise
  //////////////////
  cout << __PRETTY_FUNCTION__ << ": computing noise" << endl;
  double **noise = new double*[cfg -> _nDRS];
  double **noiseError = new double*[cfg -> _nDRS];
  //  TF1 ***fNoise = new TF1**[cfg -> _nDRS];
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    //    fNoise[iDRS] = new TF1*[cfg -> _nCH[iDRS]];
    noise[iDRS] = new double[cfg -> _nCH[iDRS]];
    noiseError[iDRS] = new double[cfg -> _nCH[iDRS]];
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){
      // fit is not Gaussian...
      // if re-enabling the fit, remember to uncomment the delete statements for fNoise
      // char name[1000];
      // sprintf(name, "fNoise_%d_%d", iDRS, iCH);
      // fNoise[iDRS][iCH] = new TF1(name, "[0] * exp(-((x-[1])*(x-[1]))/(2.*[2]*[2])) / (sqrt(2.*3.14156)*[2])", PLOTNOISEMIN, PLOTNOISEMAX);
      // fNoise[iDRS][iCH] -> SetParNames("scale", "mean", "sigma");
      // fNoise[iDRS][iCH] -> SetParameter(1, 0);
      // fNoise[iDRS][iCH] -> SetParameter(2, h1Noise[iDRS][iCH] -> GetRMS());
      // h1Noise[iDRS][iCH] -> Fit(name, "Q && R");
      //      noise[iDRS][iCH] = fNoise[iDRS][iCH] -> GetParameter(2);
      //      noiseError[iDRS][iCH] = fNoise[iDRS][iCH] -> GetParError(2);
      noise[iDRS][iCH] = h1Noise[iDRS][iCH] -> GetRMS();
      noiseError[iDRS][iCH] = 0.;
    }
  }
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){
      cout << __PRETTY_FUNCTION__ << ": noise[" << iDRS << "][" << iCH << "] = (" << noise[iDRS][iCH] << "+-" << noiseError[iDRS][iCH] << ")" << endl;
    }
  }
  
  ///////////////
  // writing tree
  ///////////////
  cout << __PRETTY_FUNCTION__ << ": writing tree" << endl;
  tree -> Write();

  ////////////////
  // writing plots
  ////////////////
  cout << __PRETTY_FUNCTION__ << ": writing plots" << endl;
  gStyle -> SetPalette(55);
  gStyle -> SetOptStat(0);

  // timeline
  grTimeline -> Write();

  // noise
  TCanvas *cNoise = new TCanvas("cNoise", "noise", 0, 0, 1000, 1000);
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){
      h1Noise[iDRS][iCH] -> Write();
    }
  }
  
  // reset selection
  TCanvas *cResetWaveforms = new TCanvas("cResetWaveforms", "cResetWaveforms", 0, 0, 1600, 800);
  h2ResetWaveforms -> GetXaxis() -> SetTitle("time bin");
  h2ResetWaveforms -> GetYaxis() -> SetTitle("amplitude [a.u.]");
  h2ResetWaveforms -> Draw("col");
  cResetWaveforms -> SetLogz();
  cResetWaveforms -> Write();

  // linear fit selection
  TCanvas *cLinearRedChi2 = new TCanvas("cLinearRedChi2", "cLinearRedChi2", 0, 0, 1000, 1000);
  cLinearRedChi2 -> SetLogy();
  h1LinearRedChi2 -> SetLineColor(1);
  h1LinearRedChi2 -> SetLineWidth(2);
  h1LinearRedChi2 -> GetXaxis() -> SetTitle("#chi^{2}_{red}");
  h1LinearRedChi2 -> GetYaxis() -> SetTitle("#frac{dN}{d#chi^{2}_{red}}");
  h1LinearRedChi2 -> Draw();
  h1LinearRedChi2Selected -> SetFillColor(5);
  h1LinearRedChi2Selected -> Draw("same");
  h1LinearRedChi2 -> Draw("same");
  cLinearRedChi2 -> Write();
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){
      h2LinearExcludedWaveforms[iDRS][iCH] -> Write();
      h2LinearSelectedWaveforms[iDRS][iCH] -> Write();
    }
  }
  
  // pulse fit selection
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){
      h2PulseExcludedWaveforms[iDRS][iCH] -> Write();
      h2PulseSelectedWaveforms[iDRS][iCH] -> Write();
    }
  }
  
  //////////////////
  // cleaning memory
  //////////////////
  cout << __PRETTY_FUNCTION__ << ": cleaning memory" << endl;
  // timeline
  delete grTimeline;
  // noise
  delete cNoise;
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){
      delete h1Noise[iDRS][iCH];
      //      delete fNoise[iDRS][iCH];
    }
    delete[] h1Noise[iDRS];
    //    delete[] fNoise[iDRS];
  }
  delete[] h1Noise;
  //  delete[] fNoise;
  // reset waveforms
  delete cResetWaveforms;
  delete h2ResetWaveforms;
  // linear fit
  delete cLinearRedChi2;
  delete h1LinearRedChi2;
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){      
      delete h2LinearExcludedWaveforms[iDRS][iCH];
      delete h2LinearSelectedWaveforms[iDRS][iCH];      
    }
    delete[] h2LinearExcludedWaveforms[iDRS];
    delete[] h2LinearSelectedWaveforms[iDRS];    
  }
  delete[] h2LinearExcludedWaveforms;
  delete[] h2LinearSelectedWaveforms;
  // pulse fit
  for(unsigned int iDRS=0; iDRS<cfg -> _nDRS; iDRS++){
    for(unsigned int iCH=0; iCH<cfg -> _nCH[iDRS]; iCH++){
      delete h2PulseExcludedWaveforms[iDRS][iCH];
      delete h2PulseSelectedWaveforms[iDRS][iCH];      
    }
    delete[] h2PulseExcludedWaveforms[iDRS];
    delete[] h2PulseSelectedWaveforms[iDRS];    
  }
  delete[] h2PulseExcludedWaveforms;
  delete[] h2PulseSelectedWaveforms;
  // objects / files
  delete fitWaveform;
  DUTFile.close();
  delete DRSData;
  delete tree;
  fileOut -> Close();
  delete fileOut;
  delete cfg;

  cout << __PRETTY_FUNCTION__ << ": output file: " << fileNameOut << endl;
  
  return 0;
}
