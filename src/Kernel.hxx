#ifndef KERNEL_HXX_
#define KERNEL_HXX_

#include <string>

using namespace std;

class Kernel {

	string name;

public:
	Kernel(string name) {
		this->name = name;
	}
	string getName() { return name; }

};

#endif /* KERNEL_HXX_ */
