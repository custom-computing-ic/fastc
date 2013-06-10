#ifndef PASS_HXX
#define PASS_HXX

#include "../Design.hxx"

class Pass
{
public:
  Pass(){}
  virtual void runPass(Design *design)=0;
  virtual string logPass()=0;
};

#endif
