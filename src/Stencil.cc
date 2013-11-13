#include "Stencil.h"

using namespace std;

int Stencil::varIndex(string loopVar) {
  vector<string>::iterator it = find(loopVars.begin(), loopVars.end(), loopVar);
  return it - loopVars.begin();
}

string Stencil::getLoopIncrement(string loopVar) {
  return loopIncs[varIndex(loopVar)];
}

string Stencil::getLowerBound(string loopVar) {
  return lowerBounds[varIndex(loopVar)];
}

string Stencil::getUpperBound(string loopVar) {
  return upperBounds[varIndex(loopVar)];
}

void Stencil::computeLoopProperties(SgStatement* st) {

  while (isSgForStatement(st)) {

    SgExpression* step, *lb, *ub;
    SgInitializedName *ivar;
    SgStatement* body;

    if (SageInterface::isCanonicalForLoop(st, &ivar, &lb, &ub, &step, &body)) {
      loopVars.push_back(ivar->unparseToString());
      loopIncs.push_back(step->unparseToString());
      lowerBounds.push_back(lb->unparseToString());
      upperBounds.push_back(ub->unparseToString());
      st = body;

    } else {
      cerr << "Could not update stencil properties from non-canonical for loop: " << endl;
      cerr << "\t " << st->unparseToString() << endl;
      break;
    }

    if (st == NULL)
      break;
    int st_count = isSgBasicBlock(st)->get_statements().size();
    if (st_count == 1 )
      st = isSgBasicBlock(st)->get_statements()[0];
  }

  this->dim = loopVars.size();
}
