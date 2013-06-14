#include "precompiled.hxx"
#include "utils.hxx"
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
