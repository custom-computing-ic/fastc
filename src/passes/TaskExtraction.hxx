#ifndef _TASKEXTRACTION_HXX_
#define _TASKEXTRACTION_HXX_

#include "Pass.hxx"

class TaskExtraction : public Pass
{
public:
  TaskExtraction() {}
  void runPass(Design* design);
  string logPass();
};


#endif /* _TASKEXTRACTION_HXX_ */
