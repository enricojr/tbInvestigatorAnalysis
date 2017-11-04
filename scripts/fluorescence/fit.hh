#ifndef FIT_HH
#define FIT_HH

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <TStyle.h>
#include <TColor.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TH1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TFile.h>
#include <TTree.h>

#define NDRSCHANNELS 5
#define SPECTRUMNBINS 65536
#define SPECTRUMMIN 0 
#define SPECTRUMMAX 4096

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
	   const string path){
    init();
    _wafer = wafer;
    _matrix = matrix;
    _target = target;
    _pixel = pixel;
    _path = path;
  };
  
  ~setClass(){
    delete _h1Spectrum;
  };

  void print() const{
    cout << __PRETTY_FUNCTION__ << ": wafer = " << _wafer << endl;
    cout << __PRETTY_FUNCTION__ << ": matrix = " << _matrix << endl;
    cout << __PRETTY_FUNCTION__ << ": target = " << _target << endl;
    cout << __PRETTY_FUNCTION__ << ": pixel = " << _pixel << endl;
    cout << __PRETTY_FUNCTION__ << ": path = " << _path << endl;
  };

  string getPath() const{
    return _path;
  };

  unsigned int getPixel(){
    return _pixel;
  };

  void addEntryToSpectrum(const double amplitude){
    _h1Spectrum -> Fill(amplitude);
    return ;
  };

  int save(){
    _h1Spectrum -> SaveAs("tmp.root");
  };
  
private:

  string _wafer;
  unsigned int _matrix;
  string _target;
  unsigned int _pixel;
  string _path;

  TH1F *_h1Spectrum;
  
  void init(){
    _wafer = "";
    _matrix = 0;
    _target = "";
    _pixel = 777;
    _path = "";
    _h1Spectrum = new TH1F("spectrum", "spectrum", SPECTRUMNBINS, SPECTRUMMIN, SPECTRUMMAX);
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
  int fitSpectra();
  fluorescenceDataClass *getFluorescenceData() const;
  void addTarget(targetClass *target);
private:
  fluorescenceDataClass *_fluorescenceData;
  unsigned int _nSets;
  vector<setClass *> _set;
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
    char path[1000];
    sscanf(line.c_str(), "%s\t%d\t%s\t%d\t%s", wafer, &matrix, target, &pixel, path);
    addSet(new setClass(wafer, matrix, target, pixel, path));
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

int fitClass::fitSpectra(){
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
      if(pulseAmplitude == 0) continue;
      getSet(iSet) -> addEntryToSpectrum(pulseAmplitude);
    }

    getSet(iSet) -> save();
    
    delete tree;
    file -> Close();
    delete file;
  }
  return 0;
}

#endif

