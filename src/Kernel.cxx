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


bool Kernel::isStreamArrayType(string identifier) {
  cout <<" Checking stream type " << identifier << endl;
  SgInitializedNamePtrList args = decl->get_args();
  foreach_(SgInitializedName* param, args) {
    if (param->get_name().getString() == identifier) {
      SgArrayType* type = isSgArrayType(param->get_type());

      if (type!= NULL) {
        cout << "Type is " << type->unparseToString() << endl;
        vector<SgExpression*> dims = SageInterface::get_C_array_dimensions(type);
          if (dims.size() == 2)
            return true;
          else
            return false;
      }
      }
    }
  return false;
}

string Kernel::getSource() {
  extractIO();
  generateIO();
  ASTtoMaxJVisitor visitor(this);
  visitor.traverse(decl->get_definition());
  addSource(visitor.getSource());

  return declarations + "\n" + source + "\n" + output + "}}";
}

void Kernel::addSource(string source) {
  this->source += source;
}

void Kernel::addInput(string inputName, string type, string width) {
  inputs.push_back(inputName);
  if (width == "1") {
    declarations += "HWVar " + inputName + " =  io.input(\"" + inputName
      + "\", " + type + ");\n";
  } else {
    string t = "new KArrayType<HWVar>(" + type + "," + width + ")";
    declarations += "KArray<HWVar> " + inputName +
      " = io.input(\"" + inputName + "\",  " + t +");\n";
  }
}

void Kernel::addOutput(string outputName, string type, string width) {
  if (width != "1") {
    type = "new KArrayType<HWVar>(" + type + "," + width + ")";
    declarations += "KArray<HWVar> " + outputName +
      " = (" + type +").newInstance(this);\n";
    cout << "Added array output" << endl;
  }
  outputs.push_back(new OutputNode(this, outputName, type, width));
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

string Kernel::convertWidth(SgType *type) {
  if (isSgArrayType(type)) {
    vector<SgExpression*> dims = SageInterface::get_C_array_dimensions(isSgArrayType(type));
    if (dims.size() == 2)
      // declaration of type: float *in[par]
      return dims[1]->unparseToString();
    else
      return "-1";
  }
  return "1";
}

set<string> Kernel::findModset(SgNode* sgNode) {
  Rose_STL_Container<SgNode* > assigns;
  assigns = NodeQuery::querySubTree(sgNode, V_SgAssignOp);
  set<string> modSet;
  cout << "modSet ";
  foreach_(SgNode* node, assigns) {
    SgAssignOp* op = isSgAssignOp(node);
    string name = op->get_lhs_operand()->unparseToString();
    SgExpression* nameExp;
    if (SageInterface::isArrayReference(op->get_lhs_operand(), &nameExp)) {
      cout << "Found arr reference; Adding base name ";
      name = nameExp->unparseToString();
    }
    modSet.insert(name);
    cout << name << " ";
    cout << endl;
  }
  cout << endl;
  return modSet;
}

void Kernel::extractIO() {
  cout << "Generating Kernel IO Links " << endl;

  // extract kernel inputs and outputs
  SgInitializedNamePtrList args = decl->get_args();
  SgInitializedNamePtrList::iterator it = args.begin();

  set<string> modSet = findModset(decl->get_definition());

  for ( ; it != args.end(); it++) {
    SgInitializedName* param = *it;
    SgType* param_type = param->get_type();
    string inputName = param->get_name().getString();
    string inputType = convertType(param_type->unparseToString());
    string inputWidth = convertWidth(param_type);
    cout << "input name " << inputName << "Original Type:" << param_type->unparseToString();
    cout <<" Type " << inputType ;
    cout <<" width " << inputWidth << endl;
    if (isSgPointerType(param_type) || isSgArrayType(param_type)) {
      if (modSet.find(inputName) != modSet.end()) {
        cout << "Adding output " << inputName << endl;
        addOutput(inputName, inputType, inputWidth);
      } else {
        addInput(inputName, inputType, inputWidth);
      }
    } else {
      // scalar inputs
      addScalarInput(inputName, inputType);
    }
  }

  removeOutputAssignments();

  //visit(functionDeclaration->get_definition()); // visit the function body
}


void Kernel::addOffsetExpression(string var, string max, string min) {
  declarations+= "OffsetExpr " + var + " = stream.makeOffsetParam(" +
    "\"nx\", " + min + ", " + max + ");\n";
}
