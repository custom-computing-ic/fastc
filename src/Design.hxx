#ifndef DESIGN_HXX_
#define DESIGN_HXX_

#include <list>
#include <ostream>
#include "Kernel.hxx"

using namespace std;

class Design {

    list<Kernel*> kernels;

public:
    void generateCode(ostream& out);
    void writeCodeFiles();
    void addKernel(Kernel* k);
    Kernel* getKernel(string functionName);
};

#endif /* DESIGN_HXX_ */
