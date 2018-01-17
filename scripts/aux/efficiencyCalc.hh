#include <iostream>
using namespace std;

#include <TH1.h>
#include <TLine.h>
#include <TMath.h>
#include <TGraph.h>
#include <TF1.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraphAsymmErrors.h>

#define CL1SIGMA 0.6827
#define CL2SIGMA 0.9545
#define CL3SIGMA 0.9974

class pointClass{
public:
  pointClass(){
    _val = 0.;
    _err1SigmaInf = 0.;
    _err1SigmaSup = 0.;
    _err2SigmaInf = 0.;
    _err2SigmaSup = 0.;
    _err3SigmaInf = 0.;
    _err3SigmaSup = 0.;
    _h1 = NULL;
    _h1_1Sigma = NULL;
    _h1_2Sigma = NULL;
    _h1_3Sigma = NULL;
    _nBins = 0;
  };
  pointClass(const double val,
	     const double err1SigmaInf,
	     const double err1SigmaSup){
    _val = val;
    _err1SigmaInf = err1SigmaInf;
    _err1SigmaSup = err1SigmaSup;
    _err2SigmaInf = 2.*err1SigmaInf;
    _err2SigmaSup = 2.*err1SigmaSup;
    _err3SigmaInf = 3.*err1SigmaInf;
    _err3SigmaSup = 3.*err1SigmaSup;
    _h1 = NULL;
    _h1_1Sigma = NULL;
    _h1_2Sigma = NULL;
    _h1_3Sigma = NULL;
    _nBins = 0;
  };
  ~pointClass(){
    if(_h1 != NULL){
      delete _h1;
      delete _h1_1Sigma;
      delete _h1_2Sigma;
      delete _h1_3Sigma;		  
    }
  };
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": val = " << _val << endl;
    cout << __PRETTY_FUNCTION__ << ": err1SigmaInf = " << _err1SigmaInf << endl;
    cout << __PRETTY_FUNCTION__ << ": err2SigmaInf = " << _err2SigmaInf << endl;
    cout << __PRETTY_FUNCTION__ << ": err3SigmaInf = " << _err3SigmaInf << endl;
    cout << __PRETTY_FUNCTION__ << ": err1SigmaSup = " << _err1SigmaSup << endl;
    cout << __PRETTY_FUNCTION__ << ": err2SigmaSup = " << _err2SigmaSup << endl;
    cout << __PRETTY_FUNCTION__ << ": err3SigmaSup = " << _err3SigmaSup << endl;
    return ;
  }
  void setVal(const double val){_val = val;};
  void setErr1SigmaSup(const double err1SigmaSup){_err1SigmaSup = err1SigmaSup;};
  void setErr1SigmaInf(const double err1SigmaInf){_err1SigmaInf = err1SigmaInf;};
  void setErr2SigmaSup(const double err2SigmaSup){_err2SigmaSup = err2SigmaSup;};
  void setErr2SigmaInf(const double err2SigmaInf){_err2SigmaInf = err2SigmaInf;};
  void setErr3SigmaSup(const double err3SigmaSup){_err3SigmaSup = err3SigmaSup;};
  void setErr3SigmaInf(const double err3SigmaInf){_err3SigmaInf = err3SigmaInf;};
  void setNBins(const unsigned int nBins){_nBins = nBins;};
  double getVal() const{ return _val;};
  double getErr1SigmaSup() const{ return _err1SigmaSup;};
  double getErr1SigmaInf() const{ return _err1SigmaInf;};
  double getErr2SigmaSup() const{ return _err2SigmaSup;};
  double getErr2SigmaInf() const{ return _err2SigmaInf;};
  double getErr3SigmaSup() const{ return _err3SigmaSup;};
  double getErr3SigmaInf() const{ return _err3SigmaInf;};
  unsigned int getNBins() const{ return _nBins;};
  void draw(){
    if(_h1 == NULL){
      cout << __PRETTY_FUNCTION__ << ": WARNING - not drawing, histogram not allocated yet" << endl;
    }
    else{
      _h1 -> Draw();
      _h1_1Sigma -> SetFillColor(kBlue);
      _h1_2Sigma -> SetFillColor(kBlue-5);
      _h1_3Sigma -> SetFillColor(kBlue-10);
      _h1_3Sigma -> Draw("same");
      _h1_2Sigma -> Draw("same");
      _h1_1Sigma -> Draw("same");
    }
    return ;
  }
  void buildHistogram(const char *name,
		      const char *title,
		      const char *name1Sigma,
		      const char *title1Sigma,
		      const char *name2Sigma,
		      const char *title2Sigma,
		      const char *name3Sigma,
		      const char *title3Sigma,
		      const unsigned int nBins){
    _nBins = nBins;
    if(_h1 != NULL) delete _h1;
    const double inf = getVal() - 5 * getErr1SigmaInf();
    const double sup = getVal() + 5 * getErr1SigmaSup();
    _h1 = new TH1F(name, title, nBins, inf, sup);
    _h1_1Sigma = new TH1F(name1Sigma, title1Sigma, nBins, inf, sup);
    _h1_2Sigma = new TH1F(name2Sigma, title2Sigma, nBins, inf, sup);
    _h1_3Sigma = new TH1F(name3Sigma, title3Sigma, nBins, inf, sup);
    const double dx = (sup-inf)/nBins;
    const double sqrt2 = sqrt(2);
    for(unsigned int iBin=0; iBin<nBins; iBin++){
      const double xx = inf + iBin * dx;
      double zz = 0;
      if(xx < _val) zz = (xx-_val) / (sqrt2 * _err1SigmaInf);
      else zz = (xx-_val) / (sqrt2 * _err1SigmaSup);
      _h1 -> SetBinContent(iBin+1, exp(-zz*zz));
    }
    normalize();
    fillCL();
    return ;
  }
  void setBinContent(const unsigned int iBin,
		     const double val){
    _h1 -> SetBinContent(iBin, val);
    return ;
  }
  double getBinContent(const unsigned int iBin) const{
    return _h1 -> GetBinContent(iBin);
  }
  double getBinCenter(const unsigned int iBin) const{
    return _h1 -> GetBinCenter(iBin);
  }
  void computeEfficiency(pointClass *num,
			 pointClass *den,
			 const char *name,
			 const char *title,
			 const char *name1Sigma,
			 const char *title1Sigma,
			 const char *name2Sigma,
			 const char *title2Sigma,
			 const char *name3Sigma,
			 const char *title3Sigma,			 
			 const unsigned int nBins){

    double min = 1000000.;
    double max = -1000000.;
    vector<pair<double, double> > entry;
    for(unsigned int iBinNum=0; iBinNum<num -> getNBins(); iBinNum++){
      const double xxNum = num -> getBinCenter(iBinNum+1);
      for(unsigned int iBinDen=0; iBinDen<den -> getNBins(); iBinDen++){
	const double xxDen = den -> getBinCenter(iBinDen+1);
	if(xxNum > xxDen) continue;
	const double eff = xxNum / xxDen;
	const double weight = num -> getBinContent(iBinNum+1) * den -> getBinContent(iBinDen+1);
	entry.push_back(pair<double, double>(eff, weight));
	if(eff < min) min = eff;
	if(eff > max) max = eff;
      }
    }
    
    _nBins = nBins;
    if(_h1 != NULL) delete _h1;
    _h1 = new TH1F(name, title, nBins, min, max);
    _h1_1Sigma = new TH1F(name1Sigma, title1Sigma, nBins, min, max);
    _h1_2Sigma = new TH1F(name2Sigma, title2Sigma, nBins, min, max);
    _h1_3Sigma = new TH1F(name3Sigma, title3Sigma, nBins, min, max);
    for(unsigned int iEntry=0; iEntry<entry.size(); iEntry++){
      _h1 -> Fill(entry[iEntry].first, entry[iEntry].second);
    }
    entry.clear();

    _h1 -> Rebin(10);
    _h1_1Sigma -> Rebin(10);
    _h1_2Sigma -> Rebin(10);
    _h1_3Sigma -> Rebin(10);
    _nBins = _h1 -> GetXaxis() -> GetNbins();
    normalize();
    computeCL();
    return ;
  }
private:
  double _val;
  double _err1SigmaInf;
  double _err1SigmaSup;
  double _err2SigmaInf;
  double _err2SigmaSup;
  double _err3SigmaInf;
  double _err3SigmaSup;
  unsigned int _nBins;
  TH1F *_h1;
  TH1F *_h1_1Sigma;
  TH1F *_h1_2Sigma;
  TH1F *_h1_3Sigma;
  void normalize(){
    const double integral = _h1 -> Integral();
    _h1 -> Scale(1./integral);
    return ;
  }
  void fillCL(){
    for(unsigned int iBin=0; iBin<_nBins; iBin++){
      const double xx = _h1 -> GetBinCenter(iBin+1);
      const double val = _h1 -> GetBinContent(iBin+1);
      if(xx > _val - _err1SigmaInf && xx < _val + _err1SigmaSup) _h1_1Sigma -> SetBinContent(iBin+1, val);
      if(xx > _val - _err2SigmaInf && xx < _val + _err2SigmaSup) _h1_2Sigma -> SetBinContent(iBin+1, val);
      if(xx > _val - _err3SigmaInf && xx < _val + _err3SigmaSup) _h1_3Sigma -> SetBinContent(iBin+1, val);
    }
    return ;
  }
  void computeCL(){
    const unsigned int range = _nBins/20;
    const unsigned int maximumBin = _h1 -> GetMaximumBin();
    if(maximumBin != 0 && maximumBin != _nBins){
      const double min = _h1 -> GetBinCenter(maximumBin-range);
      const double max = _h1 -> GetBinCenter(maximumBin+range);
      TF1 *f1 = new TF1("f1", "[0]*(x-[1])*(x-[1])+[2]", min, max);
      f1 -> SetParameter(1, _h1 -> GetBinCenter(maximumBin));
      f1 -> SetParLimits(0, -1000000000., 0.);
      f1 -> SetParLimits(2, 0, +1000000000.);
      TCanvas *cc = new TCanvas();
      _h1 -> Fit("f1", "R&&Q");
      delete cc;
      const double xxPeak = f1 -> GetParameter(1);
      delete f1;
      const unsigned int binPeak = _h1 -> FindBin(xxPeak);
      TH1F *h1Left = (TH1F *) _h1 -> Clone();
      for(unsigned int iBin=binPeak; iBin<_nBins; iBin++){
	h1Left -> SetBinContent(iBin+1, 0);
      }
      h1Left -> Scale(1. / h1Left -> Integral());
      TH1F *h1Right = (TH1F *) _h1 -> Clone();
      for(unsigned int iBin=0; iBin<binPeak; iBin++){
	h1Right -> SetBinContent(iBin+1, 0);
      }
      h1Right -> Scale(1. / h1Right -> Integral());
      TGraph *grLeft = new TGraph();
      TGraph *grRight = new TGraph();
      TH1F *h1CumulativeLeft = (TH1F *) h1Left -> GetCumulative();
      TH1F *h1CumulativeRight = (TH1F *) h1Right -> GetCumulative();
      for(unsigned int iBin=0; iBin<_nBins; iBin++){
	const double xxLeft = h1CumulativeLeft -> GetBinCenter(iBin+1);
	const double yyLeft = h1CumulativeLeft -> GetBinContent(iBin+1);
	grLeft -> SetPoint(iBin, yyLeft, xxLeft);
	const double xxRight = h1CumulativeRight -> GetBinCenter(iBin+1);
	const double yyRight = h1CumulativeRight -> GetBinContent(iBin+1);
	grRight -> SetPoint(iBin, yyRight, xxRight);
      }
      _err1SigmaInf = fabs(grLeft -> Eval(1.-CL1SIGMA)-xxPeak);
      _err2SigmaInf = fabs(grLeft -> Eval(1.-CL2SIGMA)-xxPeak);
      _err3SigmaInf = fabs(grLeft -> Eval(1.-CL3SIGMA)-xxPeak);
      _err1SigmaSup = fabs(grRight -> Eval(CL1SIGMA)-xxPeak);
      _err2SigmaSup = fabs(grRight -> Eval(CL2SIGMA)-xxPeak);
      _err3SigmaSup = fabs(grRight -> Eval(CL3SIGMA)-xxPeak);
      _val = xxPeak;
      print();
      fillCL();
      delete h1Left;
      delete h1Right;
      delete h1CumulativeLeft;
      delete h1CumulativeRight;
      delete grLeft;
      delete grRight;
    }
    else{
      _h1 -> Scale(1. / _h1 -> Integral());
      TH1F *h1Cumulative = (TH1F *) _h1 -> GetCumulative();
      TGraph *gr = new TGraph();
      for(unsigned int iBin=0; iBin<_nBins; iBin++){
	const double xx = h1Cumulative -> GetBinCenter(iBin+1);
	const double yy = h1Cumulative -> GetBinContent(iBin+1);
	gr -> SetPoint(iBin, yy, xx);
      }
      const double xxPeak = 1.;
      _err1SigmaInf = fabs(gr -> Eval(1.-CL1SIGMA)-xxPeak);
      _err2SigmaInf = fabs(gr -> Eval(1.-CL2SIGMA)-xxPeak);
      _err3SigmaInf = fabs(gr -> Eval(1.-CL3SIGMA)-xxPeak);
      _err1SigmaSup = 0.;
      _err2SigmaSup = 0.;
      _err3SigmaSup = 0.;
      _val = xxPeak;
      print();
      fillCL();
      delete h1Cumulative;
      delete gr;
    }
    return ;
  }
};

pointClass *efficiencyCalc(pointClass *num,
			   pointClass *den,
			   const unsigned int nBins){
  num -> buildHistogram("num", "numerator",
			"num_1Sigma", "numerator 1#sigma CL",
			"num_2Sigma", "numerator 2#sigma CL",
			"num_3Sigma", "numerator 3#sigma CL",
			nBins);
  den -> buildHistogram("den", "denominator",
			"den_1Sigma", "denominator 1#sigma CL",
			"den_2Sigma", "denominator 2#sigma CL",
			"den_3Sigma", "denominator 3#sigma CL",
			nBins);
  
  pointClass *eff = new pointClass();
  eff -> computeEfficiency(num, den,
			   "eff", "efficiency",
			   "eff_1Sigma", "efficiency 1#sigma CL",
			   "eff_2Sigma", "efficiency 2#sigma CL",
			   "eff_3Sigma", "efficiency 3#sigma CL",
			   nBins);
  
  return eff;
}

void showResult(pointClass *num,
		pointClass *den,
		pointClass *eff){
  
  TH1F *h1NumDen = new TH1F("h1NumDen", "numerator and denominator", 1, -0.5, 0.5);
  const double max = den -> getVal() + den -> getErr1SigmaSup();
  h1NumDen -> SetBinContent(1, max);
  
  TGraphAsymmErrors *grNum = new TGraphAsymmErrors();
  grNum -> SetPoint(0, 0, num -> getVal());
  grNum -> SetPointError(0, 0, 0, num -> getErr1SigmaInf(), num -> getErr1SigmaSup());

  TGraphAsymmErrors *grDen = new TGraphAsymmErrors();
  grDen -> SetPoint(0, 0, den -> getVal());
  grDen -> SetPointError(0, 0, 0, den -> getErr1SigmaInf(), den -> getErr1SigmaSup());

  TH1F *h1Eff = new TH1F("h1Eff", "efficiency", 1, -0.5, 0.5);
  h1Eff -> SetBinContent(1, 1);

  TGraphAsymmErrors *grEff1Sigma = new TGraphAsymmErrors();
  grEff1Sigma -> SetPoint(0, 0, eff -> getVal());
  grEff1Sigma -> SetPointError(0, 0, 0, eff -> getErr1SigmaInf(), eff -> getErr1SigmaSup());
  TGraphAsymmErrors *grEff2Sigma = new TGraphAsymmErrors();
  grEff2Sigma -> SetPoint(0, 0, eff -> getVal());
  grEff2Sigma -> SetPointError(0, 0, 0, eff -> getErr2SigmaInf(), eff -> getErr2SigmaSup());
  TGraphAsymmErrors *grEff3Sigma = new TGraphAsymmErrors();
  grEff3Sigma -> SetPoint(0, 0, eff -> getVal());
  grEff3Sigma -> SetPointError(0, 0, 0, eff -> getErr3SigmaInf(), eff -> getErr3SigmaSup());

  gStyle -> SetOptStat(0);
  TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 1800, 1200);
  cc -> Divide(3,2);

  cc -> cd(1);
  num -> draw();
  
  cc -> cd(2);
  den -> draw();
  
  cc -> cd(3);
  eff -> draw();

  cc -> cd(4);
  h1NumDen -> SetLineColor(0);
  h1NumDen -> GetYaxis() -> SetRangeUser(0, max*1.1);
  h1NumDen -> Draw();
  grDen -> SetMarkerStyle(20);
  grDen -> SetMarkerColor(1);
  grDen -> SetLineColor(1);
  grDen -> SetLineWidth(6);
  grDen -> Draw("p");
  grNum -> SetMarkerStyle(20);
  grNum -> SetMarkerColor(2);
  grNum -> SetLineColor(2);
  grNum -> SetLineWidth(2);
  grNum -> Draw("p");

  cc -> cd(5);
  h1Eff -> SetLineColor(0);
  h1Eff -> GetYaxis() -> SetRangeUser(0.8, 1.);
  h1Eff -> Draw();
  grEff3Sigma -> SetMarkerStyle(20);
  grEff3Sigma -> SetMarkerColor(1);
  grEff3Sigma -> SetLineColor(kBlue-10);
  grEff3Sigma -> SetLineWidth(8);
  grEff3Sigma -> Draw("p");
  grEff2Sigma -> SetMarkerStyle(20);
  grEff2Sigma -> SetMarkerColor(1);
  grEff2Sigma -> SetLineColor(kBlue-5);
  grEff2Sigma -> SetLineWidth(4);
  grEff2Sigma -> Draw("p");
  grEff1Sigma -> SetMarkerStyle(20);
  grEff1Sigma -> SetMarkerColor(1);
  grEff1Sigma -> SetLineColor(kBlue);
  grEff1Sigma -> SetLineWidth(2);
  grEff1Sigma -> Draw("p");
    
  return ;
}
