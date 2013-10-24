#ifndef UTILS_HXX
#define UTILS_HXX

#include <string>

#include "precompiled.hxx"

//#define DEBUG

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

#endif
