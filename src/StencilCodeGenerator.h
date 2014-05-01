#ifndef STENCILCODEGENERATOR_H_
#define STENCILCODEGENERATOR_H_

#include "Kernel.hxx"

#include <string>

class StencilCodeGenerator {

  Kernel* kernel;
  std::string source;

  void generateInputs();
  void generateBoundaryCounters();
  void generateCache();
  void generateDataPath();
  void generateOutputs();
  void addComment(string);
  void findSharedInputs();
  void generateInputMatching();

public:
  StencilCodeGenerator(Kernel* kernel) {
    this->kernel = kernel;
    source = "";
  }
  std::string generateStencilCode();


};

#endif
