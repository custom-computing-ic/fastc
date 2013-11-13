#include "StencilOffset.h"

#include <map>
#include <boost/lexical_cast.hpp>

using namespace std;

string StencilOffset::getDataflowOffsetExpresion() {

  map<string, int>::iterator it;

  string expr ("");

  bool all_zero = true;
  bool first = true;

  for (it = dim_offset.begin(); it != dim_offset.end(); it++) {
    string offset = it->first == "" ? "1" : it->first;
    string dim = boost::lexical_cast<string>(it->second);
    if (it->second != 0)
      all_zero = false;
    if (first) {
      expr += offset + "*" + dim;
      first = false;
    } else {
      expr += "+" + offset + "*" + dim;
    }
  }

  if (all_zero) return "0";
  return expr;
}
