#ifndef GENERATEANALYSISOVERVIEW_H_
#define GENERATEANALYSISOVERVIEW_H_

#include "../precompiled.hxx"
#include "../Design.hxx"
#include "Pass.hxx"

#include <string>

#include <boost/filesystem.hpp>

#include <fstream>

#define TD "<TD>"
#define ETD "</TD>"
#define CTD "</TD><TD>"
#define TABLE "<TABLE>"
#define ETABLE "</TABLE>"
#define TR "<TR>"
#define ETR "</TR>"

class GenerateAnalysisOverview : public Pass {

  static std::string analysis_overview;
  static std::string colors[3];

 public:
  GenerateAnalysisOverview() {}

  void runPass(Design* design);

  std::string logPass() { return "Generating Analysis Overview"; }

};

#endif // GENERATEANALYSISOVERVIEW_H_


