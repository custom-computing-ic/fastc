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
  ExtractDesignConstants() {}

  void runPass(Design* design) {
    using namespace NodeQuery;
    set<SgInitializedName*> roVariables;
    foreach_(Kernel* kernel, design->getKernels()) {
      Rose_STL_Container<SgNode*> defs;
      defs = queryNodeList(
                           querySubTree(kernel->getDeclaration()->get_definition(), V_SgVarRefExp),
                           &findConstants);

      cout << "\t Kernel: " << kernel->getName() << endl;
      foreach_(SgNode* n, defs) {
        SgVarRefExp *e = isSgVarRefExp(n);
        if (e != NULL) {
          SgVariableSymbol* sym = e->get_symbol();
          string name = sym->get_name();
          SgInitializer *init = sym->get_declaration()->get_initializer();
          string value = init->unparseToString();
          kernel->addDesignConstant(name, value);

          /*   SgTypeTable* typeTable = n->get_globalTypeTable();
               SgType* type = typeTable->lookup_type(sym->get_name()); */
          //          cout << "\t\t" << type->isFloatType() << endl;
          cout << "\t\t " << name << " = " << value << ", type: float" << endl;
        }
      }
    }
  }

  string logPass() {
    return "Extract Design Constants";
  }

};


#endif /* _EXTRACTDESIGNCONSTANTS_HXX_ */
