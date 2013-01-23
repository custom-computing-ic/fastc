#ifndef KERNEL_HXX_
#define KERNEL_HXX_

#include <string>
#include <list>
#include <assert.h>

using namespace std;

class Kernel {
    string name, source, declarations, output;
    list<string> inputs, outputs, scalars;

    string declaration();
    string imports();
    string import(list<string>  imports);

public:
    Kernel(string name);
    string getName();
    string getFunctionName();
    string getSource();
    void addSource(string source);
    void addInput(string inputName, string type);
    void addInput(string inputName, string type, string width);
    void addOutput(string outputName, string type);
    void addOutput(string outputName, string type, string width);
    void addScalarInput(string inputName, string type);
};

#endif /* KERNEL_HXX_ */
