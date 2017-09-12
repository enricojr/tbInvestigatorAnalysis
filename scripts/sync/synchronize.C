#include "synchronize.hh"

int synchronize(const char *fileNameInTelescope,
		const char *fileNameInDUT,
		const char *fileNameCfg,
		const char *fileNameOut,
		const unsigned int nEvents = 0){
  
  const char *telescopeTreeNameTracks = "Tracks";
  const char *telescopeTreeNameEvent = "Event";
  const char *DUTTreeName = "DUTTree";

  /////////////////////////////
  // reading configuration file
  /////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": reading configuration file " << fileNameCfg << endl;
  configClass *cfg = new configClass();
  if(cfg -> load(fileNameCfg)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot load configuration" << endl;
    return 1;
  }
  cfg -> print();

  ////////////////////////////
  // opening telescope file //
  ////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": opening telescope file " << fileNameInTelescope << endl;
  TFile *fileInTelescope = TFile::Open(fileNameInTelescope);
  if(fileInTelescope == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileNameInTelescope << endl;
    return 1;
  }
  TTree *tEvent = (TTree *) fileInTelescope -> Get(telescopeTreeNameEvent);
  if(tEvent == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open telescope tree " << telescopeTreeNameEvent << endl;
    return 1;
  }
  ULong64_t TimeStamp = 0;
  tEvent -> SetBranchAddress("TimeStamp", &TimeStamp);
  ULong64_t FrameNumber = 0;
  tEvent -> SetBranchAddress("FrameNumber", &FrameNumber);
  TTree *tTracks = (TTree *) fileInTelescope -> Get(telescopeTreeNameTracks);
  if(tTracks == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open telescope tree " << telescopeTreeNameTracks << endl;
    return 1;
  }
  Int_t NTracks = 0;
  tTracks -> SetBranchAddress("NTracks", &NTracks);
  Double_t Chi2[NTracks];
  tTracks -> SetBranchAddress("Chi2", &Chi2);
  Int_t Dof[NTracks];
  tTracks -> SetBranchAddress("Dof", &Dof);
  Double_t X[NTracks];
  tTracks -> SetBranchAddress("X", &X);
  Double_t Y[NTracks];
  tTracks -> SetBranchAddress("Y", &Y);
  Double_t SlopeX[NTracks];
  tTracks -> SetBranchAddress("SlopeX", &SlopeX);
  Double_t SlopeY[NTracks];
  tTracks -> SetBranchAddress("SlopeY", &SlopeY);

  //////////////////////////
  // getting telescope T0 //
  //////////////////////////
  cout << __PRETTY_FUNCTION__ << ": getting telescope T0" << endl;
  tEvent -> GetEntry(0);
  const ULong64_t T0 = TimeStamp;
  cout << __PRETTY_FUNCTION__ << ": T0 = " << T0 << endl;
  
  //////////////////////
  // opening DUT file //
  //////////////////////
  cout << __PRETTY_FUNCTION__ << ": opening DUT file " << fileNameInDUT << endl;
  TFile *fileInDUT = TFile::Open(fileNameInDUT);
  if(fileInDUT == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileNameInDUT << endl;
    return 1;
  }
  TTree *treeDUT = (TTree *) fileInDUT -> Get(DUTTreeName);
  if(treeDUT == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open DUT tree " << DUTTreeName << endl;
    return 1;
  }
  Int_t eventIDDUT = 0;
  treeDUT -> SetBranchAddress("eventNumber", &eventIDDUT);
  Double_t timeDUT = 0;
  treeDUT -> SetBranchAddress("time", &timeDUT);
  Int_t DRSNumber = 0;
  treeDUT -> SetBranchAddress("DRSNumber", &DRSNumber);
  Int_t CHNumber = 0;
  treeDUT -> SetBranchAddress("CHNumber", &CHNumber);
  Float_t linearOffset = 0.;
  treeDUT -> SetBranchAddress("linearOffset", &linearOffset);
  Float_t linearSlope = 0.;
  treeDUT -> SetBranchAddress("linearSlope", &linearSlope);
  Float_t linearRedChi2 = 0.;
  treeDUT -> SetBranchAddress("linearRedChi2", &linearRedChi2);
  Float_t pulseOffset = 0.;
  treeDUT -> SetBranchAddress("pulseOffset", &pulseOffset);
  Float_t pulseSlope = 0.;
  treeDUT -> SetBranchAddress("pulseSlope", &pulseSlope);
  Float_t pulseAmplitude = 0.;
  treeDUT -> SetBranchAddress("pulseAmplitude", &pulseAmplitude);
  Float_t pulseT0 = 0.;
  treeDUT -> SetBranchAddress("pulseT0", &pulseT0);
  Float_t pulseRiseTime = 0.;
  treeDUT -> SetBranchAddress("pulseRiseTime", &pulseRiseTime);
  Float_t pulseDecay = 0.;
  treeDUT -> SetBranchAddress("pulseDecay", &pulseDecay);
  Float_t pulseCharge = 0.;
  treeDUT -> SetBranchAddress("pulseCharge", &pulseCharge);
  Float_t pulseRedChi2 = 0.;
  treeDUT -> SetBranchAddress("pulseRedChi2", &pulseRedChi2);
  // nMax
  unsigned int nMax = treeDUT -> GetEntries();
  if(nEvents!=0) nMax = nEvents;
  cout << __PRETTY_FUNCTION__ << ": nMax = " << nMax << endl;

  ///////////////////////////////
  // checking number of events //
  ///////////////////////////////
  bool doSpillAnalysis = true;
  const unsigned int nEventsDUT = treeDUT -> GetEntries() / cfg -> _nChannels;
  cout << __PRETTY_FUNCTION__ << ": nEventsDUT = " << nEventsDUT << endl;
  tEvent -> GetEntry(tEvent -> GetEntries()-1);
  const unsigned int nEventsTelescope = FrameNumber;
  cout << __PRETTY_FUNCTION__ << ": nEventsTelescope = " << nEventsTelescope << endl;
  if(nEventsDUT == nEventsTelescope) doSpillAnalysis = false;
  
  /////////////////////////
  // opening output file //
  /////////////////////////
  cout << __PRETTY_FUNCTION__ << ": opening output file " << fileNameOut << endl;
  TFile *fileOut = TFile::Open(fileNameOut, "RECREATE");
  if(fileOut == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileNameOut << endl;
    return 1;
  }
  TTree * treeOut = new TTree("syncedData","syncedData");
  Double_t xTrack = 0.;
  treeOut -> Branch("xTrack",&xTrack,"xTrack/D");
  Double_t yTrack = 0.;
  treeOut -> Branch("yTrack",&yTrack,"yTrack/D");
  Double_t timeTelescope = 0.;
  treeOut -> Branch("timeTelescope",&timeTelescope,"timeTelescope/D");
  treeOut -> Branch("FrameNumber", &FrameNumber);
  Double_t redChi2 = 0.;
  treeOut->Branch("redChi2Telescope",&redChi2,"redChi2Telescope/D");
  Int_t nTrack = 0;
  treeOut->Branch("nTrack",&nTrack,"nTrack/I");
  treeOut->Branch("timeDUT",&timeDUT,"timeDUT/D");
  treeOut->Branch("linearOffset",&linearOffset,"linearOffset/F");
  treeOut->Branch("linearSlope",&linearSlope,"linearSlope/F");
  treeOut->Branch("linearRedChi2",&linearRedChi2,"linearRedChi2/F");
  treeOut->Branch("pulseOffset",&pulseOffset,"pulseOffset/F");
  treeOut->Branch("pulseSlope",&pulseSlope,"pulseSlope/F");
  treeOut->Branch("pulseAmplitude",&pulseAmplitude,"pulseAmplitude/F");
  treeOut->Branch("pulseT0",&pulseT0,"pulseT0/F");
  treeOut->Branch("pulseRiseTime",&pulseRiseTime,"pulseRiseTime/F");
  treeOut->Branch("pulseDecay",&pulseDecay,"pulseDecay/F");
  treeOut->Branch("pulseCharge",&pulseCharge,"pulseCharge/F");
  treeOut->Branch("pulseRedChi2",&pulseRedChi2,"pulseRedChi2/F");
  treeOut->Branch("eventIDDUT",&eventIDDUT,"eventIDDUT/I");
  treeOut->Branch("DRSNumber",&DRSNumber,"DRSNumber/I");
  treeOut->Branch("CHNumber",&CHNumber,"CHNumber/I");

  ///////////////////////////
  // collecting DUT spills //
  ///////////////////////////
  cout << __PRETTY_FUNCTION__ << ": collecting DUT spills" << endl;
  vector<spillClass *> spillDUT;
  spillClass *currentSpillDUT = new spillClass();
  for(unsigned int iDUT=0; iDUT<nMax; iDUT+=cfg -> _nChannels){

    // computing dt
    treeDUT -> GetEntry(iDUT+cfg -> _nChannels);
    double timeNext = timeDUT;
    treeDUT -> GetEntry(iDUT);
    double timeCurrent = timeDUT;
    double dt = timeNext-timeCurrent;

    // collecting hits
    eventClass *eventDUT = new eventClass();
    for(unsigned int iHit=iDUT; iHit<iDUT+cfg -> _nChannels; iHit++){
      treeDUT -> GetEntry(iHit);
      hitClass *hitDUT = new hitClass();
      hitDUT -> _ID = eventIDDUT;
      hitDUT -> _index = iHit;
      hitDUT -> _time = timeDUT;
      eventDUT -> _hit.push_back(hitDUT);
    }
    currentSpillDUT -> _event.push_back(eventDUT);
    
    // adding spill
    if(dt > cfg -> _dtSpill){
      spillDUT.push_back(currentSpillDUT);
      currentSpillDUT = new spillClass();
    }
    
  }

  // cout << __PRETTY_FUNCTION__ << ": nSpillsDUT = " << spillDUT.size() << endl;
  // for(unsigned int iSpill=0; iSpill<10; iSpill++){
  //   cout << __PRETTY_FUNCTION__ << ": spill " << iSpill << endl;
  //   spillDUT[iSpill] -> print();
  // }
  
  /////////////////////////////////
  // collecting Telescope spills //
  /////////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": collecting telescope spills" << endl;
  vector<spillClass *> spillTelescope;
  spillClass *currentSpillTelescope = new spillClass();
  for(unsigned int iTelescope=0; iTelescope<tEvent -> GetEntries()-1;){ // the increment is done in the loop

    // computing dt
    tEvent -> GetEntry(iTelescope+1);
    double timeNext = (TimeStamp - T0) * TIMECONVERSION;
    tEvent -> GetEntry(iTelescope);
    double timeCurrent = (TimeStamp - T0) * TIMECONVERSION;
    double dt = timeNext-timeCurrent;

    // counting multiple tracks
    unsigned int nSimultaneous = 1;
    for(unsigned int i=iTelescope+1; i<tEvent -> GetEntries()-1; i++){
      tEvent -> GetEntry(i);
      if(TimeStamp == timeCurrent) nSimultaneous++;
      else break;
    }

    // collecting hits
    eventClass *eventTelescope = new eventClass();
    for(unsigned int iHit=0; iHit<nSimultaneous; iHit++){
      tEvent -> GetEntry(iTelescope);
      hitClass *hitTelescope = new hitClass();
      hitTelescope -> _ID = FrameNumber;
      hitTelescope -> _index = iTelescope;
      hitTelescope -> _time = timeCurrent;
      eventTelescope -> _hit.push_back(hitTelescope);
      iTelescope++;
    }
    currentSpillTelescope -> _event.push_back(eventTelescope);
    
    // adding spill
    if(dt > cfg -> _dtSpill){
      spillTelescope.push_back(currentSpillTelescope);
      currentSpillTelescope = new spillClass();
    }
    
  }

  // cout << __PRETTY_FUNCTION__ << ": nSpillsTelescope = " << spillTelescope.size() << endl;
  // for(unsigned int iSpill=0; iSpill<10; iSpill++){
  //   cout << __PRETTY_FUNCTION__ << ": spill " << iSpill << endl;
  //   spillTelescope[iSpill] -> print();
  // }

  ////////////////////
  // spill analysis //
  ////////////////////

  // checking number of spills
  unsigned int nSpills = spillDUT.size();
  if(spillTelescope.size() < nSpills) nSpills = spillTelescope.size();
  cout << __PRETTY_FUNCTION__ << ": nSpills = " << nSpills << endl;

  // matching event by event, if event size is the same
  for(unsigned int iSpill=0; iSpill<nSpills; iSpill++){
    loadBar(iSpill, nSpills);
    if(spillTelescope[iSpill] -> _event.size() == spillDUT[iSpill] -> _event.size()){
      // matching hit by hit
      for(unsigned int iEvent=0; iEvent<spillDUT[iSpill] -> _event.size(); iEvent++){	
	for(unsigned int iHitTelescope=0; iHitTelescope<spillTelescope[iSpill] -> _event[iEvent] -> _hit.size(); iHitTelescope++){
	  const unsigned int indexTelescope = spillTelescope[iSpill] -> _event[iEvent] -> _hit[iHitTelescope] -> _index;
	  tTracks -> GetEntry(indexTelescope);
	  tEvent -> GetEntry(indexTelescope);
	  timeTelescope = (TimeStamp - T0) * TIMECONVERSION;
	  nTrack =NTracks;
	  for(int iTrack=0; iTrack<NTracks; iTrack++){
	    xTrack = X[iTrack] + SlopeX[iTrack] * DZ;
	    yTrack = Y[iTrack] + SlopeY[iTrack] * DZ;
	    redChi2 = Chi2[iTrack] / Dof[iTrack];
	    for(unsigned int iHitDUT=0; iHitDUT<spillDUT[iSpill] -> _event[iEvent] -> _hit.size(); iHitDUT++){
	      treeDUT -> GetEntry(spillDUT[iSpill] -> _event[iEvent] -> _hit[iHitDUT] -> _index);	  
	      treeOut -> Fill();
	    }
	  }
	} 
      }
    
    }
    else{
      cout << __PRETTY_FUNCTION__ << ": ERROR!!! - mismatching number of events at spill " << iSpill << ": " << spillDUT[iSpill] -> _event.size() << " (DUT) vs " << spillTelescope[iSpill] -> _event.size() << " (telescope)" << endl;
      drawSpill(spillDUT,
		spillTelescope,
		iSpill-2);
      drawSpill(spillDUT,
		spillTelescope,
		iSpill-1);
      drawSpill(spillDUT,
		spillTelescope,
		iSpill);
      drawSpill(spillDUT,
		spillTelescope,
		iSpill+1);
      break;
    }
    
  }

  delete tTracks;
  delete tEvent;
  fileInTelescope -> Close();
  delete fileInTelescope;
  delete treeDUT;
  fileInDUT -> Close();
  delete fileInDUT;
  treeOut -> Write();
  delete treeOut;
  fileOut -> Close();
  delete fileOut;
  delete cfg;
  
  return 0;
}
