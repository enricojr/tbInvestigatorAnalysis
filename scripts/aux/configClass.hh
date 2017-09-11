#ifndef CONFIGCLASS_HH
#define CONFIGCLASS_HH

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

class rangeClass{
public:
  rangeClass(){};
  ~rangeClass(){};
  double min;
  double max;
};

class parameterClass{
public:
  parameterClass(){};
  ~parameterClass(){};
  double val;
  rangeClass range;
};

class configClass{
public:
  configClass();
  ~configClass();
  void print() const;
  int load(const string cfgFileName);
  string _cfgFileName;
  unsigned int _nDRS;
  vector<unsigned int> _nCH;
  pair<unsigned int, unsigned int> _resetCH; // <DRS,CH>
  rangeClass **_linear;
  parameterClass _linearOffset;
  parameterClass _linearSlope;
  rangeClass **_pulse;
  parameterClass _pulseOffset;
  parameterClass _pulseSlope;
  parameterClass _pulseAmplitude;
  parameterClass **_pulseT0;
  parameterClass _pulseRiseTime;
  parameterClass _pulseDecay;
  double _linearRedChi2Threshold;
  double _pulseRedChi2Threshold;
  rangeClass **_pulseT0Range;
  rangeClass _pulseRiseTimeRange;
  double _dtSpill;
  double _dtInterSpill;
  unsigned int _debugLevel;
  // derived quantities
  unsigned int _nChannels;
private:
  void init();
  void reset();
  void computeDerivedQuantities();
};

configClass::configClass(){
  init();
}

configClass::~configClass(){
  for(unsigned int iDRS=0; iDRS<_nDRS; iDRS++){
    delete[] _pulseT0Range[iDRS];
  }
  delete[] _pulseT0Range;   
  _nCH.clear();  
}

void configClass::init(){
  reset();
  return ;
}

void configClass::reset(){
  _cfgFileName = "";
  _nDRS = 0;
  _nCH.clear();
  _resetCH = pair<unsigned int, unsigned int>(0, 0);
  _linearRedChi2Threshold = 0.;
  _pulseRedChi2Threshold = 0.;
  _debugLevel = 0;
  return ;
}

void configClass::print() const{
  cout << __PRETTY_FUNCTION__ << ": cfgFileName = " << _cfgFileName << endl;
  cout << __PRETTY_FUNCTION__ << ": nDRS = " << _nDRS << endl;
  for(unsigned int iCH=0; iCH<_nCH.size(); iCH++){
    cout << __PRETTY_FUNCTION__ << ": nCH[" << iCH << "] = " << _nCH[iCH] << endl;
  }
  cout << __PRETTY_FUNCTION__ << ": resetCH = " << _resetCH.first << ", " << _resetCH.second << endl;
  for(unsigned int iDRS=0; iDRS<_nDRS; iDRS++){
    for(unsigned int iCH=0; iCH<_nCH[iDRS]; iCH++){
      cout << __PRETTY_FUNCTION__ << ": linear[" << iDRS << "][" << iCH << "] = " << _linear[iDRS][iCH].min << ", " << _linear[iDRS][iCH].max << endl;
    }
  }
  cout << __PRETTY_FUNCTION__ << ": linearOffset = " << _linearOffset.val << ", " << _linearOffset.range.min << ", " << _linearOffset.range.max << endl;
  cout << __PRETTY_FUNCTION__ << ": linearSlope = " << _linearSlope.val << ", " << _linearSlope.range.min << ", " << _linearSlope.range.max << endl;
  for(unsigned int iDRS=0; iDRS<_nDRS; iDRS++){
    for(unsigned int iCH=0; iCH<_nCH[iDRS]; iCH++){
      cout << __PRETTY_FUNCTION__ << ": pulse[" << iDRS << "][" << iCH << "] = " << _pulse[iDRS][iCH].min << ", " << _pulse[iDRS][iCH].max << endl;
    }
  }
  cout << __PRETTY_FUNCTION__ << ": pulseOffset = " << _pulseOffset.val << ", " << _pulseOffset.range.min << ", " << _pulseOffset.range.max << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseSlope = " << _pulseSlope.val << ", " << _pulseSlope.range.min << ", " << _pulseSlope.range.max << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseAmplitude = " << _pulseAmplitude.val << ", " << _pulseAmplitude.range.min << ", " << _pulseAmplitude.range.max << endl;
  for(unsigned int iDRS=0; iDRS<_nDRS; iDRS++){
    for(unsigned int iCH=0; iCH<_nCH[iDRS]; iCH++){
      cout << __PRETTY_FUNCTION__ << ": pulseT0[" << iDRS << "][" << iCH << "] = " << _pulseT0[iDRS][iCH].val << ", " << _pulseT0[iDRS][iCH].range.min << ", " << _pulseT0[iDRS][iCH].range.max << endl;
    }
  }
  cout << __PRETTY_FUNCTION__ << ": pulseRiseTime = " << _pulseRiseTime.val << ", " << _pulseRiseTime.range.min << ", " << _pulseRiseTime.range.max << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseDecay = " << _pulseDecay.val << ", " << _pulseDecay.range.min << ", " << _pulseDecay.range.max << endl;
  cout << __PRETTY_FUNCTION__ << ": linearRedChi2Threshold = " << _linearRedChi2Threshold << endl;
  cout << __PRETTY_FUNCTION__ << ": pulseRedChi2Threshold = " << _pulseRedChi2Threshold << endl;
  for(unsigned int iDRS=0; iDRS<_nDRS; iDRS++){
    for(unsigned int iCH=0; iCH<_nCH[iDRS]; iCH++){
      cout << __PRETTY_FUNCTION__ << ": pulseT0Range[" << iDRS << "][" << iCH << "] = " << _pulseT0Range[iDRS][iCH].min << ", " << _pulseT0Range[iDRS][iCH].max << endl;
    }
  }
  cout << __PRETTY_FUNCTION__ << ": pulseRiseTimeRange = " << _pulseRiseTimeRange.min << ", " << _pulseRiseTimeRange.max << endl;
  cout << __PRETTY_FUNCTION__ << ": dtSpill = " << _dtSpill << endl;
  cout << __PRETTY_FUNCTION__ << ": dtInterSpill = " << _dtInterSpill << endl;
  cout << __PRETTY_FUNCTION__ << ": debugLevel = " << _debugLevel << endl;
  cout << __PRETTY_FUNCTION__ << ": (derived) nChannels = " << _nChannels << endl;
  return ;
}

void configClass::computeDerivedQuantities(){

  // nChannels
  _nChannels = 0;
  for(unsigned int iDRS=0; iDRS<_nDRS; iDRS++){
    _nChannels += _nCH[iDRS];
  }

  return ;
}

int configClass::load(const string cfgFileName){
  // opening file
  ifstream file(cfgFileName.c_str());
  if(!file){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << cfgFileName << endl;
    return 1;
  }
  // cfgFileName
  _cfgFileName = cfgFileName;
  // nDRS
  string line = "";
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%d", &_nDRS);
  // nCH
  for(unsigned int iDRS=0; iDRS<_nDRS; iDRS++){
    unsigned int nCH = 0;
    getline(file, line);
    getline(file, line);
    sscanf(line.c_str(), "%d", &nCH);
    _nCH.push_back(nCH);
  }
  // resetCH
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%d", &_resetCH.first);
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%d", &_resetCH.second);
  // linear
  getline(file, line);
  _linear = new rangeClass*[_nDRS];
  for(unsigned int iDRS=0; iDRS<_nDRS; iDRS++){
    _linear[iDRS] = new rangeClass[_nCH[iDRS]];
    for(unsigned int iCH=0; iCH<_nCH[iDRS]; iCH++){
      getline(file, line);
      sscanf(line.c_str(), "%lf %lf", &_linear[iDRS][iCH].min, &_linear[iDRS][iCH].max);
    }
  }
  // linearOffset
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf %lf %lf", &_linearOffset.val, &_linearOffset.range.min, &_linearOffset.range.max);
  // linearSlope
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf %lf %lf", &_linearSlope.val, &_linearSlope.range.min, &_linearSlope.range.max);
  // pulse
  getline(file, line);
  _pulse = new rangeClass*[_nDRS];
  for(unsigned int iDRS=0; iDRS<_nDRS; iDRS++){
    _pulse[iDRS] = new rangeClass[_nCH[iDRS]];
    for(unsigned int iCH=0; iCH<_nCH[iDRS]; iCH++){
      getline(file, line);
      sscanf(line.c_str(), "%lf %lf", &_pulse[iDRS][iCH].min, &_pulse[iDRS][iCH].max);
    }
  }
  // pulseOffset
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf %lf %lf", &_pulseOffset.val, &_pulseOffset.range.min, &_pulseOffset.range.max);
  // pulseSlope
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf %lf %lf", &_pulseSlope.val, &_pulseSlope.range.min, &_pulseSlope.range.max);
  // pulseAmplitude
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf %lf %lf", &_pulseAmplitude.val, &_pulseAmplitude.range.min, &_pulseAmplitude.range.max);
  // pulseT0
  getline(file, line);
  _pulseT0 = new parameterClass*[_nDRS];
  for(unsigned int iDRS=0; iDRS<_nDRS; iDRS++){
    _pulseT0[iDRS] = new parameterClass[_nCH[iDRS]];
    for(unsigned int iCH=0; iCH<_nCH[iDRS]; iCH++){
      getline(file, line);
      sscanf(line.c_str(), "%lf %lf %lf", &_pulseT0[iDRS][iCH].val, &_pulseT0[iDRS][iCH].range.min, &_pulseT0[iDRS][iCH].range.max);
    }
  }
  // pulseRiseTime
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf %lf %lf", &_pulseRiseTime.val, &_pulseRiseTime.range.min, &_pulseRiseTime.range.max);
  // pulseDecay
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf %lf %lf", &_pulseDecay.val, &_pulseDecay.range.min, &_pulseDecay.range.max);
  // linearRedChi2Threshold
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf", &_linearRedChi2Threshold);
  // pulseRedChi2Threshold
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf", &_pulseRedChi2Threshold);
  // pulseT0Range
  getline(file, line);
  _pulseT0Range = new rangeClass*[_nDRS];
  for(unsigned int iDRS=0; iDRS<_nDRS; iDRS++){
    _pulseT0Range[iDRS] = new rangeClass[_nCH[iDRS]];
    for(unsigned int iCH=0; iCH<_nCH[iDRS]; iCH++){
      getline(file, line);
      sscanf(line.c_str(), "%lf %lf", &_pulseT0Range[iDRS][iCH].min, &_pulseT0Range[iDRS][iCH].max);      
    }
  }
  // pulseRiseTimeRange
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf %lf", &_pulseRiseTimeRange.min, &_pulseRiseTimeRange.max);
  // dtSpill
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf", &_dtSpill);
  // dtInterSpill
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%lf", &_dtInterSpill);
  // debugLevel
  getline(file, line);
  getline(file, line);
  sscanf(line.c_str(), "%d", &_debugLevel);
  // closing file
  file.close();

  computeDerivedQuantities();

  return 0;
}

#endif
