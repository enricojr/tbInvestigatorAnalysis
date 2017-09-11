#include <iostream>
using namespace std;

#include "../aux/progressBar.h"

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TGraph.h>

#define DZ 270000.
#define TIMECONVERSION 0.000000025

int extractInfo(const char *fileNameIn,
		const char *fileNameOut){

  // opening track tree
  cout << __PRETTY_FUNCTION__ << ": opening track tree from " << fileNameIn << endl;
  TFile *fileIn = TFile::Open(fileNameIn);
  if(fileIn == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameIn << endl;
    return 1;
  }
  TTree *tTracks = (TTree *) fileIn -> Get("Tracks");
  if(tTracks == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot load Tracks tree" << endl;
    return 1;
  }
  Int_t NTracks = 0;
  tTracks -> SetBranchAddress("NTracks", &NTracks);
  Double_t Chi2[NTracks];
  tTracks -> SetBranchAddress("Chi2", &Chi2);
  Int_t Dof[NTracks];
  tTracks -> SetBranchAddress("Dof", &Dof);
  Double_t X[NTracks];
  tTracks -> SetBranchAddress("X", &X);
  Double_t Y[NTracks];
  tTracks -> SetBranchAddress("Y", &Y);
  Double_t SlopeX[NTracks];
  tTracks -> SetBranchAddress("SlopeX", &SlopeX);
  Double_t SlopeY[NTracks];
  tTracks -> SetBranchAddress("SlopeY", &SlopeY);

  // opening event tree
  cout << __PRETTY_FUNCTION__ << ": opening event tree from " << fileNameIn << endl;
  TTree *tEvent = (TTree *) fileIn -> Get("Event");
  if(tEvent == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot load Event tree" << endl;
    return 1;
  }
  ULong64_t TimeStamp = 0;
  tEvent -> SetBranchAddress("TimeStamp", &TimeStamp);
  ULong64_t FrameNumber = 0;
  tEvent -> SetBranchAddress("FrameNumber", &FrameNumber);

  // getting number of entries
  cout << __PRETTY_FUNCTION__ << ": getting number of entries" << endl;
  unsigned int nEntries = 0;
  if(tEvent -> GetEntries() == tTracks -> GetEntries()){
    nEntries = tEvent -> GetEntries();
  }
  else{
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - Tracks tree and Event tree have different number of entries" << endl;
    return 1;
  }
  cout << __PRETTY_FUNCTION__ << ": nEntries = " << nEntries << endl;

  // getting T0
  cout << __PRETTY_FUNCTION__ << ": getting T0" << endl;
  tEvent -> GetEntry(0);
  const ULong64_t T0 = TimeStamp;
  cout << __PRETTY_FUNCTION__ << ": T0 = " << T0 << endl;

  // opening output tree
  cout << __PRETTY_FUNCTION__ << ": opening output trees in " << fileNameOut << endl;
  TFile *fileOut = TFile::Open(fileNameOut, "RECREATE");
  if(fileOut == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameOut << endl;
    return 1;
  }
  TTree *tOut = new TTree("trackOnDUT", "trackOnDUT");
  tOut -> Branch("eventID", &FrameNumber);
  Double_t time = 0;
  tOut -> Branch("time", &time);
  Double_t x;
  tOut -> Branch("x", &x);
  Double_t y;
  tOut -> Branch("y", &y);
  Double_t redChi2;
  tOut -> Branch("redChi2", &redChi2);
  ULong64_t frame = 0;
  tOut -> Branch("frame", &frame);

  // allocating timeline plot
  cout << __PRETTY_FUNCTION__ << ": allocating timeline plot" << endl;
  TGraph *grTimeline = new TGraph();
  grTimeline -> SetName("grTimeline");
  grTimeline -> SetTitle("timeline");
  
  // extracting
  cout << __PRETTY_FUNCTION__ << ": extracting" << endl;
  for(unsigned int iEntry=0; iEntry<nEntries; iEntry++){
    tTracks -> GetEntry(iEntry);
    tEvent -> GetEntry(iEntry);
    time = (TimeStamp - T0) * TIMECONVERSION;
    frame = FrameNumber;
    grTimeline -> SetPoint(grTimeline -> GetN(), iEntry, time);
    for(int iTrack=0; iTrack<NTracks; iTrack++){
      x = X[iTrack] + SlopeX[iTrack] * DZ;
      y = Y[iTrack] + SlopeY[iTrack] * DZ;
      redChi2 = Chi2[iTrack] / Dof[iTrack];
      tOut -> Fill();
    }
    loadBar(iEntry, nEntries);
  }
  grTimeline -> Write();
  tOut -> Write();

  // cleaning memory
  cout << __PRETTY_FUNCTION__ << ": cleaning memory" << endl;
  // delete grTimeline;
  // delete tTracks;
  // delete tEvent;
  // delete tOut;
  // fileIn -> Close();
  // fileOut -> Close();
  // delete fileIn;
  // delete fileOut;
  
  return 0;
}
