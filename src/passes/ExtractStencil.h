#ifndef _EXTRACTSTENCIL_H_
#define _EXTRACTSTENCIL_H_

#include "Pass.hxx"

class ExtractStencil : public Pass {

  int getStencilDimension(SgStatement *);
  int getStencilShape(SgStatement *);
  vector<string> getLoopVariables(SgStatement *);

  SgStatement* getStencilUpdateStatement(SgStatement *);
  SgStatement* getStencilUpdateAssignment(SgStatement *st);

  vector<string> getStencilInputs(SgStatement *st);
  vector<string> getStencilOutputs(SgStatement *st);


public:
  ExtractStencil() {}

  
  void runPass(Design* design);
  string logPass();
};


#endif // _EXTRACTSTENCIL_H_
