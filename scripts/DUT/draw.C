int draw(const unsigned int run){

  gStyle -> SetOptStat(0);
  gStyle -> SetPalette(55);
  
  char fileName[1000];
  sprintf(fileName, "../../../output/DUT_%06d_000000.root", run);
  cout << ": fileName = " << fileName << endl;

  TFile *file = TFile::Open(fileName);
  if(file == NULL){
    cout << ": ERROR!!! - cannot open file" << endl;
    return 1;
  }

  TCanvas *cc = new TCanvas("cc", "cc", 0, 0, 2000, 1000);
  cc -> Divide(4,4);

  cc -> cd(1);
  cc -> cd(1) -> SetGridx();
  TH2F *h2LinearExcludedWaveforms_DRS_0_CH_0 = (TH2F *) file -> Get("h2LinearExcludedWaveforms_DRS_0_CH_0");
  h2LinearExcludedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms_DRS_0_CH_0 -> Draw("colz");
  
  cc -> cd(2);
  cc -> cd(2) -> SetGridx();
  TH2F *h2LinearSelectedWaveforms_DRS_0_CH_0 = (TH2F *) file -> Get("h2LinearSelectedWaveforms_DRS_0_CH_0");
  h2LinearSelectedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms_DRS_0_CH_0 -> Draw("colz");
  
  cc -> cd(5);
  cc -> cd(5) -> SetGridx();
  TH2F *h2LinearExcludedWaveforms_DRS_0_CH_1 = (TH2F *) file -> Get("h2LinearExcludedWaveforms_DRS_0_CH_1");
  h2LinearExcludedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms_DRS_0_CH_1 -> Draw("colz");
  
  cc -> cd(6);
  cc -> cd(6) -> SetGridx();
  TH2F *h2LinearSelectedWaveforms_DRS_0_CH_1 = (TH2F *) file -> Get("h2LinearSelectedWaveforms_DRS_0_CH_1");
  h2LinearSelectedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms_DRS_0_CH_1 -> Draw("colz");
  
  cc -> cd(9);
  cc -> cd(9) -> SetGridx();
  TH2F *h2LinearExcludedWaveforms_DRS_0_CH_2 = (TH2F *) file -> Get("h2LinearExcludedWaveforms_DRS_0_CH_2");
  h2LinearExcludedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms_DRS_0_CH_2 -> Draw("colz");
  
  cc -> cd(10);
  cc -> cd(10) -> SetGridx();
  TH2F *h2LinearSelectedWaveforms_DRS_0_CH_2 = (TH2F *) file -> Get("h2LinearSelectedWaveforms_DRS_0_CH_2");
  h2LinearSelectedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms_DRS_0_CH_2 -> Draw("colz");
  
  cc -> cd(13);
  cc -> cd(13) -> SetGridx();
  TH2F *h2LinearExcludedWaveforms_DRS_1_CH_0 = (TH2F *) file -> Get("h2LinearExcludedWaveforms_DRS_1_CH_0");
  h2LinearExcludedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2LinearExcludedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearExcludedWaveforms_DRS_1_CH_0 -> Draw("colz");
  
  cc -> cd(14);
  cc -> cd(14) -> SetGridx();
  TH2F *h2LinearSelectedWaveforms_DRS_1_CH_0 = (TH2F *) file -> Get("h2LinearSelectedWaveforms_DRS_1_CH_0");
  h2LinearSelectedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2LinearSelectedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2LinearSelectedWaveforms_DRS_1_CH_0 -> Draw("colz");
  
  cc -> cd(3);
  cc -> cd(3) -> SetGridx();
  TH2F *h2PulseExcludedWaveforms_DRS_0_CH_0 = (TH2F *) file -> Get("h2PulseExcludedWaveforms_DRS_0_CH_0");
  h2PulseExcludedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms_DRS_0_CH_0 -> Draw("colz");
  
  cc -> cd(4);
  cc -> cd(4) -> SetGridx();
  TH2F *h2PulseSelectedWaveforms_DRS_0_CH_0 = (TH2F *) file -> Get("h2PulseSelectedWaveforms_DRS_0_CH_0");
  h2PulseSelectedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms_DRS_0_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms_DRS_0_CH_0 -> Draw("colz");
  
  cc -> cd(7);
  cc -> cd(7) -> SetGridx();
  TH2F *h2PulseExcludedWaveforms_DRS_0_CH_1 = (TH2F *) file -> Get("h2PulseExcludedWaveforms_DRS_0_CH_1");
  h2PulseExcludedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms_DRS_0_CH_1 -> Draw("colz");
  
  cc -> cd(8);
  cc -> cd(8) -> SetGridx();
  TH2F *h2PulseSelectedWaveforms_DRS_0_CH_1 = (TH2F *) file -> Get("h2PulseSelectedWaveforms_DRS_0_CH_1");
  h2PulseSelectedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms_DRS_0_CH_1 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms_DRS_0_CH_1 -> Draw("colz");
  
  cc -> cd(11);
  cc -> cd(11) -> SetGridx();
  TH2F *h2PulseExcludedWaveforms_DRS_0_CH_2 = (TH2F *) file -> Get("h2PulseExcludedWaveforms_DRS_0_CH_2");
  h2PulseExcludedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms_DRS_0_CH_2 -> Draw("colz");
  
  cc -> cd(12);
  cc -> cd(12) -> SetGridx();
  TH2F *h2PulseSelectedWaveforms_DRS_0_CH_2 = (TH2F *) file -> Get("h2PulseSelectedWaveforms_DRS_0_CH_2");
  h2PulseSelectedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms_DRS_0_CH_2 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms_DRS_0_CH_2 -> Draw("colz");
  
  cc -> cd(15);
  cc -> cd(15) -> SetGridx();
  TH2F *h2PulseExcludedWaveforms_DRS_1_CH_0 = (TH2F *) file -> Get("h2PulseExcludedWaveforms_DRS_1_CH_0");
  h2PulseExcludedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2PulseExcludedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseExcludedWaveforms_DRS_1_CH_0 -> Draw("colz");
  
  cc -> cd(16);
  cc -> cd(16) -> SetGridx();
  TH2F *h2PulseSelectedWaveforms_DRS_1_CH_0 = (TH2F *) file -> Get("h2PulseSelectedWaveforms_DRS_1_CH_0");
  h2PulseSelectedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetNdivisions(712);
  h2PulseSelectedWaveforms_DRS_1_CH_0 -> GetXaxis() -> SetLabelSize(0.065);
  h2PulseSelectedWaveforms_DRS_1_CH_0 -> Draw("colz");  
  
  return 0;
}
