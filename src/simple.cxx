#include "precompiled.hxx"
#include <iterator>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace boost;


class DFGNode {
	protected:
		string name;
	public:
		DFGNode(string name);
};

DFGNode :: DFGNode(string name) {
	this->name = name;
}

class DFGInputNode : public DFGNode  {
	public:
		DFGInputNode(string name);
};

DFGInputNode :: DFGInputNode(string name) : DFGNode(name) {}

class visitorTraversal : public AstSimpleProcessing {
	set<DFGInputNode> inputs;
//	set<Stream> outputs;
        regex PRAGMA_IN; //, PRAGMA_OUT("out (\\w*)";

	public:
		visitorTraversal();
		virtual void visit (SgNode* n);
		virtual void atTraversalEnd();
};

visitorTraversal :: visitorTraversal() {
	PRAGMA_IN = regex("in (\\w*)");
}

void visitorTraversal :: visit(SgNode *n) {
	if (isSgForStatement(n) != NULL)
		cout << "Found a for loop..." << endl;
	if (isSgPragma(n))  {
		printf("Found a pragma.. \n");
		SgPragma* pragma = (SgPragma *)n;
		string s = pragma->get_pragma();
		cout << "Found pragma:" << cout << s << endl;
		cmatch sm;
		if ( regex_match(s.c_str(), sm, PRAGMA_IN) ) {
			DFGInputNode inputNode = DFGInputNode(sm[1]);
//			inputs.insert(inputNode);
		} /* else if (regex_match(s.c_str(), sm, PRAGMA_OUT) {
			outputs.insert(sm[1]);
		} */
			
	}
}

void visitorTraversal :: atTraversalEnd() {
	cout << "Traversal end" << endl;
	//copy(inputs.begin(), inputs.end(), ostream_iterator<string>(cout, " "));
}

int main( int argc, char * argv[] ) {

	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);

	// Run internal consistency tests on AST
	AstTests::runAllTests(project);

	// Manipulation of the AST
	// generateDOT( *project );
	// generateAstGraph(project, 20000);
	visitorTraversal visitor;
	visitor.traverseInputFiles(project, preorder);
	

	// Generate source code from AST and call the vendor's compiler
	return backend(project);
}
