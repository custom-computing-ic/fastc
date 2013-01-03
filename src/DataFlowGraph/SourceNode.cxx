#include "SourceNode.hxx"

using namespace std;

string SourceNode::imports() {
	return "import com.maxeler.maxcompiler.v1.kernelcompiler.Kernel;\n"
			"import com.maxeler.maxcompiler.v1.kernelcompiler.KernelParameters;\n"
			"import com.maxeler.maxcompiler.v1.kernelcompiler.types.base.HWVar;";
}

string SourceNode::kernelName() {
	return "KernelFunc";
}

string SourceNode::toMaxJ() {
	// TODO how to handle different constructor?
	return "package source;\n\n" + imports() + "\n\n"
			"public class" + kernelName() + "extends Kernel {\n\n"
			"public" + kernelName() + "(KernelParameters parameters) {\n"
			"super(parameters);";
}
