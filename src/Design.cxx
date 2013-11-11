#include "precompiled.hxx"
#include "Design.hxx"

#include <fstream>
#include <string>

void Design::generateCode(ostream& out) {
    foreach_ (Kernel* k, kernels) {
        out << k->generateSourceCode();
        out << "\n\n";
    }
    int i = 0; i++;
}


void Design::writeEngineFiles(string path) {
  foreach_ (Kernel* k, kernels) {
    string name = path + "/" + k->getName() + ".java";
    ofstream fout(name.c_str());
    fout << k->generateSourceCode();
  }
}


void Design::writeCPUFiles() {
}


void Design::writeCodeFiles() {
  foreach_ (Kernel* k, kernels) {
    string name = k->getName() + ".java";
    ofstream fout(name.c_str());
    fout << k->generateSourceCode();
  }
}


void Design::addKernel(Kernel* k) {
    kernels.push_back(k);
}


Kernel* Design::getKernel(string functionName) {
    foreach_(Kernel* k, kernels) {
        if (k->getFunctionName() == functionName)
            return k;
    }
    return NULL;
}


Kernel* Design::getKernelMatchingFunctionCall(SgFunctionCallExp *fcall) {
  string fname = fcall->getAssociatedFunctionSymbol()->get_name();
  return this->getKernel(fname);
}


void Design::addDfeTask(DfeTask* task) {
  string fname = task->getName();

  map<string, set<string> >::iterator it;

  // compute in deps of the task
  vector<string> in_deps;
  foreach_(string s, task->getInputs()) {
    it = outputs_to_fname.find(s);
    if (it != outputs_to_fname.end()) {
      foreach_(string f, it->second) {
	in_deps.push_back(f);
      }
    }
  }

  if (in_deps.size() == 0) {
    dfe->addSource(task);
  } else {
    dfe->addTask(task);
    foreach_(string s, in_deps) {
      DfeTask *n = dfe->findTask(s);
      if (n != NULL)
        n->addNeighbour(task);
    }
  }

  // add the outputs of this task to the mapping
  foreach_(string s, task->getOutputs()) {
    it = outputs_to_fname.find(s);
    if (it == outputs_to_fname.end()) {
      set<string> ss;
      ss.insert(fname);
      outputs_to_fname[s] = ss;
    } else {
      it->second.insert(fname);
    }
  }

}
