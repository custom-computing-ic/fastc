#ifndef _EXTRACTDESIGNCONSTANTS_HXX_
#define _EXTRACTDESIGNCONSTANTS_HXX_

#include "Pass.hxx"

NodeQuerySynthesizedAttributeType  findConstants(SgNode * astNode)
{
  ROSE_ASSERT (astNode != 0);
  NodeQuerySynthesizedAttributeType returnNodeList;

  SgVarRefExp* name = isSgVarRefExp(astNode);
  if (name != NULL) {
    if (SageInterface::isConstType(name->get_type()))
      returnNodeList.push_back(astNode);
  }

  return returnNodeList;
}

class ExtractDesignConstants : public Pass
{
public:

  ExtractDesignConstants(const Compiler& compiler) : super(compiler) {}

  void runPass(Design* design) {
    using namespace NodeQuery;
    set<SgInitializedName*> roVariables;
    foreach_(Kernel* kernel, design->getKernels()) {
      Rose_STL_Container<SgNode*> defs;
      defs = queryNodeList(
                           querySubTree(kernel->getDeclaration()->get_definition(), V_SgVarRefExp),
                           &findConstants);

      cout << "\t Kernel: " << kernel->getName() << endl;

      map<string, pair<string, string> > constant_type_value;
      foreach_(SgNode* n, defs) {
        SgVarRefExp *e = isSgVarRefExp(n);
        if (e != NULL) {
          SgVariableSymbol* sym = e->get_symbol();
          string name = sym->get_name();
          SgInitializer *init = sym->get_declaration()->get_initializer();
          string value = init->unparseToString();
          string type = e->get_type()->stripTypedefsAndModifiers()->unparseToString();
          kernel->addDesignConstant(name, value, type);
          constant_type_value[name] = make_pair(value, type);
        }
      }

      map<string, pair<string, string> >::iterator it;
      for (it = constant_type_value.begin(); it != constant_type_value.end(); it++) {
        cout << "\t\t " << it->first << " = " << it->second.first;
        cout << ", type: " << it->second.second << endl;
      }
    }
  }

  string logPass() {
    return "Extract Design Constants";
  }

};


#endif /* _EXTRACTDESIGNCONSTANTS_HXX_ */
