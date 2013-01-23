#include "Kernel.hxx"

string Kernel::declaration() {
    return "package engine;\n\n" + imports() + "\n"
        "public class " + name + " extends Kernel {\n\n"
        "public " + name + "(KernelParameters parameters) {\n"
        "super(parameters);";
}

string Kernel::imports() {
    string imps[] = { "*",
                      "types.base.HWVar",
                      "stdlib.*",
                      "stdlib.core.*    ",
                      "stdlib.core.Count.*",
                      "stdlib.KernelMath.*",
                      "stdlib.core.Stream.OffsetExpr",
                      "types.composite.*",
                      "types.base.*"};
    list<string> imports(imps, imps + 9);
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

void Kernel::addInput(string inputName, string type, string width) {
    inputs.push_back(inputName);
    string t = "new KArrayType<HWVar>(" + type + "," + width + ")";
    declarations += "KArray<HWVar> " + inputName +
        " = io.input(\"" + inputName + "\",  " + t +");\n";
}

void Kernel::addOutput(string outputName, string type, string width) {
    // XXX this is not really required, as outputs are created
    // from source code (e.h. output_i, output_ic)
}

void Kernel::addOutput(string outputName, string type) {
    // XXX this is not really required, as outputs are created
    // from source code (e.h. output_i, output_ic)
    /*  outputs.push_back(outputName);
        output += "HWVar " + outputName + " =  io.output(\"" + outputName + "\","
        + type + ");\n";*/
}

void Kernel::addScalarInput(string inputName, string type) {
    scalars.push_back(inputName);
    declarations += "HWVar " + inputName + " =  io.scalarInput(\"" + inputName
        + "\", " + type + ");\n";
}
