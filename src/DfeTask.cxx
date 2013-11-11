#include "DfeTask.hxx"
#include "precompiled.hxx"
#include "utils.hxx"

using namespace std;

DfeTask::DfeTask(string name, Kernel *k, vector<string> call_args) : Node(name) {
  this->name = name + boost::lexical_cast<string>(s_idCount);
  this->inputs = k->getDfeTaskInputs(call_args);
  this->outputs = k->getDfeTaskOutputs(call_args);
  this->kernel = k;
  this->call_args = call_args;
  s_idCount++;
}

string DfeTask::getCorrespondingKernelParam(string taskParam) {
  vector<string>::iterator it = find(call_args.begin(), call_args.end(), taskParam);
  return kernel->getParamName(it - call_args.begin());
}

int DfeTask::s_idCount = 0;
