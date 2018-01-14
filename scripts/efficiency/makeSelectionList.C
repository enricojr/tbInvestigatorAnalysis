#include <iostream>
#include <fstream>
using namespace std;

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TColor.h>
#include <TLine.h>

#include "../aux/configClass.hh"

#define NCH 4

#define AMPLITUDENBINS 100
#define AMPLITUDEMIN 0
#define AMPLITUDEMAX 10000

#define T0NBINS 128
#define T0MIN 0
#define T0MAX 1024

#define RISETIMENBINS 100
#define RISETIMEMIN 0
#define RISETIMEMAX 100

#define REDCHI2NBINS 100
#define REDCHI2MIN 0
#define REDCHI2MAX 10

bool isSelected(const double pulseT0, const pair<double, double> T0Cut,
		const double pulseAmplitude, const pair<double, double> amplitudeCut,
		const double pulseRiseTime, const pair<double, double> riseTimeCut,
		const double pulseRedChi2, const pair<double, double> redChi2Cut){

  if(pulseT0 < T0Cut.first) return false;
  if(pulseT0 > T0Cut.second) return false;
  
  if(pulseAmplitude < amplitudeCut.first) return false;
  if(pulseAmplitude > amplitudeCut.second) return false;
  
  if(pulseRiseTime < riseTimeCut.first) return false;
  if(pulseRiseTime > riseTimeCut.second) return false;
  
  if(pulseRedChi2 < redChi2Cut.first) return false;
  if(pulseRedChi2 > redChi2Cut.second) return false;
  
  return true;
}

int makeSelectionList(const unsigned int run,
		      const bool lxplus = true){

  /////////////////////
  // setting file names
  /////////////////////
  cout << __PRETTY_FUNCTION__ << ": setting file names" << endl;
  char fileNameCuts[1000];
  if(lxplus) sprintf(fileNameCuts, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/Cuts/cuts_%06d.txt", run);
  else sprintf(fileNameCuts, "../../../output/cuts_%06d.txt", run);
  cout << __PRETTY_FUNCTION__ << ": fileNameCuts = " << fileNameCuts << endl;
  char fileNameIn[1000];
  if(lxplus) sprintf(fileNameIn, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/Synced/sync_%06d_000000.root", run);
  else sprintf(fileNameIn, "../../../output/sync_%06d_000000.root", run);
  cout << __PRETTY_FUNCTION__ << ": fileNameIn = " << fileNameIn << endl;
  char fileNameListSelected[1000];
  if(lxplus) sprintf(fileNameListSelected, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/Cuts/listSelected_%06d.txt", run);
  else sprintf(fileNameListSelected, "../../../output/listSelected_%06d.txt", run);
  cout << __PRETTY_FUNCTION__ << ": fileNameListSelected = " << fileNameListSelected << endl;
  char fileNameListExcluded[1000];
  if(lxplus) sprintf(fileNameListExcluded, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/Cuts/listExcluded_%06d.txt", run);
  else sprintf(fileNameListExcluded, "../../../output/listExcluded_%06d.txt", run);
  cout << __PRETTY_FUNCTION__ << ": fileNameListExcluded = " << fileNameListExcluded << endl;
  const char *treeName = "syncedData";
  cout << __PRETTY_FUNCTION__ << ": treeName = " << treeName << endl;

  ///////////////////////////
  // allocating distributions
  ///////////////////////////
  cout << __PRETTY_FUNCTION__ << ": allocating distributions" << endl;
  TH1F **h1Amplitude = new TH1F*[NCH];
  TH1F **h1T0 = new TH1F*[NCH];
  TH1F **h1RiseTime = new TH1F*[NCH];
  TH1F **h1RedChi2 = new TH1F*[NCH];
  TH1F **h1SelectionAmplitude = new TH1F*[NCH];
  TH1F **h1SelectionT0 = new TH1F*[NCH];
  TH1F **h1SelectionRiseTime = new TH1F*[NCH];
  TH1F **h1SelectionRedChi2 = new TH1F*[NCH];
  TH1F **h1SelectionAmplitude_riseTimeCut = new TH1F*[NCH];
  TH1F **h1SelectionRedChi2_riseTimeCut = new TH1F*[NCH];
  TH1F **h1SelectionRedChi2_riseTimeCut_amplitudeCut = new TH1F*[NCH];
  for(unsigned int iCH=0; iCH<NCH; iCH++){
    char name[1000];
    
    sprintf(name, "h1AmplitudeCH%d", iCH);
    h1Amplitude[iCH] = new TH1F(name, name, AMPLITUDENBINS, AMPLITUDEMIN, AMPLITUDEMAX);
    
    sprintf(name, "h1T0CH%d", iCH);
    h1T0[iCH] = new TH1F(name, name, T0NBINS, T0MIN, T0MAX);

    sprintf(name, "h1RiseTimeCH%d", iCH);
    h1RiseTime[iCH] = new TH1F(name, name, RISETIMENBINS, RISETIMEMIN, RISETIMEMAX);       
    
    sprintf(name, "h1RedChi2CH%d", iCH);
    h1RedChi2[iCH] = new TH1F(name, name, REDCHI2NBINS, REDCHI2MIN, REDCHI2MAX);

    sprintf(name, "h1SelectionAmplitudeCH%d", iCH);
    h1SelectionAmplitude[iCH] = new TH1F(name, name, AMPLITUDENBINS, AMPLITUDEMIN, AMPLITUDEMAX);
    
    sprintf(name, "h1SelectionT0CH%d", iCH);
    h1SelectionT0[iCH] = new TH1F(name, name, T0NBINS, T0MIN, T0MAX);

    sprintf(name, "h1SelectionRiseTimeCH%d", iCH);
    h1SelectionRiseTime[iCH] = new TH1F(name, name, RISETIMENBINS, RISETIMEMIN, RISETIMEMAX);       
    
    sprintf(name, "h1SelectionRedChi2CH%d", iCH);
    h1SelectionRedChi2[iCH] = new TH1F(name, name, REDCHI2NBINS, REDCHI2MIN, REDCHI2MAX);

    sprintf(name, "h1SelectionAmplitude_riseTimeCutCH%d", iCH);
    h1SelectionAmplitude_riseTimeCut[iCH] = new TH1F(name, name, AMPLITUDENBINS, AMPLITUDEMIN, AMPLITUDEMAX);

    sprintf(name, "h1SelectionRedChi2_riseTimeCutCH%d", iCH);
    h1SelectionRedChi2_riseTimeCut[iCH] = new TH1F(name, name, REDCHI2NBINS, REDCHI2MIN, REDCHI2MAX);

    sprintf(name, "h1SelectionRedChi2_riseTimeCut_amplitudeCutCH%d", iCH);
    h1SelectionRedChi2_riseTimeCut_amplitudeCut[iCH] = new TH1F(name, name, REDCHI2NBINS, REDCHI2MIN, REDCHI2MAX);
}
  
  ///////////////
  // loading cuts
  ///////////////
  cout << __PRETTY_FUNCTION__ << ": loading cuts" << endl;
  pair<double, double> T0Cut[NCH];
  pair<double, double> amplitudeCut[NCH];
  pair<double, double> riseTimeCut[NCH];
  pair<double, double> redChi2Cut[NCH];
  ifstream fileCuts(fileNameCuts);
  if(!fileCuts){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileNameCuts << endl;
    return 1;
  }
  string text = "";
  getline(fileCuts, text);
  for(unsigned int iCH=0; iCH<NCH; iCH++){
    getline(fileCuts, text);
    sscanf(text.c_str(), "%lf %lf", &T0Cut[iCH].first, &T0Cut[iCH].second);
    cout << __PRETTY_FUNCTION__ << ": T0Cut[" << iCH << "] = " << T0Cut[iCH].first << ", " << T0Cut[iCH].second << endl;
  }
  getline(fileCuts, text);
  for(unsigned int iCH=0; iCH<NCH; iCH++){
    getline(fileCuts, text);
    sscanf(text.c_str(), "%lf %lf", &riseTimeCut[iCH].first, &riseTimeCut[iCH].second);
    cout << __PRETTY_FUNCTION__ << ": riseTimeCut[" << iCH << "] = " << riseTimeCut[iCH].first << ", " << riseTimeCut[iCH].second << endl;
  }
  getline(fileCuts, text);
  for(unsigned int iCH=0; iCH<NCH; iCH++){
    getline(fileCuts, text);
    sscanf(text.c_str(), "%lf %lf", &amplitudeCut[iCH].first, &amplitudeCut[iCH].second);
    cout << __PRETTY_FUNCTION__ << ": amplitudeCut[" << iCH << "] = " << amplitudeCut[iCH].first << ", " << amplitudeCut[iCH].second << endl;
  }
  getline(fileCuts, text);
  for(unsigned int iCH=0; iCH<NCH; iCH++){
    getline(fileCuts, text);
    sscanf(text.c_str(), "%lf %lf", &redChi2Cut[iCH].first, &redChi2Cut[iCH].second);
    cout << __PRETTY_FUNCTION__ << ": redChi2Cut[" << iCH << "] = " << redChi2Cut[iCH].first << ", " << redChi2Cut[iCH].second << endl;
  }
  fileCuts.close();

  /////////////////////////////
  // opening selected list file
  /////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": opening selected list file" << endl;
  ofstream fileListSelected(fileNameListSelected);
  if(!fileListSelected){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileNameListSelected << endl;
    return 1;
  }
  
  /////////////////////////////
  // opening excluded list file
  /////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": opening excluded list file" << endl;
  ofstream fileListExcluded(fileNameListExcluded);
  if(!fileListExcluded){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileNameListExcluded << endl;
    return 1;
  }
  
  /////////////////////
  // opening input file
  /////////////////////
  cout << __PRETTY_FUNCTION__ << ": opening input file" << endl;
  TFile *fileIn = TFile::Open(fileNameIn);
  if(fileIn == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileNameIn << endl;
    return 1;
  }

  ///////////////
  // getting tree
  ///////////////
  cout << __PRETTY_FUNCTION__ << ": getting tree" << endl;
  TTree *tree = (TTree *) fileIn -> Get(treeName);
  if(tree == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << treeName << endl;
    return 1;
  }
  Double_t xTrack = 0.;
  tree -> SetBranchAddress("xTrack", &xTrack);
  Double_t yTrack = 0.;
  tree -> SetBranchAddress("yTrack", &yTrack);
  Float_t pulseAmplitude = 0.;
  tree -> SetBranchAddress("pulseAmplitude", &pulseAmplitude);
  Float_t pulseT0 = 0.;
  tree -> SetBranchAddress("pulseT0", &pulseT0);
  Float_t pulseRiseTime = 0.;
  tree -> SetBranchAddress("pulseRiseTime", &pulseRiseTime);
  Float_t pulseRedChi2 = 0.;
  tree -> SetBranchAddress("pulseRedChi2", &pulseRedChi2);
  Int_t DRSNumber = 0;
  tree -> SetBranchAddress("DRSNumber", &DRSNumber);
  Int_t CHNumber = 0;
  tree -> SetBranchAddress("CHNumber", &CHNumber);
  Int_t FrameNumber = 0;
  tree -> SetBranchAddress("FrameNumber", &FrameNumber);

  //////////
  // looping
  //////////
  cout << __PRETTY_FUNCTION__ << ": looping" << endl;
  const unsigned int nEntries = tree -> GetEntries();
  for(unsigned int iEntry=0; iEntry<nEntries; iEntry+=5){    
    tree -> GetEntry(iEntry);

    // looking for a hit
    bool thereIsAHit = false;
    for(unsigned int i=0; i<5; i++){
      tree -> GetEntry(iEntry+i);
      if(pulseAmplitude>0){
        thereIsAHit = true;
	break;
      }
      else continue;
    }
    
    // if there is a hit
    if(thereIsAHit){

      // compute cluster charge
      double charge = 0.;
      for(unsigned int i=0; i<5; i++){
	tree -> GetEntry(iEntry+i);
	if(pulseAmplitude>0) charge += pulseAmplitude;
	else continue;
      }

      // selecting
      for(unsigned int i=0; i<5; i++){
	tree -> GetEntry(iEntry+i);
	if(pulseAmplitude==0) continue;
	unsigned int index = CHNumber;
	if(DRSNumber == 1) index = 3;	
	if(isSelected(pulseT0, T0Cut[index],
		      pulseAmplitude, amplitudeCut[index],
		      pulseRiseTime, riseTimeCut[index],
		      pulseRedChi2, redChi2Cut[index])){
	  fileListSelected << FrameNumber << " " << DRSNumber << " " << CHNumber << endl;
	}
	else{
	  fileListExcluded << FrameNumber << " " << DRSNumber << " " << CHNumber << endl;
	}

      }

      // filling distributions
      for(unsigned int i=0; i<5; i++){
	tree -> GetEntry(iEntry+i);
	unsigned int index = CHNumber;
	if(DRSNumber == 1) index = 3;
	if(pulseAmplitude>0){
	  
	  h1Amplitude[index] -> Fill(pulseAmplitude);
	  h1T0[index] -> Fill(pulseT0);
	  h1RiseTime[index] -> Fill(pulseRiseTime);
	  h1RedChi2[index] -> Fill(pulseRedChi2);
	  
	  if(pulseT0>T0Cut[index].first && pulseT0<T0Cut[index].second) h1SelectionT0[index] -> Fill(pulseT0);
	  if(pulseRiseTime>riseTimeCut[index].first && pulseRiseTime<riseTimeCut[index].second) h1SelectionRiseTime[index] -> Fill(pulseRiseTime);
	  if(pulseAmplitude>amplitudeCut[index].first && pulseAmplitude<amplitudeCut[index].second){
	    h1SelectionAmplitude[index] -> Fill(pulseAmplitude);
	    if(pulseRiseTime>riseTimeCut[index].first && pulseRiseTime<riseTimeCut[index].second){
	      h1SelectionAmplitude_riseTimeCut[index] -> Fill(pulseAmplitude);
	    }
	  }
	  if(pulseRedChi2>redChi2Cut[index].first && pulseRedChi2<redChi2Cut[index].second){
	    h1SelectionRedChi2[index] -> Fill(pulseRedChi2);
	    if(pulseRiseTime>riseTimeCut[index].first && pulseRiseTime<riseTimeCut[index].second){
	      h1SelectionRedChi2_riseTimeCut[index] -> Fill(pulseRedChi2);
	      if(pulseAmplitude>amplitudeCut[index].first && pulseAmplitude<amplitudeCut[index].second){
		h1SelectionRedChi2_riseTimeCut_amplitudeCut[index] -> Fill(pulseRedChi2);
	      }
	    }
	  }
	  
	}
	else continue;
      }
    }
    else continue;
    
  }
  
  ////////////////
  // closing files
  ////////////////
  cout << __PRETTY_FUNCTION__ << ": closing files" << endl;
  fileIn -> Close();
  delete fileIn;
  fileListSelected.close();
  fileListExcluded.close();

  ///////////
  // plotting
  ///////////
  cout << __PRETTY_FUNCTION__ << ": plotting" << endl;
  TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 2000, 1000);
  const unsigned int nHist = 4;
  cc -> Divide(nHist, NCH);

  TLine *line = NULL;
  for(unsigned int iCH=0; iCH<NCH; iCH++){
    
    cc -> cd(1+iCH*nHist);
    h1T0[iCH] -> SetLineColor(1);
    h1T0[iCH] -> SetLineWidth(2);
    h1T0[iCH] -> Draw();
    h1SelectionT0[iCH] -> SetLineColor(1);
    h1SelectionT0[iCH] -> SetFillColor(5);
    h1SelectionT0[iCH] -> Draw("same");

    cc -> cd(2+iCH*nHist);
    h1RiseTime[iCH] -> SetLineColor(1);
    h1RiseTime[iCH] -> SetLineWidth(2);
    h1RiseTime[iCH] -> Draw();
    h1SelectionRiseTime[iCH] -> SetLineColor(1);
    h1SelectionRiseTime[iCH] -> SetFillColor(5);
    h1SelectionRiseTime[iCH] -> Draw("same");

    cc -> cd(3+iCH*nHist);
    h1Amplitude[iCH] -> SetLineColor(1);
    h1Amplitude[iCH] -> SetLineWidth(2);
    h1Amplitude[iCH] -> Draw();
    h1SelectionAmplitude[iCH] -> SetLineColor(1);
    h1SelectionAmplitude[iCH] -> SetFillColor(5);
    h1SelectionAmplitude[iCH] -> Draw("same");
    h1SelectionAmplitude_riseTimeCut[iCH] -> SetLineColor(1);
    h1SelectionAmplitude_riseTimeCut[iCH] -> SetFillColor(3);
    h1SelectionAmplitude_riseTimeCut[iCH] -> Draw("same");

    cc -> cd(4+iCH*nHist);
    cc -> cd(4+iCH*nHist) -> SetLogy();
    h1RedChi2[iCH] -> SetLineColor(1);
    h1RedChi2[iCH] -> SetLineWidth(2);
    h1RedChi2[iCH] -> Draw();
    h1SelectionRedChi2[iCH] -> SetLineColor(1);
    h1SelectionRedChi2[iCH] -> SetFillColor(5);
    h1SelectionRedChi2[iCH] -> Draw("same");
    h1SelectionRedChi2_riseTimeCut[iCH] -> SetLineColor(1);
    h1SelectionRedChi2_riseTimeCut[iCH] -> SetFillColor(3);
    h1SelectionRedChi2_riseTimeCut[iCH] -> Draw("same");
    h1SelectionRedChi2_riseTimeCut_amplitudeCut[iCH] -> SetLineColor(1);
    h1SelectionRedChi2_riseTimeCut_amplitudeCut[iCH] -> SetFillColor(2);
    h1SelectionRedChi2_riseTimeCut_amplitudeCut[iCH] -> Draw("same");

  }
  
  //////////////////
  // clearing memory
  //////////////////
  cout << __PRETTY_FUNCTION__ << ": clearing memory" << endl;

  return 0;
}
