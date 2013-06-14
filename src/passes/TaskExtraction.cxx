#include "TaskExtraction.hxx"

NodeQuerySynthesizedAttributeType  findDFETasks(SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  SgPragmaDeclaration* pragma = isSgPragmaDeclaration(astNode);
  string taskPragma = "#pragma fast kernel";
  if (pragma != NULL) {
    string name = pragma->get_pragma()->unparseToString();
    if (name.substr(0,taskPragma.size()).compare(taskPragma) == 0)
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
      //      cout << SageInterface::getNextStatement(pragma)->unparseToString() << endl;
    }
  }
}

string TaskExtraction::logPass() {
  return "DFE Task Extraction";
}
