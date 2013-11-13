#include "StencilCodeGenerator.h"
#include "precompiled.hxx"
#include "StencilAstToMaxJVisitor.h"
#include "Stencil.h"

using namespace std;

void StencilCodeGenerator::generateInputs() {

  Stencil* stencil = this->kernel->getFirstStencil();

  foreach_ (string in, this->kernel->getInputNames()) {
    // FIXME obviously, the type shouldn't be hardcoded...
    //    string type = "hwFloat(8, 24)";
    string type = this->kernel->getInputType(in);
    source += "HWVar " + in + " = io.input( \"" + in + "\"";
    source += "," + type + ");\n";
  }

  source += "\n\n";
  addComment("--------Scalar inputs -------");

  foreach_ (string in, this->kernel->getScalarInputNames()) {
    // FIXME obviously, the type shouldn't be hardcoded...
    string type = this->kernel->getInputType(in);
    //    string type = "hwFloat(8, 24)";
    source += "HWVar " + in + " = io.scalarinput( \"" + in + "\"";
    source += "," + type + ");\n";
  }

}

void StencilCodeGenerator::generateBoundaryCounters() {

  Stencil *stencil = this->kernel->getFirstStencil();

  source += "CounterChain chain = control.count.makeCounterChain();\n";

  foreach_(string var, stencil->getLoopVariables()) {
    string inc = stencil->getLoopIncrement(var);
    source += "HWVar " + var + " = chain.addCounter(TODO, " + inc + ").cast(hwUInt(32));\n";
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
    // FIXME obviously, the type shouldn't be hardcoded...
    //    string type = "hwFloat(8, 24)";
    string type = this->kernel->getInputType(in);

    if (out == stencil->getDestination()) {
      // if this is a stencil output, make it conditional on the control block
      string control_var = "update";
      source += "io.output( \"" + out + "\"";
      //source +=  ", update ? " + out + " : " + stencil->getSource() +  "," + type + ");\n";
      source +=  ", update ? " + out + " : TODO " +  "," + type + ");\n";
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

  addComment("-----Offset Expressions------");
  // TODO


  addComment("-----Inputs------");
  generateInputs();

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
