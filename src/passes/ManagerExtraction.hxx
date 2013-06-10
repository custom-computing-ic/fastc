#ifndef MANAGER_EXTRACTION_HXX
#define MANAGER_EXTRACTION_HXX

#include "../precompiled.hxx"
#include "../Design.hxx"

class ManagerExtraction : public AstSimpleProcessing
{
  Design* design;
public:
  ManagerExtraction(Design* design) {
    this->design = design;
  }
  void visit(SgNode *node) {

    cout << "Extracting manager design" << endl;
  }

};

#endif
