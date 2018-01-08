#ifndef SYNCHRONIZE_HH
#define SYNCHRONIZE_HH

#include <iostream>
#include <fstream>
using namespace std;

#include "../aux/progressBar.h"
#include "../aux/configClass.hh"

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TPad.h>
#include <TLegend.h>
#include <TStyle.h>

#define DZ 270000. // to compute track coordinates on DUT
#define TIMECONVERSION 0.000000025 // to convert telescope time

#define PLOTTRACKMAPNBINSX 1000
#define PLOTTRACKMAPMINX -10000
#define PLOTTRACKMAPMAXX 10000
#define PLOTTRACKMAPNBINSY 1000
#define PLOTTRACKMAPMINY -10000
#define PLOTTRACKMAPMAXY 10000
#define ROITHRESHOLDFRACTION 0.1
#define PLOTDUTMAPNBINSX 100
#define PLOTDUTMAPNBINSY 100
#define PLOTTIMEDISTANCENBINS 100000
#define PLOTTIMEDISTANCEMIN 0
#define PLOTTIMEDISTANCEMAX 100

class hitClass{
public:
  hitClass(){};
  ~hitClass(){};
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": ID = " << _ID << endl;
    cout << __PRETTY_FUNCTION__ << ": index = " << _index << endl;
    cout << __PRETTY_FUNCTION__ << ": time = " << _time << endl;    
  }
  unsigned int _ID; // raw ID
  unsigned int _index; // position in tree
  double _time;
};

class eventClass{
public:
  eventClass(){};
  ~eventClass(){
    for(unsigned int iHit=0; iHit<_hit.size(); iHit++){
      delete _hit[iHit];
    }
    _hit.clear();
  };
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": nHits = " << _hit.size() << endl;
    for(unsigned int iHit=0; iHit<_hit.size(); iHit++){
      _hit[iHit] -> print();
    }
    return ;
  };
  vector<hitClass *> _hit;
};

class spillClass{
public:
  spillClass(){};
  ~spillClass(){
    for(unsigned int iEvent=0; iEvent<_event.size(); iEvent++){
      delete _event[iEvent];
    }
    _event.clear();
  };
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": nEvents = " << _event.size() << endl;
    for(unsigned int iEvent=0; iEvent<_event.size(); iEvent++){
      _event[iEvent] -> print();
    }
    return ;
  };
  vector<eventClass *> _event;
};

void drawSpill(const vector<spillClass *> spillDUT,
	       const vector<spillClass *> spillTelescope,
	       const unsigned int iSpill){

  gStyle -> SetPaperSize(20, 20);
  gStyle -> SetPadTopMargin(0.12);
  gStyle -> SetPadRightMargin(0.07);
  gStyle -> SetPadBottomMargin(0.15);
  gStyle -> SetPadLeftMargin(0.2);
  gStyle -> SetOptTitle(0);
  gStyle -> SetOptFit(1111);
  gStyle->SetTitleW(0.6);
  gStyle -> SetOptStat(0);
  gStyle -> SetPadTickX(1);
  gStyle -> SetPadTickY(1);

  TGraph *grDUT = new TGraph();
  for(unsigned int iEvent=0; iEvent<spillDUT[iSpill] -> _event.size(); iEvent++){
    for(unsigned int iHit=0; iHit<spillDUT[iSpill] -> _event[iEvent] -> _hit.size(); iHit++){
      hitClass *hit = spillDUT[iSpill] -> _event[iEvent] -> _hit[iHit];
      grDUT -> SetPoint(grDUT -> GetN(), hit -> _ID, hit -> _time);
    }
  }
  grDUT -> SetFillColor(0);
  grDUT -> SetFillStyle(0);
  grDUT -> SetMarkerStyle(20);
  grDUT -> SetMarkerColor(2);
  grDUT -> SetName("DUT");

  TGraph *grTelescope = new TGraph();
  for(unsigned int iEvent=0; iEvent<spillTelescope[iSpill] -> _event.size(); iEvent++){
    for(unsigned int iHit=0; iHit<spillTelescope[iSpill] -> _event[iEvent] -> _hit.size(); iHit++){
      hitClass *hit = spillTelescope[iSpill] -> _event[iEvent] -> _hit[iHit];
      grTelescope -> SetPoint(grTelescope -> GetN(), hit -> _ID, hit -> _time);
    }
  }
  grTelescope -> SetFillColor(0);
  grTelescope -> SetFillStyle(0);
  grTelescope -> SetMarkerStyle(20);
  grTelescope -> SetMarkerColor(1);
  grTelescope -> SetName("Telescope");  

  TCanvas *cc = new TCanvas("cc", "cc", 1000, 0, 1000, 1000);
  TMultiGraph *mg = new TMultiGraph();
  mg -> Add(grDUT, "p");
  mg -> Add(grTelescope, "p");
  mg -> Draw("ap");
  mg -> GetXaxis() -> SetTitle("Frame number");
  mg -> GetYaxis() -> SetTitle("Timestamp");
  mg -> GetYaxis() -> SetTitleOffset(2);
  TLegend *leg = cc -> BuildLegend(0.2815631,0.713258,0.5190381,0.7944502);
  leg -> SetFillColor(0);
  leg -> SetFillStyle(0);
  leg -> SetLineColor(0);
  leg -> Draw();
  cc -> Modified();
  cc -> Update();

  char name[1000];
  sprintf(name, "spill_%d.C", iSpill);
  cc -> SaveAs(name);
  sprintf(name, "spill_%d.png", iSpill);
  cc -> SaveAs(name);
  sprintf(name, "spill_%d.pdf", iSpill);
  cc -> SaveAs(name);

  delete mg;
  delete cc;
  delete leg;
  
  return ;
}

#endif
