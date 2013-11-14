#include "Stencil.h"
#include "utils.hxx"

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


std::vector<int> Stencil::getArrayDimensions() {
  vector<int> dims;

  if (offsets.size() == 0) {
    cerr << "Could not extract dimensions for stencil array:";
    cerr << "Missing stencil offsets!" << endl;
    return dims;
  }

  int prod = 1;
  StencilOffset* so = offsets[0];

  bool first = true;

  foreach_r_ (string var, loopVars) {
    if (first) {
      first = false;
      continue;
    }

    int d = boost::lexical_cast<int>(so->var_dim[var]);
    dims.push_back(d / prod);
    prod *= d;
  }

  dims.push_back(dims.back());

  return dims;
}

void Stencil::print() {
  cout << "\tUpdate: " << getDestination() << " <-- " << getSource() << endl;
  cout << "\tDimension: " << getDimension() << endl;
  cout << "\tInputs: ";
  printVect(getInputs());
  cout << endl;

  cout << "\tOutputs: ";
  printVect(getOutputs());
  cout << endl;

  cout << "\tLoop variables: ";
  printVect(getLoopVariables());
  cout << endl;

  cout << "\tOffsets " << endl;
  foreach_(StencilOffset* of, getOffsets()) {
    map<string, int>::iterator it;
    cout << "\t\tvars: ";
    for (it = of->var_offset.begin(); it != of->var_offset.end(); it++) {
      cout << it->first << " " << it->second << ",";
    }
    cout << " dims: ";
    for (it = of->dim_offset.begin(); it != of->dim_offset.end(); it++) {
      cout << it->first << " " << it->second << ",";
    }

    map<string, string>::iterator itt;
    cout << " var_dims: ";
    for (itt = of->var_dim.begin(); itt != of->var_dim.end(); itt++) {
      cout << itt->first << " " << itt->second << ",";
    }
    cout << endl;

  }

  cout << "\tLoop increments: ";
  printVect(getLoopIncrements());
  cout << endl;

  cout << "\tLower bounds: ";
  printVect(getLowerBounds());
  cout << endl;

  cout << "\tUpper bounds: ";
  printVect(getUpperBounds());

  cout << endl;
  cout << "\tArray Dimensions: ";
  printVect(getArrayDimensions());
  cout << endl;
}
