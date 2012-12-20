#include "main.hxx"

#include "precompiled.hxx"
#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>
#include <fstream>
//#include "HelloConfig.h"
#include "DataFlowGraph/Node.hxx"
#include "DataFlowGraph/InputNode.hxx"
#include "DataFlowGraph/DataFlowGraph.cxx"

#include "AstToDfgVisitor.hxx"
#include "AstToMaxJVisitor.hxx"

int main(int argc, char** argv) { 

  // build ast
  SgProject* project = frontend(argc, argv);
  AstTests :: runAllTests(project);
  
  // traverse ast and build graph
  //ASTtoDFGVisitor visitor;
  //visitor.traverseInputFiles(project, preorder);

  ASTtoMaxJVisitor maxJVisitor;
  maxJVisitor.traverseInputFiles(project, preorder);

  ofstream out("KernelFunc.java");
  out << maxJVisitor.getSource();


  // construct dataflow graph

  // print graph in dot


  //  Node inX("inX"), inY("inY"), outRes("outRes");

  //  DataFlowGraph graph;

  //  graph.addInputNode(&inX);
  //  graph.addInputNode(&inY);
  //  graph.addOutputNode(&outRes);
  //  inX.addNeighbour(&outRes);
  //  inY.addNeighbour(&outRes);
  // cout << graph.getSource() << endl;
  // cout << inX << endl;
  //  cout << graph.getDotRepresentation();	
 // generateDOT(*project);


  return 0;
}


