#ifndef SUMMARY_HH
#define SUMMARY_HH

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <set>
#include <sstream>
#include <vector>
using namespace std;

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH2.h>
#include <TH1.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TColor.h>
#include <TPad.h>
#include <TLine.h>
#include <TF1.h>

#include "../aux/configClass.hh"

#define NDUTCHANNELS 5
#define TIMECONVERSION 0.000000025 // to convert telescope time
#define NSAMPLES 1024
#define H1OFFSETMIN 20000
#define H1OFFSETMAX 40000
#define H1OFFSETBINS 100
#define H1SLOPEMIN -5
#define H1SLOPEMAX 5
#define H1SLOPEBINS 100
#define H1AMPLITUDEMIN 0
#define H1AMPLITUDEMAX 10000
#define H1AMPLITUDEBINS 100
#define H1T0MIN 0
#define H1T0MAX 1024
#define H1T0BINS 1024
#define H1RISETIMEMIN 0
#define H1RISETIMEMAX 100
#define H1RISETIMEBINS 100
#define H2PULSETIMEMIN 0
#define H2PULSETIMEMAX 1024
#define H2PULSETIMEBINS 1024
#define H2PULSEAMPLITUDEMIN 0
#define H2PULSEAMPLITUDEMAX 65536
#define H2PULSEAMPLITUDEBINS 256
#define XMIN -8000
#define XMAX 4000
#define XBINS 1500
#define YMIN -6000
#define YMAX 6000
#define YBINS 1500
#define XERRORMIN 0
#define XERRORMAX 100
#define XERRORBINS 100
#define YERRORMIN 0
#define YERRORMAX 100
#define YERRORBINS 100
#define NTRACKSMIN 1
#define NTRACKSMAX 5
#define NTRACKSBINS 4

template <class _II, class _OI>
static _OI escapeSomeChars(const _II inIt, const _II endIt, _OI outIt)
{
  for (_II it=inIt; it!=endIt; ++it)
    switch (*it)
      {
      case '\0': outIt++ = '\\'; outIt++ = '0'; break;
      case '\n': outIt++ = '\\'; outIt++ = 'n'; break;
      case '_': outIt++ = '\\'; outIt++ = '_'; break;
      case '"' : 
      case '$' : 
      default  : outIt++ = *it;
      }

  return outIt;
}

static std::string escapeSomeChars(const std::string& input)
{
  std::ostringstream os;
  escapeSomeChars(input.begin(), input.end(), std::ostream_iterator<char>(os));
  return os.str();
}

namespace /*anon*/ {
  struct rawchar {   // helper - see e.g. http://bytes.com/topic/c/answers/436124-copy-istream_iterator-question
    char _c; rawchar(char c=0) : _c(c) {} 
    operator const char&() const { return _c; }
    friend std::istream& operator>>(std::istream& is, rawchar& out) { return is.get(out._c); }
  };
}

int getNumberOfDUTHits(const char *fileNameDUTConversion){
  int nHits = -1;
  TFile *file = TFile::Open(fileNameDUTConversion);
  if(file == NULL) cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open DUT conversion file" << endl;
  else{
    TTree *tree = (TTree *) file -> Get("DUTTree");
    if(tree == NULL) cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open DUT conversion tree" << endl;
    else{
      nHits = tree -> GetEntries() / NDUTCHANNELS;
      delete tree;
    }
    file -> Close();
    delete file;
  }

  return nHits;
}

int getNumberOfTelescopeHits(const char *fileNameTelescopeConversion){
  int nHits = -1;
  TFile *file = TFile::Open(fileNameTelescopeConversion);
  if(file == NULL) cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open Telescope conversion file" << endl;
  else{
    TTree *tree = (TTree *) file -> Get("Event");
    if(tree == NULL) cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open Telescope conversion tree" << endl;
    else{
      nHits = tree -> GetEntries();
      delete tree;
    }
    file -> Close();
    delete file;
  }

  return nHits;
}

int writeDUTConversion(configClass *cfg,
		       ofstream &fileLatex, 
		       const char *fileNameDUTConversion, 
		       const string plotPath){

  cout << __PRETTY_FUNCTION__ << ": processing " << fileNameDUTConversion << endl;

  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{DUT conversion}" << endl;

  gStyle -> SetOptTitle(1);

  TFile *file = TFile::Open(fileNameDUTConversion);
  if(file == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open DUT conversion file" << endl;
    return 1;
  }

  TLine *line = NULL;
  TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 2000, 1000);

  vector<pair<unsigned int, unsigned int> > channel;
  channel.push_back(pair<unsigned int, unsigned int> (0, 0));
  channel.push_back(pair<unsigned int, unsigned int> (0, 1));
  channel.push_back(pair<unsigned int, unsigned int> (0, 2));
  channel.push_back(pair<unsigned int, unsigned int> (1, 0));

  string plotNameLinearExcludedPng[channel.size()];
  string plotNameLinearExcludedRoot[channel.size()];
  string plotNameLinearSelectedPng[channel.size()];
  string plotNameLinearSelectedRoot[channel.size()];
  string plotNamePulseExcludedPng[channel.size()];
  string plotNamePulseExcludedRoot[channel.size()];
  string plotNamePulseSelectedPng[channel.size()];
  string plotNamePulseSelectedRoot[channel.size()];

  // hit selection plots
  gStyle -> SetPadTopMargin(0.1);
  gStyle -> SetPadRightMargin(0.1);
  gStyle -> SetPadBottomMargin(0.1);
  gStyle -> SetPadLeftMargin(0.1);
  for(unsigned int iChannel=0; iChannel<channel.size(); iChannel++){

    stringstream DRS;
    DRS << channel[iChannel].first;
    stringstream CH;
    CH << channel[iChannel].second;    

    plotNameLinearExcludedPng[iChannel] = "/DUTConversion_h2LinearExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".png";
    plotNameLinearExcludedRoot[iChannel] = "/DUTConversion_h2LinearExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".root";
    plotNameLinearSelectedPng[iChannel] = "/DUTConversion_h2LinearSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".png";
    plotNameLinearSelectedRoot[iChannel] = "/DUTConversion_h2LinearSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".root";
    plotNamePulseExcludedPng[iChannel] = "/DUTConversion_h2PulseExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".png";
    plotNamePulseExcludedRoot[iChannel] = "/DUTConversion_h2PulseExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".root";
    plotNamePulseSelectedPng[iChannel] = "/DUTConversion_h2PulseSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".png";
    plotNamePulseSelectedRoot[iChannel] = "/DUTConversion_h2PulseSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".root";

    TH2F *h2LinearExcludedWaveforms_ = (TH2F *) file -> Get(("h2LinearExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str()).c_str());
    h2LinearExcludedWaveforms_ -> GetXaxis() -> SetNdivisions(712);
    h2LinearExcludedWaveforms_ -> GetXaxis() -> SetTitle("Time [0.5 ns]");
    h2LinearExcludedWaveforms_ -> GetYaxis() -> SetTitle("Signal [mV]");
    h2LinearExcludedWaveforms_ -> GetYaxis() -> SetTitleOffset(1.2);
    h2LinearExcludedWaveforms_ -> Draw("colz");  
    cc -> Update();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].val, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].val, gPad->GetUymax());
    line -> SetLineColor(2);
    line -> SetLineWidth(2);
    line -> Draw();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.min, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.min, gPad->GetUymax());
    line -> SetLineColor(3);
    line -> SetLineWidth(2);
    line -> Draw();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.max, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.max, gPad->GetUymax());
    line -> SetLineColor(3);
    line -> SetLineWidth(2);
    line -> Draw();
    cc -> SaveAs((plotPath+plotNameLinearExcludedPng[iChannel]).c_str());
    cc -> SaveAs((plotPath+plotNameLinearExcludedRoot[iChannel]).c_str());

    TH2F *h2LinearSelectedWaveforms_ = (TH2F *) file -> Get(("h2LinearSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str()).c_str());
    h2LinearSelectedWaveforms_ -> GetXaxis() -> SetNdivisions(712);
    h2LinearSelectedWaveforms_ -> GetXaxis() -> SetTitle("Time [0.5 ns]");
    h2LinearSelectedWaveforms_ -> GetYaxis() -> SetTitle("Signal [mV]");
    h2LinearSelectedWaveforms_ -> GetYaxis() -> SetTitleOffset(1.2);
    h2LinearSelectedWaveforms_ -> Draw("colz");  
    cc -> Update();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].val, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].val, gPad->GetUymax());
    line -> SetLineColor(2);
    line -> SetLineWidth(2);
    line -> Draw();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.min, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.min, gPad->GetUymax());
    line -> SetLineColor(3);
    line -> SetLineWidth(2);
    line -> Draw();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.max, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.max, gPad->GetUymax());
    line -> SetLineColor(3);
    line -> SetLineWidth(2);
    line -> Draw();
    cc -> SaveAs((plotPath+plotNameLinearSelectedPng[iChannel]).c_str());
    cc -> SaveAs((plotPath+plotNameLinearSelectedRoot[iChannel]).c_str());

    TH2F *h2PulseExcludedWaveforms_ = (TH2F *) file -> Get(("h2PulseExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str()).c_str());
    h2PulseExcludedWaveforms_ -> GetXaxis() -> SetNdivisions(712);
    h2PulseExcludedWaveforms_ -> GetXaxis() -> SetTitle("Time [0.5 ns]");
    h2PulseExcludedWaveforms_ -> GetYaxis() -> SetTitle("Signal [mV]");
    h2PulseExcludedWaveforms_ -> GetYaxis() -> SetTitleOffset(1.2);
    h2PulseExcludedWaveforms_ -> Draw("colz");  
    cc -> Update();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].val, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].val, gPad->GetUymax());
    line -> SetLineColor(2);
    line -> SetLineWidth(2);
    line -> Draw();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.min, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.min, gPad->GetUymax());
    line -> SetLineColor(3);
    line -> SetLineWidth(2);
    line -> Draw();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.max, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.max, gPad->GetUymax());
    line -> SetLineColor(3);
    line -> SetLineWidth(2);
    line -> Draw();
    cc -> SaveAs((plotPath+plotNamePulseExcludedPng[iChannel]).c_str());
    cc -> SaveAs((plotPath+plotNamePulseExcludedRoot[iChannel]).c_str());

    TH2F *h2PulseSelectedWaveforms_ = (TH2F *) file -> Get(("h2PulseSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str()).c_str());
    h2PulseSelectedWaveforms_ -> GetXaxis() -> SetNdivisions(712);
    h2PulseSelectedWaveforms_ -> GetXaxis() -> SetTitle("Time [0.5 ns]");
    h2PulseSelectedWaveforms_ -> GetYaxis() -> SetTitle("Signal [mV]");
    h2PulseSelectedWaveforms_ -> GetYaxis() -> SetTitleOffset(1.2);
    h2PulseSelectedWaveforms_ -> Draw("colz");  
    cc -> Update();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].val, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].val, gPad->GetUymax());
    line -> SetLineColor(2);
    line -> SetLineWidth(2);
    line -> Draw();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.min, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.min, gPad->GetUymax());
    line -> SetLineColor(3);
    line -> SetLineWidth(2);
    line -> Draw();
    line = new TLine(cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.max, gPad->GetUymin(),
		     cfg -> _pulseT0[channel[iChannel].first][channel[iChannel].second].range.max, gPad->GetUymax());
    line -> SetLineColor(3);
    line -> SetLineWidth(2);
    line -> Draw();
    cc -> SaveAs((plotPath+plotNamePulseSelectedPng[iChannel]).c_str());
    cc -> SaveAs((plotPath+plotNamePulseSelectedRoot[iChannel]).c_str());

  }
  delete cc;

  // fit results plots
  TTree *tree = (TTree *) file -> Get("DUTTree");
  if(tree == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open DUT conversion tree" << endl;
    return 1;
  }
  Int_t eventIDDUT = 0;
  tree -> SetBranchAddress("eventNumber", &eventIDDUT);
  Double_t timeDUT = 0;
  tree -> SetBranchAddress("time", &timeDUT);
  Int_t DRSNumber = 0;
  tree -> SetBranchAddress("DRSNumber", &DRSNumber);
  Int_t CHNumber = 0;
  tree -> SetBranchAddress("CHNumber", &CHNumber);
  Float_t linearOffset = 0.;
  tree -> SetBranchAddress("linearOffset", &linearOffset);
  Float_t linearSlope = 0.;
  tree -> SetBranchAddress("linearSlope", &linearSlope);
  Float_t linearRedChi2 = 0.;
  tree -> SetBranchAddress("linearRedChi2", &linearRedChi2);
  Float_t pulseOffset = 0.;
  tree -> SetBranchAddress("pulseOffset", &pulseOffset);
  Float_t pulseSlope = 0.;
  tree -> SetBranchAddress("pulseSlope", &pulseSlope);
  Float_t pulseAmplitude = 0.;
  tree -> SetBranchAddress("pulseAmplitude", &pulseAmplitude);
  Float_t pulseT0 = 0.;
  tree -> SetBranchAddress("pulseT0", &pulseT0);
  Float_t pulseRiseTime = 0.;
  tree -> SetBranchAddress("pulseRiseTime", &pulseRiseTime);
  Float_t pulseDecay = 0.;
  tree -> SetBranchAddress("pulseDecay", &pulseDecay);
  Float_t pulseCharge = 0.;
  tree -> SetBranchAddress("pulseCharge", &pulseCharge);
  Float_t pulseRedChi2 = 0.;
  tree -> SetBranchAddress("pulseRedChi2", &pulseRedChi2);

  // allocating plots
  TGraph *grFrameTimeline = new TGraph();
  grFrameTimeline -> SetName("grFrameTimeline");
  grFrameTimeline -> SetTitle("DUT frames timeline");
  TH1F **h1Offset = new TH1F*[channel.size()];
  TH1F **h1Slope = new TH1F*[channel.size()];
  TH1F **h1Amplitude = new TH1F*[channel.size()];
  TH1F **h1T0 = new TH1F*[channel.size()];
  TH1F **h1RiseTime = new TH1F*[channel.size()];
  TGraph **grAmplitude = new TGraph*[channel.size()];
  TGraph **grOffset = new TGraph*[channel.size()];
  TGraph **grSlope = new TGraph*[channel.size()];
  TGraph **grT0 = new TGraph*[channel.size()];
  TGraph **grRiseTime = new TGraph*[channel.size()];
  TH2F **h2Pulse = new TH2F*[channel.size()];
  for(unsigned int iCH=0; iCH<channel.size(); iCH++){
    char name[1000];
    char title[1000];

    sprintf(name, "h1Offset_DRS%d_CH%d", channel[iCH].first, channel[iCH].second);
    sprintf(title, "Offset distribution DRS %d CH %d", channel[iCH].first, channel[iCH].second);
    h1Offset[iCH] = new TH1F(name, title, H1OFFSETBINS, H1OFFSETMIN, H1OFFSETMAX);

    sprintf(name, "h1Slope_DRS%d_CH%d", channel[iCH].first, channel[iCH].second);
    sprintf(title, "Slope distribution DRS %d CH %d", channel[iCH].first, channel[iCH].second);
    h1Slope[iCH] = new TH1F(name, title, H1SLOPEBINS, H1SLOPEMIN, H1SLOPEMAX);

    sprintf(name, "h1Amplitude_DRS%d_CH%d", channel[iCH].first, channel[iCH].second);
    sprintf(title, "Amplitude distribution DRS %d CH %d", channel[iCH].first, channel[iCH].second);
    h1Amplitude[iCH] = new TH1F(name, title, H1AMPLITUDEBINS, H1AMPLITUDEMIN, H1AMPLITUDEMAX);

    sprintf(name, "h1T0_DRS%d_CH%d", channel[iCH].first, channel[iCH].second);
    sprintf(title, "T0 distribution DRS %d CH %d", channel[iCH].first, channel[iCH].second);
    h1T0[iCH] = new TH1F(name, title, H1T0BINS, H1T0MIN, H1T0MAX);

    sprintf(name, "h1RiseTime_DRS%d_CH%d", channel[iCH].first, channel[iCH].second);
    sprintf(title, "RiseTime distribution DRS %d CH %d", channel[iCH].first, channel[iCH].second);
    h1RiseTime[iCH] = new TH1F(name, title, H1RISETIMEBINS, H1RISETIMEMIN, H1RISETIMEMAX);

    sprintf(name, "grAmplitude_DRS%d_CH%d", channel[iCH].first, channel[iCH].second);
    sprintf(title, "Amplitude timeline DRS %d CH %d", channel[iCH].first, channel[iCH].second);
    grAmplitude[iCH] = new TGraph();
    grAmplitude[iCH] -> SetName(name);
    grAmplitude[iCH] -> SetTitle(title);

    sprintf(name, "grOffset_DRS%d_CH%d", channel[iCH].first, channel[iCH].second);
    sprintf(title, "Offset timeline DRS %d CH %d", channel[iCH].first, channel[iCH].second);
    grOffset[iCH] = new TGraph();
    grOffset[iCH] -> SetName(name);
    grOffset[iCH] -> SetTitle(title);

    sprintf(name, "grSlope_DRS%d_CH%d", channel[iCH].first, channel[iCH].second);
    sprintf(title, "Slope timeline DRS %d CH %d", channel[iCH].first, channel[iCH].second);
    grSlope[iCH] = new TGraph();
    grSlope[iCH] -> SetName(name);
    grSlope[iCH] -> SetTitle(title);

    sprintf(name, "grT0_DRS%d_CH%d", channel[iCH].first, channel[iCH].second);
    sprintf(title, "T0 timeline DRS %d CH %d", channel[iCH].first, channel[iCH].second);
    grT0[iCH] = new TGraph();
    grT0[iCH] -> SetName(name);
    grT0[iCH] -> SetTitle(title);

    sprintf(name, "grRiseTime_DRS%d_CH%d", channel[iCH].first, channel[iCH].second);
    sprintf(title, "RiseTime timeline DRS %d CH %d", channel[iCH].first, channel[iCH].second);
    grRiseTime[iCH] = new TGraph();
    grRiseTime[iCH] -> SetName(name);
    grRiseTime[iCH] -> SetTitle(title);

    sprintf(name, "h2Pulse_DRS%d_CH%d", channel[iCH].first, channel[iCH].second);
    sprintf(title, "Pulse fit result DRS %d CH %d", channel[iCH].first, channel[iCH].second);
    h2Pulse[iCH] = new TH2F(name, title, H2PULSETIMEBINS, H2PULSETIMEMIN, H2PULSETIMEMAX, H2PULSEAMPLITUDEBINS, H2PULSEAMPLITUDEMIN, H2PULSEAMPLITUDEMAX);

  }

  // filling plots
  for(unsigned int iEntry=0; iEntry<tree -> GetEntries(); iEntry++){
    tree -> GetEntry(iEntry);
    grFrameTimeline -> SetPoint(grFrameTimeline -> GetN(), timeDUT, eventIDDUT);
    unsigned int CH = 0;
    if(DRSNumber == 0 && CHNumber == 0) CH = 0;
    else if(DRSNumber == 0 && CHNumber == 1) CH = 1;
    else if(DRSNumber == 0 && CHNumber == 2) CH = 2;
    else if(DRSNumber == 0 && CHNumber == 3) continue; // reset signal
    else if(DRSNumber == 1 && CHNumber == 0) CH = 3;
    else{
      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - unknown channel = DRS " << DRSNumber << ", CH " << CHNumber << endl;
      return 1;
    }
    // selection, please check that this corresponds to the actual one implemented
    if(pulseT0 > cfg -> _pulseT0Range[DRSNumber][CHNumber].min && pulseT0 < cfg -> _pulseT0Range[DRSNumber][CHNumber].max // selection from pulse fit
       &&
       pulseRiseTime > cfg -> _pulseRiseTimeRange.min && pulseRiseTime < cfg -> _pulseRiseTimeRange.max
      &&
       pulseRedChi2 < cfg -> _pulseRedChi2Threshold){
      h1Amplitude[CH] -> Fill(pulseAmplitude);
      h1Offset[CH] -> Fill(pulseOffset);
      h1Slope[CH] -> Fill(pulseSlope);
      h1T0[CH] -> Fill(pulseT0);
      h1RiseTime[CH] -> Fill(pulseRiseTime);
      grAmplitude[CH] -> SetPoint(grAmplitude[CH]->GetN(), timeDUT, pulseAmplitude);
      grOffset[CH] -> SetPoint(grOffset[CH]->GetN(), timeDUT, pulseOffset);
      grSlope[CH] -> SetPoint(grSlope[CH]->GetN(), timeDUT, pulseSlope);
      grT0[CH] -> SetPoint(grT0[CH]->GetN(), timeDUT, pulseT0);
      grRiseTime[CH] -> SetPoint(grRiseTime[CH]->GetN(), timeDUT, pulseRiseTime);
      TF1 *fPulse = new TF1("fPulse", "[0]+[1]*x + 0.5*[2]*(1+TMath::Erf(100*(x-[3])))*(1-exp(-(x-[3])/[4]))*(1.+[5]*(x-[3]))", cfg -> _pulse[DRSNumber][CHNumber].min, cfg -> _pulse[DRSNumber][CHNumber].max);
      fPulse -> SetParameter(0, pulseOffset);
      fPulse -> SetParameter(1, pulseSlope);
      fPulse -> SetParameter(2, pulseAmplitude);
      fPulse -> SetParameter(3, pulseT0);
      fPulse -> SetParameter(4, pulseRiseTime);
      fPulse -> SetParameter(5, pulseDecay);
      for(unsigned int iSample=0; iSample<NSAMPLES; iSample++){
	h2Pulse[CH] -> Fill(iSample, fPulse -> Eval(iSample));
      }
      delete fPulse;
    }
}

  // drawing
  gStyle -> SetPadTopMargin(0.1);
gStyle -> SetPadRightMargin(0.05);
gStyle -> SetPadBottomMargin(0.1);
gStyle -> SetPadLeftMargin(0.15);
gStyle -> SetOptStat(1);
cc = new TCanvas("cc", "cc", 0, 0, 1000, 1000);

grFrameTimeline -> GetXaxis() -> SetTitle("Time [s]");
grFrameTimeline -> GetYaxis() -> SetTitle("Frame Number");
grFrameTimeline -> GetYaxis() -> SetTitleOffset(2);
grFrameTimeline -> SetMarkerColor(1);
grFrameTimeline -> SetMarkerStyle(7);
grFrameTimeline -> Draw("ap");
string plotNameFrameTimeline = "/grDUTFrameTimeline.root";
cc -> SaveAs((plotPath+plotNameFrameTimeline).c_str());
plotNameFrameTimeline = "/grDUTFrameTimeline.png";
cc -> SaveAs((plotPath+plotNameFrameTimeline).c_str());

string plotNameAmplitudeDistrPng[channel.size()];
string plotNameOffsetDistrPng[channel.size()];
string plotNameSlopeDistrPng[channel.size()];
string plotNameT0DistrPng[channel.size()];
string plotNameRiseTimeDistrPng[channel.size()];
string plotNameAmplitudeTimelinePng[channel.size()];
string plotNameSlopeTimelinePng[channel.size()];
string plotNameOffsetTimelinePng[channel.size()];
string plotNameT0TimelinePng[channel.size()];
string plotNameRiseTimeTimelinePng[channel.size()];
string plotNamePulsePng[channel.size()];
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  stringstream drs_ss;
  drs_ss << channel[iCH].first;
  stringstream ch_ss;
  ch_ss << channel[iCH].second;
  string plotName = "";

  h1Amplitude[iCH] -> GetXaxis() -> SetTitle("Signal S [a.u.]");
  h1Amplitude[iCH] -> GetYaxis() -> SetTitle("#frac{dN}{dS} #left[a.u.^{-1}#right]");
  h1Amplitude[iCH] -> GetYaxis() -> SetTitleOffset(2);
  h1Amplitude[iCH] -> SetLineColor(1);
  h1Amplitude[iCH] -> SetLineWidth(3);
  h1Amplitude[iCH] -> Draw();
  plotName = "/h1Amplitude_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".png";
  plotNameAmplitudeDistrPng[iCH] = plotName;
  cc -> SaveAs((plotPath+plotName).c_str());
  plotName = "/h1Amplitude_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".root";
  cc -> SaveAs((plotPath+plotName).c_str());

  h1Offset[iCH] -> GetXaxis() -> SetTitle("Offset S_{0} [a.u.]");
  h1Offset[iCH] -> GetYaxis() -> SetTitle("#frac{dN}{dS_{0}} #left[a.u.^{-1}#right]");
  h1Offset[iCH] -> GetYaxis() -> SetTitleOffset(2);
  h1Offset[iCH] -> SetLineColor(1);
  h1Offset[iCH] -> SetLineWidth(3);
  h1Offset[iCH] -> Draw();
  plotName = "/h1Offset_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".png";
  plotNameOffsetDistrPng[iCH] = plotName;
  cc -> SaveAs((plotPath+plotName).c_str());
  plotName = "/h1Offset_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".root";
  cc -> SaveAs((plotPath+plotName).c_str());

  h1Slope[iCH] -> GetXaxis() -> SetTitle("Slope S_{1} [a.u./0.5 ns]");
  h1Slope[iCH] -> GetYaxis() -> SetTitle("#frac{dN}{dS_{1}} #left[0.5 ns / a.u.#right]");
  h1Slope[iCH] -> GetYaxis() -> SetTitleOffset(2);
  h1Slope[iCH] -> SetLineColor(1);
  h1Slope[iCH] -> SetLineWidth(3);
  h1Slope[iCH] -> Draw();
  plotName = "/h1Slope_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".png";
  plotNameSlopeDistrPng[iCH] = plotName;
  cc -> SaveAs((plotPath+plotName).c_str());
  plotName = "/h1Slope_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".root";
  cc -> SaveAs((plotPath+plotName).c_str());

  h1T0[iCH] -> GetXaxis() -> SetTitle("T0 [0.5 ns]");
  h1T0[iCH] -> GetYaxis() -> SetTitle("#frac{dN}{dT0} #left[0.5 ns^{-1}#right]");
  h1T0[iCH] -> GetYaxis() -> SetTitleOffset(2);
  h1T0[iCH] -> SetLineColor(1);
  h1T0[iCH] -> SetLineWidth(3);
  h1T0[iCH] -> Draw();
  plotName = "/h1T0_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".png";
  plotNameT0DistrPng[iCH] = plotName;
  cc -> SaveAs((plotPath+plotName).c_str());
  plotName = "/h1T0_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".root";
  cc -> SaveAs((plotPath+plotName).c_str());

  h1RiseTime[iCH] -> GetXaxis() -> SetTitle("Rise Time t [0.5 ns]");
  h1RiseTime[iCH] -> GetYaxis() -> SetTitle("#frac{dN}{dt} #left[0.5 ns^{-1}#right]");
  h1RiseTime[iCH] -> GetYaxis() -> SetTitleOffset(2);
  h1RiseTime[iCH] -> SetLineColor(1);
  h1RiseTime[iCH] -> SetLineWidth(3);
  h1RiseTime[iCH] -> Draw();
  plotName = "/h1RiseTime_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".png";
  plotNameRiseTimeDistrPng[iCH] = plotName;
  cc -> SaveAs((plotPath+plotName).c_str());
  plotName = "/h1RiseTime_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".root";
  cc -> SaveAs((plotPath+plotName).c_str());

  grAmplitude[iCH] -> GetXaxis() -> SetTitle("Time [s]");
  grAmplitude[iCH] -> GetYaxis() -> SetTitle("Signal [a.u.]");
  grAmplitude[iCH] -> GetYaxis() -> SetTitleOffset(2);
  grAmplitude[iCH] -> SetMarkerColor(1);
  grAmplitude[iCH] -> SetMarkerStyle(7);
  grAmplitude[iCH] -> Draw("ap");
  plotName = "/grAmplitude_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".png";
  plotNameAmplitudeTimelinePng[iCH] = plotName;
  cc -> SaveAs((plotPath+plotName).c_str());
  plotName = "/grAmplitude_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".root";
  cc -> SaveAs((plotPath+plotName).c_str());

  grOffset[iCH] -> GetXaxis() -> SetTitle("Time [s]");
  grOffset[iCH] -> GetYaxis() -> SetTitle("Offset [a.u.]");
  grOffset[iCH] -> GetYaxis() -> SetTitleOffset(2);
  grOffset[iCH] -> SetMarkerColor(1);
  grOffset[iCH] -> SetMarkerStyle(7);
  grOffset[iCH] -> Draw("ap");
  plotName = "/grOffset_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".png";
  plotNameOffsetTimelinePng[iCH] = plotName;
  cc -> SaveAs((plotPath+plotName).c_str());
  plotName = "/grOffset_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".root";
  cc -> SaveAs((plotPath+plotName).c_str());

  grSlope[iCH] -> GetXaxis() -> SetTitle("Time [s]");
  grSlope[iCH] -> GetYaxis() -> SetTitle("Slope [a.u./0.5 ns]");
  grSlope[iCH] -> GetYaxis() -> SetTitleOffset(2);
  grSlope[iCH] -> SetMarkerColor(1);
  grSlope[iCH] -> SetMarkerStyle(7);
  grSlope[iCH] -> Draw("ap");
  plotName = "/grSlope_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".png";
  plotNameSlopeTimelinePng[iCH] = plotName;
  cc -> SaveAs((plotPath+plotName).c_str());
  plotName = "/grSlope_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".root";
  cc -> SaveAs((plotPath+plotName).c_str());

  grT0[iCH] -> GetXaxis() -> SetTitle("Time [s]");
  grT0[iCH] -> GetYaxis() -> SetTitle("T0 [0.5 ns]");
  grT0[iCH] -> GetYaxis() -> SetTitleOffset(2);
  grT0[iCH] -> SetMarkerColor(1);
  grT0[iCH] -> SetMarkerStyle(7);
  grT0[iCH] -> Draw("ap");
  plotName = "/grT0_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".png";
  plotNameT0TimelinePng[iCH] = plotName;
  cc -> SaveAs((plotPath+plotName).c_str());
  plotName = "/grT0_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".root";
  cc -> SaveAs((plotPath+plotName).c_str());

  grRiseTime[iCH] -> GetXaxis() -> SetTitle("Time [s]");
  grRiseTime[iCH] -> GetYaxis() -> SetTitle("Rise Time [0.5 ns]");
  grRiseTime[iCH] -> GetYaxis() -> SetTitleOffset(2);
  grRiseTime[iCH] -> SetMarkerColor(1);
  grRiseTime[iCH] -> SetMarkerStyle(7);
  grRiseTime[iCH] -> Draw("ap");
  plotName = "/grRiseTime_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".png";
  plotNameRiseTimeTimelinePng[iCH] = plotName;
  cc -> SaveAs((plotPath+plotName).c_str());
  plotName = "/grRiseTime_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".root";
  cc -> SaveAs((plotPath+plotName).c_str());

 }
delete cc;

gStyle -> SetPadTopMargin(0.1);
gStyle -> SetPadRightMargin(0.1);
gStyle -> SetPadBottomMargin(0.1);
gStyle -> SetPadLeftMargin(0.1);
cc = new TCanvas("cc", "cc", 0, 0, 2000, 1000);
cc -> SetLogz();
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  stringstream drs_ss;
  drs_ss << channel[iCH].first;
  stringstream ch_ss;
  ch_ss << channel[iCH].second;
  string plotName = "";
  h2Pulse[iCH] -> GetXaxis() -> SetTitle("Time [0.5 ns]");
  h2Pulse[iCH] -> GetYaxis() -> SetTitle("Signal [mV]");
  h2Pulse[iCH] -> Draw("colz");
  plotName = "/h2Pulse_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".png";
  plotNamePulsePng[iCH] = plotName;
  cc -> SaveAs((plotPath+plotName).c_str());
  plotName = "/h2Pulse_DRS_" + drs_ss.str() + "_CH_" + ch_ss.str() + ".root";
  cc -> SaveAs((plotPath+plotName).c_str());
 }

// deleting
delete grFrameTimeline;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  delete h1Amplitude[iCH];
  delete h1Offset[iCH];
  delete h1Slope[iCH];
  delete h1T0[iCH];
  delete h1RiseTime[iCH];
  delete grAmplitude[iCH];
  delete grSlope[iCH];
  delete grOffset[iCH];
  delete grT0[iCH];
  delete grRiseTime[iCH];
  delete h2Pulse[iCH];
 }
delete[] h1Amplitude;
delete[] h1Offset;
delete[] h1Slope;
delete[] h1T0;
delete[] h1RiseTime;
delete[] grAmplitude;
delete[] grSlope;
delete[] grOffset;
delete[] grT0;
delete[] grRiseTime;
delete[] h2Pulse;

delete tree;
delete cc;
file -> Close();
delete file;

// writing timeline
fileLatex << "\\subsection{Frame timeline}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
fileLatex << "\\includegraphics[width=0.75\\textwidth]{plots" << plotNameFrameTimeline << "}" << endl;
fileLatex << "\\end{figure}" << endl;
fileLatex << "\\newpage" << endl;

// writing selection
for(unsigned int iChannel=0; iChannel<channel.size(); iChannel++){
  fileLatex << "\\subsection{DUT hit selection on channel DRS " << channel[iChannel].first << " CH " << channel[iChannel].second << " }" << endl;
  fileLatex << "\\begin{figure}[H]" << endl;
  fileLatex << "\\centering" << endl;
  fileLatex << "\\includegraphics[width=0.7\\textwidth]{plots" << plotNameLinearExcludedPng[iChannel] << "}" << endl;
  fileLatex << "\\\\" << endl;
  fileLatex << "\\includegraphics[width=0.7\\textwidth]{plots" << plotNameLinearSelectedPng[iChannel] << "}" << endl;
  fileLatex << "\\\\" << endl;
  fileLatex << "\\includegraphics[width=0.7\\textwidth]{plots" << plotNamePulseExcludedPng[iChannel] << "}" << endl;
  fileLatex << "\\\\" << endl;
  fileLatex << "\\includegraphics[width=0.7\\textwidth]{plots" << plotNamePulseSelectedPng[iChannel] << "}" << endl;
  fileLatex << "\\end{figure}" << endl;
  fileLatex << "\\newpage" << endl;
 }

// writing fit results
fileLatex << "\\subsection{Fitted functions}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  fileLatex << "\\includegraphics[width=0.7\\textwidth]{plots/" << plotNamePulsePng[iCH] << "}" << endl;
  if(iCH != channel.size()-1) fileLatex << "\\\\" << endl;
 }
fileLatex << "\\end{figure}" << endl;
fileLatex << "\\newpage" << endl;

// writing slope
fileLatex << "\\subsection{Fit result on selected pulses: slope}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots/" << plotNameSlopeTimelinePng[iCH] << "}" << endl;
  if(iCH == 1) fileLatex << "\\\\" << endl;
  else if(iCH!=channel.size()-1) fileLatex << "\\hspace{2cm}" << endl;
 }
fileLatex << "\\end{figure}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots/" << plotNameSlopeDistrPng[iCH] << "}" << endl;
  if(iCH == 1) fileLatex << "\\\\" << endl;
  else if(iCH!=channel.size()-1) fileLatex << "\\hspace{2cm}" << endl;
 }
fileLatex << "\\end{figure}" << endl;

// writing offset
fileLatex << "\\subsection{Fit result on selected pulses: offset}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots/" << plotNameOffsetTimelinePng[iCH] << "}" << endl;
  if(iCH == 1) fileLatex << "\\\\" << endl;
  else if(iCH!=channel.size()-1) fileLatex << "\\hspace{2cm}" << endl;
 }
fileLatex << "\\end{figure}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots/" << plotNameOffsetDistrPng[iCH] << "}" << endl;
  if(iCH == 1) fileLatex << "\\\\" << endl;
  else if(iCH!=channel.size()-1) fileLatex << "\\hspace{2cm}" << endl;
 }
fileLatex << "\\end{figure}" << endl;

// writing amplitude
fileLatex << "\\subsection{Fit result on selected pulses: amplitude}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots/" << plotNameAmplitudeTimelinePng[iCH] << "}" << endl;
  if(iCH == 1) fileLatex << "\\\\" << endl;
  else if(iCH!=channel.size()-1) fileLatex << "\\hspace{2cm}" << endl;
 }
fileLatex << "\\end{figure}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots/" << plotNameAmplitudeDistrPng[iCH] << "}" << endl;
  if(iCH == 1) fileLatex << "\\\\" << endl;
  else if(iCH!=channel.size()-1) fileLatex << "\\hspace{2cm}" << endl;
 }
fileLatex << "\\end{figure}" << endl;

// writing T0
fileLatex << "\\subsection{Fit result on selected pulses: T0}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots/" << plotNameT0TimelinePng[iCH] << "}" << endl;
  if(iCH == 1) fileLatex << "\\\\" << endl;
  else if(iCH!=channel.size()-1) fileLatex << "\\hspace{2cm}" << endl;
 }
fileLatex << "\\end{figure}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots/" << plotNameT0DistrPng[iCH] << "}" << endl;
  if(iCH == 1) fileLatex << "\\\\" << endl;
  else if(iCH!=channel.size()-1) fileLatex << "\\hspace{2cm}" << endl;
 }
fileLatex << "\\end{figure}" << endl;

// writing RiseTime
fileLatex << "\\subsection{Fit result on selected pulses: rise time}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots/" << plotNameRiseTimeTimelinePng[iCH] << "}" << endl;
  if(iCH == 1) fileLatex << "\\\\" << endl;
  else if(iCH!=channel.size()-1) fileLatex << "\\hspace{2cm}" << endl;
 }
fileLatex << "\\end{figure}" << endl;
fileLatex << "\\begin{figure}[H]" << endl;
fileLatex << "\\centering" << endl;
for(unsigned int iCH=0; iCH<channel.size(); iCH++){
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots/" << plotNameRiseTimeDistrPng[iCH] << "}" << endl;
  if(iCH == 1) fileLatex << "\\\\" << endl;
  else if(iCH!=channel.size()-1) fileLatex << "\\hspace{2cm}" << endl;
 }
fileLatex << "\\end{figure}" << endl;

return 0;
}

int writeTelescopeConversion(ofstream &fileLatex, 
			     const char *fileNameTelescopeConversion, 
			     const string plotPath){

  cout << __PRETTY_FUNCTION__ << ": processing " << fileNameTelescopeConversion << endl;
  
  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Telescope conversion}" << endl;

  TFile *file = TFile::Open(fileNameTelescopeConversion);
  if(file == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open telescope conversion file" << endl;
    return 1;
  }
  TTree *tree = (TTree *) file -> Get("Event");
  if(tree == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open telescope tree " << endl;
    return 1;
  }
  ULong64_t TimeStamp = 0;
  tree -> SetBranchAddress("TriggerTime", &TimeStamp);
  ULong64_t FrameNumber = 0;
  tree -> SetBranchAddress("FrameNumber", &FrameNumber);

  TGraph *grFrameTimeline = new TGraph();
  grFrameTimeline -> SetName("grFrameTimeline");
  grFrameTimeline -> SetTitle("DUT frames timeline");
  tree -> GetEntry(0);
  const unsigned int T0 = TimeStamp;
  for(unsigned int iEntry=0; iEntry<tree -> GetEntries(); iEntry++){
    tree -> GetEntry(iEntry);
    grFrameTimeline -> SetPoint(grFrameTimeline -> GetN(), TIMECONVERSION*(TimeStamp-T0), FrameNumber);
  }

  gStyle -> SetPadTopMargin(0.1);
  gStyle -> SetPadRightMargin(0.05);
  gStyle -> SetPadBottomMargin(0.1);
  gStyle -> SetPadLeftMargin(0.15);
  gStyle -> SetOptStat(1);
  TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 1000, 1000);

  grFrameTimeline -> GetXaxis() -> SetTitle("Time [s]");
  grFrameTimeline -> GetYaxis() -> SetTitle("Frame Number");
  grFrameTimeline -> GetYaxis() -> SetTitleOffset(2);
  grFrameTimeline -> SetMarkerColor(1);
  grFrameTimeline -> SetMarkerStyle(7);
  grFrameTimeline -> Draw("ap");
  string plotNameFrameTimeline = "/grTelescopeFrameTimeline.root";
  cc -> SaveAs((plotPath+plotNameFrameTimeline).c_str());
  plotNameFrameTimeline = "/grTelescopeFrameTimeline.png";
  cc -> SaveAs((plotPath+plotNameFrameTimeline).c_str());

  delete grFrameTimeline;
  delete cc;
  delete tree;
  file -> Close();
  delete file;

  // writing timeline
  fileLatex << "\\subsection{Frame timeline}" << endl;
  fileLatex << "\\begin{figure}[H]" << endl;
  fileLatex << "\\centering" << endl;
  fileLatex << "\\includegraphics[width=0.75\\textwidth]{plots" << plotNameFrameTimeline << "}" << endl;
  fileLatex << "\\end{figure}" << endl;
  fileLatex << "\\newpage" << endl;

  return 0;
}

int writeTelescopeAlignment(ofstream &fileLatex, 
			    const char *fileNameTelescopeAlignment, 
			    const string plotPath){
  
  cout << __PRETTY_FUNCTION__ << ": processing " << fileNameTelescopeAlignment << endl;

  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Telescope alignment}" << endl;

  return 0;
}

int writeTelescopeTracking(ofstream &fileLatex, 
			   const char *fileNameTelescopeTracking, 
			   const string plotPath){

  cout << __PRETTY_FUNCTION__ << ": processing " << fileNameTelescopeTracking << endl;
  
  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Telescope tracking}" << endl;

  TFile *file = TFile::Open(fileNameTelescopeTracking);
  if(file == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileNameTelescopeTracking << endl;
    return 1;
  }

  file -> Close();
  delete file;

  return 0;
}

int writeSynchronization(ofstream &fileLatex, 
			 const char *fileNameSynchronization, 
			 const string plotPath){

  cout << __PRETTY_FUNCTION__ << ": processing " << fileNameSynchronization << endl;
  
  fileLatex << "\\newpage" << endl;
  //  fileLatex << "\\section{Synchronization}" << endl;
  fileLatex << "\\section{Tracking and synchronization}" << endl;

  TH2F *h2RoI = new TH2F("h2RoI", "RoI", XBINS, XMIN, XMAX, YBINS, YMIN, YMAX);
  TH1F *h1X = new TH1F("h1X", "Global coordinate X", XBINS, XMIN, XMAX);
  TH1F *h1Y = new TH1F("h1Y", "Global coordinate Y", YBINS, YMIN, YMAX);
  TGraph *grX = new TGraph();
  grX -> SetName("grX");
  grX -> SetTitle("Global coordinate X timeline");
  TGraph *grY = new TGraph();
  grY -> SetName("grY");
  grY -> SetTitle("Global coordinate Y timeline");
  TH1F *h1XErr = new TH1F("h1XErr", "Global coordinate error X", XERRORBINS, XERRORMIN, XERRORMAX);
  TH1F *h1YErr = new TH1F("h1YErr", "Global coordinate error Y", YERRORBINS, YERRORMIN, YERRORMAX);
  TGraph *grXErr = new TGraph();
  grXErr -> SetName("grXErr");
  grXErr -> SetTitle("Global coordinate error X timeline");
  TGraph *grYErr = new TGraph();
  grYErr -> SetName("grYErr");
  grYErr -> SetTitle("Global coordinate error Y timeline");
  TH1F *h1NTracks = new TH1F("h1NTracks", "Track multiplicity", NTRACKSBINS, NTRACKSMIN, NTRACKSMAX);
  TGraph *grNTracks = new TGraph();
  grNTracks -> SetName("grNTracks");
  grNTracks -> SetTitle("Track multiplicity timeline");

  // opening root file
  TFile *file = TFile::Open(fileNameSynchronization);
  if(file == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameSynchronization << endl;
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
  Double_t xTrack_Error = 0.;
  tree -> SetBranchAddress("xTrack_Error",&xTrack_Error);
  Double_t yTrack_Error = 0.;
  tree -> SetBranchAddress("yTrack_Error",&yTrack_Error);
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

  for(unsigned int iEntry=0; iEntry<tree -> GetEntries(); iEntry++){
    tree -> GetEntry(iEntry);
    h2RoI -> Fill(xTrack, yTrack);
    h1X -> Fill(xTrack);
    h1Y -> Fill(yTrack);
    grX -> SetPoint(grX -> GetN(), timeDUT, xTrack);
    grY -> SetPoint(grY -> GetN(), timeDUT, yTrack);
    h1XErr -> Fill(xTrack_Error);
    h1YErr -> Fill(yTrack_Error);
    grXErr -> SetPoint(grXErr -> GetN(), timeDUT, xTrack_Error);
    grYErr -> SetPoint(grYErr -> GetN(), timeDUT, yTrack_Error);
    h1NTracks -> Fill(nTrack);
    grNTracks -> SetPoint(grNTracks -> GetN(), timeDUT, nTrack);
  }

  delete tree;
  file -> Close();
  delete file;

  gStyle -> SetPadTopMargin(0.2);
  gStyle -> SetPadRightMargin(0.2);
  gStyle -> SetPadBottomMargin(0.2);
  gStyle -> SetPadLeftMargin(0.2);
  gStyle -> SetOptStat(0);
  TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 1000, 1000);

  h2RoI -> GetXaxis() -> SetTitle("Global coordinate x [#mum]");
  h2RoI -> GetXaxis() -> SetTitleOffset(1.5);
  h2RoI -> GetYaxis() -> SetTitle("Global coordinate y [#mum]");
  h2RoI -> GetYaxis() -> SetTitleOffset(2);
  h2RoI -> GetZaxis() -> SetTitle("#frac{d^{2}N}{dxdy} #left[#mum^{-2}#right]");
  h2RoI -> GetZaxis() -> SetTitleOffset(1.5);
  h2RoI -> Draw("colz");
  const string plotNameRoIPng = "/h2RoI.png";
  cc -> SaveAs((plotPath+plotNameRoIPng).c_str());
  const string plotNameRoIRoot = "/h2RoI.root";
  cc -> SaveAs((plotPath+plotNameRoIRoot).c_str());

  h2RoI -> GetXaxis() -> SetRangeUser(h1X -> GetMean()-1*h1X->GetRMS(), h1X -> GetMean()+1*h1X->GetRMS());
  h2RoI -> GetYaxis() -> SetRangeUser(h1Y -> GetMean()-1*h1Y->GetRMS(), h1Y -> GetMean()+1*h1Y->GetRMS());
  h2RoI -> Draw("colz");
  const string plotNameRoIZoomPng = "/h2RoIZoom.png";
  cc -> SaveAs((plotPath+plotNameRoIZoomPng).c_str());
  delete cc;

  gStyle -> SetPadTopMargin(0.1);
  gStyle -> SetPadRightMargin(0.05);
  gStyle -> SetPadBottomMargin(0.1);
  gStyle -> SetPadLeftMargin(0.15);
  cc = new TCanvas("cc", "cc", 0, 0, 1000, 1000);

  h1X -> SetLineColor(1);
  h1X -> SetLineWidth(3);
  h1X -> GetXaxis() -> SetTitle("Global coordinate X [#mum]");
  h1X -> GetXaxis() -> SetTitleOffset(1.5);
  h1X -> GetYaxis() -> SetTitle("#frac{dN}{dx} #left[#mum^{-1}#right]");
  h1X -> GetYaxis() -> SetTitleOffset(2);
  h1X -> GetXaxis() -> SetRangeUser(h1X -> GetMean()-1*h1X->GetRMS(), h1X -> GetMean()+1*h1X->GetRMS());
  h1X -> Draw();
  const string plotNameXPng = "/h1X.png";
  cc -> SaveAs((plotPath+plotNameXPng).c_str());
  const string plotNameXRoot = "/h1X.root";
  cc -> SaveAs((plotPath+plotNameXRoot).c_str());

  h1Y -> SetLineColor(1);
  h1Y -> SetLineWidth(3);
  h1Y -> GetXaxis() -> SetTitle("Global coordinate Y [#mum]");
  h1Y -> GetXaxis() -> SetTitleOffset(1.5);
  h1Y -> GetYaxis() -> SetTitle("#frac{dN}{dx} #left[#mum^{-1}#right]");
  h1Y -> GetYaxis() -> SetTitleOffset(2);
  h1Y -> GetXaxis() -> SetRangeUser(h1Y -> GetMean()-1*h1Y->GetRMS(), h1Y -> GetMean()+1*h1Y->GetRMS());
  h1Y -> Draw();
  const string plotNameYPng = "/h1Y.png";
  cc -> SaveAs((plotPath+plotNameYPng).c_str());
  const string plotNameYRoot = "/h1Y.root";
  cc -> SaveAs((plotPath+plotNameYRoot).c_str());

  grX -> SetMarkerStyle(7);
  grX -> SetMarkerColor(1);
  grX -> GetXaxis() -> SetTitle("Time [0.5 ns]");
  grX -> GetYaxis() -> SetTitle("Global coordinate X [#mum]");
  grX -> Draw("ap");
  const string plotNameXTimelinePng = "/grX.png";
  cc -> SaveAs((plotPath+plotNameXTimelinePng).c_str());
  const string plotNameXTimelineRoot = "/grX.root";
  cc -> SaveAs((plotPath+plotNameXTimelineRoot).c_str());

  grY -> SetMarkerStyle(7);
  grY -> SetMarkerColor(1);
  grY -> GetXaxis() -> SetTitle("Time [0.5 ns]");
  grY -> GetYaxis() -> SetTitle("Global coordinate Y [#mum]");
  grY -> Draw("ap");
  const string plotNameYTimelinePng = "/grY.png";
  cc -> SaveAs((plotPath+plotNameYTimelinePng).c_str());
  const string plotNameYTimelineRoot = "/grY.root";
  cc -> SaveAs((plotPath+plotNameYTimelineRoot).c_str());

  h1XErr -> SetLineColor(1);
  h1XErr -> SetLineWidth(3);
  h1XErr -> GetXaxis() -> SetTitle("Global coordinate error #sigma_{x} [#mum]");
  h1XErr -> GetXaxis() -> SetTitleOffset(1.5);
  h1XErr -> GetYaxis() -> SetTitle("#frac{dN}{d#sigma_{x}} #left[#mum^{-1}#right]");
  h1XErr -> GetYaxis() -> SetTitleOffset(2);
  h1XErr -> Draw();
  const string plotNameXErrPng = "/h1XErr.png";
  cc -> SaveAs((plotPath+plotNameXErrPng).c_str());
  const string plotNameXErrRoot = "/h1XErr.root";
  cc -> SaveAs((plotPath+plotNameXErrRoot).c_str());

  h1YErr -> SetLineColor(1);
  h1YErr -> SetLineWidth(3);
  h1YErr -> GetXaxis() -> SetTitle("Global coordinate error #sigma_{y} [#mum]");
  h1YErr -> GetXaxis() -> SetTitleOffset(1.5);
  h1YErr -> GetYaxis() -> SetTitle("#frac{dN}{d#sigma_{y}} #left[#mum^{-1}#right]");
  h1YErr -> GetYaxis() -> SetTitleOffset(2);
  h1YErr -> Draw();
  const string plotNameYErrPng = "/h1YErr.png";
  cc -> SaveAs((plotPath+plotNameYErrPng).c_str());
  const string plotNameYErrRoot = "/h1YErr.root";
  cc -> SaveAs((plotPath+plotNameYErrRoot).c_str());

  grXErr -> SetMarkerStyle(7);
  grXErr -> SetMarkerColor(1);
  grXErr -> GetXaxis() -> SetTitle("Time [0.5 ns]");
  grXErr -> GetYaxis() -> SetTitle("Global coordinate error #sigma_{x} [#mum]");
  grXErr -> Draw("ap");
  const string plotNameXErrTimelinePng = "/grXErr.png";
  cc -> SaveAs((plotPath+plotNameXErrTimelinePng).c_str());
  const string plotNameXErrTimelineRoot = "/grXErr.root";
  cc -> SaveAs((plotPath+plotNameXErrTimelineRoot).c_str());

  grYErr -> SetMarkerStyle(7);
  grYErr -> SetMarkerColor(1);
  grYErr -> GetXaxis() -> SetTitle("Time [0.5 ns]");
  grYErr -> GetYaxis() -> SetTitle("Global coordinate error #sigma_{x} [#mum]");
  grYErr -> Draw("ap");
  const string plotNameYErrTimelinePng = "/grYErr.png";
  cc -> SaveAs((plotPath+plotNameYErrTimelinePng).c_str());
  const string plotNameYErrTimelineRoot = "/grYErr.root";
  cc -> SaveAs((plotPath+plotNameYErrTimelineRoot).c_str());

  h1NTracks -> SetLineColor(1);
  h1NTracks -> SetLineWidth(3);
  h1NTracks -> GetXaxis() -> SetTitle("Track multiplitity n");
  h1NTracks -> GetXaxis() -> SetTitleOffset(1.5);
  h1NTracks -> GetYaxis() -> SetTitle("#frac{dN}{dn}");
  h1NTracks -> GetYaxis() -> SetTitleOffset(2);
  h1NTracks -> Draw();
  const string plotNameNTracksPng = "/h1NTracks.png";
  cc -> SaveAs((plotPath+plotNameNTracksPng).c_str());
  const string plotNameNTracksRoot = "/h1NTracks.root";
  cc -> SaveAs((plotPath+plotNameNTracksRoot).c_str());

  grNTracks -> SetMarkerStyle(7);
  grNTracks -> SetMarkerColor(1);
  grNTracks -> GetXaxis() -> SetTitle("Time [0.5 ns]");
  grNTracks -> GetYaxis() -> SetTitle("Track multiplicity");
  grNTracks -> Draw("ap");
  const string plotNameNTracksTimelinePng = "/grNTracks.png";
  cc -> SaveAs((plotPath+plotNameNTracksTimelinePng).c_str());
  const string plotNameNTracksTimelineRoot = "/grNTracks.root";
  cc -> SaveAs((plotPath+plotNameNTracksTimelineRoot).c_str());

  delete cc;

  delete h2RoI;
  delete h1X;
  delete h1Y;
  delete grX;
  delete grY;
  delete h1XErr;
  delete h1YErr;
  delete grXErr;
  delete grYErr;
  delete h1NTracks;
  delete grNTracks;

  fileLatex << "\\subsection{RoI hit coordinates}" << endl;

  fileLatex << "\\begin{figure}[H]" << endl;
  fileLatex << "\\centering" << endl;
  fileLatex << "\\includegraphics[width=0.75\\textwidth,trim={0 0 0 2cm},clip]{plots" << plotNameRoIZoomPng << "}" << endl;
  //  fileLatex << "\\\\" << endl;
  //  fileLatex << "\\includegraphics[width=0.75\\textwidth,trim={0 0 0 2cm},clip]{plots" << plotNameRoIPng << "}" << endl;
  fileLatex << "\\end{figure}" << endl;
  fileLatex << "\\begin{figure}[H]" << endl;
  fileLatex << "\\centering" << endl;
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots" << plotNameNTracksTimelinePng << "}" << endl;
  fileLatex << "\\hspace{2cm}" << endl;
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots" << plotNameNTracksPng << "}" << endl;
  fileLatex << "\\end{figure}" << endl;
  fileLatex << "\\newpage" << endl;

  fileLatex << "\\begin{figure}[H]" << endl;
  fileLatex << "\\centering" << endl;
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots" << plotNameXTimelinePng << "}" << endl;
  fileLatex << "\\hspace{2cm}" << endl;
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots" << plotNameYTimelinePng << "}" << endl;
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots" << plotNameXPng << "}" << endl;
  fileLatex << "\\hspace{2cm}" << endl;
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots" << plotNameYPng << "}" << endl;
  fileLatex << "\\end{figure}" << endl;

  fileLatex << "\\begin{figure}[H]" << endl;
  fileLatex << "\\centering" << endl;
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots" << plotNameXErrTimelinePng << "}" << endl;
  fileLatex << "\\hspace{2cm}" << endl;
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots" << plotNameYErrTimelinePng << "}" << endl;
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots" << plotNameXErrPng << "}" << endl;
  fileLatex << "\\hspace{2cm}" << endl;
  fileLatex << "\\includegraphics[width=0.35\\textwidth]{plots" << plotNameYErrPng << "}" << endl;
  fileLatex << "\\end{figure}" << endl;

  fileLatex << "\\newpage" << endl;

  return 0;
}

int writePositionCorrection(ofstream &fileLatex, 
			    const char *fileNamePositionCorrection, 
			    const string plotPath){
  
  cout << __PRETTY_FUNCTION__ << ": processing " << fileNamePositionCorrection << endl;

  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Position correction}" << endl;

  return 0;
}

int writeEfficiency(ofstream &fileLatex, 
		    const char *fileNameEfficiency, 
		    const string plotPath){
  
  cout << __PRETTY_FUNCTION__ << ": processing " << fileNameEfficiency << endl;

  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Efficiency}" << endl;

  return 0;
}

#endif
