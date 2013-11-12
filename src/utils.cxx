#include "utils.hxx"

#include <boost/filesystem.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

std::string* get_pragma_param(std::string pragma, std::string param) {
  using namespace std;
  int pos = pragma.find(param);
  string param_value = "";
  if (pos != string::npos) {
    int semic1 = pragma.find(":", pos);
    int semic2 = pragma.find(":", semic1 + 1);
    if (semic2 == string::npos) {
      param_value = pragma.substr(semic1 + 1);
    } else {
      int p2 = semic2 - 2;
      while (pragma[p2] != ' ') {
        p2--;
      }
      param_value = pragma.substr(semic1 + 1, p2 - semic1 - 1);
    }
  }
  boost::algorithm::trim(param_value);
  return new string(param_value);
}

SgProject* load_rose_project(int argc, char** argv) {
  SgProject* project = frontend(argc, argv);
  //  AstTests :: runAllTests(project);
  if (project == NULL) {
    std::cerr << "Could not run compiler frontend! Shutting down! " << std::endl;
    exit(0);
  }

return project;
}


void setupBuild() {
  using namespace boost::filesystem;
  path dir("build"), cpu("build/cpu"), engine("build/engine");
  create_directory(dir);
  create_directory(cpu);
  create_directory(engine);
}


std::vector<std::string> getFunctionArgNames(SgFunctionDeclaration* fdecl) {
  std::vector<std::string> args;
  foreach_(SgInitializedName* arg, fdecl->get_args()) {
    args.push_back(arg->unparseToString());
  }
  return args;
}


std::vector<std::string> getFunctionCallArgNames(SgFunctionCallExp* exp) {
  std::vector<std::string> args;
  foreach_(SgExpression* arg, exp->get_args()->get_expressions()) {
    args.push_back(arg->unparseToString());
  }
  return args;

}

StencilOffset* Utils::getStencilOffset(int value, std::string var, std::string bound) {
  return new StencilOffset();
}

