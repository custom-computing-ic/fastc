AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = maxcc

passes = $(shell find -name "src/passes/*.cxx")
passes_h = $(shell find -name "src/passes/*.hxx")

maxcc_SOURCES = src/main.cxx src/main.hxx \
                src/AstToDfgVisitor.hxx src/AstToDfgVisitor.cxx \
                src/AstToMaxJVisitor.hxx src/AstToMaxJVisitor.cxx \
                src/DotDFSVisitor.hxx src/DotDFSVisitor.cxx \
                src/Kernel.hxx src/Kernel.cxx \
                src/Design.hxx src/Design.cxx \
                src/PragmaVisitor.hxx src/PragmaVisitor.cxx \
                src/DataFlowGraph/DataFlowGraph.hxx src/DataFlowGraph/DataFlowGraph.cxx \
                src/DataFlowGraph/InputNode.hxx src/DataFlowGraph/InputNode.cxx  \
                src/DataFlowGraph/OutputNode.hxx src/DataFlowGraph/OutputNode.cxx \
                src/DataFlowGraph/Node.hxx src/DataFlowGraph/Node.cxx \
                src/DataFlowGraph/SinkNode.hxx src/DataFlowGraph/SinkNode.cxx \
                src/DataFlowGraph/SourceNode.hxx src/DataFlowGraph/SourceNode.cxx \
                src/DataFlowGraph/DFSVisitor.hxx src/DataFlowGraph/DFSVisitor.cxx \
                src/DataFlowGraph/BFSVisitor.hxx src/DataFlowGraph/BFSVisitor.cxx \
                src/precompiled.hxx src/utils.hxx src/utils.cxx \
                src/passes/ExtractOutputs.hxx \
		src/passes/ManagerExtraction.hxx \
		src/Compiler.hxx \
		src/passes/RemoveFast.hxx src/passes/Pass.hxx \
		src/passes/KernelVisitor.hxx src/passes/KernelVisitor.cxx \
		src/passes/CodeGeneration.hxx src/passes/CodeGeneration.cxx \
		src/passes/KernelExtraction.hxx src/passes/KernelExtraction.cxx \
		src/passes/TaskExtraction.hxx src/passes/TaskExtraction.cxx

BUILT_SOURCES = src/precompiled.hxx.gch
src/precompiled.hxx.gch: src/precompiled.hxx
	$(CXXCOMPILE) -c $<

dist_noinst_SCRIPTS = autogen.sh

test: all
	./tests.sh

test-remote: all
	REMOTE=true ./tests.sh

cscope:
	cscope -b -s src

.PHONY: test
