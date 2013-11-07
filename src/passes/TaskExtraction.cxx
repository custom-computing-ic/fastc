#include "TaskExtraction.hxx"
#include "../DfeTask.hxx"
#include "../Design.hxx"

NodeQuerySynthesizedAttributeType  findDFETasks(SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  SgPragmaDeclaration* pragma = isSgPragmaDeclaration(astNode);
  string taskPragma = "#pragma fast hw";
  if (pragma != NULL) {
    string name = pragma->get_pragma()->unparseToString();
    if (name.substr(0,taskPragma.size()) == taskPragma)
      returnNodeList.push_back(astNode);
  }

  return returnNodeList;
}


void TaskExtraction::runPass(Design* design) {
  SgProject* project = design->getProject();
  Rose_STL_Container<SgNode*> dfePragmas;
  dfePragmas = NodeQuery::queryNodeList(
                                        NodeQuery::querySubTree(project, V_SgPragmaDeclaration),
                                        &findDFETasks);

  foreach_ (SgNode* node, dfePragmas) {
    SgPragmaDeclaration *pragma = isSgPragmaDeclaration(node);
    if (pragma != NULL) {
      SgStatement* st = SageInterface::getNextStatement(pragma);

      // check if this a function call statement
      SgExprStatement *expr_st = isSgExprStatement(st);
      SgFunctionCallExp* fcall;
      if (expr_st != NULL &&
	  ((fcall = isSgFunctionCallExp(expr_st->get_expression())) != NULL)) {
	Kernel *k = design->getKernelMatchingFunctionCall(fcall);
	vector<string> dfe_args = getFunctionCallArgNames(fcall);
	DfeTask* task = new DfeTask(k->getName(), k, dfe_args);
	design->addDfeTask(task);
      }
    }
  }
}



string TaskExtraction::logPass() {
  return "DFE Task Extraction";
}
