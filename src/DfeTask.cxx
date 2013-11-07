#include "DfeTask.hxx"
#include "precompiled.hxx"
#include "utils.hxx"

using namespace std;

DfeTask::DfeTask(string name, Kernel *k, vector<string> call_args) : Node(name) {
  this->name = boost::lexical_cast<string>(s_idCount) + name;
  this->inputs = k->getDfeTaskInputs(call_args);
  this->outputs = k->getDfeTaskOutputs(call_args);
  this->kernel = k;
  s_idCount++;
}

int DfeTask::s_idCount = 0;
