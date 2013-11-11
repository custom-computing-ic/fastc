#ifndef UTILS_HXX
#define UTILS_HXX

#include <string>
#include <vector>
#include <iostream>

#include "precompiled.hxx"

//#define DEBUG

#define DBG(x) std::cout << #x << ": " << x << std::endl;

#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif

#define LOG_CERR() (cerr << __FILE__ ": " << __LINE__ << ":")

std::string* get_pragma_param(std::string pragma, std::string param);

SgProject* load_rose_project(int argc, char** argv);

void setupBuild();
/*
namespace FASTAnalysis {
  std::set<std::string> findModset(SgNode *sgNode) {
    std::set<std::string> ret;
    return ret;
  }
}
*/

template<class T> void printVect(std::vector<T> v) {
  std::copy(v.begin(), v.end(), std::ostream_iterator<T>(std::cout, " "));
}

std::vector<std::string> getFunctionDeclArgNames(SgFunctionDeclaration* f);
std::vector<std::string> getFunctionCallArgNames(SgFunctionCallExp* exp);

#endif
