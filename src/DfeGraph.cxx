#include "DfeGraph.hxx"

DfeGraph::DfeGraph() {
}


DfeTask* DfeGraph::addSource(DfeTask *n) {
  addTask(n);
  sources.push_front(n);
  return n;
}

DfeTask* DfeGraph::findTask(string name) {
  list<DfeTask *>::iterator it;
  for (it = tasks.begin(); it != tasks.end(); it++) {
    if ((*it)->getName() == name)
      return *it;
  }
  return NULL;
}

DfeTask* DfeGraph::addTask(DfeTask *n) {
  tasks.push_front(n);
  return n;
}

list<DfeTask *> DfeGraph::getSources() {
  list<DfeTask *> tasks_copy(sources.begin(), sources.end());
  return tasks_copy;
}

list<DfeTask *> DfeGraph::getTasks() {
  list<DfeTask *> tasks_copy(tasks.begin(), tasks.end());
  return tasks_copy;
}
