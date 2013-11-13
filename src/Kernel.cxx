#include "Kernel.hxx"
#include "passes/ExtractOutputs.hxx"
#include "AstToMaxJVisitor.hxx"
#include "StencilAstToMaxJVisitor.h"
#include "utils.hxx"

#include "DataFlowGraph/Node.hxx"
#include "DataFlowGraph/InputNode.hxx"
#include <iostream>

Kernel::Kernel(string name, SgFunctionDeclaration* decl) {
  this->name = name;
  preamble = declaration() + "\n";
  declarations = "";
  this->decl = decl;

  this->dfg = new DataFlowGraph();
}

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
  ExtractOutputs extractOutputs(getOutputs());
  extractOutputs.traverse(decl->get_definition(), preorder);
}


string Kernel::import(list<string> imports) {
  string import = "";
  list<string>::iterator it;
  for (it = imports.begin(); it != imports.end(); it++)
    import += "import com.maxeler.maxcompiler.v1.kernelcompiler." + (*it)
      + ";\n";
  return import;
}


bool Kernel::isStreamArrayType(string identifier) {
  SgInitializedNamePtrList args = decl->get_args();
  foreach_(SgInitializedName* param, args) {
    if (param->get_name().getString() == identifier) {
      SgArrayType* type = isSgArrayType(param->get_type());
      if (type!= NULL) {
        vector<SgExpression*> dims = SageInterface::get_C_array_dimensions(type);
        return dims.size() == 2;
      }
    }
  }
  return false;
}

string Kernel::generateSourceCode() {
  if (!isStencilKernel()) {
    cout << "Generating code for kernel " << getName() << endl;
    extractIO();
    generateIO();

    ASTtoMaxJVisitor visitor(this);
    visitor.traverse(decl->get_definition());
    addSource(visitor.getSource());
  } else {
    // TODO Run stencil code generator...

    /*extractIO();
    generateIO();

    StencilAstToMaxJVisitor visitor(this);
    visitor.traverse(decl->get_definition());
    addSource(visitor.getSource()); */
  }

  preamble += constants +
    "public " + name + "(KernelParameters parameters) {\n"
    "super(parameters);";
  return preamble + "\n"+ declarations + "\n" + source + "\n" + output + "}}";
}

void Kernel::addSource(string source) {
  this->source += source;
}

void Kernel::addInput(string varName, string inputName, string ioType, string computeType, string width) {
  inputs.push_back(inputName);
  if (width == "1") {
    declarations += "HWVar " + varName + " =  io.input(\"" + inputName
      + "\", " + ioType + ")";

    if (ioType != computeType)
      declarations += ".cast(" + computeType + ")";

    declarations+=";\n";
  } else {
    string t = "new KArrayType<HWVar>(" + ioType + "," + width + ")";
    string ct = "new KArrayType<HWVar>(" + computeType + "," + width + ")";

    declarations += "KArray<HWVar> " + varName +
      " = io.input(\"" + inputName + "\",  " + t +")";

    if (ioType != computeType)
      declarations += ".cast(" + ct + ")";

    declarations += ";\n";
  }
}


void Kernel::addOutput(string varName, string outputName, string ioType, string computeType, string width) {
  string type = computeType;
  if (width != "1") {
    type = "new KArrayType<HWVar>(" + computeType + "," + width + ")";
    declarations += "KArray<HWVar> " + varName +
      " = (" + type +").newInstance(this);\n";
  }
  outputs.push_back(new OutputNode(this, varName, outputName, ioType, computeType, width));
}


void Kernel::addScalarInput(string varName, string inputName, string ioType, string computeType) {
  scalars.push_back(inputName);
  declarations += "HWVar " + varName + " =  io.scalarInput(\"" + inputName
    + "\", " + ioType + ")";
  if (ioType != computeType)
    declarations += ".cast(" + computeType + ")";
  declarations+= ";\n";
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
  regex floatType("float \\( (\\d*), (\\d*) \\)");
  regex doubleType("double \\( (\\d*), (\\d*) \\)");
  cmatch cm;

  if (regex_match(type.c_str(), cm, floatType) ||
      regex_match(type.c_str(), cm, doubleType)) {
    hwType = "hwFloat(" + cm[1].str() + ", " + cm[2].str() + ")";
  }
  return hwType;

}


string Kernel::convertWidth(SgType *type) {
  using namespace SageInterface;
  if (isSgArrayType(type)) {
    vector<SgExpression*> dims = get_C_array_dimensions(isSgArrayType(type));
    if (dims.size() == 2) {
      // declaration of type: float *in[par]
      string s;
      if (isSgValueExp(dims[1]))
        s = boost::lexical_cast<string>(getIntegerConstantValue(isSgValueExp(dims[1])));
      else
        s = dims[1]->unparseToString();
      return s;
    } else
      return "-1";
  }
  return "1";
}


set<string> Kernel::findModset(SgNode* sgNode) {
  Rose_STL_Container<SgNode* > assigns;
  assigns = NodeQuery::querySubTree(sgNode, V_SgAssignOp);
  set<string> modSet;
  foreach_(SgNode* node, assigns) {
    SgAssignOp* op = isSgAssignOp(node);
    string name = op->get_lhs_operand()->unparseToString();
    SgExpression* nameExp;
    if (SageInterface::isArrayReference(op->get_lhs_operand(), &nameExp)) {
      name = nameExp->unparseToString();
    } else if (isSgPointerDerefExp(op->get_lhs_operand())) {
      // XXX this is a hack since it assumes only one pointer dereference
      // TODO should check for multiple dereferences
      name = isSgPointerDerefExp(op->get_lhs_operand())->get_operand_i()->unparseToString();
    }
    modSet.insert(name);
  }
  return modSet;
}


void Kernel::extractIO() {

  // extract kernel inputs and outputs
  SgInitializedNamePtrList args = decl->get_args();
  SgInitializedNamePtrList::iterator it = args.begin();

  set<string> modSet = findModset(decl->get_definition());
  int paramNumber = 0;
  foreach_(SgInitializedName* param, decl->get_args()) {

    SgType* param_type = param->get_type();
    string varName = param->get_name().getString();
    string inputName = originalParams[paramNumber];
    string inputType = convertType(param_type->unparseToString());

    string computeType;
    if (computeTypeMap.find(inputName) != computeTypeMap.end())
      computeType = computeTypeMap[inputName];
    else
      computeType = inputType;
    string inputWidth = convertWidth(param_type);
    // ignore offsets
    if (find(offsets.begin(), offsets.end(), inputName) != offsets.end())
      continue;

    if (isSgPointerType(param_type) || isSgArrayType(param_type)) {
      if (modSet.find(varName) != modSet.end()) {
        addOutput(varName, inputName, inputType,  computeType, inputWidth);
      } else {
        addInput(varName, inputName,  inputType, computeType, inputWidth);
      }
    } else {
      // scalar inputs
      addScalarInput(varName, inputName, inputType, computeType);
    }
    paramNumber++;
  }

  removeOutputAssignments();
}


void Kernel::saveOriginalInputOutputNodes() {
  SgInitializedNamePtrList args = decl->get_args();
  set<string> modSet = findModset(decl->get_definition());

  foreach_(SgInitializedName* param, decl->get_args()) {
    SgType* paramType = param->get_type();
    string paramName = param->get_name().getString();
    originalParams.push_back(paramName);
    if (isSgPointerType(paramType) || isSgArrayType(paramType)) {
      if (modSet.find(paramName) != modSet.end()) {
        streamOutputParams.push_back(paramName);

        // add output node to DFG
        dfg->addOutputNode(new OutputNode(paramName));
      } else {
        streamInputParams.push_back(paramName);

        // add input node to DFG
        dfg->addInputNode(new InputNode(paramName));
      }
    } else {
      scalarInputs.push_back(paramName);
    }
  }
}


void Kernel::addOffsetExpression(string var, string max, string min) {
  offsets.push_back(var);
  declarations+= "OffsetExpr " + var + " = stream.makeOffsetParam(" +
    "\"" + var + "\", " + min + ", " + max + ");\n";
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


string Kernel::getName() {
  return name;
}


string Kernel::getFunctionName() {
  // XXX for now assume kernels are kernel_<KernelName>
  return "kernel_" + name;
}


void Kernel::print(ostream& out) {
  out << this->getName() << "[Outputs: ";
  foreach_(string s, streamOutputParams) {
    out << s << " ";
  }
  out << ", Inputs: ";
  foreach_(string s, streamInputParams) {
    out << s << " ";
  }
  out << "]";
}


vector<string> Kernel::getParamOffsets(
                                       vector<string> dfeTaskArguments,
                                       list<string> param_names) {
  vector<string> task_inputs;
  foreach_(int o, getKernelParamOffsets(param_names)) {
    task_inputs.push_back(dfeTaskArguments[o]);
  }
  return task_inputs;
}


vector<int> Kernel::getKernelParamOffsets(list<string> param_name_vector) {
  int pos = 0;
  vector<int> arg_offsets;
  foreach_(SgInitializedName* arg, decl->get_args()) {
    string arg_name = arg->unparseToString();
    list<string>::iterator it = find(
                                     param_name_vector.begin(),
                                     param_name_vector.end(),
                                     arg_name);
    if (it != param_name_vector.end())
      arg_offsets.push_back(pos);

    pos++;
  }

  return arg_offsets;
}


string Kernel::getParamName(int index) {
  return decl->get_args()[index]->unparseToString();
}
