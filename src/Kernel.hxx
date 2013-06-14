#ifndef KERNEL_HXX_
#define KERNEL_HXX_

#include "precompiled.hxx"
#include <string>
#include <list>
#include <assert.h>
#include "DataFlowGraph/OutputNode.hxx"


using namespace std;

class Kernel {
  string name, source, declarations, output, preamble;
  string constants;
  list<string> inputs, scalars;
  list<OutputNode*> outputs;
  SgFunctionDeclaration* decl;

  set<string> designConstants;
  list<string> streamOutputParams;
  list<string> streamInputParams;
  list<string> scalarInputs;
  list<string> offsets;

  string convertType(string type);
  string convertWidth(SgType *type);
  string convertHwType(string type);

  set<string> findModset(SgNode* sgNode);

  map<string, string> ioTypeMap;
  map<string, string> computeTypeMap;

  string declaration();
  string imports();
  string import(list<string>  imports);

public:
  Kernel(string name, SgFunctionDeclaration* decl);
  string getName();
  string getFunctionName();
  string getSource();
  SgFunctionDeclaration* getDeclaration(){
    return decl;
  }

  list<OutputNode*> getOutputs() {
    return outputs;
  }

  void addSource(string source);
  void addInput(string inputName, string ioType, string computeType, string width);
  void addOutput(string outputName, string ioType, string computeType, string width);
  void addScalarInput(string inputName, string type);
  void addDesignConstant(string name, string value);
  void removeOutputAssignments();

  void extractIO();
  bool isStreamArrayType(string identifer);

  void generateIO();
  void addOffsetExpression(string var, string max, string min);
  void updateTypeInfo(string identifier, string ioType, string computeType);

};

#endif /* KERNEL_HXX_ */
