#ifndef IFEVISITOR_HXX_
#define IFEVISITOR_HXX_

#include "../DfeGraph.hxx"

#include <string>
#include <vector>

class IFEVisitor {

  DfeGraph *dfe;

  public:
  IFEVisitor(DfeGraph *dfe);
  void ExtractProperties();
  void ATAPLevel();
  void CombineTasks();
  void CombineSegments();
  void OptimiseConfigurations();
  void GenerateSolutions(); 
  Offset* FindSink(DfeTask* task, string name);
  void FindSource(DfeTask* task);

};

#endif /* IFEVISITOR_HXX_ */
