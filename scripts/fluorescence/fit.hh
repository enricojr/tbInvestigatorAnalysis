#ifndef FIT_HH
#define FIT_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

#include <TStyle.h>
#include <TColor.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TFile.h>
#include <TTree.h>
#include <TLine.h>
#include <TMath.h>

#define NDRSCHANNELS 5
#define AMPLITUDENBINS 1024
#define AMPLITUDEMIN 0 
#define AMPLITUDEMAX 65536
#define AMPLITUDECL 0.998
#define RISETIMENBINS 500
#define RISETIMEMIN 0 
#define RISETIMEMAX 100
#define RISETIMECL 0.68
#define RISETIMELOW 10
#define RISETIMEHIGH 40

void getCLInterval(TH1F *h1,		   
		   double &low,
		   double &high,
		   const double center,
		   const double cl){
  const unsigned int centralBin = h1 -> GetXaxis() -> FindBin(center);
  TH1F *h1Left = (TH1F *) h1 -> Clone();
  TH1F *h1Right = (TH1F *) h1 -> Clone();
  for(unsigned int iBin=0; iBin<centralBin; iBin++){
    h1Right -> SetBinContent(iBin,0);
  }
  for(int iBin=centralBin; iBin<h1->GetXaxis()->GetNbins(); iBin++){
    h1Left -> SetBinContent(iBin,0);
  }
  TH1 *h1RightCumulative = h1Right -> GetCumulative();
  TH1 *h1LeftCumulative = h1Left -> GetCumulative();
  const double normLeft = h1Left -> Integral();
  const double normRight = h1Right -> Integral();
  h1LeftCumulative -> Scale(1./normLeft);
  h1RightCumulative -> Scale(1./normRight);
  TGraph *grLeft =  new TGraph();
  grLeft -> SetName("grLeft");
  for(int iBin=1; iBin<h1LeftCumulative->GetXaxis()->GetNbins()-1; iBin++){
    const double cl = h1LeftCumulative -> GetBinContent(iBin);
    const double val = h1LeftCumulative -> GetBinLowEdge(iBin);
    grLeft -> SetPoint(grLeft -> GetN(), cl, val);
    if(cl==1.) break;
  }
  TGraph *grRight =  new TGraph();
  grRight -> SetName("grRight");
  for(int iBin=1; iBin<h1RightCumulative->GetXaxis()->GetNbins()-1; iBin++){
    const double cl = h1RightCumulative -> GetBinContent(iBin);
    const double val = h1RightCumulative -> GetBinLowEdge(iBin);
    grRight -> SetPoint(grRight -> GetN(), cl, val);
    if(cl==1.) break;
  }
  low = grLeft -> Eval(1.-cl);
  high = grRight -> Eval(cl);
  delete h1Left;
  delete h1Right;
  delete h1LeftCumulative;
  delete h1RightCumulative;
  delete grLeft;
  delete grRight;
  return ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class targetClass{
public:
  targetClass(){};
  targetClass(string element,
	      unsigned int Z,
	      double Kb2,
	      double Kb1,
	      double Ka1,
	      double Ka2,
	      double Lg1,
	      double Lb2,
	      double Lb1,
	      double La1,
	      double La2){
    _element = element;
    _Z = Z;
    _Kb2 = Kb2;
    _Kb1 = Kb1;
    _Ka1 = Ka1;
    _Ka2 = Ka2;
    _Lg1 = Lg1;
    _Lb2 = Lb2;
    _Lb1 = Lb1;
    _La1 = La1;
    _La2 = La2;
  };
  ~targetClass(){};
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": element = " << _element << endl;
    cout << __PRETTY_FUNCTION__ << ": Z = " << _Z << endl;
    cout << __PRETTY_FUNCTION__ << ": Kb2 = " << _Kb2 << endl;
    cout << __PRETTY_FUNCTION__ << ": Kb1 = " << _Kb1 << endl;
    cout << __PRETTY_FUNCTION__ << ": Ka2 = " << _Ka2 << endl;
    cout << __PRETTY_FUNCTION__ << ": Ka1 = " << _Ka1 << endl;
    cout << __PRETTY_FUNCTION__ << ": Lg1 = " << _Lg1 << endl;
    cout << __PRETTY_FUNCTION__ << ": Lb2 = " << _Lb2 << endl;
    cout << __PRETTY_FUNCTION__ << ": Lb1 = " << _Lb1 << endl;
    cout << __PRETTY_FUNCTION__ << ": La1 = " << _La1 << endl;
    cout << __PRETTY_FUNCTION__ << ": La2 = " << _La2 << endl;
  }
  string getElement() const{
    return _element;
  }
  double getKa1() const{
    return _Ka1;
  }
  double getKa2() const{
    return _Ka2;
  }
  double getKb1() const{
    return _Kb1;
  }
  double getKb2() const{
    return _Kb2;
  }
  double getLb1() const{
    return _Lb1;
  }
  double getLb2() const{
    return _Lb2;
  }
  double getLa1() const{
    return _La1;
  }
  double getLa2() const{
    return _La2;
  }
  double getLg1() const{
    return _Lg1;
  }  
private:
  string _element;
  unsigned int _Z;
  double _Kb2;
  double _Kb1;
  double _Ka1;
  double _Ka2;
  double _Lg1;
  double _Lb2;
  double _Lb1;
  double _La1;
  double _La2;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class fluorescenceDataClass{
public:

  fluorescenceDataClass(){
    init();
  };

  ~fluorescenceDataClass(){
    for(unsigned int iTarget=0; iTarget<getNTargets(); iTarget++){
      delete _target[iTarget];
    }
    _target.clear();
  };
  
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": nTargets = " << getNTargets() << endl;
    for(unsigned int iTarget=0; iTarget<getNTargets(); iTarget++){
      _target[iTarget] -> print();
    }
  }

  void addTarget(targetClass *target){
    _target.push_back(target);
    _nTargets ++;
    return ;
  }

  unsigned int getNTargets() const{
    return _nTargets;
  }

  double getTargetKa1(const string element) const{
    for(unsigned int iTarget=0; iTarget<getNTargets(); iTarget++){
      if(_target[iTarget] -> getElement() == element) return _target[iTarget] -> getKa1();
      else continue;
    }
    return 0.;
  }
  
  double getTargetKa2(const string element) const{
    for(unsigned int iTarget=0; iTarget<getNTargets(); iTarget++){
      if(_target[iTarget] -> getElement() == element) return _target[iTarget] -> getKa2();
      else continue;
    }
    return 0.;
  }
  
  double getTargetKb1(const string element) const{
    for(unsigned int iTarget=0; iTarget<getNTargets(); iTarget++){
      if(_target[iTarget] -> getElement() == element) return _target[iTarget] -> getKb1();
      else continue;
    }
    return 0.;
  }
  
  double getTargetKb2(const string element) const{
    for(unsigned int iTarget=0; iTarget<getNTargets(); iTarget++){
      if(_target[iTarget] -> getElement() == element) return _target[iTarget] -> getKb2();
      else continue;
    }
    return 0.;
  }

  double getTargetLa1(const string element) const{
    for(unsigned int iTarget=0; iTarget<getNTargets(); iTarget++){
      if(_target[iTarget] -> getElement() == element) return _target[iTarget] -> getLa1();
      else continue;
    }
    return 0.;
  }
  
  double getTargetLa2(const string element) const{
    for(unsigned int iTarget=0; iTarget<getNTargets(); iTarget++){
      if(_target[iTarget] -> getElement() == element) return _target[iTarget] -> getLa2();
      else continue;
    }
    return 0.;
  }
  
  double getTargetLb1(const string element) const{
    for(unsigned int iTarget=0; iTarget<getNTargets(); iTarget++){
      if(_target[iTarget] -> getElement() == element) return _target[iTarget] -> getLb1();
      else continue;
    }
    return 0.;
  }
  
  double getTargetLb2(const string element) const{
    for(unsigned int iTarget=0; iTarget<getNTargets(); iTarget++){
      if(_target[iTarget] -> getElement() == element) return _target[iTarget] -> getLb2();
      else continue;
    }
    return 0.;
  }
  
  double getTargetLg1(const string element) const{
    for(unsigned int iTarget=0; iTarget<getNTargets(); iTarget++){
      if(_target[iTarget] -> getElement() == element) return _target[iTarget] -> getLg1();
      else continue;
    }
    return 0.;
  }   

  double getTargetKa(const string element) const{
    const double targetKa1 = getTargetKa1(element);
    if(targetKa1 == 0.) return 0.;
    const double targetKa2 = getTargetKa2(element);
    if(targetKa2 == 0.) return 0.;
    return 0.5 * (targetKa1 + targetKa2);
  }
  
  double getTargetKb(const string element) const{
    const double targetKb1 = getTargetKb1(element);
    if(targetKb1 == 0.) return 0.;
    const double targetKb2 = getTargetKb2(element);
    if(targetKb2 == 0.) return 0.;
    return 0.5 * (targetKb1 + targetKb2);
  }
  
  double getTargetLa(const string element) const{
    const double targetLa1 = getTargetLa1(element);
    if(targetLa1 == 0.) return 0.;
    const double targetLa2 = getTargetLa2(element);
    if(targetLa2 == 0.) return 0.;
    return 0.5 * (targetLa1 + targetLa2);
  }
  
  double getTargetLb(const string element) const{
    const double targetLb1 = getTargetLb1(element);
    if(targetLb1 == 0.) return 0.;
    const double targetLb2 = getTargetLb2(element);
    if(targetLb2 == 0.) return 0.;
    return 0.5 * (targetLb1 + targetLb2);
  }
  
  double getTargetLg(const string element) const{
    return getTargetLg1(element);
  }
  
private:
  unsigned int _nTargets;
  vector<targetClass *> _target;
  void init(){
    _nTargets = 0;
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class setClass{
public:
  
  setClass(){
    init();
  };

  setClass(const string wafer,
	   const unsigned int matrix,
	   const string target,
	   const unsigned int pixel,
	   const double peak,
	   const vector<unsigned int> nPeaks,
	   const double threshold,
	   const string path){
    init();
    _wafer = wafer;
    _matrix = matrix;
    _target = target;
    _pixel = pixel;
    _peak = peak;
    _nPeaks = nPeaks;
    _threshold = threshold;
    _path = path;
  };
  
  ~setClass(){
    delete _h1Amplitude;
    delete _h1RiseTime;
    delete _h2RiseTimeVsAmplitude;
    delete _fFit;
    delete _fKAlpha;
    if(_nPeaks[2] != 0) delete _fKBeta;
    delete _fBg;
    _peakFromFit.clear();
  };

  void print() const{
    cout << __PRETTY_FUNCTION__ << ": wafer = " << _wafer << endl;
    cout << __PRETTY_FUNCTION__ << ": matrix = " << _matrix << endl;
    cout << __PRETTY_FUNCTION__ << ": target = " << _target << endl;
    cout << __PRETTY_FUNCTION__ << ": pixel = " << _pixel << endl;
    cout << __PRETTY_FUNCTION__ << ": peak = " << _peak << endl;
    cout << __PRETTY_FUNCTION__ << ": nPeaks = ";
    for(unsigned int iPeak=0; iPeak<_nPeaks.size(); iPeak++){
      cout << _nPeaks[iPeak];
      if(iPeak != _nPeaks.size()-1) cout << ",";
    }
    cout << endl;
    cout << __PRETTY_FUNCTION__ << ": threshold = " << _threshold << endl;
    cout << __PRETTY_FUNCTION__ << ": path = " << _path << endl;
    cout << __PRETTY_FUNCTION__ << ": amplitudeMin = " << _amplitudeMin << endl;
    cout << __PRETTY_FUNCTION__ << ": amplitudeMax = " << _amplitudeMax << endl;
    cout << __PRETTY_FUNCTION__ << ": amplitudeLow = " << _amplitudeLow << endl;
    cout << __PRETTY_FUNCTION__ << ": amplitudeHigh = " << _amplitudeHigh << endl;
    cout << __PRETTY_FUNCTION__ << ": riseTimeMin = " << _riseTimeMin << endl;
    cout << __PRETTY_FUNCTION__ << ": riseTimeMax = " << _riseTimeMax << endl;
    cout << __PRETTY_FUNCTION__ << ": riseTimeLow = " << _riseTimeLow << endl;
    cout << __PRETTY_FUNCTION__ << ": riseTimeHigh = " << _riseTimeHigh << endl;
  };

  string getPath() const{
    return _path;
  };

  unsigned int getPixel() const{
    return _pixel;
  };

  string getWafer() const{
    return _wafer;
  };
  
  unsigned int getMatrix() const{
    return _matrix;
  };
  
  string getTarget() const{
    return _target;
  };
  
  unsigned int getNPeaks(const unsigned int index) const{
    return _nPeaks[index];
  };

  double getThreshold() const{
    return _threshold;
  };
  
  void setAmplitudeMin(const double amplitudeMin){
    _amplitudeMin = amplitudeMin;
    return ;
  };
  
  void setAmplitudeMax(const double amplitudeMax){
    _amplitudeMax = amplitudeMax;
    return ;
  };
  
  void setRiseTimeMin(const double riseTimeMin){
    _riseTimeMin = riseTimeMin;
    return ;
  };
  
  void setRiseTimeMax(const double riseTimeMax){
    _riseTimeMax = riseTimeMax;
    return ;
  };
  
  double getAmplitudeMin() const{
    return _amplitudeMin;
  };
  
  double getAmplitudeMax() const{
    return _amplitudeMax;
  };

  double getRiseTimeMin() const{
    return _riseTimeMin;
  };

  double getRiseTimeMax() const{
    return _riseTimeMax;
  };
  
  void addEntryToSpectrum(const double amplitude,
			  const double weight = 1.){
    _h1Amplitude -> Fill(amplitude, weight);
    return ;
  };

  void addEntryToRiseTime(const double riseTime){
    _h1RiseTime -> Fill(riseTime);
    return ;
  };

  void addEntryToRiseTimeVsAmplitude(const double pulseRiseTime,
				     const double pulseAmplitude){
    _h2RiseTimeVsAmplitude -> Fill(pulseAmplitude, pulseRiseTime);
    return ;
  };

  void findRiseTimeRange(){
    const double mean = _h1RiseTime -> GetMean();
    getCLInterval(_h1RiseTime, _riseTimeLow, _riseTimeHigh, mean, RISETIMECL);    
    return ;
  };
  
  void findAmplitudeRange(){
    if(_peak == 0.) _peak = _h1Amplitude -> GetBinLowEdge(_h1Amplitude -> GetMaximumBin());
    getCLInterval(_h1Amplitude, _amplitudeLow, _amplitudeHigh, _peak, AMPLITUDECL);    
    return ;
  };
  
  void buildSpectrum(){
    for(int iX=0; iX<_h2RiseTimeVsAmplitude -> GetXaxis() -> GetNbins(); iX++){
      const double xx = _h2RiseTimeVsAmplitude -> GetXaxis() -> GetBinLowEdge(iX+1);
      for(int iY=0; iY<_h2RiseTimeVsAmplitude -> GetYaxis() -> GetNbins(); iY++){
	const double yy = _h2RiseTimeVsAmplitude -> GetYaxis() -> GetBinLowEdge(iY+1);
	if(yy > _riseTimeLow && yy < _riseTimeHigh){
	  const double ww = _h2RiseTimeVsAmplitude -> GetBinContent(iX+1, iY+1);
	  addEntryToSpectrum(xx, ww);
	}
	else continue;
      }
    }
    return ;
  };

  int fitSpectrum(){

    if(_nPeaks[0] == 1 && _nPeaks[1] == 0 && _nPeaks[2] == 0 && _nPeaks[3] == 0 && _nPeaks[4] == 0){
      _fFit = new TF1("fFit", "[0]+[1]*x+[2]*exp(-0.5*((x-[3])/[4])**2)");
      _fFit -> SetParNames("bg_{offset}", "bg_{slope}", "scale_", "#mu_{#alpha}", "#sigma");
    }
    else if(_nPeaks[0] == 1 && _nPeaks[1] == 0 && _nPeaks[2] == 1 && _nPeaks[3] == 0 && _nPeaks[4] == 0){
      _fFit = new TF1("fFit", "[0]+[1]*x+[2]*(exp(-0.5*((x-[3])/[4])**2)+[5]*exp(-0.5*((x-[3]-[6])/[4])**2))");
      _fFit -> SetParNames("bg_{offset}", "bg_{slope}", "scale_", "#mu_{#alpha}", "#sigma", "#frac{#beta}{#alpha}", "#Delta#mu_{#beta}");
    }
    else{
      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - invalid nPeaks = ";
      for(unsigned int iPeak=0; iPeak<_nPeaks.size(); iPeak++){
	cout << _nPeaks[iPeak];
	if(iPeak != _nPeaks.size()-1) cout << ",";
      }
      cout << endl;    
      return 1;
    }
    
    _fFit -> SetRange(_peak-200., _amplitudeHigh);

    _fFit -> SetParameter(0, 1.);
    _fFit -> SetParameter(1, -0.001);
    _fFit -> SetParameter(2, _h1Amplitude -> GetBinContent(_h1Amplitude -> FindBin(_peak)));
    _fFit -> SetParameter(3, _peak);
    _fFit -> SetParameter(4, 200.);

    _fFit -> SetParLimits(0, 0., 100.);
    _fFit -> SetParLimits(1, -0.01, 0.);    
    _fFit -> SetParLimits(2, 1., 100000.);
    _fFit -> SetParLimits(3, 0., AMPLITUDEMAX);
    _fFit -> SetParLimits(4, 0., 1000.);

    if(_nPeaks[0] == 1 && _nPeaks[1] == 0 && _nPeaks[2] == 1 && _nPeaks[3] == 0 && _nPeaks[4] == 0){
      _fFit -> SetParameter(5, 0.2);
      _fFit -> SetParameter(6, 200.);
      _fFit -> SetParLimits(5, 0., 1.);
      _fFit -> SetParLimits(6, 0., 2000.);
    }
    
    _h1Amplitude -> Fit("fFit", "R && Q");

    _peakFromFit.push_back(pair<double, double> (_fFit -> GetParameter(3), _fFit -> GetParameter(4)));
    if(_nPeaks[0] == 1 && _nPeaks[1] == 0 && _nPeaks[2] == 1 && _nPeaks[3] == 0 && _nPeaks[4] == 0) _peakFromFit.push_back(pair<double, double> (_fFit -> GetParameter(3)+_fFit -> GetParameter(6), _fFit -> GetParameter(4)));

    double fitRangeMin = 0;
    double fitRangeMax = 0;
    _fFit -> GetRange(fitRangeMin, fitRangeMax);

    _fBg = new TF1("fBg", "[0]+[1]*x", fitRangeMin, fitRangeMax);
    _fBg -> SetParNames("bg_{offset}", "bg_{slope}");
    _fBg -> SetParameter(0, _fFit -> GetParameter(0));
    _fBg -> SetParameter(1, _fFit -> GetParameter(1));

    _fKAlpha = new TF1("fKAlpha", "gaus(0)", fitRangeMin, fitRangeMax);
    _fKAlpha -> SetParNames("scale", "#mu_{#alpha}", "#sigma");
    _fKAlpha -> SetParameter(0, _fFit -> GetParameter(2));
    _fKAlpha -> SetParameter(1, _fFit -> GetParameter(3));
    _fKAlpha -> SetParameter(2, _fFit -> GetParameter(4));

    if(_nPeaks[0] == 1 && _nPeaks[1] == 0 && _nPeaks[2] == 1 && _nPeaks[3] == 0 && _nPeaks[4] == 0){
      _fKBeta = new TF1("fKBeta", "gaus(0)", fitRangeMin, fitRangeMax);
      _fKBeta -> SetParNames("scale", "#mu_{#beta}", "#sigma");
      _fKBeta -> SetParameter(0, _fFit -> GetParameter(2) * _fFit -> GetParameter(5));
      _fKBeta -> SetParameter(1, _fFit -> GetParameter(3)+_fFit -> GetParameter(6));
      _fKBeta -> SetParameter(2, _fFit -> GetParameter(4));
    }
    
    return 0;
  };
  
  int save(const string path){
    
    stringstream matrix_ss;
    matrix_ss << _matrix;
    stringstream pixel_ss;
    pixel_ss << _pixel;
    string name = "";
    string title = "";

    gStyle->SetNumberContours(255);
    gStyle -> SetPalette(55);
    gStyle -> SetOptStat(0);
    gStyle -> SetOptFit(0);
    TLine *line = NULL;

    // 2D plots
    gStyle -> SetPadTopMargin(0.1);
    gStyle -> SetPadRightMargin(0.2);
    gStyle -> SetPadBottomMargin(0.1);
    gStyle -> SetPadLeftMargin(0.15);    
    TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 1200, 1000);

    title = _wafer + " M" + matrix_ss.str() + " " + _target + " pixel " + pixel_ss.str();
    _h2RiseTimeVsAmplitude -> SetTitle(title.c_str());
    _h2RiseTimeVsAmplitude -> GetXaxis() -> SetTitle("Signal S #left[a.u.#right]");
    _h2RiseTimeVsAmplitude -> GetYaxis() -> SetTitle("Rise time t #left[0.5 ns#right]");
    _h2RiseTimeVsAmplitude -> GetZaxis() -> SetTitle("#frac{d^{2}N}{dSdt} #left[(a.u. #times 0.5 ns)^{-1}#right]");
    _h2RiseTimeVsAmplitude -> GetXaxis() -> SetTitleOffset(1.5);
    _h2RiseTimeVsAmplitude -> GetYaxis() -> SetTitleOffset(1.5);
    _h2RiseTimeVsAmplitude -> GetZaxis() -> SetTitleOffset(1.5);
    _h2RiseTimeVsAmplitude -> GetXaxis() -> SetRangeUser(getAmplitudeMin(), getAmplitudeMax());
    _h2RiseTimeVsAmplitude -> GetYaxis() -> SetRangeUser(getRiseTimeMin(), getRiseTimeMax());
    _h2RiseTimeVsAmplitude -> Draw("colz");
    line = new TLine(getAmplitudeMin(), _riseTimeLow, getAmplitudeMax(), _riseTimeLow);
    line -> SetLineColor(1);
    line -> SetLineWidth(3);
    line -> SetLineStyle(2);
    line -> Draw();
    line = new TLine(getAmplitudeMin(), _riseTimeHigh, getAmplitudeMax(), _riseTimeHigh);
    line -> SetLineColor(1);
    line -> SetLineWidth(3);
    line -> SetLineStyle(2);
    line -> Draw();
    line = new TLine(_amplitudeLow, getRiseTimeMin(), _amplitudeLow, getRiseTimeMax());
    line -> SetLineColor(1);
    line -> SetLineWidth(3);
    line -> SetLineStyle(2);
    line -> Draw();
    line = new TLine(_amplitudeHigh, getRiseTimeMin(), _amplitudeHigh, getRiseTimeMax());
    line -> SetLineColor(1);
    line -> SetLineWidth(3);
    line -> SetLineStyle(2);
    line -> Draw();
    name = path + "/h2RiseTimeVsAmplitude" + _wafer + "-M" + matrix_ss.str() + "-" + _target + "-p" + pixel_ss.str();
    cc -> SaveAs((name + ".pdf").c_str());
    cc -> SaveAs((name + ".png").c_str());
    cc -> SaveAs((name + ".root").c_str());
    delete cc;

    // 1D plots
    gStyle -> SetPadTopMargin(0.1);
    gStyle -> SetPadRightMargin(0.05);
    gStyle -> SetPadBottomMargin(0.1);
    gStyle -> SetPadLeftMargin(0.15);    
    cc = new TCanvas("cc", "cc", 0, 0, 1000, 1000);
    
    title = _wafer + " M" + matrix_ss.str() + " " + _target + " pixel " + pixel_ss.str();
    _h1Amplitude -> SetTitle(title.c_str());
    _h1Amplitude -> SetLineColor(1);
    _h1Amplitude -> SetLineWidth(3);
    _h1Amplitude -> SetFillColor(20);
    _h1Amplitude -> GetXaxis() -> SetTitle("Signal S #left[a.u.#right]");
    _h1Amplitude -> GetYaxis() -> SetTitle("#frac{dN}{dS} #left[(a.u.)^{-1}#right]");
    _h1Amplitude -> GetXaxis() -> SetTitleOffset(1.5);
    _h1Amplitude -> GetYaxis() -> SetTitleOffset(2);
    _h1Amplitude -> GetXaxis() -> SetRangeUser(_amplitudeLow, _amplitudeHigh);
    _h1Amplitude -> Draw();
    _fFit -> SetLineWidth(3);
    _fFit -> SetLineColor(2);
    _fFit -> Draw("same");
    _fBg -> SetLineWidth(3);
    _fBg -> SetLineColor(3);
    _fBg -> Draw("same");
    _fKAlpha -> SetLineWidth(3);
    _fKAlpha -> SetLineColor(4);
    _fKAlpha -> Draw("same");
    if(_nPeaks[0] == 1 && _nPeaks[1] == 0 && _nPeaks[2] == 1 && _nPeaks[3] == 0 && _nPeaks[4] == 0){
      _fKBeta -> SetLineWidth(3);
      _fKBeta -> SetLineColor(7);
      _fKBeta -> Draw("same");
    }
    line = new TLine(_peak, 0, _peak, _h1Amplitude->GetBinContent(_h1Amplitude->GetMaximumBin()));
    line -> SetLineColor(1);
    line -> SetLineWidth(3);
    line -> SetLineStyle(2);
    line -> Draw();    
    name = path + "/h1Amplitude-" + _wafer + "-M" + matrix_ss.str() + "-" + _target + "-p" + pixel_ss.str();
    cc -> SaveAs((name + ".pdf").c_str());
    cc -> SaveAs((name + ".png").c_str());
    cc -> SaveAs((name + ".root").c_str());

    _h1RiseTime -> SetTitle(title.c_str());
    _h1RiseTime -> SetLineColor(1);
    _h1RiseTime -> SetLineWidth(3);
    _h1RiseTime -> SetFillColor(30);
    _h1RiseTime -> GetXaxis() -> SetTitle("Rise time t #left[0.5 ns#right]");
    _h1RiseTime -> GetYaxis() -> SetTitle("#frac{dN}{dt} #left[(0.5 ns)^{-1}#right]");
    _h1RiseTime -> GetXaxis() -> SetTitleOffset(1.5);
    _h1RiseTime -> GetYaxis() -> SetTitleOffset(2);
    _h1RiseTime -> GetXaxis() -> SetRangeUser(getRiseTimeMin(), getRiseTimeMax());
    _h1RiseTime -> Draw();
    line = new TLine(_riseTimeLow, 0, _riseTimeLow, _h1RiseTime->GetBinContent(_h1RiseTime->GetMaximumBin()));
    line -> SetLineColor(1);
    line -> SetLineWidth(3);
    line -> SetLineStyle(2);
    line -> Draw();
    line = new TLine(_riseTimeHigh, 0, _riseTimeHigh, _h1RiseTime->GetBinContent(_h1RiseTime->GetMaximumBin()));
    line -> SetLineColor(1);
    line -> SetLineWidth(3);
    line -> SetLineStyle(2);
    line -> Draw();
    name = path + "/h1RiseTime-" + _wafer + "-M" + matrix_ss.str() + "-" + _target + "-p" + pixel_ss.str();
    cc -> SaveAs((name + ".pdf").c_str());
    cc -> SaveAs((name + ".png").c_str());
    cc -> SaveAs((name + ".root").c_str());

    delete cc;
  };

  TF1 *getFitFunction() const{
    return _fFit;
  };
  
private:

  string _wafer;
  unsigned int _matrix;
  string _target;
  unsigned int _pixel;
  double _peak;
  vector<unsigned int> _nPeaks;
  double _threshold;
  string _path;
  double _amplitudeMin;
  double _amplitudeMax;
  double _amplitudeLow;
  double _amplitudeHigh;
  double _riseTimeMin;
  double _riseTimeMax;
  double _riseTimeLow;
  double _riseTimeHigh;

  TH2F *_h2RiseTimeVsAmplitude;
  TH1F *_h1RiseTime;
  TH1F *_h1Amplitude;
  TF1 *_fFit;
  TF1 *_fKAlpha;
  TF1 *_fKBeta;
  TF1 *_fBg;

  vector<pair<double, double> > _peakFromFit;
  
  void init(){
    _wafer = "";
    _matrix = 0;
    _target = "";
    _pixel = 777;
    _peak = 0.;
    _threshold = 0.;
    _path = "";
    _amplitudeMin = 0.;
    _amplitudeMax = 0.;
    _amplitudeLow = 0.;
    _amplitudeHigh = 0.;
    _riseTimeMin = 0.;
    _riseTimeMax = 0.;
    _riseTimeLow = 0.;
    _riseTimeHigh = 0.;
    _h1Amplitude = new TH1F("spectrum", "spectrum", AMPLITUDENBINS, AMPLITUDEMIN, AMPLITUDEMAX);
    _h1RiseTime = new TH1F("riseTime", "riseTime", RISETIMENBINS, RISETIMEMIN, RISETIMEMAX);
    _h2RiseTimeVsAmplitude = new TH2F("riseTimeVsAmplitude", "rise time vs amplitude", AMPLITUDENBINS, AMPLITUDEMIN, AMPLITUDEMAX, RISETIMENBINS, RISETIMEMIN, RISETIMEMAX);
    _fFit = NULL;
    _fKAlpha = NULL;
    _fKBeta = NULL;
    _fBg = NULL;
  };
  
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class calibrationClass{
public:
  
  calibrationClass(){
    init();
  };
  
  calibrationClass(const string wafer,
		   const unsigned int matrix,
		   const unsigned int pixel){    
    init();
    _wafer = wafer;
    _matrix = matrix;
    _pixel = pixel;
    buildGraphName();    
  };
  
  ~calibrationClass(){
    if(_name == NULL) delete _name;
    if(_title == NULL) delete _title;
    delete _gr;
    delete _fFit;
  };

  void buildGraphName(){
    _name = new char[1000];
    sprintf(_name, "%s-M%d-p%d", _wafer.c_str(), _matrix, _pixel);
    _gr -> SetName(_name);
    _title = new char[1000];
    sprintf(_title, "%s M%d pixel %d", _wafer.c_str(), _matrix, _pixel);
    _gr -> SetTitle(_title);
  };

  void print() const{
    cout << __PRETTY_FUNCTION__ << ": wafer = " << _wafer << endl;
    cout << __PRETTY_FUNCTION__ << ": matrix = " << _matrix << endl;
    cout << __PRETTY_FUNCTION__ << ": pixel = " << _pixel << endl;
    cout << __PRETTY_FUNCTION__ << ": name = " << _name << endl;
    cout << __PRETTY_FUNCTION__ << ": title = " << _title << endl;
    return ;
  };

  void addPoint(const double energy,
		const double signal,
		const double error){
    _gr -> SetPoint(_gr -> GetN(), energy, signal);
    _gr -> SetPointError(_gr -> GetN()-1, 0., error);
    return ;
  };

  void fit(){
    _fFit = new TF1(_name, "[0]+[1]*x");    
    _gr -> Fit(_name, "Q");
    return ;
  };
  
  void save(const string path){

    stringstream matrix_ss;
    matrix_ss << _matrix;
    stringstream pixel_ss;
    pixel_ss << _pixel;
    string name = "";

    gStyle->SetNumberContours(255);
    gStyle -> SetPalette(55);
    gStyle -> SetOptStat(0);
    gStyle -> SetOptFit(0);
    TLine *line = NULL;

    // 1D plots
    gStyle -> SetPadTopMargin(0.1);
    gStyle -> SetPadRightMargin(0.05);
    gStyle -> SetPadBottomMargin(0.1);
    gStyle -> SetPadLeftMargin(0.15);    
    TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 1200, 1000);
    _gr -> GetXaxis() -> SetTitle("Energy #left[KeV#right]");    
    _gr -> GetYaxis() -> SetTitle("Signal #left[a.u.#right]");
    _gr -> GetXaxis() -> SetTitleOffset(1.5);
    _gr -> GetYaxis() -> SetTitleOffset(2.0);
    _gr -> GetXaxis() -> SetRangeUser(0., 30.); // hardcoded
    _gr -> GetYaxis() -> SetRangeUser(0., 10000); // hardcoded
    _gr -> SetMarkerStyle(20);
    _gr -> SetMarkerColor(1);
    _gr -> SetLineColor(1);
    _gr -> SetLineWidth(3);    
    _gr -> Draw("ap");
    name = path + "/grCalibration" + _wafer + "-M" + matrix_ss.str() + "-p" + pixel_ss.str();
    cc -> SaveAs((name + ".pdf").c_str());
    cc -> SaveAs((name + ".png").c_str());
    cc -> SaveAs((name + ".root").c_str());
    delete cc;
    return ;
  };

  void writeToFile(ofstream &file){
    file << _wafer << "\t"
	 << _matrix << "\t"
	 << _pixel << "\t";
    for(int iPar=0; iPar<_fFit -> GetNpar(); iPar++){
      file << _fFit -> GetParameter(iPar) << "\t"
	   << _fFit -> GetParError(iPar) << "\t";
    }
    file << endl;
    return ;
  }
  
private:
  string _wafer;
  unsigned int _matrix;
  unsigned int _pixel;
  char *_name;
  char *_title;
  TGraphErrors *_gr = new TGraphErrors();
  TF1 *_fFit;
  void init(){
    _wafer = "";
    _matrix = 0;
    _pixel = 0;
    _name = NULL;
    _title = NULL;
    _gr = new TGraphErrors();
    return ;
  };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class fitClass{
public:
  fitClass();
  ~fitClass();
  void print() const;
  int loadConfig(const char *fileName);
  unsigned int getNSets() const;
  setClass *getSet(const unsigned int iSet);
  void addSet(setClass *set);
  int fitSpectra(const string outputFolder);
  int buildCalibrationCurves(const string outputFolder);
  fluorescenceDataClass *getFluorescenceData() const;
  void addTarget(targetClass *target);
  int saveResults(const string outputFolder) const;
private:
  fluorescenceDataClass *_fluorescenceData;
  unsigned int _nSets;
  vector<setClass *> _set;
  vector<calibrationClass *> _calibration;
  void init();
  int loadFluorescenceData(const char *fileName);
};

fitClass::fitClass(){
  init();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

fitClass::~fitClass(){
  for(unsigned int iSet=0; iSet<getNSets(); iSet++){
    delete _set[iSet];
  }
  _set.clear();
  delete _fluorescenceData;
  for(unsigned int i=0; i<_calibration.size(); i++){
    delete _calibration[i];
  }
  _calibration.clear();
};

void fitClass::init(){
  const char *fileNameFluorescenceData = "../aux/fluorescence.dat";
  _nSets = 0.;
  if(loadFluorescenceData(fileNameFluorescenceData)){
    cout << __PRETTY_FUNCTION__ << ": cannot load fluorescence data" << endl;
    return ;
  }
  return ;
}

void fitClass::print() const{
  cout << __PRETTY_FUNCTION__ << ": fluorescence data" << endl;
  getFluorescenceData() -> print();
  cout << __PRETTY_FUNCTION__ << ": nSets = " << getNSets() << endl;
  for(unsigned int iSet=0; iSet<getNSets(); iSet++){
    cout << __PRETTY_FUNCTION__ << ": set #" << iSet << endl;
    _set[iSet] -> print();
  }
  return ;
}

fluorescenceDataClass *fitClass::getFluorescenceData() const{
  return _fluorescenceData;
}

void fitClass::addTarget(targetClass *target){
  _fluorescenceData -> addTarget(target);
  return ;
}

int fitClass::loadFluorescenceData(const char *fileName){

  ifstream file(fileName);
  if(!file){
    cout << __PRETTY_FUNCTION__ << ": cannot open file " << fileName << endl;
    return 0;
  }

  _fluorescenceData = new fluorescenceDataClass();
  
  string line = "";
  getline(file, line);
  while(getline(file, line)){
    char element[10];
    unsigned int Z = 0;
    double Kb2 = 0.;
    double Kb1 = 0.;
    double Ka1 = 0.;
    double Ka2 = 0.;
    double Lg1 = 0.;
    double Lb2 = 0.;
    double Lb1 = 0.;
    double La1 = 0.;
    double La2 = 0.;
    sscanf(line.c_str(), "%s\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf", element, &Z, &Kb2, &Kb1, &Ka1, &Ka2, &Lg1, &Lb2, &Lb1, &La1, &La2);
    addTarget(new targetClass(element, Z, Kb2, Kb1, Ka1, Ka2, Lg1, Lb2, Lb1, La1, La2));
  }
  file.close();
  
  return 0;
}

int fitClass::loadConfig(const char *fileName){

  ifstream file(fileName);
  if(!file){
    cout << __PRETTY_FUNCTION__ << ": cannot open file " << fileName << endl;
    return 0;
  }
  string line = "";
  while(getline(file, line)){
    if(line[0] == '#') continue;
    char wafer[100];
    unsigned int matrix = 0;
    char target[100];
    unsigned int pixel = 0;
    double peak = 0.;
    unsigned int NKAlpha = 0;
    unsigned int NKBeta = 0;
    unsigned int NLAlpha = 0;
    unsigned int NLBeta = 0;
    unsigned int NLGamma = 0;
    double threshold = 0.;
    char path[1000];
    sscanf(line.c_str(), "%s\t%d\t%s\t%d\t%lf\t%d,%d,%d,%d,%d\t%lf\t%s", wafer, &matrix, target, &pixel, &peak, &NKAlpha, &NKBeta, &NLAlpha, &NLBeta, & NLGamma, &threshold, path);
    vector<unsigned int> nPeaks;
    nPeaks.push_back(NKAlpha);
    nPeaks.push_back(NKBeta);
    nPeaks.push_back(NLAlpha);
    nPeaks.push_back(NLBeta);
    nPeaks.push_back(NLGamma);
    addSet(new setClass(wafer, matrix, target, pixel, peak, nPeaks, threshold, path));
  }
  file.close();

  return 0;
}

unsigned int fitClass::getNSets() const{
  return _nSets;
}

setClass *fitClass::getSet(const unsigned int iSet){
  return _set[iSet];
}

void fitClass::addSet(setClass *set){
  _set.push_back(set);
  _nSets++;
  return ;
}

int fitClass::fitSpectra(const string outputFolder){
  
  for(unsigned int iSet=0; iSet<getNSets(); iSet++){
    cout << __PRETTY_FUNCTION__ << ": processing set #" << iSet << endl;
    TFile *file = TFile::Open(getSet(iSet) -> getPath().c_str());
    if(file == NULL){
      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << getSet(iSet) -> getPath() << endl;
      return 1;
    }

    TTree *tree = (TTree *) file -> Get("DUTTree");
    if(tree == NULL){
      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot load tree" << endl;
      return 0;
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

    unsigned int indexShift = 0;
    if(getSet(iSet) -> getPixel() == 0) indexShift = 0;
    else if(getSet(iSet) -> getPixel() == 1) indexShift = 1;
    else if(getSet(iSet) -> getPixel() == 2) indexShift = 2;
    else if(getSet(iSet) -> getPixel() == 3) indexShift = 0;
    else{
      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - undefined pixel number = " << getSet(iSet) -> getPixel() << endl;
      return 1;
    }
    cout << __PRETTY_FUNCTION__ << ": indexShift = " << indexShift << endl;
    
    for(unsigned int iEntry=0; iEntry<tree -> GetEntries(); iEntry+=NDRSCHANNELS){
      tree -> GetEntry( iEntry + indexShift );

      // selection
      if(pulseAmplitude < getSet(iSet) -> getThreshold()) continue;      
      if(pulseRiseTime<RISETIMELOW || pulseRiseTime>RISETIMEHIGH) continue;

      // min/max
      if(pulseAmplitude < getSet(iSet) -> getAmplitudeMin()) getSet(iSet) -> setAmplitudeMin(pulseAmplitude);
      if(pulseAmplitude > getSet(iSet) -> getAmplitudeMax()) getSet(iSet) -> setAmplitudeMax(pulseAmplitude);
      if(pulseRiseTime < getSet(iSet) -> getRiseTimeMin()) getSet(iSet) -> setRiseTimeMin(pulseRiseTime);
      if(pulseRiseTime > getSet(iSet) -> getRiseTimeMax()) getSet(iSet) -> setRiseTimeMax(pulseRiseTime);
      
      // filling plots
      getSet(iSet) -> addEntryToRiseTimeVsAmplitude(pulseRiseTime, pulseAmplitude);
      getSet(iSet) -> addEntryToRiseTime(pulseRiseTime);
    }

    getSet(iSet) -> findRiseTimeRange();
    getSet(iSet) -> buildSpectrum();
    getSet(iSet) -> findAmplitudeRange();
    if(getSet(iSet) -> fitSpectrum()){
      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot fit spectrum for set " << iSet << endl;
      return 1;
    }
    getSet(iSet) -> save(outputFolder);
    
    delete tree;
    file -> Close();
    delete file;
  }
  
  return 0;
}

int fitClass::buildCalibrationCurves(const string outputFolder){

  // collecting wafers
  cout << __PRETTY_FUNCTION__ << ": collecting wafers" << endl;
  vector<string> wafer;
  for(unsigned int iSet=0; iSet<getNSets(); iSet++){
    bool found = false;
    for(unsigned int iWafer=0; iWafer<wafer.size(); iWafer++){
      if(getSet(iSet) -> getWafer() == wafer[iWafer]){
	found = true;
	break;
      }
    }
    if(!found) wafer.push_back(getSet(iSet) -> getWafer());
  }
  for(unsigned int iWafer=0; iWafer<wafer.size(); iWafer++){
    cout << __PRETTY_FUNCTION__ << ": wafer[" << iWafer << "] = " << wafer[iWafer] << endl;
  }
  
  // collecting matrices
  cout << __PRETTY_FUNCTION__ << ": collecting matrices" << endl;
  vector<unsigned int> matrix;
  for(unsigned int iSet=0; iSet<getNSets(); iSet++){
    bool found = false;
    for(unsigned int iMatrix=0; iMatrix<matrix.size(); iMatrix++){
      if(getSet(iSet) -> getMatrix() == matrix[iMatrix]){
	found = true;
	break;
      }
    }
    if(!found) matrix.push_back(getSet(iSet) -> getMatrix());
  }
  for(unsigned int iMatrix=0; iMatrix<matrix.size(); iMatrix++){
    cout << __PRETTY_FUNCTION__ << ": matrix[" << iMatrix << "] = " << matrix[iMatrix] << endl;
  }
  
  // collecting pixels
  cout << __PRETTY_FUNCTION__ << ": collecting pixels" << endl;
  vector<unsigned int> pixel;
  for(unsigned int iSet=0; iSet<getNSets(); iSet++){
    bool found = false;
    for(unsigned int iPixel=0; iPixel<pixel.size(); iPixel++){
      if(getSet(iSet) -> getPixel() == pixel[iPixel]){
	found = true;
	break;
      }
    }
    if(!found) pixel.push_back(getSet(iSet) -> getPixel());
  }
  for(unsigned int iPixel=0; iPixel<pixel.size(); iPixel++){
    cout << __PRETTY_FUNCTION__ << ": pixel[" << iPixel << "] = " << pixel[iPixel] << endl;
  }

  // building calibration curves
  cout << __PRETTY_FUNCTION__ << ": building calibration curves" << endl;
  for(unsigned int iWafer=0; iWafer<wafer.size(); iWafer++){
    for(unsigned int iMatrix=0; iMatrix<matrix.size(); iMatrix++){
      for(unsigned int iPixel=0; iPixel<pixel.size(); iPixel++){
	calibrationClass *calibration = new calibrationClass(wafer[iWafer], matrix[iMatrix], pixel[iPixel]);
	for(unsigned int iSet=0; iSet<getNSets(); iSet++){
	  if(getSet(iSet) -> getWafer() == wafer[iWafer]
	     &&
	     getSet(iSet) -> getMatrix() == matrix[iMatrix]
	     &&
	     getSet(iSet) -> getPixel() == pixel[iPixel]){

	    if(getSet(iSet) -> getNPeaks(0) == 1 && getSet(iSet) -> getNPeaks(1) == 0 && getSet(iSet) -> getNPeaks(2) == 0 && getSet(iSet) -> getNPeaks(3) == 0 && getSet(iSet) -> getNPeaks(4) == 0){
	      const double KAlpha = _fluorescenceData -> getTargetKa(getSet(iSet) -> getTarget());
	      if(KAlpha == 0){
		cout << __PRETTY_FUNCTION__ << ": ERROR!!! - invalid energy at wafer " << wafer[iWafer] << ", matrix " << matrix[iMatrix] << ", pixel " << pixel[iPixel] << ", set " << iSet << endl;
		return 1;
	      }
	      cout << "ENERGY = " << KAlpha << endl;
	      cout << "SIGNAL = " << getSet(iSet) -> getFitFunction() -> GetParameter(3) << endl;
	      cout << "ERROR  = " << getSet(iSet) -> getFitFunction() -> GetParameter(4) << endl;

	      calibration -> addPoint(KAlpha,
				      getSet(iSet) -> getFitFunction() -> GetParameter(3),
				      getSet(iSet) -> getFitFunction() -> GetParameter(4));	      
	    }
	    
	    else if(getSet(iSet) -> getNPeaks(0) == 1 && getSet(iSet) -> getNPeaks(1) == 0 && getSet(iSet) -> getNPeaks(2) == 1 && getSet(iSet) -> getNPeaks(3) == 0 && getSet(iSet) -> getNPeaks(4) == 0){
	      const double KAlpha = _fluorescenceData -> getTargetKa(getSet(iSet) -> getTarget());
	      if(KAlpha == 0){
		cout << __PRETTY_FUNCTION__ << ": ERROR!!! - invalid energy at wafer " << wafer[iWafer] << ", matrix " << matrix[iMatrix] << ", pixel " << pixel[iPixel] << ", set " << iSet << endl;
		return 1;
	      }	      
	      calibration -> addPoint(KAlpha,
				      getSet(iSet) -> getFitFunction() -> GetParameter(3),
				      getSet(iSet) -> getFitFunction() -> GetParameter(4));	      
	      const double KBeta = _fluorescenceData -> getTargetKb(getSet(iSet) -> getTarget());
	      if(KBeta == 0){
		cout << __PRETTY_FUNCTION__ << ": ERROR!!! - invalid energy at wafer " << wafer[iWafer] << ", matrix " << matrix[iMatrix] << ", pixel " << pixel[iPixel] << ", set " << iSet << endl;
		return 1;
	      }	      
	      calibration -> addPoint(KBeta,
				      getSet(iSet) -> getFitFunction() -> GetParameter(3) + getSet(iSet) -> getFitFunction() -> GetParameter(6),
				      getSet(iSet) -> getFitFunction() -> GetParameter(4));	      
	      
	    }
	    
	    else{
	      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - invalid nPeaks = ";
	      cout << getSet(iSet) -> getNPeaks(0) << ", ";
	      cout << getSet(iSet) -> getNPeaks(1) << ", ";
	      cout << getSet(iSet) -> getNPeaks(2) << ", ";
	      cout << getSet(iSet) -> getNPeaks(3) << ", ";
	      cout << getSet(iSet) -> getNPeaks(4) << endl;
	      return 1;
	    }
	    
	  }
	  else continue;
	}
	_calibration.push_back(calibration);
      }
    }
  }
  for(unsigned int iCalibration=0; iCalibration<_calibration.size(); iCalibration++){
    cout << __PRETTY_FUNCTION__ << ": calibration curve #" << iCalibration << ":" << endl;
    _calibration[iCalibration] -> print();
  }

  // fitting calibration curves
  cout << __PRETTY_FUNCTION__ << ": fitting calibration curves" << endl;
  for(unsigned int iCalibration=0; iCalibration<_calibration.size(); iCalibration++){
    _calibration[iCalibration] -> fit();
  }
  
  // saving calibration curves
  cout << __PRETTY_FUNCTION__ << ": saving calibration curves" << endl;
  for(unsigned int iCalibration=0; iCalibration<_calibration.size(); iCalibration++){
    _calibration[iCalibration] -> save(outputFolder);
  }
  
  return 0;
}

int fitClass::saveResults(const string outputFolder) const{

  string fileName = outputFolder + "/results.dat";
  ofstream file(fileName);
  if(!file){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileName << endl;
    return 1;
  }

  file << "wafer" << "\t"
       << "matrix" << "\t"
       << "pixel" << "\t"
       << "par0" << "\t"
       << "err0" << "\t"
       << "..." << endl;
  
  for(unsigned int iCalibration=0; iCalibration<_calibration.size(); iCalibration++){
    _calibration[iCalibration] -> writeToFile(file);
  }

  file.close();
  cout << __PRETTY_FUNCTION__ << ": calibration results written in " << fileName << endl;
  
  return 0;
}

#endif

