#ifndef _STENCILOFFSET_H_
#define _STENCILOFFSET_H_

#include <string>
#include <map>

class StencilOffset {

public:

  std::map<std::string, int> var_offset;
  std::map<std::string, int> dim_offset;
  
  StencilOffset()  {}
  
};


#endif // _STENCILOFFSET_H_ 
