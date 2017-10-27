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

#include "../aux/configClass.hh"

#define NDUTCHANNELS 5

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

  for(unsigned int iChannel=0; iChannel<channel.size(); iChannel++){

    stringstream DRS;
    DRS << channel[iChannel].first;
    stringstream CH;
    CH << channel[iChannel].second;    

    const string plotNameLinearExcludedPdf = "/DUTConversion_h2LinearExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".pdf";
    const string plotNameLinearExcludedRoot = "/DUTConversion_h2LinearExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".root";
    const string plotNameLinearSelectedPdf = "/DUTConversion_h2LinearSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".pdf";
    const string plotNameLinearSelectedRoot = "/DUTConversion_h2LinearSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".root";
    const string plotNamePulseExcludedPdf = "/DUTConversion_h2PulseExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".pdf";
    const string plotNamePulseExcludedRoot = "/DUTConversion_h2PulseExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".root";
    const string plotNamePulseSelectedPdf = "/DUTConversion_h2PulseSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".pdf";
    const string plotNamePulseSelectedRoot = "/DUTConversion_h2PulseSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str() + ".root";

    TH2F *h2LinearExcludedWaveforms_ = (TH2F *) file -> Get(("h2LinearExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str()).c_str());
    h2LinearExcludedWaveforms_ -> GetXaxis() -> SetNdivisions(712);
    h2LinearExcludedWaveforms_ -> GetXaxis() -> SetTitle("Time bin");
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
    cc -> SaveAs((plotPath+plotNameLinearExcludedPdf).c_str());
    cc -> SaveAs((plotPath+plotNameLinearExcludedRoot).c_str());

    TH2F *h2LinearSelectedWaveforms_ = (TH2F *) file -> Get(("h2LinearSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str()).c_str());
    h2LinearSelectedWaveforms_ -> GetXaxis() -> SetNdivisions(712);
    h2LinearSelectedWaveforms_ -> GetXaxis() -> SetTitle("Time bin");
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
    cc -> SaveAs((plotPath+plotNameLinearSelectedPdf).c_str());
    cc -> SaveAs((plotPath+plotNameLinearSelectedRoot).c_str());

    TH2F *h2PulseExcludedWaveforms_ = (TH2F *) file -> Get(("h2PulseExcludedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str()).c_str());
    h2PulseExcludedWaveforms_ -> GetXaxis() -> SetNdivisions(712);
    h2PulseExcludedWaveforms_ -> GetXaxis() -> SetTitle("Time bin");
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
    cc -> SaveAs((plotPath+plotNamePulseExcludedPdf).c_str());
    cc -> SaveAs((plotPath+plotNamePulseExcludedRoot).c_str());

    TH2F *h2PulseSelectedWaveforms_ = (TH2F *) file -> Get(("h2PulseSelectedWaveforms_DRS_" + DRS.str() + "_CH_" + CH.str()).c_str());
    h2PulseSelectedWaveforms_ -> GetXaxis() -> SetNdivisions(712);
    h2PulseSelectedWaveforms_ -> GetXaxis() -> SetTitle("Time bin");
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
    cc -> SaveAs((plotPath+plotNamePulseSelectedPdf).c_str());
    cc -> SaveAs((plotPath+plotNamePulseSelectedRoot).c_str());

    fileLatex << "\\subsection{DUT hit selection on channel DRS " << channel[iChannel].first << " CH " << channel[iChannel].second << " }" << endl;
    fileLatex << "\\begin{figure}[H]" << endl;
    fileLatex << "\\centering" << endl;
    fileLatex << "\\includegraphics[width=0.45\\textwidth]{plots" << plotNameLinearExcludedPdf << "}" << endl;
    fileLatex << "\\includegraphics[width=0.45\\textwidth]{plots" << plotNameLinearSelectedPdf << "}" << endl;
    fileLatex << "\\\\" << endl;
    fileLatex << "\\includegraphics[width=0.45\\textwidth]{plots" << plotNamePulseExcludedPdf << "}" << endl;
    fileLatex << "\\includegraphics[width=0.45\\textwidth]{plots" << plotNamePulseSelectedPdf << "}" << endl;
    fileLatex << "\\end{figure}" << endl;

    if(iChannel==1) fileLatex << "\\newpage" << endl;
  }

  delete cc;
  file -> Close();
  delete file;
  return 0;
}

int writeDUTConversion(ofstream &fileLatex, 
		       const char *fileNameDUTConversion, 
		       const string plotPath){

  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Telescope conversion}" << endl;

  return 0;
}

int writeTelescopeConversion(ofstream &fileLatex, 
			     const char *fileNameTelescopeConversion, 
			     const string plotPath){
  
  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Telescope conversion}" << endl;

  return 0;
}

int writeTelescopeAlignment(ofstream &fileLatex, 
			    const char *fileNameTelescopeAlignment, 
			    const string plotPath){
  
  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Telescope alignment}" << endl;

  return 0;
}

int writeTelescopeTracking(ofstream &fileLatex, 
			   const char *fileNameTelescopeTracking, 
			   const string plotPath){
  
  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Telescope tracking}" << endl;

  return 0;
}

int writeSynchronization(ofstream &fileLatex, 
			   const char *fileNameSynchronization, 
			   const string plotPath){
  
  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Synchronization}" << endl;

  return 0;
}

int writePositionCorrection(ofstream &fileLatex, 
			   const char *fileNamePositionCorrection, 
			   const string plotPath){
  
  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Position correction}" << endl;

  return 0;
}

int writeEfficiency(ofstream &fileLatex, 
			   const char *fileNameEfficiency, 
			   const string plotPath){
  
  fileLatex << "\\newpage" << endl;
  fileLatex << "\\section{Efficiency}" << endl;

  return 0;
}

#endif
