#ifndef _STENCIL_H_
#define _STENCIL_H_

#include "precompiled.hxx"
#include <vector>
#include <string>

class StencilOffset;

class Stencil
{
  int dim;
  std::vector<std::string> inputs, outputs, loopVars, loopIncs, lowerBounds, upperBounds;
  std::string convolutionSource, convolutionDestination;
  std::vector<StencilOffset*> offsets;
  SgStatement* updateStatement;

  int varIndex(std::string loopVar);

 public:
  Stencil(std::vector<std::string> inputs,
          std::vector<std::string> outputs ) {
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

  void setOffsets(std::vector<StencilOffset*> offsets) {
    this->offsets = offsets;
  }
  std::vector<StencilOffset*> getOffsets() { return offsets; }

  void setUpdateStatement(SgStatement* statement) {
    this->updateStatement = statement;
  }

  /** Get the increment for the loop corresponding to 'loopVar' */
  std::string getLoopIncrement(std::string loopVar);
  std::vector<std::string> getLoopIncrements() { return loopIncs; }
  void setLoopIncrements(std::vector<std::string> increments) {
    this->loopIncs = increments; }

  std::vector<std::string> getLowerBounds() {
    return lowerBounds;
  }
  std::vector<std::string> getUpperBounds() {
    return upperBounds;
  }

  std::string getLowerBound(std::string loopVar);
  std::string getUpperBound(std::string loopVar);

  void computeLoopProperties(SgStatement* st);

  SgStatement* getUpdateStatement() { return updateStatement; }

  /** returns the dimension of the array we are applying this stencil
      to these are extracted from the offfset expressions used to
      access the stencil elements (e.g. i1 + i2 * n1 + i3 * n1 * n2)
      and assume that the last dimension of the stencil (that does not
      occur in the offset expression) is equal to the one before it.
      Assumes these offests are known at compile time (hence the 'int' return values)
  */
  std::vector<int> getArrayDimensions();

  void print();
};


#endif // _STENCIL_H_
