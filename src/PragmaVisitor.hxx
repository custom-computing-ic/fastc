#ifndef PRAGMAVISITOR_HXX
#define PRAGMAVISITOR_HXX

#include "precompiled.hxx"
#include "Design.hxx"

using namespace boost;

class PragmaVisitor: public AstSimpleProcessing {

    regex *PRAGMA_IN, *PRAGMA_OUT, *PRAGMA_SCALAR_IN, *TYPE;
    void visit(SgPragma*);
    string getType(string);
    Design* design;

public:
    PragmaVisitor(Design* design);
    virtual void visit(SgNode*);

};

#endif
