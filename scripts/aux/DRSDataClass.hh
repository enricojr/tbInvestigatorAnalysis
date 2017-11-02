#ifndef DRSDATACLASS_HH
#define DRSDATACLASS_HH

#define NDRSSAMPLES 1024

#include <iostream>
#include <fstream>
using namespace std;

class DRSDataClass{
public:
  DRSDataClass(){
    eventSerialNumber = -1;
    year = -1;
    month = -1;
    day = -1;
    hour = -1;
    minute = -1;
    second = -1;
    millisecond = -1;
    rangeCenter =  -1;
    boardNumber = -1;
    triggerCell = -1;
    scaler = -1;
    channel = -1;
    averageStart = -1.;
    averageEnd = -1.;
  };
  ~DRSDataClass(){};
  void print(){
    cout << __PRETTY_FUNCTION__ << ": event serial number = " << eventSerialNumber << endl;
    cout << __PRETTY_FUNCTION__ << ": year = " << year << endl;
    cout << __PRETTY_FUNCTION__ << ": month = " << month << endl;
    cout << __PRETTY_FUNCTION__ << ": day = " << day << endl;
    cout << __PRETTY_FUNCTION__ << ": hour = " << hour << endl;
    cout << __PRETTY_FUNCTION__ << ": minute = " << minute << endl;
    cout << __PRETTY_FUNCTION__ << ": second = " << second << endl;
    cout << __PRETTY_FUNCTION__ << ": millisecond = " << millisecond << endl;
    cout << __PRETTY_FUNCTION__ << ": range center = " << rangeCenter << endl;
    cout << __PRETTY_FUNCTION__ << ": board number = " << boardNumber << endl;
    cout << __PRETTY_FUNCTION__ << ": trigger cell = " << triggerCell << endl;
    cout << __PRETTY_FUNCTION__ << ": scaler = " << scaler << endl;
    cout << __PRETTY_FUNCTION__ << ": channel = " << channel << endl;
    cout << __PRETTY_FUNCTION__ << ": average start = " << averageStart << endl;
    cout << __PRETTY_FUNCTION__ << ": average end = " << averageEnd << endl;
  };
  unsigned int computeTimestamp(){
    //    return millisecond * + 1000 * ( second + 60 * ( minute + 60 * ( hour + day * 24 ) ) );
    return millisecond + 1000. * second + 1000. * 60. * minute + 1000. * 60. * 60. * hour + 1000. * 60. * 60. * 24. * day;
  };
  int eventSerialNumber;
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
  int millisecond;
  int rangeCenter;
  int boardNumber;
  int triggerCell;
  int scaler;
  int channel;
  double averageStart;
  double averageEnd;
};

#endif
