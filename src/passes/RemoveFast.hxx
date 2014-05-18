#ifndef REMOVE_FAST_HXX
#define REMOVE_FAST_HXX

#include "Pass.hxx"

NodeQuerySynthesizedAttributeType  findKernels(SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(astNode);
  if (funcDecl != NULL) {
    string functionName = funcDecl->get_name().str();
    if (functionName.substr(0,7) == "kernel_")
      returnNodeList.push_back(astNode);
  }

  return returnNodeList;
}

/** Pass to remove FAST dataflow kernels **/
class RemoveFast : public Pass
{
public:

  RemoveFast(const Compiler& compiler) : super(compiler) {
  }

  void runPass(Design* design) {
    using namespace NodeQuery;
    SgProject* project = design->getProject();

    Rose_STL_Container<SgNode*> kernels;
    kernels = NodeQuery::queryNodeList(
                                       NodeQuery::querySubTree (project, V_SgFunctionDeclaration),
                                       &findKernels);
    foreach_ (SgNode* node, kernels)
      SageInterface::removeStatement((SgFunctionDeclaration*)node);

  }

  string logPass() {
    return "Removing fast dataflow kernels";
  }
};

#endif
