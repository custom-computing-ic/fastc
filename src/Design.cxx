#include "Design.hxx"

void Design::generateCode(ostream& out) {
	list<Kernel*>::iterator it;
	for (it = kernels.begin(); it != kernels.end(); it++) {
		out << (*it)->getSource();
		out << "\n\n";
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
