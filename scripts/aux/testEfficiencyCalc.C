#include "efficiencyCalc.hh"

int testEfficiencyCalc(){

  const double numVal = 181.;
  const double numErrInf = 3.6;
  const double numErrSup = 6.11;
  const double denVal = 183.;
  const double denErrInf = 3.7;
  const double denErrSup = 6.12;
  const unsigned int nBins = 10000;
  
  pointClass *num = new pointClass(numVal, numErrInf, numErrSup);
  pointClass *den = new pointClass(denVal, denErrInf, denErrSup);
  pointClass *eff = efficiencyCalc(num, den, nBins);

  showResult(num, den, eff);

  //  delete num;
  //  delete den;
  //  delete eff;
  return 0;
}
