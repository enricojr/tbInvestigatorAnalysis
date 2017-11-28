/*
  The function loadRunData does exactly what you think it does.
  The run data input file is generated as follows:
  1) go to https://docs.google.com/spreadsheets/d/1aqYTKp-7KoSPRIHlO4xX8QjAHAG4_QRSNxxN7YON6nc/edit#gid=1667431261
  2) choose the tab Workflow
  3) File -> Download As -> Tab-separated values
  4) Open the file that has been generated and delete all the occurrences of the character "%"
 */

#ifndef LOADRUNDATA_HH
#define LOADRUNDATA_HH

#define STRINGSIZE 200

typedef struct runData{
  string wafer;
  unsigned int matrix;
  string type;
  unsigned int runNumber;
  unsigned int nEventsTelescope;
  unsigned int nEventsDUT;
  string convertedTelescope;
  string convertedDUT;
  string tracked;
  double syncedFraction;
  unsigned int nEventsSynced;
  string movementCorrected;
  void print() const{
    cout << __PRETTY_FUNCTION__ << ": wafer              = " << wafer << endl;
    cout << __PRETTY_FUNCTION__ << ": matrix             = " << matrix << endl;
    cout << __PRETTY_FUNCTION__ << ": type               = " << type << endl;
    cout << __PRETTY_FUNCTION__ << ": runNumber          = " << runNumber << endl;
    cout << __PRETTY_FUNCTION__ << ": nEventsTelescope   = " << nEventsTelescope << endl;
    cout << __PRETTY_FUNCTION__ << ": nEventsDUT         = " << nEventsDUT << endl;
    cout << __PRETTY_FUNCTION__ << ": convertedTelescope = " << convertedTelescope << endl;
    cout << __PRETTY_FUNCTION__ << ": convertedDUT       = " << convertedDUT << endl;
    cout << __PRETTY_FUNCTION__ << ": tracked            = " << tracked << endl;
    cout << __PRETTY_FUNCTION__ << ": syncedFraction     = " << syncedFraction << endl;
    cout << __PRETTY_FUNCTION__ << ": nEventsSynced      = " << nEventsSynced << endl;
    cout << __PRETTY_FUNCTION__ << ": movementCorrected  = " << movementCorrected << endl;
    return ;
  };
} runData_t;

int loadRunData(const char *fileName,
		vector<runData_t> &runData){

  ifstream file(fileName);
  if(!file){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileName << endl;
    return 1;
  }

  string line = "";
  const char *format = "%s\tM%d\t%s\t%d\t%d\t%d\t%s\t%s\t%s\t%lf\t%d\t%s";
  getline(file, line); // skipping header
  while(getline(file, line)){
    runData_t newRun;
    char wafer[STRINGSIZE];
    unsigned int matrix;
    char type[STRINGSIZE];
    unsigned int runNumber;
    unsigned int nEventsTelescope;
    unsigned int nEventsDUT;
    char convertedTelescope[STRINGSIZE];
    char convertedDUT[STRINGSIZE];
    char tracked[STRINGSIZE];
    double syncedFraction;
    unsigned int nEventsSynced;
    char movementCorrected[STRINGSIZE];
    sscanf(line.c_str(), format,
     	   wafer,
     	   &matrix,
     	   type,
     	   &runNumber,
     	   &nEventsTelescope,
     	   &nEventsDUT,
     	   convertedTelescope,
    	   convertedDUT,
     	   tracked,
     	   &syncedFraction,
     	   &nEventsSynced,
     	   movementCorrected);
    newRun.wafer = wafer;
    newRun.matrix = matrix;
    newRun.type = type;
    newRun.runNumber = runNumber;
    newRun.nEventsTelescope = nEventsTelescope;
    newRun.nEventsDUT = nEventsDUT;
    newRun.convertedTelescope = convertedTelescope;
    newRun.convertedDUT = convertedDUT;
    newRun.tracked = tracked;
    newRun.syncedFraction = syncedFraction;
    newRun.nEventsSynced = nEventsSynced;
    newRun.movementCorrected = movementCorrected;    
    runData.push_back(newRun);
  }
  
  file.close();
  
  return 0;
}

#endif
