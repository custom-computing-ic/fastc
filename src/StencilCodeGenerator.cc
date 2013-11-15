#include "StencilCodeGenerator.h"
#include "precompiled.hxx"
#include "StencilAstToMaxJVisitor.h"
#include "Stencil.h"

using namespace std;

void StencilCodeGenerator::generateInputs() {

  Stencil* stencil = this->kernel->getFirstStencil();

  foreach_ (string in, this->kernel->getInputNames()) {
    string type = this->kernel->getInputType(in);
    source += "HWVar " + in + " = io.input( \"" + in + "\"";
    source += "," + type + ");\n";
  }

  source += "\n\n";
  addComment("--------Scalar inputs -------");

  foreach_ (string in, this->kernel->getScalarInputNames()) {
    string type = this->kernel->getInputType(in);
    source += "HWVar " + in + " = io.scalarinput( \"" + in + "\"";
    source += "," + type + ");\n";
  }

}

void StencilCodeGenerator::generateBoundaryCounters() {

  Stencil *stencil = this->kernel->getFirstStencil();

  source += "CounterChain chain = control.count.makeCounterChain();\n";

  vector<int> dims = stencil->getArrayDimensions();

  int pos = 0;
  foreach_(string var, stencil->getLoopVariables()) {
    string inc = stencil->getLoopIncrement(var);
    string bound = boost::lexical_cast<string>(dims[0]);
    source += "HWVar " + var + " = chain.addCounter(" + bound + ", " + inc + ").cast(hwUInt(32));\n";
  }

  source += "HWVar update = ";

  bool first = true;
  foreach_(string var, stencil->getLoopVariables()) {
    if (!first) {
      source += " & ";
    }
    first = false;
    string lb = stencil->getLowerBound(var);
    string ub = stencil->getUpperBound(var);
    source += var + " < " + ub + " & " + var + " >= " + lb;
  }
  source += ";\n";

}

void StencilCodeGenerator::generateCache() {
}

void StencilCodeGenerator::generateDataPath() {

  Stencil* stencil = this->kernel->getFirstStencil();


  SgStatement* statement = stencil->getUpdateStatement();
  StencilAstToMaxJVisitor visitor(this->kernel);
  visitor.traverse(statement);
  this->source += visitor.getSource();
}

void StencilCodeGenerator::generateOutputs() {
  Stencil* stencil = this->kernel->getFirstStencil();

  foreach_ (string out, this->kernel->getOutputNames()) {
    string type = this->kernel->getOutputType(out);

    if (out == stencil->getDestination()) {
      // if this is a stencil output, make it conditional on the
      // control block and output the initial value for values outside
      // of the control block (update)
      string control_var = "update";
      source += "io.output( \"" + out + "\"";
      source +=  ", update ? " + out + " : " + stencil->getSource() +  " , " + type + ");\n";
    } else {
      source += "io.output( \"" + out + "\"";
      source +=  "," + out +  "," + type + ");\n";
    }
  }

  source += "\n\n";
}

void StencilCodeGenerator::addComment(string comment) {
  source += "/*" + comment + "*/\n";
}

string StencilCodeGenerator::generateStencilCode() {
  cout << "Generating code for " << this->kernel->getName() << endl;

  source += "\n\n";
  addComment("-----Output Control------");
  generateBoundaryCounters();

  generateCache();

  source += "\n\n";
  addComment("-----DataPath------");
  generateDataPath();

  source += "\n\n";
  addComment("-----Outputs------");
  generateOutputs();
  return source;
}
