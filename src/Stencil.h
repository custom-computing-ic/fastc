#ifndef _STENCIL_H_
#define _STENCIL_H_

#include <vector>
#include <string>

class Stencil
{
  int dim;
  std::vector<std::string> inputs, outputs, loopVars;

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

};


#endif // _STENCIL_H_ 
