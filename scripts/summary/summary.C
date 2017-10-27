#include "summary.hh"

int summary(const unsigned int runNumber,
	    const char *fileNameDUTConversion,
	    const char *fileNameTelescopeConversion,
	    const char *fileNameTelescopeAlignment,
	    const char *fileNameTelescopeTracking,
	    const char *fileNameSynchronization,
	    const char *fileNamePositionCorrection,
	    const char *fileNameEfficiency,
	    const char *fileNameLatex){

  /////////////////////
  // opening Latex file
  /////////////////////
  cout << __PRETTY_FUNCTION__ << ": opening file " << fileNameLatex << endl;
  ofstream fileLatex(fileNameLatex);
  if(!fileLatex){
    cout << __PRETTY_FUNCTION__ << ": ERROR!!! - cannot open file " << fileNameLatex << endl;
    return 1;
  }

  //////////////////////////
  // initializing Latex file
  //////////////////////////
  cout << __PRETTY_FUNCTION__ << ": initializing Latex file " << endl;
  fileLatex << "\\documentclass[a4paper,10pt,titlepage,openright ]{book}\n" << endl;
  fileLatex << "\\usepackage{verbatim} % to use multiline comments: \begin{comment}\n" << endl;
  fileLatex << "\\usepackage{graphicx} % to use \includegraphics\n" << endl;
  fileLatex << "\\usepackage{caption} % to use subfigures\n" << endl;
  fileLatex << "\\usepackage{subcaption}\usepackage{amsmath} % to use mathematics\n" << endl;
  fileLatex << "\\usepackage{wrapfig} % to use figures embedded in the text\n" << endl;
  fileLatex << "\\usepackage{hyperref}\n" << endl;
  fileLatex << "\\newcommand{\HRule}{\rule{\linewidth}{0.5mm}} % for title page\n" << endl;
  fileLatex << "\\usepackage{enumerate}\n" << endl;
  fileLatex << "\\usepackage{xfrac} % to use sfrac, slanted fractions\n" << endl;
  fileLatex << "\\usepackage{fancyhdr} % to enable fancy header style\n" << endl;
  fileLatex << "\\usepackage[margin=0.5in]{geometry}\n" << endl;
  fileLatex << "\\usepackage{array,multirow} % for tables\n" << endl;
  fileLatex << "\\usepackage[version=3]{mhchem} % for chemical formulas\n" << endl;
  fileLatex << "\\usepackage{textcomp} % to use angular brackets in text\n" << endl;
  fileLatex << "\\usepackage{xcolor}\n" << endl;
  fileLatex << "\\usepackage{wrapfig}\n" << endl;
  fileLatex << "\\usepackage{enumitem} % to resume counters in enumerate\n" << endl;
  fileLatex << "\\usepackage{upgreek}\n" << endl;
  fileLatex << "\\usepackage{float}\n" << endl;
  fileLatex << "\\usepackage{listings} % to write C++ code\n" << endl;
  fileLatex << "\\usepackage{slashed}\n" << endl;
  fileLatex << "\\usepackage{mathtools} % to use def above equal sign\n" << endl;
  fileLatex << "\\usepackage{soul} % to underline text and still have new lines\n" << endl;
  fileLatex << "\\usepackage{xcolor} % to color-underline in equations\n" << endl;
  fileLatex << "\\usepackage[makeroom]{cancel} % to cross-out terms in formulas\n" << endl;
  fileLatex << "\\begin{document}\n" << endl;

  ////////////////////////
  // finalizing Latex file
  ////////////////////////
  cout << __PRETTY_FUNCTION__ << ": finalizing Latex file" << endl;
  fileLatex << "\\end{document}\n" << endl;
  
  /////////////////////
  // closing Latex file
  /////////////////////
  fileLatex.close();
  
  return 0;
}
