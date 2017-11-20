#include "summary.hh"

int summary(const unsigned int runNumber,
	    const char *fileNameCfg,
	    const char *fileNameDUTConversion,
	    const char *fileNameTelescopeConversion,
	    const char *fileNameTelescopeAlignment,
	    const char *fileNameTelescopeTracking,
	    const char *fileNameSynchronization,
	    const char *fileNamePositionCorrection,
	    const char *fileNameEfficiency,
	    const char *fileNameLatex,
	    const string plotPath){

  gStyle->SetNumberContours(255);
  gStyle -> SetPalette(55);
  gStyle -> SetOptStat(0);
  gStyle -> SetOptTitle(0);
  gStyle -> SetOptFit(0);

  /////////////////////////////
  // loading configuration file
  /////////////////////////////
  cout << __PRETTY_FUNCTION__ << ": loading configuration file " << fileNameCfg << endl;
  configClass *cfg = new configClass();
  if(cfg -> load(fileNameCfg)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot load configuration" << endl;
    return 1;
  }
  cfg -> print();

  /////////////////////
  // opening Latex file
  /////////////////////
  cout << __PRETTY_FUNCTION__ << ": opening file " << fileNameLatex << endl;
  ofstream fileLatex(fileNameLatex);
  if(!fileLatex){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open Latex file " << endl;
    return 1;
  }

  //////////////////////////
  // initializing Latex file
  //////////////////////////
  cout << __PRETTY_FUNCTION__ << ": initializing Latex file " << endl;
  fileLatex << "\\documentclass[a4paper,10pt,titlepage,openright ]{article}" << endl;
  fileLatex << "\\usepackage{verbatim} % to use multiline comments: \\begin{comment}" << endl;
  fileLatex << "\\usepackage{graphicx} % to use \\includegraphics" << endl;
  fileLatex << "\\usepackage{caption} % to use subfigures" << endl;
  fileLatex << "%\\usepackage{subcaption}\\usepackage{amsmath} % to use mathematics" << endl;
  fileLatex << "\\usepackage{wrapfig} % to use figures embedded in the text" << endl;
  fileLatex << "\\usepackage{hyperref}" << endl;
  fileLatex << "\\newcommand{\\HRule}{\\rule{\\linewidth}{0.5mm}} % for title page" << endl;
  fileLatex << "\\usepackage{enumerate}" << endl;
  fileLatex << "%\\usepackage{xfrac} % to use sfrac, slanted fractions" << endl;
  fileLatex << "\\usepackage{fancyhdr} % to enable fancy header style" << endl;
  fileLatex << "\\usepackage[margin=0.5in]{geometry}" << endl;
  fileLatex << "\\usepackage{array,multirow} % for tables" << endl;
  fileLatex << "%\\usepackage[version=3]{mhchem} % for chemical formulas" << endl;
  fileLatex << "\\usepackage{textcomp} % to use angular brackets in text" << endl;
  fileLatex << "\\usepackage{xcolor}" << endl;
  fileLatex << "\\usepackage{wrapfig}" << endl;
  fileLatex << "\\usepackage{enumitem} % to resume counters in enumerate" << endl;
  fileLatex << "\\usepackage{upgreek}" << endl;
  fileLatex << "\\usepackage{float}" << endl;
  fileLatex << "\\usepackage{listings} % to write C++ code" << endl;
  fileLatex << "\\usepackage{slashed}" << endl;
  fileLatex << "\\usepackage{mathtools} % to use def above equal sign" << endl;
  fileLatex << "\\usepackage{soul} % to underline text and still have new lines" << endl;
  fileLatex << "\\usepackage{xcolor} % to color-underline in equations" << endl;
  fileLatex << "\\usepackage[makeroom]{cancel} % to cross-out terms in formulas" << endl;
  fileLatex << "\\begin{document}" << endl;

  /////////////
  // title page
  /////////////
  cout << __PRETTY_FUNCTION__ << ": writing title page " << endl;
  char title[1000];
  sprintf(title, "RUN %06d", runNumber);
  fileLatex << "\\begin{titlepage}" << endl;
  fileLatex << "\\centering" << endl;
  fileLatex << "{\\scshape\\LARGE CERN EP-ADE-ID \\par}" << endl;
  fileLatex << "\\vspace{1cm}" << endl;
  fileLatex << "{\\scshape\\Large TJ-Investigator test beam data report\\par}" << endl;
  fileLatex << "\\vspace{1.5cm}" << endl;
  fileLatex << "{\\huge\\bfseries " << title << " \\par}" << endl;
  fileLatex << "\\vspace{2cm}" << endl;
  fileLatex << "{\\Large\\itshape Dataset CERN SPS, May-October 2017\\par}" << endl;
  fileLatex << "\\vfill" << endl;
  fileLatex << "edited by\\par" << endl;
  fileLatex << "Enrico Jr. \\textsc{Schioppa}\\par" << endl;
  fileLatex << "Florian \\textsc{Dachs}\\par" << endl;
  fileLatex << "Maria \\textsc{Moreno Llacer}\\par" << endl;
  fileLatex << "Simone \\textsc{Monzani}" << endl;
  fileLatex << "\\vfill" << endl;
  fileLatex << "{\\large \\today\\par}" << endl;
  fileLatex << "\\title{" << title << "}" << endl;
  fileLatex << "\\end{titlepage}" << endl;

  ///////////
  // overview
  ///////////
  cout << __PRETTY_FUNCTION__ << ": writing overview " << endl;
  fileLatex << "\\section{Overview}" << endl;
  fileLatex << "{\\bf Run number                               :} " << runNumber << "\\\\" << endl;
  int nDUTHits = getNumberOfDUTHits(fileNameDUTConversion);
  if(nDUTHits < 0){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot get number of DUT hits" << endl;
    return 1;
  }
  fileLatex << "{\\bf Number of DUT hits (before any selection):} " << nDUTHits << "\\\\" << endl;
  int nTelescopeHits = getNumberOfTelescopeHits(fileNameTelescopeConversion);
  if(nTelescopeHits < 0){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot get number of Telescope hits" << endl;
    return 1;
  }
  fileLatex << "{\\bf Number of Telescope hits (before any selection):} " << nTelescopeHits << "\\\\" << endl;
  fileLatex << "{\\bf Exceeding number of DUT hits (before any selection):} " << -nTelescopeHits+nDUTHits << "\\\\" << endl;
  fileLatex << "{\\bf Exceeding fraction of DUT hits (before any selection):} " << 100.*(-nTelescopeHits+nDUTHits)/nDUTHits << "\\% \\\\" << endl;
  fileLatex << "{\\bf Number of selected DUT hits:} " << "to be added" << "\\\\" << endl;
  fileLatex << "{\\bf Fraction of selected DUT hits:} " << "to be added" << "\\\\" << endl;
  fileLatex << "{\\bf Synchronization fraction:} " << "to be added" << "\\\\" << endl;
  fileLatex << "{\\bf Input file for DUT conversion            :} \\\\ " << escapeSomeChars(fileNameDUTConversion) << "\\\\" << endl;
  fileLatex << "{\\bf Input file for telescope conversion      :} \\\\ " << escapeSomeChars(fileNameTelescopeConversion) << "\\\\" << endl;
  //  fileLatex << "{\\bf Input file for telescope alignment       :} \\\\ " << escapeSomeChars(fileNameTelescopeAlignment) << "\\\\" << endl;
  fileLatex << "{\\bf Input file for telescope tracking        :} \\\\ " << escapeSomeChars(fileNameTelescopeTracking) << "\\\\" << endl;
  fileLatex << "{\\bf Input file for synchronization           :} \\\\ " << escapeSomeChars(fileNameSynchronization) << "\\\\" << endl;
  fileLatex << "{\\bf Input file for position correction       :} \\\\ " << escapeSomeChars(fileNamePositionCorrection) << "\\\\" << endl;
  fileLatex << "{\\bf Input file for efficiency                :} \\\\ " << escapeSomeChars(fileNameEfficiency) << "\\\\" << endl;

  /////////////////
  // DUT conversion
  /////////////////
  cout << __PRETTY_FUNCTION__ << ": writing DUT conversion " << endl;
  if(writeDUTConversion(cfg, fileLatex, fileNameDUTConversion, plotPath)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot write DUT conversion" << endl;
    return 0;
  }

  ///////////////////////
  // Telescope conversion
  ///////////////////////
  cout << __PRETTY_FUNCTION__ << ": writing telescope conversion " << endl;
  if(writeTelescopeConversion(fileLatex, fileNameTelescopeConversion, plotPath)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot write Telescope conversion" << endl;
    return 0;
  }

  //////////////////////
  // Telescope alignment
  //////////////////////
  //  cout << __PRETTY_FUNCTION__ << ": writing telescope alignment " << endl;
  //  if(writeTelescopeAlignment(fileLatex, fileNameTelescopeAlignment, plotPath)){
  //    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot write Telescope alignment" << endl;
  //    return 0;
  //  }

  /////////////////////
  // Telescope tracking
  /////////////////////
  //  cout << __PRETTY_FUNCTION__ << ": writing telescope tracking " << endl;
  //  if(writeTelescopeTracking(fileLatex, fileNameTelescopeTracking, plotPath)){
  //    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot write Telescope tracking" << endl;
  //    return 0;
  //  }

  //////////////////
  // Synchronization
  //////////////////
  cout << __PRETTY_FUNCTION__ << ": writing synchronization " << endl;
  if(writeSynchronization(fileLatex, fileNameSynchronization, plotPath)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot write synchronization" << endl;
    return 0;
  }

  //////////////////////
  // Position correction
  //////////////////////
  cout << __PRETTY_FUNCTION__ << ": writing position correction " << endl;
  if(writePositionCorrection(fileLatex, fileNamePositionCorrection, plotPath)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot write position correction" << endl;
    return 0;
  }

  //////////////////
  // Efficiency
  //////////////////
  cout << __PRETTY_FUNCTION__ << ": writing efficiency " << endl;
  if(writeEfficiency(fileLatex, fileNameEfficiency, plotPath)){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot write efficiency" << endl;
    return 0;
  }

  ////////////////////////
  // finalizing Latex file
  ////////////////////////
  cout << __PRETTY_FUNCTION__ << ": finalizing Latex file" << endl;
  fileLatex << "\\end{document}" << endl;
  
  /////////////////////
  // closing Latex file
  /////////////////////
  cout << __PRETTY_FUNCTION__ << ": closing Latex file" << endl;
  fileLatex.close();

  /////////
  // ending
  /////////
  cout << __PRETTY_FUNCTION__ << ": ending" << endl;
  delete cfg;
  return 0;
}
