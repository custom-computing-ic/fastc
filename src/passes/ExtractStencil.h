#ifndef _EXTRACTSTENCIL_H_
#define _EXTRACTSTENCIL_H_

#include "Pass.hxx"
#include "../StencilOffset.h"

#include <vector>
#include <string>

class ExtractStencil : public Pass {

  int getStencilDimension(SgStatement *);
  int getStencilShape(SgStatement *);
  std::vector<std::string> getLoopVariables(SgStatement *);

  SgStatement* getStencilUpdateStatement(SgStatement *);
  SgAssignOp* getStencilUpdateAssignment(SgStatement *st);

  std::vector<std::string> getStencilInputs(SgStatement *st);
  std::vector<std::string> getStencilOutputs(SgStatement *st);
  std::vector<StencilOffset*> getStencilOffsets(SgStatement *st, 
								 Stencil* stencil);
  std::vector<StencilOffset*> getOffsetsByDimension(std::vector<SgExpression*>,
							       Stencil* );
  std::string getDimensionForOffset(SgExpression* expr, std::vector<std::string> loopVars);


  StencilOffset* extractSingleOffset(SgExpression* e, Stencil* stencil);

public:
  ExtractStencil() {}

  
  void runPass(Design* design);
  std::string logPass();
};


#endif // _EXTRACTSTENCIL_H_
