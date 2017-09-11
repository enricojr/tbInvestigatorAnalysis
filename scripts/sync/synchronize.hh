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

//////////////
// hitClass
//////////////

class hitClass{
public:
  hitClass(){
    _onTarget=false;
      };
  ~hitClass(){};
  // print
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": ID = " << _ID << endl;
    return ;
  }
  unsigned int _ID;
  bool _onTarget;
private:
};

////////////////
// eventClass
////////////////

class eventClass{
public:
  eventClass(){};
  // destructor
  ~eventClass(){
    for(unsigned int i=0; i<_hit.size(); i++){
     delete _hit[i];
    }
    _hit.clear();
  };
  // print
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": time = " << _time << ", ID = " << _ID << endl;
    for(unsigned int i=0; i<_hit.size(); i++){
      _hit[i] -> print();
    }
    return ;
  };
  // hasHitOnTarget
  bool hasHitOnTarget() const{
    for(unsigned int iHit=0; iHit<_hit.size(); iHit++){
      if(_hit[iHit] -> _onTarget){
	return true;
	break;
      }
    }
    return false;
  };
  double _time;
  unsigned int _ID;
  unsigned int _spillLocation;
  unsigned int _interSpillLocation;
  vector<hitClass *> _hit;
private:
};

/////////////////////
// interSpillClass
/////////////////////

class interSpillClass{
public:
  interSpillClass(){};
  // destructor
  ~interSpillClass(){
    for(unsigned int i=0; i<_event.size(); i++){
     delete _event[i];
    }
    _event.clear();
  };
  // print
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": number of events = " << _event.size() << endl;
    for(unsigned int i=0; i<_event.size(); i++){
      _event[i] -> print();
    }
    return ;
  };
  // getGraph
  TGraph *getGraph(const unsigned int color,
		   const unsigned int offset) const{
    TGraph *gr = new TGraph();
    for(unsigned int i=0; i<_event.size(); i++){
      gr -> SetPoint(gr -> GetN(), gr -> GetN()+offset, _event[i] -> _time);
    }
    gr -> SetMarkerStyle(20);
    gr -> SetMarkerColor(color);
    return gr;
  }
  // getGraphOnTarget
  TGraph *getGraphOnTarget(const unsigned int color,
			   const unsigned int offset) const{
    TGraph *gr = new TGraph();
    for(unsigned int i=0; i<_event.size(); i++){
      if(_event[i] -> hasHitOnTarget()) gr -> SetPoint(gr -> GetN(), i+offset, _event[i] -> _time);
    }
    gr -> SetMarkerStyle(20);
    gr -> SetMarkerColor(color);
    return gr;
  }
  // computeTimeAverage
  void computeTimeAverage(){
    _timeAverage = 0.;
    _timeRMS = 0.;
    for(unsigned int i=0; i<_event.size(); i++){
      _timeAverage += _event[i] -> _time;
      _timeRMS += (_event[i] -> _time * _event[i] -> _time);
    }
    _timeAverage /= _event.size();
    _timeRMS = sqrt(_timeRMS/_event.size() - _timeAverage * _timeAverage);
    return ;
  };
  vector<eventClass *> _event;
  double _timeAverage;
  double _timeRMS;
private:
};

////////////////
// spillClass
////////////////

class spillClass{
public:
  spillClass(){};
  // destructor
  ~spillClass(){
    for(unsigned int i=0; i<_interSpill.size(); i++){
      delete _interSpill[i];
    }
    _interSpill.clear();
  };
  // print
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": number of interspills = " << _interSpill.size() << endl;
    for(unsigned int i=0; i<_interSpill.size(); i++){
      _interSpill[i] -> print();
    }
    return ;
  };
  // getGraph
  TGraph *getGraph(const unsigned int color) const{
    TGraph *gr = new TGraph();
    for(unsigned int i=0; i<_interSpill.size(); i++){
      for(unsigned int j=0; j<_interSpill[i] -> _event.size(); j++){
	gr -> SetPoint(gr -> GetN(), gr -> GetN(), _interSpill[i] -> _event[j] -> _time);
      }
    }
    gr -> SetMarkerStyle(20);
    gr -> SetMarkerColor(color);
    return gr;
  }
  // computeTimeAverage
  void computeTimeAverage(){
    _timeAverage = 0.;
    _timeRMS = 0.;
    for(unsigned int i=0; i<_interSpill.size(); i++){
      _interSpill[i] -> computeTimeAverage();
      _timeAverage += _interSpill[i] -> _timeAverage;
      _timeRMS += _interSpill[i] -> _timeAverage * _interSpill[i] -> _timeAverage;
    }
    _timeAverage /= _interSpill.size();
    _timeRMS = sqrt(_timeRMS/_interSpill.size() - _timeAverage * _timeAverage);
    return ;
  };
  // computeEventLocations
  void computeEventLocations(){
    unsigned int spillLocation = 0;
    for(unsigned int i=0; i<_interSpill.size(); i++){
      unsigned int interSpillLocation = 0;
      for(unsigned int j=0; j<_interSpill[i] -> _event.size(); j++){
	_interSpill[i] -> _event[j] -> _spillLocation = spillLocation;
	_interSpill[i] -> _event[j] -> _interSpillLocation = interSpillLocation;
	spillLocation++;
	interSpillLocation++;
      }
    }
    return ;
  }
  vector<interSpillClass *> _interSpill;
  double _timeAverage;
  double _timeRMS;
private:
};

/////////////////
// computeNEvents
/////////////////

unsigned int computeNEvents(interSpillClass *interSpill1,
			    interSpillClass *interSpill2){
  if(interSpill1 -> _event.size() > interSpill2 -> _event.size()) return interSpill2 -> _event.size();
  else return interSpill1 -> _event.size();
}

//////////////////////
// computeNInterSpills
//////////////////////

unsigned int computeNInterSpills(spillClass *spill1,
				 spillClass *spill2){
  if(spill1 -> _interSpill.size() > spill2 -> _interSpill.size()) return spill2 -> _interSpill.size();
  else return spill1 -> _interSpill.size();
}

///////////////
// dtInterSpill
///////////////

double dtInterSpill(interSpillClass *interSpill1,
		    interSpillClass *interSpill2,
		    const unsigned int nEventsInInterSpill){
  double dt = 0.;
  for(unsigned int iEvent=0; iEvent<nEventsInInterSpill; iEvent++){
    dt += fabs(interSpill1 -> _event[iEvent] -> _time - interSpill2 -> _event[iEvent] -> _time);
  }
  return dt / nEventsInInterSpill;
}

//////////
// dtSpill
//////////

double dtSpill(spillClass *spill1,
	       spillClass *spill2,
	       const unsigned int nInterSpills){
  double dt = 0.;
  for(unsigned int iInterSpill=0; iInterSpill<nInterSpills; iInterSpill++){
    // computing number of events
    unsigned int nEventsInInterSpill = spill1 -> _interSpill[iInterSpill] -> _event.size();
    if(spill2 -> _interSpill[iInterSpill] -> _event.size() < nEventsInInterSpill) nEventsInInterSpill = spill2 -> _interSpill[iInterSpill] -> _event.size();

    // computing dtInterspill
    dt += dtInterSpill(spill1 -> _interSpill[iInterSpill],
		       spill2 -> _interSpill[iInterSpill],
		       nEventsInInterSpill);

  }
  return dt / (double)nInterSpills;;
}

/////////////////
// drawInterSpill
/////////////////

void drawInterSpill(vector<spillClass *> dataDUT,
		    vector<spillClass *> dataTelescope,
		    const unsigned int iSpill,
		    const unsigned int iSpillDUT,
		    const unsigned int iSpillTelescope,
		    const unsigned int iInterSpill,
		    const unsigned int iInterSpillDUT,
		    const unsigned int iInterSpillTelescope,
		    const unsigned int colorSpill,
		    const unsigned int colorDUT,
		    const unsigned int colorTelescope,
		    const bool save = false,
		    const unsigned int index = 0){

  TMultiGraph *mgIS = new TMultiGraph();
  char title[1000];
  sprintf(title, "S %d (DUT %d, telescope %d) IS %d (DUT %d, telescope %d)", iSpill, iSpillDUT, iSpillTelescope, iInterSpill, iInterSpillDUT, iInterSpillTelescope);
  mgIS -> SetTitle(title);
  mgIS -> Add(dataDUT[iSpillDUT] -> getGraph(colorSpill), "p");
  mgIS -> Add(dataTelescope[iSpillTelescope] -> getGraph(colorSpill), "p");
  mgIS -> Add(dataDUT[iSpillDUT] -> _interSpill[iInterSpillDUT] -> getGraph(colorDUT, dataDUT[iSpillDUT] -> _interSpill[iInterSpillDUT] -> _event[0] -> _spillLocation), "p");
  mgIS -> Add(dataTelescope[iSpillTelescope] -> _interSpill[iInterSpillTelescope] -> getGraph(colorTelescope, dataTelescope[iSpillTelescope] -> _interSpill[iInterSpillTelescope] -> _event[0] -> _spillLocation), "p");
  mgIS -> Draw("ap");  
  if(save){
    TCanvas *cc = new TCanvas("cc", "cc", 1000, 0, 1000, 1000);
    mgIS -> Draw("ap");    
    char name[1000];
    sprintf(name, "tmp/plot%06d.png", index);
    cc -> SaveAs(name);
    delete cc;
  }
  gPad -> WaitPrimitive();
  delete mgIS;

  return ;
}

///////////////////////////
// drawInterSpillWithTarget
///////////////////////////

void drawInterSpillWithTarget(vector<spillClass *> dataDUT,
			      vector<spillClass *> dataTelescope,
			      const unsigned int iSpill,
			      const unsigned int iSpillDUT,
			      const unsigned int iSpillTelescope,
			      const unsigned int iInterSpill,
			      const unsigned int iInterSpillDUT,
			      const unsigned int iInterSpillTelescope,
			      const unsigned int colorSpill,
			      const unsigned int colorDUT,
			      const unsigned int colorTelescope,
			      const unsigned int colorTarget,
			      const bool save = false,
			      const unsigned int index = 0){

  TMultiGraph *mgIS = new TMultiGraph();
  char title[1000];
  sprintf(title, "S %d (DUT %d, telescope %d) IS %d (DUT %d, telescope %d)", iSpill, iSpillDUT, iSpillTelescope, iInterSpill, iInterSpillDUT, iInterSpillTelescope);
  mgIS -> SetTitle(title);
  mgIS -> Add(dataDUT[iSpillDUT] -> getGraph(colorSpill), "p");
  mgIS -> Add(dataTelescope[iSpillTelescope] -> getGraph(colorSpill), "p");
  mgIS -> Add(dataDUT[iSpillDUT] -> _interSpill[iInterSpillDUT] -> getGraph(colorDUT, dataDUT[iSpillDUT] -> _interSpill[iInterSpillDUT] -> _event[0] -> _spillLocation), "p");
  mgIS -> Add(dataTelescope[iSpillTelescope] -> _interSpill[iInterSpillTelescope] -> getGraph(colorTelescope, dataTelescope[iSpillTelescope] -> _interSpill[iInterSpillTelescope] -> _event[0] -> _spillLocation), "p");
  TGraph *grTargetDUT = dataDUT[iSpillDUT] -> _interSpill[iInterSpillDUT] -> getGraphOnTarget(colorTarget, dataDUT[iSpillDUT] -> _interSpill[iInterSpillDUT] -> _event[0] -> _spillLocation);
  if(grTargetDUT -> GetN() != 0) mgIS -> Add(grTargetDUT, "p");
  TGraph *grTargetTelescope = dataTelescope[iSpillTelescope] -> _interSpill[iInterSpillTelescope] -> getGraphOnTarget(colorTarget, dataTelescope[iSpillTelescope] -> _interSpill[iInterSpillTelescope] -> _event[0] -> _spillLocation);
  if(grTargetTelescope -> GetN() != 0) mgIS -> Add(grTargetTelescope, "p");
  mgIS -> Draw("ap");  
  if(save){
    TCanvas *cc = new TCanvas("cc", "cc", 1000, 0, 1000, 1000);
    mgIS -> Draw("ap");
    char name[1000];
    sprintf(name, "tmp/plot%06d.png", index);
    cc -> SaveAs(name);
    delete cc;
  }
  gPad -> WaitPrimitive();
  delete mgIS;

  return ;
}

//////////
// isInRoI
//////////

bool isInRoI(const double x,
	     const double y,
	     const double xMin,
	     const double xMax,
	     const double yMin,
	     const double yMax){
  if(x >= xMin
     &&
     x <= xMax
     &&
     y >= yMin
     &&
     y <= yMax) return true;
  else return false;
}

////////
// isHit
////////

bool isHit(configClass *cfg,
	   const Float_t pulseT0,
	   const Float_t pulseRiseTime,
	   const Float_t pulseAmplitude){
  if(pulseT0 > cfg -> _pulseT0Range.min && pulseT0 < cfg -> _pulseT0Range.max
     &&
     pulseRiseTime > cfg -> _pulseRiseTimeRange.min && pulseRiseTime < cfg -> _pulseRiseTimeRange.max){
    return true;
  }
  else return false;
}

#endif
