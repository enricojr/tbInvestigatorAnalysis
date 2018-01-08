#include <iostream>
#include <fstream>
using namespace std;

#define NSAMPLES 1024

int makeConfigFile(const unsigned int run,
		   const char *path,
		   const double T00 = 200.,
		   const double T01 = 200.,
		   const double T02 = 200.,
		   const double T03 = 200.){

  double T0[5];
  T0[0] = T00;
  T0[1] = T01;
  T0[2] = T02;
  T0[3] = 0.;
  T0[4] = T03;
  
  char fileName[1000];
  sprintf(fileName, "%s/run_%06d.cfg", path, run);
  ofstream file(fileName);
  if(!file){
    cout << __PRETTY_FUNCTION__ << ": cannot open file " << fileName << endl;
    return 1;
  }

  const double linearRangeLeft = 150.;
  const double linearRangeRight = 150.;
  pair<double, double> linearRange[5];

  const double pulseRangeLeft = 200.;
  const double pulseRangeRight = 200.;
  pair<double, double> pulseRange[5];
  
  const double pulseT0Left = 100.;
  const double pulseT0Right = 100.;
  pair<double, double> pulseT0[5];
  
  const double pulseT0SelectionLeft = 125.;
  const double pulseT0SelectionRight = 50.;
  pair<double, double> pulseT0Selection[5];
  
  for(unsigned int i=0; i<5; i++){
    if(i==3) continue;
    linearRange[i].first = T0[i]-linearRangeLeft;
    linearRange[i].second = T0[i]+linearRangeRight;
    pulseRange[i].first = T0[i]-pulseRangeLeft;
    pulseRange[i].second = T0[i]+pulseRangeRight;
    pulseT0[i].first = T0[i]-pulseT0Left;
    pulseT0[i].second = T0[i]+pulseT0Right;
    pulseT0Selection[i].first = T0[i]-pulseT0SelectionLeft;
    pulseT0Selection[i].second = T0[i]+pulseT0SelectionRight;
  }

  const double clearanceLeft = 20.;
  const double clearanceRight = 20.;
  for(unsigned int i=0; i<5; i++){
    if(i==3) continue;
    if(linearRange[i].first < clearanceLeft) linearRange[i].first = clearanceLeft;
    if(linearRange[i].second > NSAMPLES-clearanceRight) linearRange[i].second = NSAMPLES-clearanceRight;
    if(pulseRange[i].first < clearanceLeft) pulseRange[i].first = clearanceLeft;
    if(pulseRange[i].second > NSAMPLES-clearanceRight) pulseRange[i].second = NSAMPLES-clearanceRight;
    if(pulseT0[i].first < clearanceLeft) pulseT0[i].first = clearanceLeft;
    if(pulseT0[i].second > NSAMPLES-clearanceRight) pulseT0[i].second = NSAMPLES-clearanceRight;
    if(pulseT0Selection[i].first < clearanceLeft) pulseT0Selection[i].first = clearanceLeft;
    if(pulseT0Selection[i].second > NSAMPLES-clearanceRight) pulseT0Selection[i].second = NSAMPLES-clearanceRight;
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
  for(unsigned int i=0; i<5; i++){
    file << linearRange[i].first << " " << linearRange[i].second << endl;
  }
  file << "# linear fit, parameters initialization (value, min, max): offset" << endl;
  file << "32768. 0. 65536." << endl;
  file << "# linear fit, parameters initialization (value, min, max): slope" << endl;
  file << "0. -100. 100." << endl;
  file << "# pulse fit, parameters initialization (min, max): range" << endl;
  for(unsigned int i=0; i<5; i++){
    file << pulseRange[i].first << " " << pulseRange[i].second << endl;
  }
  file << "# pulse fit, parameters initialization (value, min, max): offset (automatically initialized to waveform beginning)" << endl;
  file << "32768. 0. 65536." << endl;
  file << "# pulse fit, parameters initialization (value, min, max): slope" << endl;
  file << "0. -3. 5." << endl;
  file << "# pulse fit, parameters initialization (value, min, max): amplitude (automatically initialized to peak-to-peak)" << endl;
  file << "1000. 0. 65536." << endl;
  file << "# pulse fit, parameters initialization (value, min, max): T0" << endl;
  for(unsigned int i=0; i<5; i++){
    file << T0[i] << " " << pulseT0[i].first << " " << pulseT0[i].second << endl;
  }
  file << "# pulse fit, parameters initialization (value, min, max): rise time" << endl;
  file << "6. 1. 100." << endl;
  file << "# pulse fit, parameters initialization (value, min, max): decay" << endl;
  file << "0. -100. 0." << endl;
  file << "# threshold for the reduced chi2 of the linear fit" << endl;
  file << "1." << endl;
  file << "# threshold for the reduced chi2 of the pulse fit" << endl;
  file << "10." << endl;
  file << "# limits for pulse fit: T0" << endl;
  for(unsigned int i=0; i<5; i++){
    file << pulseT0Selection[i].first << " " << pulseT0Selection[i].second << endl;
  }
  file << "# limits for pulse fit: rise time" << endl;
  file << "1. 100." << endl;
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
