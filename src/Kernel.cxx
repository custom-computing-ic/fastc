#include "Kernel.hxx"
#include "passes/ExtractOutputs.hxx"
#include "AstToMaxJVisitor.hxx"
#include "utils.hxx"

string Kernel::declaration() {
  return "package engine;\n\n" + imports() + "\n"
    "public class " + name + " extends Kernel {\n\n"
    "public " + name + "(KernelParameters parameters) {\n"
    "super(parameters);";
}

string Kernel::imports() {
  string imps[] = { "*",
                    "types.base.HWVar",
                    "stdlib.*",
                    "stdlib.core.*    ",
                    "stdlib.core.Count.*",
                    "stdlib.KernelMath.*",
                    "stdlib.core.Stream.OffsetExpr",
                    "types.composite.*",
                    "types.base.*"};
  list<string> imports(imps, imps + 9);
  return import(imports);

}

void Kernel::removeOutputAssignments() {
  cout << "removing output assignments " << endl;
  cout << getOutputs().size() << endl;
  ExtractOutputs extractOutputs(getOutputs());
  extractOutputs.traverse(decl->get_definition(), preorder);
  cout << "traversed" << endl;
}


string Kernel::import(list<string> imports) {
  string import = "";
  list<string>::iterator it;
  for (it = imports.begin(); it != imports.end(); it++)
    import += "import com.maxeler.maxcompiler.v1.kernelcompiler." + (*it)
      + ";\n";
  return import;
}

Kernel::Kernel(string name, SgFunctionDeclaration* decl) {
  this->name = name;
  declarations = declaration() + "\n";
  this->decl = decl;
}

string Kernel::getName() {
  return name;
}

string Kernel::getFunctionName() {
  // XXX for now assume kernels are kernel_<KernelName>
  return "kernel_" + name;
}

string Kernel::getSource() {
  extractIO();
  generateIO();
  ASTtoMaxJVisitor visitor;
  visitor.traverse(decl->get_definition());
  addSource(visitor.getSource());

  return declarations + "\n" + source + "\n" + output + "}}";
}

void Kernel::addSource(string source) {
  this->source += source;
}

void Kernel::addInput(string inputName, string type) {
  inputs.push_back(inputName);
  declarations += "HWVar " + inputName + " =  io.input(\"" + inputName
    + "\", " + type + ");\n";
}

void Kernel::addInput(string inputName, string type, string width) {
  inputs.push_back(inputName);
  string t = "new KArrayType<HWVar>(" + type + "," + width + ")";
  declarations += "KArray<HWVar> " + inputName +
    " = io.input(\"" + inputName + "\",  " + t +");\n";
}

void Kernel::addOutput(string outputName, string type, string width) {
  outputs.push_back(new OutputNode(outputName, type, width));
}

void Kernel::addScalarInput(string inputName, string type) {
  scalars.push_back(inputName);
  declarations += "HWVar " + inputName + " =  io.scalarInput(\"" + inputName
    + "\", " + type + ");\n";
}

void Kernel::generateIO() {
  foreach_ (OutputNode* outputNode, outputs) {
    output += outputNode->toMaxJ();
  }
}

// XXX: this should account for type width
string Kernel::convertType(string type) {
  D(cerr << "Converting type " << type << endl);
  if (type == "float *" || type == "float")
    return "hwFloat(8, 24)";
  else if (type == "int *" || type == "int")
    return "hwInt(32)";
  else if (type == "bool")
    return "hwUInt(1)";
  else if (type == "unsigned int" || type == "unsigned int*")
    return "hwUInt(32)";
  return "hwFloat(8, 24)";
}

void Kernel::extractIO() {
  cout << "Generating Kernel IO Links " << endl;

  // extract kernel inputs and outputs
  SgInitializedNamePtrList args = decl->get_args();
  SgInitializedNamePtrList::iterator it = args.begin();

  std::set< SgInitializedName * > readVars, writeVars;
  if (decl->get_definition() != NULL)
    SageInterface::collectReadWriteVariables (decl->get_definition()->get_body(), readVars, writeVars);
  cout << "Modset size: "  << writeVars.size() << endl;

  set<string> modSet;  //findModifiesSet.getModifiesSet();
  modSet.insert("output");

  for ( ; it != args.end(); it++) {
    SgInitializedName* param = *it;
    SgType* param_type = param->get_type();
    string inputName = param->get_name().getString();
    string inputType = convertType(param_type->unparseToString());
    // XXX: hardcoded input type for now
    //   inputType = "hwFloat(8, 24)";
    if (isSgPointerType(param_type)) {
      if (modSet.find(inputName) != modSet.end()) {
        addOutput(inputName, inputType, "1");
      } else {
        addInput(inputName, inputType);
      }
    } else {
      // scalar inputs
      addScalarInput(inputName, inputType);
    }
  }

  removeOutputAssignments();

  //visit(functionDeclaration->get_definition()); // visit the function body
}
