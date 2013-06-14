#include "Kernel.hxx"
#include "passes/ExtractOutputs.hxx"
#include "AstToMaxJVisitor.hxx"
#include "utils.hxx"

string Kernel::declaration() {
  return "package engine;\n\n" + imports() + "\n"
    "public class " + name + " extends Kernel {\n\n";
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
  preamble = declaration() + "\n";
  declarations = "";
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

  preamble += constants +
    "public " + name + "(KernelParameters parameters) {\n"
    "super(parameters);";
  return preamble + "\n"+ declarations + "\n" + source + "\n" + output + "}}";
}

void Kernel::addSource(string source) {
  this->source += source;
}

void Kernel::addInput(string inputName, string ioType, string computeType, string width) {
  inputs.push_back(inputName);
  if (width == "1") {
    declarations += "HWVar " + inputName + " =  io.input(\"" + inputName
      + "\", " + ioType + ")";

    if (ioType != computeType)
      declarations += ".cast(" + computeType + ")";

    declarations+=";\n";
  } else {
    string t = "new KArrayType<HWVar>(" + ioType + "," + width + ")";
    string ct = "new KArrayType<HWVar>(" + computeType + "," + width + ")";

    declarations += "KArray<HWVar> " + inputName +
      " = io.input(\"" + inputName + "\",  " + t +")";

    if (ioType != computeType)
      declarations += ".cast(" + ct + ")";

    declarations += ";\n";
  }
}

void Kernel::addOutput(string outputName, string ioType, string computeType, string width) {
  string type = computeType;
  if (width != "1") {
    type = "new KArrayType<HWVar>(" + computeType + "," + width + ")";
    declarations += "KArray<HWVar> " + outputName +
      " = (" + type +").newInstance(this);\n";
    cout << "Added array output" << endl;
  }
  outputs.push_back(new OutputNode(this, outputName, ioType, computeType, width));
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

string Kernel::convertHwType(string type) {
  string hwType = type;
  cout << "Converting hw type for " << type << endl;
  regex floatType("float \\( (\\d*), (\\d*) \\)");
  regex doubleType("double \\( (\\d*), (\\d*) \\)");
  cmatch cm;

  if (regex_match(type.c_str(), cm, floatType) ||
      regex_match(type.c_str(), cm, doubleType)) {
    hwType = "hwFloat(" + cm[1].str() + ", " + cm[2].str() + ")";
  }

  cout << "Converted type: " << hwType << endl;
  return hwType;

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

    string computeType;
    if (computeTypeMap.find(inputName) != computeTypeMap.end())
      computeType = computeTypeMap[inputName];
    else
      computeType = inputType;

    string inputWidth = convertWidth(param_type);
    cout << "input name " << inputName << "Original Type:" << param_type->unparseToString();
    cout <<" Type " << inputType;
    cout <<" width " << inputWidth << endl;

    // ignore offsets
    if (find(offsets.begin(), offsets.end(), inputName) != offsets.end())
      continue;

    if (isSgPointerType(param_type) || isSgArrayType(param_type)) {
      if (modSet.find(inputName) != modSet.end()) {
        cout << "Adding output " << inputName << endl;
        addOutput(inputName, inputType,  computeType, inputWidth);
      } else {
        addInput(inputName,  inputType, computeType, inputWidth);
      }
    } else {
      // scalar inputs
      addScalarInput(inputName, inputType);
    }
  }

  removeOutputAssignments();
}

void Kernel::addOffsetExpression(string var, string max, string min) {
  offsets.push_back(var);
  declarations+= "OffsetExpr " + var + " = stream.makeOffsetParam(" +
    "\"nx\", " + min + ", " + max + ");\n";
}

void Kernel::updateTypeInfo(string identifier, string ioType, string computeType) {
  ioTypeMap[identifier] = convertHwType(ioType);
  computeTypeMap[identifier] = convertHwType(computeType);
}

void Kernel::addDesignConstant(string name, string value) {
  if ( designConstants.find(name) == designConstants.end()) {
    designConstants.insert(name);
    constants += "final int " + name + " = " + value + ";\n";
  }
}
