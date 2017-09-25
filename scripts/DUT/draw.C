#include <iostream>
#include <fstream>
using namespace std;

#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TLine.h>

#include "../aux/configClass.hh"

int draw(const unsigned int run,
	 const bool eos = true){

  gStyle -> SetOptStat(0);
  gStyle -> SetPalette(55);
  gStyle->SetNumberContours(256);

  char fileName[1000];
  if(eos) sprintf(fileName, "/eos/atlas/atlascerngroupdisk/pixel-upgrade/cmos/TowerJazz/Investigator/Testbeam/Testbeam2017/converted/run_%06d.cfg", run);
  else sprintf(fileName, "../../../cfg/run_%06d.cfg", run);
  cout << __PRETTY_FUNCTION__ << ": config fileName = " << fileName << endl;
  configClass *cfg = new configClass();
  if(cfg -> load(fileName)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot load configuration" << endl;
    return 1;
  }
  cfg -> print();
  
  if(eos) sprintf(fileName, "/eos/atlas/atlascerngroupdisk/pixel-upgrade/cmos/TowerJazz/Investigator/Testbeam/Testbeam2017/converted/DUT_%06d_000000.root", run);
  else sprintf(fileName, "../../../output/DUT_%06d_000000.root", run);
  cout << __PRETTY_FUNCTION__ << ": input fileName = " << fileName << endl;
  TFile *file = TFile::Open(fileName);
  if(file == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file" << endl;
    return 1;
  }

  TLine *line = NULL;
  TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 2000, 1000);
  cc -> Divide(4,4);

  cc -> cd(1);
  cc -> cd(1) -> SetGridx();
  TH2F *h2LinearExcludedWaveforms_DRS_0_CH_0 = (TH2F *) file -> Get("h2LinearExcludedWaveforms_DRS_0_CH_0");
  h2LinearExcludedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms_DRS_0_CH_0 -> Draw("colz");  
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(2);
  cc -> cd(2) -> SetGridx();
  TH2F *h2LinearSelectedWaveforms_DRS_0_CH_0 = (TH2F *) file -> Get("h2LinearSelectedWaveforms_DRS_0_CH_0");
  h2LinearSelectedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms_DRS_0_CH_0 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(5);
  cc -> cd(5) -> SetGridx();
  TH2F *h2LinearExcludedWaveforms_DRS_0_CH_1 = (TH2F *) file -> Get("h2LinearExcludedWaveforms_DRS_0_CH_1");
  h2LinearExcludedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms_DRS_0_CH_1 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][1].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(6);
  cc -> cd(6) -> SetGridx();
  TH2F *h2LinearSelectedWaveforms_DRS_0_CH_1 = (TH2F *) file -> Get("h2LinearSelectedWaveforms_DRS_0_CH_1");
  h2LinearSelectedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms_DRS_0_CH_1 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][1].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();  
  line = new TLine(cfg -> _pulseT0[0][1].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(9);
  cc -> cd(9) -> SetGridx();
  TH2F *h2LinearExcludedWaveforms_DRS_0_CH_2 = (TH2F *) file -> Get("h2LinearExcludedWaveforms_DRS_0_CH_2");
  h2LinearExcludedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms_DRS_0_CH_2 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][2].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();  
  line = new TLine(cfg -> _pulseT0[0][2].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(10);
  cc -> cd(10) -> SetGridx();
  TH2F *h2LinearSelectedWaveforms_DRS_0_CH_2 = (TH2F *) file -> Get("h2LinearSelectedWaveforms_DRS_0_CH_2");
  h2LinearSelectedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms_DRS_0_CH_2 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][2].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(13);
  cc -> cd(13) -> SetGridx();
  TH2F *h2LinearExcludedWaveforms_DRS_1_CH_0 = (TH2F *) file -> Get("h2LinearExcludedWaveforms_DRS_1_CH_0");
  h2LinearExcludedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms_DRS_1_CH_0 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[1][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(14);
  cc -> cd(14) -> SetGridx();
  TH2F *h2LinearSelectedWaveforms_DRS_1_CH_0 = (TH2F *) file -> Get("h2LinearSelectedWaveforms_DRS_1_CH_0");
  h2LinearSelectedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms_DRS_1_CH_0 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[1][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(3);
  cc -> cd(3) -> SetGridx();
  TH2F *h2PulseExcludedWaveforms_DRS_0_CH_0 = (TH2F *) file -> Get("h2PulseExcludedWaveforms_DRS_0_CH_0");
  h2PulseExcludedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms_DRS_0_CH_0 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();  
  line = new TLine(cfg -> _pulseT0[0][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(4);
  cc -> cd(4) -> SetGridx();
  TH2F *h2PulseSelectedWaveforms_DRS_0_CH_0 = (TH2F *) file -> Get("h2PulseSelectedWaveforms_DRS_0_CH_0");
  h2PulseSelectedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms_DRS_0_CH_0 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();  
  line = new TLine(cfg -> _pulseT0[0][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(7);
  cc -> cd(7) -> SetGridx();
  TH2F *h2PulseExcludedWaveforms_DRS_0_CH_1 = (TH2F *) file -> Get("h2PulseExcludedWaveforms_DRS_0_CH_1");
  h2PulseExcludedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms_DRS_0_CH_1 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][1].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();  
  line = new TLine(cfg -> _pulseT0[0][1].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(8);
  cc -> cd(8) -> SetGridx();
  TH2F *h2PulseSelectedWaveforms_DRS_0_CH_1 = (TH2F *) file -> Get("h2PulseSelectedWaveforms_DRS_0_CH_1");
  h2PulseSelectedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms_DRS_0_CH_1 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][1].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][1].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][1].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(11);
  cc -> cd(11) -> SetGridx();
  TH2F *h2PulseExcludedWaveforms_DRS_0_CH_2 = (TH2F *) file -> Get("h2PulseExcludedWaveforms_DRS_0_CH_2");
  h2PulseExcludedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms_DRS_0_CH_2 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][2].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(12);
  cc -> cd(12) -> SetGridx();
  TH2F *h2PulseSelectedWaveforms_DRS_0_CH_2 = (TH2F *) file -> Get("h2PulseSelectedWaveforms_DRS_0_CH_2");
  h2PulseSelectedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms_DRS_0_CH_2 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[0][2].val, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[0][2].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[0][2].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(15);
  cc -> cd(15) -> SetGridx();
  TH2F *h2PulseExcludedWaveforms_DRS_1_CH_0 = (TH2F *) file -> Get("h2PulseExcludedWaveforms_DRS_1_CH_0");
  h2PulseExcludedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms_DRS_1_CH_0 -> Draw("colz");
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[1][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  
  cc -> cd(16);
  cc -> cd(16) -> SetGridx();
  TH2F *h2PulseSelectedWaveforms_DRS_1_CH_0 = (TH2F *) file -> Get("h2PulseSelectedWaveforms_DRS_1_CH_0");
  h2PulseSelectedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms_DRS_1_CH_0 -> Draw("colz");  
  cc -> Update();
  line = new TLine(cfg -> _pulseT0[1][0].val, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].val, gPad->GetUymax());
  line -> SetLineColor(2);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.min, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.min, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();
  line = new TLine(cfg -> _pulseT0[1][0].range.max, gPad->GetUymin(),
		   cfg -> _pulseT0[1][0].range.max, gPad->GetUymax());
  line -> SetLineColor(3);
  line -> SetLineWidth(2);
  line -> Draw();

  delete cfg;
  
  return 0;
}
