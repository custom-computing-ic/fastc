AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = fastc

fastc_SOURCES = src/main.cxx src/main.hxx \
  src/Kernel.hxx src/Kernel.cxx \
  src/Design.hxx src/Design.cxx \
  src/Compiler.hxx \
  src/precompiled.hxx src/utils.hxx src/utils.cxx \
  src/AstToDfgVisitor.hxx src/AstToDfgVisitor.cxx \
  src/AstToMaxJVisitor.hxx src/AstToMaxJVisitor.cxx \
  src/DotDFSVisitor.hxx src/DotDFSVisitor.cxx \
  src/PragmaVisitor.hxx src/PragmaVisitor.cxx \
  src/DataFlowGraph/DataFlowGraph.hxx src/DataFlowGraph/DataFlowGraph.cxx \
  src/DataFlowGraph/InputNode.hxx src/DataFlowGraph/InputNode.cxx  \
  src/DataFlowGraph/OutputNode.hxx src/DataFlowGraph/OutputNode.cxx \
  src/DataFlowGraph/Node.hxx src/DataFlowGraph/Node.cxx \
  src/DataFlowGraph/SinkNode.hxx src/DataFlowGraph/SinkNode.cxx \
  src/DataFlowGraph/SourceNode.hxx src/DataFlowGraph/SourceNode.cxx \
  src/DataFlowGraph/DFSVisitor.hxx src/DataFlowGraph/DFSVisitor.cxx \
  src/DataFlowGraph/BFSVisitor.hxx src/DataFlowGraph/BFSVisitor.cxx \
  src/DataFlowGraph/CounterNode.hxx src/DataFlowGraph/CounterNode.cxx\
  src/passes/ExtractOutputs.hxx \
  src/passes/ManagerExtraction.hxx \
  src/passes/RemoveFast.hxx src/passes/Pass.hxx \
  src/passes/KernelVisitor.hxx src/passes/KernelVisitor.cxx \
  src/passes/CodeGeneration.hxx src/passes/CodeGeneration.cxx \
  src/passes/KernelExtraction.hxx src/passes/KernelExtraction.cxx \
  src/passes/TaskExtraction.hxx src/passes/TaskExtraction.cxx \
  src/passes/HostCodeGeneration.hxx \
  src/passes/PragmaExtraction.hxx \
  src/passes/InlineKernels.hxx \
  src/passes/ExtractDesignConstants.hxx \
  src/passes/InputOutputExtraction.hxx \
  src/passes/BuildDFG.hxx src/passes/BuildDFG.cxx \
  src/passes/PrintDotDFG.hxx src/passes/PrintDotDFG.cxx \
  src/highlevel/HLAVisitor.hxx src/highlevel/HLAVisitor.cxx \
  src/highlevel/HighlevelAnalysis.hxx src/highlevel/HighlevelAnalysis.cxx 


#BUILT_SOURCES = src/precompiled.hxx.gch
#src/precompiled.hxx.gch: src/precompiled.hxx
#	$(CXXCOMPILE) -c $<

dist_noinst_SCRIPTS = autogen.sh tests.sh clean.sh

EXTRA_DIST=test/ scripts doc example aspects demos include

test: all
	./tests.sh

test-remote: all
	REMOTE=true ./tests.sh

cscope:
	cscope -b -s src

.PHONY: test
