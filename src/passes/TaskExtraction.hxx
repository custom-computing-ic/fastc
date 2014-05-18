#ifndef _TASKEXTRACTION_HXX_
#define _TASKEXTRACTION_HXX_

#include "Pass.hxx"

class TaskExtraction : public Pass
{
public:
  TaskExtraction(const Compiler& compiler) : super(compiler) {}
  void runPass(Design* design);
  string logPass();
};


#endif /* _TASKEXTRACTION_HXX_ */
