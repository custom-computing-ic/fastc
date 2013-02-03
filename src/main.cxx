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
    if (project == NULL) {
        cerr << "Could not run compiler frontend! Shutting down! " << endl;
        return 1;
    }

    Design* design = new Design();

    KernelVisitor kernelVisitor(design);
    kernelVisitor.traverse(project, preorder);

    PragmaVisitor pragmaVisitor(design);
    pragmaVisitor.traverse(project,  preorder);

    design->generateCode(cout);
    design->writeCodeFiles();

    generateDOT(*project);

    return 0;
}
