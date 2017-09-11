#include "synchronize.hh"

const bool doSpillAnalysis = false;

int synchronize(const char *fileNameInTelescope,
		const char *fileNameInDUT,
		const char *fileNameCfg,
		const char *fileNameOut,
		const unsigned int nEvents = 0){

  const char *telescopeTreeName = "trackOnDUT";
  const char *DUTTreeName = "DUTTree";
  const unsigned int debugLevel = 0;

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
  TTree *treeTelescope = (TTree *) fileInTelescope -> Get(telescopeTreeName);
  if(treeTelescope == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open telescope tree " << telescopeTreeName << endl;
    return 1;
  }
  UInt_t eventIDTelescope = 0;
  treeTelescope -> SetBranchAddress("eventID", &eventIDTelescope);
  Double_t timeTelescope = 0;
  treeTelescope -> SetBranchAddress("time", &timeTelescope);
  Double_t x = 0.;
  treeTelescope -> SetBranchAddress("x", &x);
  Double_t y = 0.;
  treeTelescope -> SetBranchAddress("y", &y);
  Double_t redChi2 = 0.;
  treeTelescope -> SetBranchAddress("redChi2", &redChi2);
  TGraph *grTimelineTelescope = (TGraph *) fileInTelescope -> Get("grTimeline");
  const unsigned int nEntriesTelescope = treeTelescope -> GetEntries();
  cout << __PRETTY_FUNCTION__ << ": nEntriesTelescope = " << nEntriesTelescope << endl;

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
  Int_t eventNumberDUT = 0;
  treeDUT -> SetBranchAddress("eventNumber", &eventNumberDUT);
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
  TGraph *grTimelineDUT = (TGraph *) fileInDUT -> Get("grTimeline");
  // nMax
  unsigned int nMax = treeDUT -> GetEntries();
  if(nEvents!=0) nMax = nEvents;
  cout << __PRETTY_FUNCTION__ << ": nMax = " << nMax << endl;


  /////////////////////////
  // opening output file //
  /////////////////////////
  cout << __PRETTY_FUNCTION__ << ": opening output file " << fileNameOut << endl;
  TFile *fileOut = TFile::Open(fileNameOut, "RECREATE");
  if(fileOut == NULL){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! cannot open file " << fileNameOut << endl;
    return 1;
  }
  //Florian:
  //add tree and branches
  TTree * treeOut = new TTree("syncedData","syncedData");
  //telescope stuff
  treeOut->Branch("xTrack",&x,"xTrack/D");
  treeOut->Branch("yTrack",&y,"yTrack/D");
  treeOut->Branch("timeTelescope",&timeTelescope,"timeTelescope/D");
  treeOut->Branch("redChi2Telescope",&redChi2,"redChi2Telescope/D");
  Int_t nTrack = 0;
  treeOut->Branch("nTrack",&nTrack,"nTrack/I");
  //DUT stuff
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
  treeOut->Branch("eventNumberDUT",&eventNumberDUT,"eventNumberDUT/I");
  treeOut->Branch("DRSNumber",&DRSNumber,"DRSNumber/I");
  treeOut->Branch("CHNumber",&CHNumber,"CHNumber/I");

  /////////////////////
  // looking for RoI //
  /////////////////////
  cout << __PRETTY_FUNCTION__ << ": looking for RoI" << endl;
  TH2F *h2TrackMap = new TH2F("h2TrackMap", "track map", PLOTTRACKMAPNBINSX, PLOTTRACKMAPMINX, PLOTTRACKMAPMAXX, PLOTTRACKMAPNBINSY, PLOTTRACKMAPMINY, PLOTTRACKMAPMAXY);
  for(unsigned int eventNumberTelescope=0; eventNumberTelescope<treeTelescope -> GetEntries(); eventNumberTelescope++){
    treeTelescope -> GetEntry(eventNumberTelescope);
    h2TrackMap -> Fill(x, y);
  }
  const double trackMapMax = h2TrackMap -> GetBinContent(h2TrackMap -> GetMaximumBin());
  cout << __PRETTY_FUNCTION__ << ": trackMapMax = " << trackMapMax << endl;
  const double RoIThreshold = trackMapMax * ROITHRESHOLDFRACTION;
  cout << __PRETTY_FUNCTION__ << ": RoIThreshold = " << RoIThreshold << endl;
  TH2F *h2RoI = new TH2F("h2RoI", "RoI", PLOTTRACKMAPNBINSX, PLOTTRACKMAPMINX, PLOTTRACKMAPMAXX, PLOTTRACKMAPNBINSY, PLOTTRACKMAPMINY, PLOTTRACKMAPMAXY);
  double xRoIMin = PLOTTRACKMAPMAXX;
  double xRoIMax = PLOTTRACKMAPMINX;
  double yRoIMin = PLOTTRACKMAPMAXY;
  double yRoIMax = PLOTTRACKMAPMINY;
  for(int iX=0; iX<h2TrackMap -> GetXaxis() -> GetNbins(); iX++){
    for(int iY=0; iY<h2TrackMap -> GetYaxis() -> GetNbins(); iY++){
      const double val = h2TrackMap -> GetBinContent(iX, iY);
      if(val > RoIThreshold){
	h2RoI -> SetBinContent(iX, iY, val);
	double xCoord = h2RoI -> GetXaxis() -> GetBinLowEdge(iX);
	if(xCoord < xRoIMin) xRoIMin = xCoord;
	if(xCoord > xRoIMax) xRoIMax = xCoord;
	double yCoord = h2RoI -> GetYaxis() -> GetBinLowEdge(iY);
	if(yCoord < yRoIMin) yRoIMin = yCoord;
	if(yCoord > yRoIMax) yRoIMax = yCoord;
      }
      else continue;
    }
  }
  cout << __PRETTY_FUNCTION__ << ": RoI = (" << xRoIMin << ", " << yRoIMin << ") -> (" << xRoIMax << ", " << yRoIMax << ")" << endl;

  //////////////////////
  // allocating plots //
  //////////////////////
  cout << __PRETTY_FUNCTION__ << ": allocating plots" << endl;
  TH1F *h1TimeDistanceDUT = new TH1F("h1TimeDistanceDUT", "time difference in DUT event", PLOTTIMEDISTANCENBINS, PLOTTIMEDISTANCEMIN, PLOTTIMEDISTANCEMAX);
  TH1F *h1TimeDistanceDUTSpill = new TH1F("h1TimeDistanceDUTSpill", "time difference in DUT event", PLOTTIMEDISTANCENBINS, PLOTTIMEDISTANCEMIN, PLOTTIMEDISTANCEMAX);
  TH1F *h1TimeDistanceTelescope = new TH1F("h1TimeDistanceTelescope", "time difference in Telescope event", PLOTTIMEDISTANCENBINS, PLOTTIMEDISTANCEMIN, PLOTTIMEDISTANCEMAX);
  TH1F *h1TimeDistanceTelescopeSpill = new TH1F("h1TimeDistanceTelescopeSpill", "time difference in Telescope event", PLOTTIMEDISTANCENBINS, PLOTTIMEDISTANCEMIN, PLOTTIMEDISTANCEMAX);
  TH2F ***h2DUTMap = new TH2F**[cfg -> _nDRS];
  for(unsigned int ii=0; ii<cfg -> _nDRS; ii++){
    h2DUTMap[ii] = new TH2F*[cfg -> _nCH[ii]];
    for(unsigned int jj=0; jj<cfg -> _nCH[ii]; jj++){
      char name[1000];
      sprintf(name, "h2DUTMap_DRS_%d_CH_%d", ii, jj);
      char title[1000];
      sprintf(name, "DUT map DRS %d CH %d", ii, jj);
      h2DUTMap[ii][jj] = new TH2F(name, title, PLOTDUTMAPNBINSX, xRoIMin, xRoIMax, PLOTDUTMAPNBINSY, yRoIMin, yRoIMax);
    }
  }

  //////////////////////////////////////////////////////////
  // building time distance plot and spill vector for DUT //
  //////////////////////////////////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": building time distance plot and spill vector for DUT" << endl;
  vector<spillClass *> dataDUT;
  spillClass *spillDUT = new spillClass();
  interSpillClass *interSpillDUT = new interSpillClass();
  for(unsigned int iDUT=0; iDUT<nMax; iDUT+=cfg -> _nChannels){

    // computing dt
    treeDUT -> GetEntry(iDUT+cfg -> _nChannels);
    double timeNext = timeDUT;
    treeDUT -> GetEntry(iDUT);
    double timeCurrent = timeDUT;
    double dt = timeNext-timeCurrent;

    // filling dt plot
    h1TimeDistanceDUT -> Fill(dt);

    if(debugLevel>=2) cout << __PRETTY_FUNCTION__ << ": iDUT " << iDUT << ", eventNumberDUT = " << eventNumberDUT << ", timeNext = " << timeNext << ", timeCurrent = " << timeCurrent << ", dt = " << dt << endl;

    // collecting hits
    eventClass *eventDUT = new eventClass();
    eventDUT -> _time = timeCurrent;
    eventDUT -> _ID = iDUT/cfg -> _nChannels;
    for(unsigned int iHit=iDUT; iHit<iDUT+cfg -> _nChannels; iHit++){
      treeDUT -> GetEntry(iHit);
      if(debugLevel>=2) cout << __PRETTY_FUNCTION__ << ": iHit = " << iHit << ", time = " << timeDUT << ", DRS = " << DRSNumber << ", CH = " << CHNumber << endl;
      hitClass *hitDUT = new hitClass();
      hitDUT -> _ID = iHit;
      if(linearRedChi2 > cfg -> _linearRedChi2Threshold
	 &&
	 pulseT0 > cfg -> _pulseT0Range.min
	 &&
	 pulseT0 < cfg -> _pulseT0Range.max
	 &&
	 pulseRiseTime > cfg -> _pulseRiseTimeRange.min
	 &&
	 pulseRiseTime < cfg -> _pulseRiseTimeRange.max) hitDUT -> _onTarget = true;
      else hitDUT -> _onTarget = false;
      eventDUT -> _hit.push_back(hitDUT);
    }
    interSpillDUT -> _event.push_back(eventDUT);

    // interspill
    if(dt > cfg -> _dtInterSpill){
      spillDUT -> _interSpill.push_back(interSpillDUT);
      interSpillDUT = new interSpillClass();
    }

    // spill
    if(dt > cfg -> _dtSpill){
      dataDUT.push_back(spillDUT);
      spillDUT = new spillClass();
    }

  }

  cout << __PRETTY_FUNCTION__ << ": number of DUT spills = " << dataDUT.size() << endl;
  if(debugLevel >= 2){
    for(unsigned int iData=0; iData<dataDUT.size(); iData++){
      cout << __PRETTY_FUNCTION__ << ": spill " << iData << endl;
      dataDUT[iData] -> print();
      dataDUT[iData] -> getGraph(2) -> Draw("ap");
      gPad -> WaitPrimitive();
    }
  }

  ////////////////////////////////////////////////////////////////
  // building time distance plot and spill vector for Telescope //
  ////////////////////////////////////////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": building time distance plot for telescope" << endl;
  vector<spillClass *> dataTelescope;
  spillClass *spillTelescope = new spillClass();
  interSpillClass *interSpillTelescope = new interSpillClass();
  unsigned int eventNumberTelescope = 0;
  for(unsigned int iTelescope=0; iTelescope<treeTelescope -> GetEntries()-1;){ // the increment is done in the loop

    // computing dt
    treeTelescope -> GetEntry(iTelescope+1);
    double timeNext = timeTelescope;
    treeTelescope -> GetEntry(iTelescope);
    double timeCurrent = timeTelescope;
    double dt = timeNext-timeCurrent;

    // filling dt plot
    h1TimeDistanceTelescope -> Fill(dt);

    // creating event
    eventClass *eventTelescope = new eventClass();
    eventTelescope -> _time = timeCurrent;
    eventTelescope -> _ID = eventNumberTelescope;

    // counting multiple tracks
    unsigned int nSimultaneous = 1;
    for(unsigned int i=iTelescope+1; i<treeTelescope -> GetEntries()-1; i++){
      treeTelescope -> GetEntry(i);
      if(timeTelescope == timeCurrent) nSimultaneous++;
      else break;
    }

    if(debugLevel>=2) cout << __PRETTY_FUNCTION__ << ": iTelescope " << iTelescope << ", eventNumberTelescope = " << eventNumberTelescope << ", timeNext = " << timeNext << ", timeCurrent = " << timeCurrent << ", dt = " << dt << ", nSimultaneous = " << nSimultaneous << endl;

    for(unsigned int iHit=0; iHit<nSimultaneous; iHit++){
      treeTelescope -> GetEntry(iTelescope);
      hitClass *hitTelescope = new hitClass();
      hitTelescope -> _ID = iTelescope;
      if(x>-2870. && x<-2700 && y>-3660 && y<-3570) hitTelescope -> _onTarget = true;
      else hitTelescope -> _onTarget = false;
      eventTelescope -> _hit.push_back(hitTelescope);
      iTelescope++;
    }
    interSpillTelescope -> _event.push_back(eventTelescope);

    // interspill
    if(dt > cfg -> _dtInterSpill){
      spillTelescope -> _interSpill.push_back(interSpillTelescope);
      interSpillTelescope = new interSpillClass();
    }

    // spill
    if(dt > cfg -> _dtSpill){
      dataTelescope.push_back(spillTelescope);
      spillTelescope = new spillClass();
    }

    eventNumberTelescope++;
  }

  cout << __PRETTY_FUNCTION__ << ": number of Telescope spills = " << dataTelescope.size() << endl;
  if(debugLevel >= 2){
    for(unsigned int iData=0; iData<dataTelescope.size(); iData++){
      cout << __PRETTY_FUNCTION__ << ": spill " << iData << endl;
      dataTelescope[iData] -> print();
      dataTelescope[iData] -> getGraph(1) -> Draw("ap");
      gPad -> WaitPrimitive();
    }
  }

  ///////////////////////////////////////////
  // computing spill/interspill time averages
  ///////////////////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": computing spill/interspill time averages" << endl;
  for(unsigned int iData=0; iData<dataDUT.size(); iData++){
    dataDUT[iData] -> computeTimeAverage();
  }
  for(unsigned int iData=0; iData<dataTelescope.size(); iData++){
    dataTelescope[iData] -> computeTimeAverage();
  }

  // ///////////////////////////////////
  // // building target plots for DUT //
  // ///////////////////////////////////
  // const double chi2LinMin = 1.;
  // const double T0Min = 75.;
  // const double T0Max = 250.;
  // vector<TGraph *> grTargetDUT;
  // for(unsigned int iSpill=0; iSpill<dataDUT.size(); iSpill++){
  //   TGraph *gr = new TGraph();
  //   for(unsigned int iInterSpill=0; iInterSpill<dataDUT[iSpill] -> _interSpill.size(); iInterSpill++){
  //     for(unsigned int iEvent=0; iEvent<dataDUT[iSpill] -> _interSpill[iInterSpill] -> _event.size(); iEvent++){
  // 	for(unsigned int iHit=0; iHit<dataDUT[iSpill] -> _interSpill[iInterSpill] -> _event[iEvent] -> _hit.size(); iHit++){
  // 	  treeDUT -> GetEntry(dataDUT[iSpill] -> _interSpill[iInterSpill] -> _event[iEvent] -> _hit[iHit] -> _ID);
  // 	  if(linearRedChi2>chi2LinMin && pulseT0>T0Min && pulseT0<T0Max) gr -> SetPoint(gr -> GetN(), gr -> GetN() + dataDUT[iSpillDUT] -> _interSpill[iInterSpillDUT] -> _event[0] -> _spillLocation);
  // 	}
  //     }
  //   }
  //   grTarget.push_back(gr);
  // }
  
  ////////////////////
  // spill analysis //
  ////////////////////
  if(doSpillAnalysis){
    cout << __PRETTY_FUNCTION__ << ": spill analysis" << endl;
    unsigned int spillDelay = 0;
    unsigned int plotIndex = 0;
    for(unsigned int iSpill=0; iSpill<dataTelescope.size(); iSpill++){
      loadBar(iSpill,dataTelescope.size());
      
      // spill indices
      const unsigned int iSpillTelescope = iSpill;
      const unsigned int iSpillDUT = iSpill+spillDelay;
      if(iSpillDUT>=dataDUT.size()){
	cout << __PRETTY_FUNCTION__ << ": end of DUT events" << endl;
	break;
      }
      
      // computing number of interspills in common
      const unsigned int nInterSpills = computeNInterSpills(dataDUT[iSpillDUT], dataTelescope[iSpillTelescope]);
      
      // computing event locations (indices within spill and inter-spill)
      dataDUT[iSpillDUT] -> computeEventLocations();
      dataTelescope[iSpillTelescope] -> computeEventLocations();
      
      // computing spill delay, if unmatching spills, continue
      if(dtSpill(dataDUT[iSpillDUT], dataTelescope[iSpillTelescope], nInterSpills) > cfg -> _dtSpill){
	spillDelay++;
	continue;
      }
      
      // if first event has DUT happening after telescope, go to next telescope spill
      unsigned int interSpillDelayTelescope = 0;
      const double dtFirstEvent = dataDUT[iSpillDUT] -> _interSpill[0] -> _event[0] -> _time - dataTelescope[iSpillTelescope] -> _interSpill[0] -> _event[0] -> _time;
      if(dtFirstEvent > cfg -> _dtInterSpill){
	interSpillDelayTelescope ++;
      }
      
      // interspills
      unsigned int interSpillDelayDUT = 0;
      for(unsigned int iInterSpill=0; iInterSpill<nInterSpills; iInterSpill++){
	//cout << "iInterSpill: " << iInterSpill << endl;
	
	// interSpill indices
	const unsigned int iInterSpillTelescope = iInterSpill + interSpillDelayTelescope;
	const unsigned int iInterSpillDUT = iInterSpill+interSpillDelayDUT;
	if(iInterSpillDUT>=dataDUT[iSpillDUT] -> _interSpill.size()
	   ||
	   iInterSpillTelescope>=dataTelescope[iSpillTelescope] -> _interSpill.size()){
	  break;
	}
	
	// computing number of events in common
	const unsigned int nEventsInInterSpill = computeNEvents(dataDUT[iSpillDUT] -> _interSpill[iInterSpillDUT],
								dataTelescope[iSpillTelescope] -> _interSpill[iInterSpillTelescope]);
	
	// computing interSpill delay, if unmatching inter-spills, continue
	const double dtIS = dtInterSpill(dataDUT[iSpillDUT] -> _interSpill[iInterSpillDUT], dataTelescope[iSpillTelescope] -> _interSpill[iInterSpillTelescope], nEventsInInterSpill);
	
	if(dtIS > cfg -> _dtInterSpill){
	  interSpillDelayDUT++;
	  
	  //	if(debugLevel>=1) drawInterSpill(dataDUT, dataTelescope, iSpill, iSpillDUT, iSpillTelescope, iInterSpill, iInterSpillDUT, iInterSpillTelescope, 18, 30, 30, false, plotIndex);
	  if(debugLevel>=1) drawInterSpillWithTarget(dataDUT, dataTelescope, iSpill, iSpillDUT, iSpillTelescope, iInterSpill, iInterSpillDUT, iInterSpillTelescope, 18, 30, 30, 3, true, plotIndex);
	  
	}
	else{
	  
	  //	if(debugLevel>=1) drawInterSpill(dataDUT, dataTelescope, iSpill, iSpillDUT, iSpillTelescope, iInterSpill, iInterSpillDUT, iInterSpillTelescope, 18, 2, 1, false, plotIndex);
	  if(debugLevel>=1) drawInterSpillWithTarget(dataDUT, dataTelescope, iSpill, iSpillDUT, iSpillTelescope, iInterSpill, iInterSpillDUT, iInterSpillTelescope, 18, 2, 1, 3, true, plotIndex);
	  
	  // Florian
	  // Fill branches of out file
	  for(uint iter = 0; iter < nEventsInInterSpill; iter++){
	    //cout << "Iter: " << iter << endl;
	    eventClass * telescopeEvent = dataTelescope[iSpillTelescope] -> _interSpill[iInterSpillTelescope] -> _event[iter];
	    nTrack = telescopeEvent -> _hit.size();
	    eventClass * dutEvent = dataDUT[iSpillDUT] -> _interSpill[iInterSpillDUT] -> _event[iter];
	    for(uint i = 0; i < telescopeEvent -> _hit.size(); i++){
	      for(uint j = 0; j < dutEvent -> _hit.size(); j++){
		treeDUT -> GetEntry(dutEvent -> _hit[j] -> _ID);
		treeTelescope -> GetEntry(telescopeEvent -> _hit[i] -> _ID);
		treeOut -> Fill();
	      }
	    }
	  }
	}
	plotIndex++;
      }
      
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // 1-to-1 match, for runs with equal number of events in DUT and telescope //
  /////////////////////////////////////////////////////////////////////////////
  else{

    cout << __PRETTY_FUNCTION__ << ": 1-to-1 match" << endl;

    treeDUT -> GetEntry(treeDUT -> GetEntries()-1);
    const unsigned int maxEventID = eventNumberDUT;
    cout << __PRETTY_FUNCTION__ << ": maxEventID = " << maxEventID << endl;

    unsigned int iEventDUT = 0;
    unsigned int iEventTelescope = 0;
    for(unsigned int iEvent=0; iEvent<maxEventID; iEvent++){

      loadBar(iEvent, maxEventID);
      
      // collecting DUT events
      vector<unsigned int> indexDUT;
      for(;iEventDUT<treeDUT -> GetEntries(); iEventDUT++){
	treeDUT -> GetEntry(iEventDUT);
	if(eventNumberDUT == iEvent) indexDUT.push_back(iEventDUT);
	else{
	  break;
	}
      }

      // cout << "indexDUT = ";
      // for(unsigned int i=0; i<indexDUT.size(); i++){
      // 	treeDUT -> GetEntry(indexDUT[i]);
      // 	cout << indexDUT[i] << " (" << eventNumberDUT << "), ";
      // }
      // cout << endl;

      // collecting telescope events
      vector<unsigned int> indexTelescope;
      for(;iEventTelescope<treeTelescope -> GetEntries(); iEventTelescope++){
	treeTelescope -> GetEntry(iEventTelescope);
	if(eventIDTelescope == iEvent) indexTelescope.push_back(iEventTelescope);
	else{
	  break;
	}
      }

      // cout << "indexTelescope = ";
      // for(unsigned int i=0; i<indexTelescope.size(); i++){
      // 	treeTelescope -> GetEntry(indexTelescope[i]);
      // 	cout << indexTelescope[i] << " (" << eventIDTelescope << "), ";
      // }
      // cout << endl;

      // pairing
      for(unsigned int i=0; i<indexTelescope.size(); i++){
	nTrack = indexTelescope.size();
      	treeTelescope -> GetEntry(indexTelescope[i]);
	for(unsigned int i=0; i<indexDUT.size(); i++){
	  treeDUT -> GetEntry(indexDUT[i]);
	  treeOut -> Fill();
	}
      }
      
      indexDUT.clear();
      indexTelescope.clear();      
    }
    
  }
  
  /////////////////////////
  // writing output file //
  /////////////////////////
  fileOut -> Write();
  cout << __PRETTY_FUNCTION__ << ": writing output file: " << fileNameOut << endl;
  TCanvas *cTimeDistanceDUT = new TCanvas("cTimeDistanceDUT", "cTimeDistanceDUT", 0, 0, 1000, 1000);
  cTimeDistanceDUT -> SetLogx();
  cTimeDistanceDUT -> SetLogy();
  h1TimeDistanceDUT -> SetLineColor(1);
  h1TimeDistanceDUT -> SetLineWidth(2);
  h1TimeDistanceDUT -> Draw();
  h1TimeDistanceDUTSpill -> SetFillColor(5);
  h1TimeDistanceDUTSpill -> Draw("same");
  h1TimeDistanceDUT -> Draw("same");
  cTimeDistanceDUT -> Write();
  TCanvas *cTimeDistanceTelescope = new TCanvas("cTimeDistanceTelescope", "cTimeDistanceTelescope", 0, 0, 1000, 1000);
  cTimeDistanceTelescope -> SetLogx();
  cTimeDistanceTelescope -> SetLogy();
  h1TimeDistanceTelescope -> SetLineColor(1);
  h1TimeDistanceTelescope -> SetLineWidth(2);
  h1TimeDistanceTelescope -> Draw();
  h1TimeDistanceTelescopeSpill -> SetFillColor(5);
  h1TimeDistanceTelescopeSpill -> Draw("same");
  h1TimeDistanceTelescope -> Draw("same");
  cTimeDistanceTelescope -> Write();
  TCanvas *cTrackMap = new TCanvas("cTrackMap", "cTrackMap", 0, 0, 1000, 1000);
  h2TrackMap -> Draw("colz");
  cTrackMap -> Write();
  TCanvas *cRoI = new TCanvas("cRoI", "cRoI", 0, 0, 1000, 1000);
  h2RoI -> Draw("colz");
  cRoI -> Write();
  for(unsigned int ii=0; ii<cfg -> _nDRS; ii++){
    for(unsigned int jj=0; jj<cfg -> _nCH[ii]; jj++){
      char name[1000];
      sprintf(name, "ccDUTMap_DRS_%d_CH_%d", ii, jj);
      TCanvas *cDUTMap = new TCanvas(name, name, 0, 0, 1000, 1000);
      h2DUTMap[ii][jj] -> Draw("colz");
      cDUTMap -> Write();
      delete cDUTMap;
    }
  }

  /////////////////////
  // cleaning memory //
  /////////////////////
  cout << __PRETTY_FUNCTION__ << ": cleaning memory" << endl;
  for(unsigned int iSpill=0; iSpill<dataDUT.size(); iSpill++){
    delete dataDUT[iSpill];
  }
  dataDUT.clear();
  for(unsigned int iSpill=0; iSpill<dataTelescope.size(); iSpill++){
    delete dataTelescope[iSpill];
  }
  dataTelescope.clear();
  delete h1TimeDistanceDUT;
  delete h1TimeDistanceDUTSpill;
  delete cTimeDistanceDUT;
  delete h1TimeDistanceTelescope;
  delete h1TimeDistanceTelescopeSpill;
  delete cTimeDistanceTelescope;
  delete grTimelineDUT;
  delete grTimelineTelescope;
  for(unsigned int ii=0; ii<cfg -> _nDRS; ii++){
    for(unsigned int jj=0; jj<cfg -> _nCH[ii]; jj++){
      delete h2DUTMap[ii][jj];
    }
    delete[] h2DUTMap[ii];
  }
  delete[] h2DUTMap;
  delete h2TrackMap;
  delete cTrackMap;
  delete h2RoI;
  delete cRoI;
  delete treeTelescope;
  fileInTelescope -> Close();
  delete fileInTelescope;
  delete treeDUT;
  fileInDUT -> Close();
  delete fileInDUT;
  fileOut -> Close();
  delete fileOut;
  delete cfg;

  return 0;
}
