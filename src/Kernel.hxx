#ifndef KERNEL_HXX_
#define KERNEL_HXX_

#include "precompiled.hxx"
#include <string>
#include <list>
#include <assert.h>
#include "DataFlowGraph/OutputNode.hxx"


using namespace std;

class Kernel {
  string name, source, declarations, output;
  list<string> inputs, scalars;
  list<OutputNode*> outputs;
  SgFunctionDeclaration* decl;

  list<string> streamOutputParams;
  list<string> streamInputParams;
  list<string> scalarInputs;

  string convertType(string type);
  string convertWidth(SgType *type);
  set<string> findModset(SgNode* sgNode);


  string declaration();
  string imports();
  string import(list<string>  imports);

public:
  Kernel(string name, SgFunctionDeclaration* decl);
  string getName();
  string getFunctionName();
  string getSource();
  list<OutputNode*> getOutputs() {
    return outputs;
  }
  void addSource(string source);
  void addInput(string inputName, string type, string width);
  void addOutput(string outputName, string type, string width);
  void addScalarInput(string inputName, string type);
  void removeOutputAssignments();

  void extractIO();
  bool isStreamArrayType(string identifer);

  void generateIO();

};

#endif /* KERNEL_HXX_ */
