#include "precompiled.hxx"
#include "Design.hxx"

#include <fstream>
#include <string>

void Design::generateCode(ostream& out) {
    foreach_ (Kernel* k, kernels) {
        out << k->getSource();
        out << "\n\n";
    }
	int i = 0; i++;
}

void Design::writeEngineFiles(string path) {
  foreach_ (Kernel* k, kernels) {
    string name = path + "/" + k->getName() + ".java";
    ofstream fout(name.c_str());
    fout << k->getSource();
  }
}

void Design::writeCPUFiles() {
}

void Design::writeCodeFiles() {
    foreach_ (Kernel* k, kernels) {
        string name = k->getName() + ".java";
        ofstream fout(name.c_str());
        fout << k->getSource();
    }
}

void Design::addKernel(Kernel* k) {
    kernels.push_back(k);
}

Kernel* Design::getKernel(string functionName) {
    foreach_(Kernel* k, kernels) {
        if (k->getFunctionName().compare(functionName) == 0)
            return k;
    }
    return NULL;
}
