#include "MaxJVisitor.hxx"

using namespace std;

void MaxJVisitor::visit(Node *n) {
    code += n->toMaxJ() + ";\n";
}

string MaxJVisitor::getMaxJCode() {
    return code;
}
