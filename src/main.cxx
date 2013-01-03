#include "precompiled.hxx"

#include "main.hxx"
#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>
#include <fstream>

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
//  maxJVisitor.traverseInputFiles(project, preorder);

  traverseMemoryPoolVisitorPattern(maxJVisitor);

  ofstream out("KernelFunc.java");
  out << maxJVisitor.getSource();

  cout << maxJVisitor.getSource();

//  maxJVisitor.writeKernels(cout);

  generateDOT(*project);

  return 0;
}


