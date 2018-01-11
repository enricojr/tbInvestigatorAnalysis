#include <iostream>
using namespace std;

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TF1.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TPaveText.h>

#define XNBINS 2000
#define XMIN -10000
#define XMAX +10000

#define YNBINS 2000
#define YMIN -10000
#define YMAX +10000

#define AMPLITUDENBINS 1000
#define AMPLITUDEMIN 0
#define AMPLITUDEMAX 32000

int quickEfficiency(const unsigned int run,
		    const double pixelSize,
		    const bool lxplus = false){

  gStyle -> SetOptFit(1);
  
  ////////////
  // constants
  ////////////
  const char *treeName = "syncedData";

  ////////////////////////
  // allocating histograms
  ////////////////////////
  cout << __PRETTY_FUNCTION__ << ": allocating histograms" << endl;
  TH1F *h1X = new TH1F("h1X", "h1X", XNBINS, XMIN, XMAX);
  TH1F *h1Y = new TH1F("h1Y", "h1Y", YNBINS, YMIN, YMAX);
  TGraph *grXY = new TGraph();
  TGraph *grDUT = new TGraph();
  TH1F *h1Amplitude = new TH1F("h1Amplitude", "amplitude distribution", AMPLITUDENBINS, AMPLITUDEMIN, AMPLITUDEMAX);
  TH1F *h1Charge = new TH1F("h1Charge", "charge distribution", AMPLITUDENBINS, AMPLITUDEMIN, AMPLITUDEMAX);
  
  ///////////////
  // opening file
  ///////////////
  cout << __PRETTY_FUNCTION__ << ": opening file" << endl;
  char fileName[1000];
  if(lxplus) sprintf(fileName, "/afs/cern.ch/work/f/fdachs/public/TB2017/Analysis/Output/Synced/sync_%06d_000000.root", run);
  else sprintf(fileName, "../../../output/sync_%06d_000000.root", run);
  cout << __PRETTY_FUNCTION__ << ": fileName = " << fileName << endl;
  TFile *file = TFile::Open(fileName);
  if(file == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileName << endl;
    return 1;
  }

  ///////////////
  // getting tree
  ///////////////
  cout << __PRETTY_FUNCTION__ << ": getting tree" << endl;
  TTree *tree = (TTree *) file -> Get(treeName);
  if(tree == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << treeName << endl;
    return 1;
  }
  Double_t xTrack = 0.;
  tree -> SetBranchAddress("xTrack", &xTrack);
  Double_t yTrack = 0.;
  tree -> SetBranchAddress("yTrack", &yTrack);
  Float_t pulseAmplitude = 0.;
  tree -> SetBranchAddress("pulseAmplitude", &pulseAmplitude);

  //////////
  // looping
  //////////
  cout << __PRETTY_FUNCTION__ << ": looping" << endl;
  const unsigned int nEntries = tree -> GetEntries();
  for(unsigned int iEntry=0; iEntry<nEntries; iEntry+=5){    
    tree -> GetEntry(iEntry);
    
    h1X -> Fill(xTrack);
    h1Y -> Fill(yTrack);    
    grXY -> SetPoint(grXY -> GetN(), xTrack, yTrack);
    
    bool found = false;
    double charge = 0.;
    for(unsigned int i=0; i<5; i++){
      tree -> GetEntry(iEntry+i);
      if(pulseAmplitude>0){
	found = true;
	h1Amplitude -> Fill(pulseAmplitude);
	charge += pulseAmplitude;
      }
      else continue;
    }
    
    if(found){
      grDUT -> SetPoint(grDUT -> GetN(), xTrack, yTrack);
      h1Charge -> Fill(charge);
    }
    else continue;
    
  }

  ///////////////
  // closing file
  ///////////////
  cout << __PRETTY_FUNCTION__ << ": closing file" << endl;
  file -> Close();
  delete file;

  //////////////////////////
  // getting rough RoI edges
  //////////////////////////
  cout << __PRETTY_FUNCTION__ << ": getting rough RoI edges" << endl;

  const double maxX = h1X -> GetBinContent(h1X -> GetMaximumBin());
  cout << __PRETTY_FUNCTION__ << ": maxX = " << maxX << endl;
  double edgeInfX = 0.;
  for(unsigned int iBin=0; iBin<XNBINS; iBin++){
    if(h1X -> GetBinContent(iBin) > maxX/4.){
      edgeInfX = h1X -> GetBinLowEdge(iBin);
      break;
    }
  }
  cout << __PRETTY_FUNCTION__ << ": edgeInfX = " << edgeInfX << endl;
  double edgeSupX = 0.;
  for(unsigned int iBin=XNBINS-1; iBin>0; iBin--){
    if(h1X -> GetBinContent(iBin) > maxX/4.){
      edgeSupX = h1X -> GetBinLowEdge(iBin);
      break;
    }
  }
  cout << __PRETTY_FUNCTION__ << ": edgeSupX = " << edgeSupX << endl;
  
  const double maxY = h1Y -> GetBinContent(h1Y -> GetMaximumBin());
  cout << __PRETTY_FUNCTION__ << ": maxY = " << maxY << endl;
  double edgeInfY = 0.;
  for(unsigned int iBin=0; iBin<YNBINS; iBin++){
    if(h1Y -> GetBinContent(iBin) > maxY/4.){
      edgeInfY = h1Y -> GetBinLowEdge(iBin);
      break;
    }
  }
  cout << __PRETTY_FUNCTION__ << ": edgeInfY = " << edgeInfY << endl;
  double edgeSupY = 0.;
  for(unsigned int iBin=YNBINS-1; iBin>0; iBin--){
    if(h1Y -> GetBinContent(iBin) > maxY/4.){
      edgeSupY = h1Y -> GetBinLowEdge(iBin);
      break;
    }
  }
  cout << __PRETTY_FUNCTION__ << ": edgeSupY = " << edgeSupY << endl;
  
  /////////////////////
  // computing plateaus
  /////////////////////
  cout << __PRETTY_FUNCTION__ << ": computing plateaus" << endl;
  
  const double meanX = h1X -> GetMean();
  cout << __PRETTY_FUNCTION__ << ": meanX = " << meanX << endl;
  const double sigmaInfX = fabs(meanX-edgeInfX);
  cout << __PRETTY_FUNCTION__ << ": sigmaInfX = " << sigmaInfX << endl;
  const double sigmaSupX = fabs(meanX-edgeSupX);
  cout << __PRETTY_FUNCTION__ << ": sigmaSupX = " << sigmaSupX << endl;
  const double plateauRangeInfX = meanX-0.75*sigmaInfX;
  cout << __PRETTY_FUNCTION__ << ": plateauRangeInfX = " << plateauRangeInfX << endl;
  const double plateauRangeSupX = meanX+0.75*sigmaSupX;
  cout << __PRETTY_FUNCTION__ << ": plateauRangeSupX = " << plateauRangeSupX << endl;
  double plateauX = 0.;
  unsigned int countX = 0;
  for(unsigned int iBin=0; iBin<XNBINS; iBin++){
    const double xVal = h1X -> GetBinLowEdge(iBin);
    if(xVal < plateauRangeInfX) continue;
    else if(xVal > plateauRangeSupX) break;
    else{
      plateauX += h1X -> GetBinContent(iBin);
      countX++;
    }
  }
  plateauX /= countX;
  cout << __PRETTY_FUNCTION__ << ": plateauX = " << plateauX << endl;
  
  const double meanY = h1Y -> GetMean();
  cout << __PRETTY_FUNCTION__ << ": meanY = " << meanY << endl;
  const double sigmaInfY = fabs(meanY-edgeInfY);
  cout << __PRETTY_FUNCTION__ << ": sigmaInfY = " << sigmaInfY << endl;
  const double sigmaSupY = fabs(meanY-edgeSupY);
  cout << __PRETTY_FUNCTION__ << ": sigmaSupY = " << sigmaSupY << endl;
  const double plateauRangeInfY = meanY-0.75*sigmaInfY;
  cout << __PRETTY_FUNCTION__ << ": plateauRangeInfY = " << plateauRangeInfY << endl;
  const double plateauRangeSupY = meanY+0.75*sigmaSupY;
  cout << __PRETTY_FUNCTION__ << ": plateauRangeSupY = " << plateauRangeSupY << endl;
  double plateauY = 0.;
  unsigned int countY = 0;
  for(unsigned int iBin=0; iBin<YNBINS; iBin++){
    const double xVal = h1Y -> GetBinLowEdge(iBin);
    if(xVal < plateauRangeInfY) continue;
    else if(xVal > plateauRangeSupY) break;
    else{
      plateauY += h1Y -> GetBinContent(iBin);
      countY++;
    }
  }
  plateauY /= countY;
  cout << __PRETTY_FUNCTION__ << ": plateauY = " << plateauY << endl;
  
  ////////////////
  // computing RoI
  ////////////////
  cout << __PRETTY_FUNCTION__ << ": computing RoI" << endl;
  
  const double fitRangeInfX = meanX-3*sigmaInfX;
  cout << __PRETTY_FUNCTION__ << ": fitRangeInfX = " << fitRangeInfX << endl;
  const double fitRangeSupX = meanX+3*sigmaSupX;
  cout << __PRETTY_FUNCTION__ << ": fitRangeSupX = " << fitRangeSupX << endl;
  TCanvas *cTmp = new TCanvas();
  TF1 *fX = new TF1("fX", "[0]*(2+TMath::Erf([1]*(x-[2]))+TMath::Erf(-[1]*(x-[3])))/4+[4]", fitRangeInfX, fitRangeSupX);
  fX -> SetParNames("scale ver", "scale hor", "left", "right", "offset");
  fX -> SetParameter(0, plateauX);
  fX -> SetParameter(1, 1.);
  fX -> SetParameter(2, edgeInfX);
  fX -> SetParameter(3, edgeSupX);
  fX -> SetParameter(4, 1);
  h1X -> Fit("fX", "R && Q");
  const double RoIInfX = fX -> GetParameter(2);
  cout << __PRETTY_FUNCTION__ << ": RoIInfX = " << RoIInfX << endl;
  const double RoISupX = fX -> GetParameter(3);
  cout << __PRETTY_FUNCTION__ << ": RoISupX = " << RoISupX << endl;
  const double RoISizeX = fabs(RoISupX-RoIInfX);
  cout << __PRETTY_FUNCTION__ << ": RoISizeX = " << RoISizeX << endl;
  
  const double fitRangeInfY = meanY-3*sigmaInfY;
  cout << __PRETTY_FUNCTION__ << ": fitRangeInfY = " << fitRangeInfY << endl;
  const double fitRangeSupY = meanY+3*sigmaSupY;
  cout << __PRETTY_FUNCTION__ << ": fitRangeSupY = " << fitRangeSupY << endl;
  TF1 *fY = new TF1("fY", "[0]*(2+TMath::Erf([1]*(x-[2]))+TMath::Erf(-[1]*(x-[3])))/4+[4]", fitRangeInfY, fitRangeSupY);
  fY -> SetParNames("scale ver", "scale hor", "left", "right", "offset");
  fY -> SetParameter(0, plateauY);
  fY -> SetParameter(1, 1.);
  fY -> SetParameter(2, edgeInfY);
  fY -> SetParameter(3, edgeSupY);
  fY -> SetParameter(4, 1);
  h1Y -> Fit("fY", "R && Q");
  const double RoIInfY = fY -> GetParameter(2);
  cout << __PRETTY_FUNCTION__ << ": RoIInfY = " << RoIInfY << endl;
  const double RoISupY = fY -> GetParameter(3);
  cout << __PRETTY_FUNCTION__ << ": RoISupY = " << RoISupY << endl;
  const double RoISizeY = fabs(RoISupY-RoIInfY);
  cout << __PRETTY_FUNCTION__ << ": RoISizeY = " << RoISizeY << endl;
  delete cTmp;

  ///////////////////////////////////
  // counting number of events in RoI
  ///////////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": counting number of events in RoI" << endl;
  const unsigned int nPoints = grXY -> GetN();
  unsigned int nEventsInRoI = 0;
  for(unsigned int iPoint=0; iPoint<nPoints; iPoint++){
    double xx = 0.;
    double yy = 0.;
    grXY -> GetPoint(iPoint, xx, yy);
    if(xx > RoIInfX && xx < RoISupX && yy > RoIInfY && yy < RoISupY) nEventsInRoI++;
  }
  cout << __PRETTY_FUNCTION__ << ": nEventsInRoI = " << nEventsInRoI << endl;
  
  ////////////////////
  // computing numbers
  ////////////////////
  cout << __PRETTY_FUNCTION__ << ": computing numbers" << endl;
  const double RoIPixelX = RoISizeX/pixelSize;
  cout << __PRETTY_FUNCTION__ << ": RoIPixelX = " << RoIPixelX << endl;
  const double RoIPixelY = RoISizeY/pixelSize;
  cout << __PRETTY_FUNCTION__ << ": RoIPixelY = " << RoIPixelY << endl;
  const double RoIPixel = RoIPixelX*RoIPixelY;
  cout << __PRETTY_FUNCTION__ << ": RoIPixel = " << RoIPixel << endl;
  const double nEventsPerPixelExpected = nEventsInRoI / RoIPixel;
  cout << __PRETTY_FUNCTION__ << ": nEventsPerPixelExpected = " << nEventsPerPixelExpected << endl;
  const double nEventsPerPixelMeasured = grDUT -> GetN()  / 4;
  cout << __PRETTY_FUNCTION__ << ": nEventsPerPixelMeasured = " << nEventsPerPixelMeasured << endl;
  
  ///////////
  // plotting
  ///////////
  cout << __PRETTY_FUNCTION__ << ": plotting" << endl;
  TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 2000, 1000);
  cc -> Divide(2,2);

  cc -> cd(1);
  h1X -> SetLineColor(1);
  h1X -> SetLineWidth(3);
  h1X -> GetXaxis() -> SetRangeUser(RoIInfX-RoISizeX, RoISupX+RoISizeX);
  h1X -> Draw();
  
  cc -> cd(2);
  h1Y -> SetLineColor(1);
  h1Y -> SetLineWidth(3);
  h1Y -> GetXaxis() -> SetRangeUser(RoIInfY-RoISizeY, RoISupY+RoISizeY);
  h1Y -> Draw();

  cc -> cd(3);
  grXY -> Draw("ap");
  grXY -> GetXaxis() -> SetRangeUser(RoIInfX-RoISizeX, RoISupX+RoISizeX);
  grXY -> GetYaxis() -> SetRangeUser(RoIInfY-RoISizeY, RoISupY+RoISizeY);
  grDUT -> SetMarkerColor(2);
  grDUT -> Draw("p");

  cc -> cd(4);
  //  TPaveText *pt = new TPaveText(0,0,1,1);
  //  pt -> AddText("pippo");
  //  pt -> Draw();
  h1Amplitude -> SetLineColor(1);
  h1Amplitude -> SetLineWidth(3);
  //  h1Amplitude -> GetCumulative() -> Draw();
  h1Charge -> SetLineColor(1);
  h1Charge -> SetLineWidth(3);
  //  h1Charge -> GetCumulative() -> Draw();
  h1Charge -> Draw();
  
  return 0;
}
