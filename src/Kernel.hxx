#ifndef KERNEL_HXX_
#define KERNEL_HXX_

#include "precompiled.hxx"
#include <string>
#include <list>
#include <assert.h>
#include "utils.hxx"

#include "DataFlowGraph/DataFlowGraph.hxx"
#include "DataFlowGraph/OutputNode.hxx"

#include "Stencil.h"

using namespace std;

/** Stores information about a single dataflow kernel in the design
    including input and output streams, the corresponding ROSE
    function declaration, variable declarations, scalars etc. */
class Kernel {
  string name, source, declarations, output, preamble, constants;
  list<string> inputs, scalars;
  list<OutputNode*> outputs;
  SgFunctionDeclaration* decl;

  set<string> designConstants;
  list<string> streamOutputParams, streamInputParams, scalarInputs, offsets;
  vector<string> originalParams;
  vector<Stencil*> stencils;

  map<string, string> ioTypeMap;
  map<string, string> computeTypeMap;

  // The dataflow graph for this kernel
  DataFlowGraph* dfg;

  string convertType(string type);
  string convertWidth(SgType *type);
  string convertHwType(string type);

  set<string> findModset(SgNode* sgNode);
  string declaration();
  string imports();
  string import(list<string>  imports);

  /** Removes the AST nodes corresponding to output assignments. This
      is required to ensure that output assignment expressions are not
      interpreted twice. */
  void removeOutputAssignments();

  vector<string> getParamOffsets(vector<string> dfeTaskArguments,
                                 list<string> param_names);

  vector<int> getKernelParamOffsets(list<string> param_name_vector);


public:
  /** Create a kernel with the given name and the corresponding FAST
      declaration from the ROSE AST. */
  Kernel(string name, SgFunctionDeclaration* decl);

  Kernel() {
  }


  /** Generate MaxJ source code for this kernel. */
  string generateSourceCode();

  /** Returns the original FAST declaration corresponding  to this kernel. */
  SgFunctionDeclaration* getDeclaration(){
    return decl;
  }

  void addSource(string source);
  void addInput(string varName, string inputName, string ioType, string computeType, string width);
  void addOutput(string varName, string outputName, string ioType, string computeType, string width);
  void addScalarInput(string varName, string inputName, string ioType, string computeType);
  void addDesignConstant(string name, string value);


  /** Retrieves and stores IO nodes from the original FAST
      description. Removes these nodes from the original AST to
      prevent them from being interpreted twice */
  void extractIO(bool removeOutputAssignments);

  /** Saves the names of the input output nodes corresponding to the
      original FAST description. This is required to prevent kernel
      inlining from changing the input/output names which would break
      the interface with the CPU side.*/
  void saveOriginalInputOutputNodes();
  void generateIO();

  bool isStreamArrayType(string identifer);
  void addOffsetExpression(string var, string max, string min);
  void updateTypeInfo(string identifier, string ioType, string computeType);

  /** Returns the name of this kernel. */
  string getName();

  /** Returns the name of the FAST function corresponding to this kernel. */
  string getFunctionName();


  list<string> getInputNames() {
    return streamInputParams;
  }

  string getInputType(string inputName) {
    return ioTypeMap[inputName];
  }

  list<string> getOutputNames() {
    return streamOutputParams;
  }

  list<string> getScalarInputNames() {
    return scalarInputs;
  }

  /** Returns the list of output nodes for this kernel. */
  list<OutputNode*> getOutputs() {
    return outputs;
  }

  string getOutputType(string outputName) {
    foreach_(OutputNode* n, outputs) {
      if (n->getName() == outputName)
        return n->getType();
    }
    return "";
  }

  DataFlowGraph* getDataFlowGraph() {
    return this->dfg;
  }

  string printDotDFG() {
    return dfg->getDotRepresentation();
  }

  vector<string> getDfeTaskInputs(vector<string> dfeTaskArguments) {
    return getParamOffsets(dfeTaskArguments, streamInputParams);
  }

  vector<string> getDfeTaskOutputs(vector<string> dfeTaskArguments) {
    return getParamOffsets(dfeTaskArguments, streamOutputParams);
  }

  void print(ostream&);

  /** Returns the name of the parameter at position 'index' in the
      kernel definition. Example: for the definition kernel_f(p0, ...)
      getParamName(0) returns p1. */
  string getParamName(int index);

  bool isStencilKernel() { return !stencils.empty(); }

  void addStencil(Stencil *s) { stencils.push_back(s); }

  Stencil* getFirstStencil() { return stencils[0]; }


  /** merge the given kernel with this one if possible */
  void mergeKernel(Kernel* kernel_to_merge) {
    variableRename();
    mergeInputs(kernel_to_merge);
    mergeOutputs(kernel_to_merge);
    mergeDataPath(kernel_to_merge);
  }

  void variableRename() {}
  void mergeInputs(Kernel *k) {}
  void mergeOutputs(Kernel *k) {}
  void mergeDataPath(Kernel *k) {}
  
};

#endif /* KERNEL_HXX_ */
