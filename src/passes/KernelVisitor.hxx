#ifndef KERNELVISITOR_HXX_
#define KERNELVISITOR_HXX_

#include "../precompiled.hxx"
#include <list>

#include "../Kernel.hxx"
#include "../Design.hxx"

using namespace boost;

class KernelVisitor: public AstSimpleProcessing {

	regex* KERNEL_FUNC;
	list<Kernel *> kernels;
	Design* design;

public:
	KernelVisitor(Design* design);
	virtual void visit(SgNode*);
	void writeKernels(ostream&);
};

#endif /* KERNELVISITOR_HXX_ */
