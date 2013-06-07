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

  cout << "---- Adding declarations " << endl;
  cout << "---- Adding inputs "       << endl;

  cout << "---- Adding outputs"       << endl;
  generateIO();

  cout << "---- Adding dataflow code" << endl;
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

string Kernel::convertType(string type) {
  D(cerr << "Converting type " << type << endl);
  if (type == "float *" || type == "float")
    return "hwFloat(8, 24)";
  else if (type == "int *" || type == "int")
    return "hwInt(32)";
  return "hwFloat(8, 24)";
}

void Kernel::extractIO() {
  cout << "Generating Kernel IO Links " << endl;

  // extract kernel inputs and outputs
  SgInitializedNamePtrList args = decl->get_args();
  SgInitializedNamePtrList::iterator it = args.begin();
  cout << "Function " << decl->get_name().getString();
  cout << args.size() << endl;

  //     FindModifiesSet findModifiesSet(decl);
  //      findModifiesSet.traverse(decl->get_definition);
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
        cout << "Addin output type " << inputType << endl;
      } else {
        addInput(inputName, inputType);
      }
    } else {
      // scalar inputs
      cout << "Scalar name: " << inputName << " type: " << inputType << endl;
      addScalarInput(inputName, inputType);
    }
  }

  removeOutputAssignments();

  //visit(functionDeclaration->get_definition()); // visit the function body
}
