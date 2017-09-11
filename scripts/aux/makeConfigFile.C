#include <iostream>
#include <fstream>
using namespace std;

int makeConfigFile(const unsigned int run,
		   const double T00 = 200.,
		   const double T01 = 200.,
		   const double T02 = 200.,
		   const double T03 = 200.){

  char fileName[1000];
  sprintf(fileName, "../../../cfg/run_%06d.cfg", run);
  ofstream file(fileName);
  if(!file){
    cout << __PRETTY_FUNCTION__ << ": cannot open file " << fileName << endl;
    return 1;
  }

  file << "# number of DRSs" << endl;
  file << "2" << endl;
  file << "# DRS 0, # channels" << endl;
  file << "4" << endl;
  file << "# DRS 1, # channels" << endl;
  file << "1" << endl;
  file << "# reset DRS" << endl;
  file << "0" << endl;
  file << "# reset channel" << endl;
  file << "3" << endl;
  file << "# linear fit, parameters initialization (min, max): range" << endl;
  file << T00-150. << " " << T00+150. << endl;
  file << T01-150. << " " << T01+150. << endl;
  file << T02-150. << " " << T02+150. << endl;
  file << "0. 0." << endl;
  file << T03-150. << " " << T03+150. << endl;
  file << "# linear fit, parameters initialization (value, min, max): offset" << endl;
  file << "32768. 0. 65536." << endl;
  file << "# linear fit, parameters initialization (value, min, max): slope" << endl;
  file << "0. -100. 100." << endl;
  file << "# pulse fit, parameters initialization (min, max): range" << endl;
  file << T00-150. << " " << T00+150. << endl;
  file << T01-150. << " " << T01+150. << endl;
  file << T02-150. << " " << T02+150. << endl;
  file << "0. 0." << endl;
  file << T03-150. << " " << T03+150. << endl;
  file << "# pulse fit, parameters initialization (value, min, max): offset (automatically initialized to waveform beginning)" << endl;
  file << "32768. 0. 65536." << endl;
  file << "# pulse fit, parameters initialization (value, min, max): slope" << endl;
  file << "0. -3. 5." << endl;
  file << "# pulse fit, parameters initialization (value, min, max): amplitude (automatically initialized to peak-to-peak)" << endl;
  file << "1000. 0. 65536." << endl;
  file << "# pulse fit, parameters initialization (value, min, max): T0" << endl;
  file << T00 << " " << T00-150. << " " << T00+50. << endl;
  file << T01 << " " << T01-150. << " " << T01+50. << endl;
  file << T02 << " " << T02-150. << " " << T02+50. << endl;
  file << "0. 0. 0." << endl;
  file << T03 << " " << T03-150. << " " << T03+50. << endl;
  file << "# pulse fit, parameters initialization (value, min, max): rise time" << endl;
  file << "6. 1. 50." << endl;
  file << "# pulse fit, parameters initialization (value, min, max): decay" << endl;
  file << "0. -100. 0." << endl;
  file << "# threshold for the reduced chi2 of the linear fit" << endl;
  file << "1." << endl;
  file << "# threshold for the reduced chi2 of the pulse fit" << endl;
  file << "10." << endl;
  file << "# limits for pulse fit: T0" << endl;
  file << T00-125. << " " << T00+50. << endl;
  file << T01-125. << " " << T01+50. << endl;
  file << T02-125. << " " << T02+50. << endl;
  file << "0. 0." << endl;
  file << T03-125. << " " << T03+50. << endl;
  file << "# limits for pulse fit: rise time" << endl;
  file << "1. 20." << endl;
  file << "# time difference threshold for spill identification" << endl;
  file << "10." << endl;
  file << "# time difference threshold for inter-spill identification" << endl;
  file << "0.06" << endl;
  file << "# debug level" << endl;
  file << "0" << endl;
  
  file.close();

  cout << __PRETTY_FUNCTION__ << ": created file " << fileName << endl;
  
  return 0;
}
