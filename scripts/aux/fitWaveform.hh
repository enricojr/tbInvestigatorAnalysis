#ifndef FITWAVEFORM_HH
#define FITWAVEFORM_HH

#include "../aux/configClass.hh"
#include <TF1.h>

#define NPRINTSAMPLES 10 // number of samples to be printed on screen when calling print()
#define NSAMPLESAVERAGE 20 // number of samples on which to average when computing start/end values

class fitWaveformClass{
public:
  fitWaveformClass(const unsigned int nSamples);
  ~fitWaveformClass();
  void reset();
  void print() const;
  void fit(const double *waveform, 
	   configClass *cfg,
	   const unsigned int iEvent,
	   const unsigned int iDRS,
	   const unsigned int iCH,
	   TH1F ***h1Noise,
	   const bool savePlot = false);
  void fillNoiseHistogram(const double *waveform, 
			  configClass *cfg,
			  const unsigned int iEvent,
			  const unsigned int iDRS,
			  const unsigned int iCH,
			  TH1F ***h1Noise);  
  double getPeakToPeak() const;
  double getValStart() const;
  double getDifference() const;
  double getNoiseStart() const;
  double getNoiseEnd() const;
  double getLinearSlope() const;
  double getLinearOffset() const;
  double getLinearRedChi2() const;
  double getPulseSlope() const;
  double getPulseOffset() const;
  double getPulseAmplitude() const;
  double getPulseT0() const;
  double getPulseRiseTime() const;
  double getPulseDecay() const;
  double getPulseCharge() const;
  double getPulseRedChi2() const;
  double getPulseNoise() const;
private:
  TH1F *_h1Waveform;
  unsigned int _nSamples;
  double _min;
  double _max;
  double _peakToPeak;
  double _valStart;
  double _valEnd;
  double _valDiff;
  double _noiseStart;
  double _noiseEnd;
  double _linearSlope;
  double _linearOffset;
  double _linearRedChi2;
  double _pulseSlope;
  double _pulseOffset;
  double _pulseAmplitude;
  double _pulseT0;
  double _pulseRiseTime;
  double _pulseDecay;
  double _pulseCharge;
  double _pulseRedChi2;
  double _pulseNoise;
  void init(const unsigned int nSamples);
  void setWaveform(const double *waveform);
  void computePeakToPeak();
  void computeDifference();
  void computeNoise();
  void fitLinear(configClass *cfg,
		 const unsigned int iDRS,
		 const unsigned int iCH);
  void fitPulse(configClass *cfg,
		const unsigned int iEvent,
		const unsigned int iDRS,
		const unsigned int iCH,
		const bool savePlot = false);		
};

fitWaveformClass::fitWaveformClass(const unsigned int nSamples){
  init(nSamples);
}

fitWaveformClass::~fitWaveformClass(){
  delete _h1Waveform;
}

void fitWaveformClass::init(const unsigned int nSamples){
  _nSamples = nSamples;
  _h1Waveform = new TH1F("h1Waveform", "waveform", _nSamples, 0, _nSamples);
  reset();
  return ;
}

void fitWaveformClass::reset(){
  _min = 0.;
  _max = 0.;
  _peakToPeak = 0.;
  _valStart = 0.;
  _valEnd = 0.;
  _valDiff = 0.;
  _noiseStart = 0.;
  _noiseEnd = 0.;
  _linearSlope = 0.;
  _linearOffset = 0.;
  _linearRedChi2 = 0.;
  _pulseSlope = 0.;
  _pulseOffset = 0.;
  _pulseAmplitude = 0.;
  _pulseT0 = 0.;
  _pulseRiseTime = 0.;
  _pulseDecay = 0.;
  _pulseCharge = 0.;
  _pulseRedChi2 = 0.;
  return ;
}

void fitWaveformClass::print() const{
  cout << __PRETTY_FUNCTION__ << ": nSamples = " << _nSamples << endl;
  cout << __PRETTY_FUNCTION__ << ": waveform = ";
  for(unsigned int iSample=0; iSample<NPRINTSAMPLES; iSample++){
    cout << _h1Waveform -> GetBinContent(iSample+1) << " ";
  }
  cout << __PRETTY_FUNCTION__ << ": min = " << _min << endl;
  cout << __PRETTY_FUNCTION__ << ": max = " << _max << endl;
  cout << __PRETTY_FUNCTION__ << ": peakToPeak = " << _peakToPeak << endl;
  cout << __PRETTY_FUNCTION__ << ": valStart = " << _valStart << endl;
  cout << __PRETTY_FUNCTION__ << ": valEnd = " << _valEnd << endl;
  cout << __PRETTY_FUNCTION__ << ": valDiff = " << _valDiff << endl;
  cout << __PRETTY_FUNCTION__ << ": noiseStart = " << _noiseStart << endl;
  cout << __PRETTY_FUNCTION__ << ": noiseEnd = " << _noiseEnd << endl;
  cout << __PRETTY_FUNCTION__ << ": linearSlope = " << _linearSlope << endl;
  cout << __PRETTY_FUNCTION__ << ": linearOffset = " << _linearOffset << endl;
  cout << __PRETTY_FUNCTION__ << ": linearRedChi2 = " << _linearRedChi2 << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseSlope = " << _pulseSlope << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseOffset = " << _pulseOffset << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseAmplitude = " << _pulseAmplitude << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseT0 = " << _pulseT0 << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseRiseTime = " << _pulseRiseTime << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseDecay = " << _pulseDecay << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseCharge = " << _pulseCharge << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseChi2 = " << _pulseRedChi2 << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseNoise = " << _pulseNoise << endl;
  cout << endl;
  return ;
}

double fitWaveformClass::getPeakToPeak() const{
  return _peakToPeak;
}

double fitWaveformClass::getValStart() const{
  return _valStart;
}

double fitWaveformClass::getDifference() const{
  return _valDiff;
}

double fitWaveformClass::getNoiseStart() const{
  return _noiseStart;
}

double fitWaveformClass::getNoiseEnd() const{
  return _noiseEnd;
}

double fitWaveformClass::getLinearSlope() const{
  return _linearSlope;
}

double fitWaveformClass::getLinearOffset() const{
  return _linearOffset;
}

double fitWaveformClass::getLinearRedChi2() const{
  return _linearRedChi2;
}

double fitWaveformClass::getPulseSlope() const{
  return _pulseSlope;
}

double fitWaveformClass::getPulseOffset() const{
  return _pulseOffset;
}

double fitWaveformClass::getPulseAmplitude() const{
  return _pulseAmplitude;
}

double fitWaveformClass::getPulseT0() const{
  return _pulseT0;
}

double fitWaveformClass::getPulseRiseTime() const{
  return _pulseRiseTime;
}

double fitWaveformClass::getPulseDecay() const{
  return _pulseDecay;
}

double fitWaveformClass::getPulseCharge() const{
  return _pulseCharge;
}

double fitWaveformClass::getPulseRedChi2() const{
  return _pulseRedChi2;
}

double fitWaveformClass::getPulseNoise() const{
  return _pulseNoise;
}

void fitWaveformClass::setWaveform(const double *waveform){
  for(unsigned int iSample=0; iSample<_nSamples; iSample++){
    _h1Waveform -> SetBinContent(iSample+1, waveform[iSample]);
  }
  return ;
}

void fitWaveformClass::computePeakToPeak(){
  _min = _h1Waveform -> GetBinContent(_h1Waveform -> GetMinimumBin());
  _max = _h1Waveform -> GetBinContent(_h1Waveform -> GetMaximumBin());
  _peakToPeak = _max - _min;
  return ;
}

void fitWaveformClass::computeDifference(){
  _valStart = 0.;
  _valEnd = 0.;
  for(unsigned int iSample=0; iSample<NSAMPLESAVERAGE; iSample++){
    _valStart += _h1Waveform -> GetBinContent(iSample+1);
    _valEnd += _h1Waveform -> GetBinContent(_nSamples-iSample);
  }
  _valStart /= NSAMPLESAVERAGE;
  _valEnd /= NSAMPLESAVERAGE;
  _valDiff = _valEnd - _valStart;
  return ;
}

void fitWaveformClass::computeNoise(){
  _noiseStart = 0.;
  _noiseEnd = 0.;
  for(unsigned int iSample=0; iSample<NSAMPLESAVERAGE; iSample++){
    const double diffStart = _h1Waveform -> GetBinContent(iSample+1) - _valStart;
    const double diffEnd = _h1Waveform -> GetBinContent(_nSamples-iSample) - _valEnd;
    _noiseStart += (diffStart * diffStart);
    _noiseEnd += (diffEnd * diffEnd);
  }
  _noiseStart = sqrt(_noiseStart / NSAMPLESAVERAGE);
  _noiseEnd = sqrt(_noiseEnd / NSAMPLESAVERAGE);
  return ;
}

void fitWaveformClass::fitLinear(configClass *cfg,
				 const unsigned int iDRS,
				 const unsigned int iCH){
  // function
  TF1 *fLine = new TF1("fLine", "[0]+[1]*x", cfg -> _linear[iDRS][iCH].min, cfg -> _linear[iDRS][iCH].max);
  fLine -> SetParNames("offset", "slope");
  // set parameter: offset
  fLine -> SetParameter(0, _valStart);
  fLine -> SetParLimits(0, cfg -> _linearOffset.range.min, cfg -> _linearOffset.range.max);
  // set parameter: slope
  fLine -> SetParameter(1, cfg -> _linearSlope.val);
  fLine -> SetParLimits(1, cfg -> _linearSlope.range.min, cfg -> _linearSlope.range.max);
  // fit
  _h1Waveform -> Fit("fLine", "R && Q");
  // result
  _linearSlope = fLine -> GetParameter(1);
  _linearOffset = fLine -> GetParameter(0);
  _linearRedChi2 = fLine -> GetChisquare() / fLine -> GetNDF();
  // cleaning memory
  delete fLine;
  return ;
}

void fitWaveformClass::fitPulse(configClass *cfg,
				const unsigned int iEvent,
				const unsigned int iDRS,
				const unsigned int iCH,
				const bool savePlot){				
  // function
  TF1 *fPulse = new TF1("fPulse", "[0]+[1]*x + 0.5*[2]*(1+TMath::Erf(100*(x-[3])))*(1-exp(-(x-[3])/[4]))*(1.+[5]*(x-[3]))", cfg -> _pulse[iDRS][iCH].min, cfg -> _pulse[iDRS][iCH].max);
  fPulse -> SetParNames("offset", "slope", "amplitude", "T0", "riseTime", "decay");
  // set parameter: offset
  fPulse -> SetParameter(0, _valStart);
  fPulse -> SetParLimits(0, cfg -> _pulseOffset.range.min, cfg -> _pulseOffset.range.max);
  // set parameter: slope
  fPulse -> SetParameter(1, cfg -> _pulseSlope.val);
  fPulse -> SetParLimits(1, cfg -> _pulseSlope.range.min, cfg -> _pulseSlope.range.max);
  // set parameter: amplitude
  fPulse -> SetParameter(2, _peakToPeak);
  fPulse -> SetParLimits(2, cfg -> _pulseAmplitude.range.min, cfg -> _pulseAmplitude.range.max);
  // set parameter: T0
  fPulse -> SetParameter(3, cfg -> _pulseT0[iDRS][iCH].val);
  fPulse -> SetParLimits(3, cfg -> _pulseT0[iDRS][iCH].range.min, cfg -> _pulseT0[iDRS][iCH].range.max);  
  // set parameter: riseTime
  fPulse -> SetParameter(4, cfg -> _pulseRiseTime.val);
  fPulse -> SetParLimits(4, cfg -> _pulseRiseTime.range.min, cfg -> _pulseRiseTime.range.max);  
  // set parameter: decay
  fPulse -> SetParameter(5, cfg -> _pulseDecay.val);
  fPulse -> SetParLimits(5, cfg -> _pulseDecay.range.min, cfg -> _pulseDecay.range.max);  
  // fit  
  _h1Waveform -> Fit("fPulse", "R && Q");
  // result
  _pulseSlope = fPulse -> GetParameter(1);
  _pulseOffset = fPulse -> GetParameter(0);
  _pulseAmplitude = fPulse -> GetParameter(2);
  _pulseT0 = fPulse -> GetParameter(3);
  _pulseRiseTime = fPulse -> GetParameter(4);
  _pulseDecay = fPulse -> GetParameter(5);
  _pulseRedChi2 = fPulse -> GetChisquare() / fPulse -> GetNDF();
  // computing noise as RMS from fit function in fit range. Relies on the fact that the i index is also the range index
  _pulseNoise = 0.;
  unsigned int count = 0;
  for(unsigned int i=cfg -> _pulse[iDRS][iCH].min; i<= cfg -> _pulse[iDRS][iCH].max; i++){
    const double diff = fPulse -> Eval(i) - _h1Waveform -> GetBinContent(i+1);
    _pulseNoise += (diff * diff);
    count ++;
  }
  _pulseNoise = sqrt(_pulseNoise / (double)count);
  if(savePlot){
    gStyle -> SetOptStat(0);
    gStyle -> SetOptFit(0);
    TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 1000, 1000);
    _h1Waveform -> Draw();
    char name[1000];
    sprintf(name, "tmp/%06d_%d_%d.png", iEvent, iDRS, iCH);
    cc -> SaveAs(name);
    sprintf(name, "tmp/%06d_%d_%d.root", iEvent, iDRS, iCH);
    cc -> SaveAs(name);
    delete cc;
  }
  delete fPulse;
  return ;
}



void fitWaveformClass::fit(const double *waveform,
			   configClass *cfg,
			   const unsigned int iEvent,
			   const unsigned int iDRS,
			   const unsigned int iCH,
			   TH1F ***h1Noise,
			   const bool savePlot){

  const bool debugLevel = 1;

  setWaveform(waveform);
  computePeakToPeak();
  computeDifference();
  computeNoise();
  fitLinear(cfg, iDRS, iCH);
  if(_linearRedChi2 > cfg -> _linearRedChi2Threshold){
    fitPulse(cfg, iEvent, iDRS, iCH, savePlot);
  }
  else{
    fillNoiseHistogram(waveform, cfg, iEvent, iDRS, iCH, h1Noise);
  }

  return ;
}

void fitWaveformClass::fillNoiseHistogram(const double *waveform, 
					  configClass *cfg,
					  const unsigned int iEvent,
					  const unsigned int iDRS,
					  const unsigned int iCH,
					  TH1F ***h1Noise){
  const bool debugLevel = 1;

  if(_linearRedChi2 <= cfg -> _linearRedChi2Threshold){
    for(unsigned int iSample=0; iSample<_nSamples; iSample++){
      const double valueMeas = _h1Waveform -> GetBinContent(iSample+1);
      const double valueFit = _linearOffset + iSample * _linearSlope;
      const double residual = valueFit-valueMeas;
      h1Noise[iDRS][iCH] -> Fill(residual);
    }    
  }
  
  return ;
}

#endif
