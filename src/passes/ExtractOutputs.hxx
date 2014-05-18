#ifndef EXTRACT_OUTPUTS_HXX
#define EXTRACT_OUTPUTS_HXX

#include "../precompiled.hxx"
#include "../DataFlowGraph/OutputNode.hxx"

class ExtractOutputs : public AstSimpleProcessing
{

  list<OutputNode*> kernelOutputs;

public:
  ExtractOutputs(list<OutputNode*> kernelOutputs);

  virtual void visit(SgNode *node);
};
#endif
