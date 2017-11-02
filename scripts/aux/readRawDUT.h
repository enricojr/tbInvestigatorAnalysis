#ifndef READRAWDUT_H
#define READRAWDUT_H

#include "DRSDataClass.hh"
#include "../aux/progressBar.h"

#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TAxis.h>
#include <TGraph.h>
#include <TStyle.h>

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

bool goToEventHeader(ifstream &file,
		     const unsigned int debug){
  char _word[1] = {' '};
  unsigned char sentence[4] = {' ', ' ', ' ', ' '};
  unsigned int nWords = 0;
  while(true){
    file.read(_word, 1);
    if(file.eof()){
      cout << __PRETTY_FUNCTION__ << ": reached end of file" << endl;
      return true;
    }
    nWords++;
    sentence[0] = sentence[1];
    sentence[1] = sentence[2];
    sentence[2] = sentence[3];
    sentence[3] = static_cast<unsigned char>(_word[0]);
    if(sentence[0] == 'E' && sentence[1] == 'H' && sentence[2] == 'D' && sentence[3] == 'R'){
      //    if(sentence[2] == 'D' && sentence[3] == 'R'){
      if(debug >= 3) cout << __PRETTY_FUNCTION__ << ": nWords = " << nWords << endl;  
      if(debug >= 3) cout << __PRETTY_FUNCTION__ << ": event header: " << sentence << endl;
      return false;
    }
  }
  cout << __PRETTY_FUNCTION__ << ": reached end of file" << endl;
  return true;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

int readTwoBytes(ifstream &file,
		 bool swap,
		 const unsigned int debug){
  char _byte1[1] = {' '};
  char _byte2[1] = {' '};
  file.read(_byte1, 1);
  file.read(_byte2, 1);
  int byte1 = (int)static_cast<unsigned char>(_byte1[0]);
  int byte2 = (int)static_cast<unsigned char>(_byte2[0]);
  int decimal = -1;
  if(swap) decimal = byte1*256+byte2;
  else decimal = byte2*256+byte1;
  if(debug >= 4){
    cout << __PRETTY_FUNCTION__ << ": byte1 = " << byte1 << "\tbyte2 = " << byte2 << "\tdecimal = " << decimal << endl;
  }
  return decimal;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

int readFourBytes(ifstream &file,
		  bool swap,
		  const unsigned int debug){
  char _byte1[1] = {' '};
  char _byte2[1] = {' '};
  char _byte3[1] = {' '};
  char _byte4[1] = {' '};
  file.read(_byte1, 1);
  file.read(_byte2, 1);
  file.read(_byte3, 1);
  file.read(_byte4, 1);
  int byte1 = (int)static_cast<unsigned char>(_byte1[0]);
  int byte2 = (int)static_cast<unsigned char>(_byte2[0]);
  int byte3 = (int)static_cast<unsigned char>(_byte3[0]);
  int byte4 = (int)static_cast<unsigned char>(_byte4[0]);
  int decimal = -1;
  if(swap) decimal = byte2*256+byte1+byte4*16777216+byte3*65536;
  else decimal = byte4*256+byte3+byte1*65536+byte2*16777216;
  if(debug >= 4){
    cout << __PRETTY_FUNCTION__ << ": byte1 = " << byte1 << "\tbyte2 = " << byte2 << "\tbyte3 = " << byte3 << "\tbyte4 = " << byte4 << "\tdecimal = " << decimal << endl;
  }
  return decimal;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void readHeaderInfo(ifstream &file,
		    DRSDataClass *DRSData,
		    const unsigned int debug){
  DRSData -> eventSerialNumber = readFourBytes(file, true, debug);
  if(debug >= 3) cout << __PRETTY_FUNCTION__ << ": eventSerialNumber = " << DRSData -> eventSerialNumber << endl;
  DRSData -> year = readTwoBytes(file, false, debug);
  if(debug >= 3) cout << __PRETTY_FUNCTION__ << ": year = " << DRSData -> year << endl;
  DRSData -> month = readTwoBytes(file, false, debug);
  if(debug >= 3) cout << __PRETTY_FUNCTION__ << ": month = " << DRSData -> month << endl;
  DRSData -> day = readTwoBytes(file, false, debug);
  if(debug >= 3) cout << __PRETTY_FUNCTION__ << ": day = " << DRSData -> day << endl;
  DRSData -> hour = readTwoBytes(file, false, debug);
  if(debug >= 3) cout << __PRETTY_FUNCTION__ << ": hour = " << DRSData -> hour << endl;
  DRSData -> minute = readTwoBytes(file, false, debug);
  if(debug >= 3) cout << __PRETTY_FUNCTION__ << ": minute = " << DRSData -> minute << endl;
  DRSData -> second = readTwoBytes(file, false, debug);
  if(debug >= 3) cout << __PRETTY_FUNCTION__ << ": second = " << DRSData -> second << endl;
  DRSData -> millisecond = readTwoBytes(file, false, debug);
  if(debug >= 3) cout << __PRETTY_FUNCTION__ << ": millisecond = " << DRSData -> millisecond << endl;
  DRSData -> rangeCenter = readTwoBytes(file, false, debug);
  if(debug >= 3) cout << __PRETTY_FUNCTION__ << ": rangeCenter = " << DRSData -> rangeCenter << endl;
  return ;
}

#endif
