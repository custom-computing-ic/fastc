#include "Kernel.hxx"

string Kernel::declaration() {
	return "package engine;\n\n" + imports() + "\n\n"
			"public class " + name + " extends Kernel {\n\n"
			"public " + name + "(KernelParameters parameters) {\n"
			"super(parameters);";
}

string Kernel::imports() {
	string imps[] = { "Kernel", "KernelParameters", "types.base.HWVar",
	//"stdlib.core.Stream.OffsetExpr",
			"stdlib.core.CounterChain" };
	list<string> imports(imps, imps + 4);
	return import(imports);

}

string Kernel::import(list<string> imports) {
	string import = "";
	list<string>::iterator it;
	for (it = imports.begin(); it != imports.end(); it++)
		import += "import com.maxeler.maxcompiler.v1.kernelcompiler." + (*it)
				+ ";\n";
	return import;
}

Kernel::Kernel(string name) {
	this->name = name;
	declarations = declaration() + "\n";
}
string Kernel::getName() {
	return name;
}
string Kernel::getFunctionName() {
	// XXX for now assume kernels are kernel_<KernelName>
	return "kernel_" + name;
}
string Kernel::getSource() {
	return declarations + "\n" + source + "\n" + output;
}
void Kernel::addSource(string source) {
	this->source += source;
}
void Kernel::addInput(string inputName, string type) {
	inputs.push_back(inputName);
	declarations += "HWVar " + inputName + " =  io.input(\"" + inputName
			+ "\", " + type + ");\n";
}

void Kernel::addOutput(string outputName, string type) {
	outputs.push_back(outputName);
	output += "HWVar " + outputName + " =  io.output(\"" + outputName + "\","
			+ type + ");\n";
}

void Kernel::addScalarInput(string inputName, string type) {
	scalars.push_back(inputName);
	declarations += "HWVar " + inputName + " =  io.scalarInput(\"" + inputName
			+ "\", " + type + ");\n";
}
