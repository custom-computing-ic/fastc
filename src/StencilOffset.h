#ifndef _STENCILOFFSET_H_
#define _STENCILOFFSET_H_

#include <string>
#include <map>
#include "Stencil.h"

class StencilOffset {

public:
  Stencil* stencil;
  std::map<std::string, int> var_offset;
  std::map<std::string, int> dim_offset;
  std::map<std::string, std::string> var_dim; 
  
  StencilOffset(Stencil* stencil)  {this->stencil = stencil;}
  
};


#endif // _STENCILOFFSET_H_ 
