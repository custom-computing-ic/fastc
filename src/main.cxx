#include "precompiled.hxx"

#include "main.hxx"
#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>
#include <fstream>

#include "AstToDfgVisitor.hxx"
#include "AstToMaxJVisitor.hxx"
#include "KernelVisitor.hxx"
#include "PragmaVisitor.hxx"

int main(int argc, char** argv) { 

  SgProject* project = frontend(argc, argv);
//  AstTests :: runAllTests(project);

  Design* design = new Design();
  
  KernelVisitor kernelVisitor(design);
  kernelVisitor.traverseInputFiles(project, preorder);

  PragmaVisitor pragmaVisitor(design);
  pragmaVisitor.traverseInputFiles(project,  preorder);

  design->generateCode(cout);

  generateDOT(*project);

  return 0;
}


