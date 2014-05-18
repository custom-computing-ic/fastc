#ifndef PASS_HXX
#define PASS_HXX

#include "../Design.hxx"
#include "Config.hxx"

class Compiler;

class Pass {

protected:

  const Compiler& compiler_;

public:

  typedef Pass super;

  Pass(const Compiler& compiler) : compiler_(compiler) {}

  virtual void runPass(Design *design)=0;
  virtual string logPass()=0;

  Config& config();
};

#endif
