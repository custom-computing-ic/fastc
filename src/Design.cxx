#include "Design.hxx"

#include <fstream>

void Design::generateCode(ostream& out) {
    list<Kernel*>::iterator it;
    for (it = kernels.begin(); it != kernels.end(); it++) {
        out << (*it)->getSource();
        out << "\n\n";
    }
}

void Design::writeCodeFiles() {
    for (Kernel* k : kernels ) {
        ofstream fout(k->getName() + ".java");
        fout << k->getSource();
    }

}

void Design::addKernel(Kernel* k) {
    kernels.push_back(k);
}

Kernel* Design::getKernel(string functionName) {
    list<Kernel*>::iterator it;
    for (it = kernels.begin(); it != kernels.end(); it++) {
        if ((*it)->getFunctionName().compare(functionName) == 0)
            return *it;
    }
    return NULL;
}
