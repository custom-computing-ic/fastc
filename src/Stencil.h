#ifndef _STENCIL_H_
#define _STENCIL_H_

#include "StencilOffset.h"

#include <vector>
#include <string>


class Stencil
{
  int dim;
  std::vector<std::string> inputs, outputs, loopVars;
  std::string convolutionSource, convolutionDestination;
  std::vector<StencilOffset*> offsets;

public:
  Stencil(int dim, 
	  std::vector<std::string> inputs, 
	  std::vector<std::string> outputs ) { 
    this->dim = dim;
    this->inputs = inputs;
    this->outputs = outputs;
  }

  int getDimension() { return dim; }

  std::vector<std::string> getInputs() { return inputs; }
  std::vector<std::string> getOutputs() { return outputs; }
  void setLoopVariables(std::vector<std::string> loopVars) { 
    this->loopVars = loopVars; 
  }
  std::vector<std::string> getLoopVariables() { return loopVars; }
  void setSource(std::string source) { convolutionSource = source; }
  std::string getSource() { return convolutionSource; }

  void setDestination(std::string dest) { convolutionDestination = dest; }
  std::string getDestination() { return convolutionDestination; }

  void setOffsets(std::vector<StencilOffset*> offsets) { this->offsets = offsets; }
  std::vector<StencilOffset*> getOffsets() { return offsets; }
};


#endif // _STENCIL_H_ 
