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
  src/StencilAstToMaxJVisitor.hxx src/StencilAstToMaxJVisitor.cxx \
  src/DotDFSVisitor.hxx src/DotDFSVisitor.cxx \
  src/DotPrint.hxx src/DotPrint.cxx\
  src/DfeTask.hxx src/DfeTask.cxx \
  src/DfeGraph.hxx src/DfeGraph.cxx \
  src/Stencil.h src/Stencil.cc \
  src/StencilCodeGenerator.h src/StencilCodeGenerator.cc \
  src/StencilOffset.h src/StencilOffset.cc \
  src/DataFlowGraph/DataFlowGraph.hxx src/DataFlowGraph/DataFlowGraph.cxx \
  src/DataFlowGraph/InputNode.hxx src/DataFlowGraph/InputNode.cxx  \
  src/DataFlowGraph/OutputNode.hxx src/DataFlowGraph/OutputNode.cxx \
  src/DataFlowGraph/Node.hxx src/DataFlowGraph/Node.cxx \
  src/DataFlowGraph/SinkNode.hxx src/DataFlowGraph/SinkNode.cxx \
  src/DataFlowGraph/SourceNode.hxx src/DataFlowGraph/SourceNode.cxx \
  src/DataFlowGraph/MuxNode.hxx src/DataFlowGraph/MuxNode.cxx\
  src/DataFlowGraph/DFSVisitor.hxx src/DataFlowGraph/DFSVisitor.cxx \
  src/DataFlowGraph/BFSVisitor.hxx src/DataFlowGraph/BFSVisitor.cxx \
  src/DataFlowGraph/TopSortVisitor.hxx src/DataFlowGraph/TopSortVisitor.cxx \
  src/DataFlowGraph/CounterNode.hxx src/DataFlowGraph/CounterNode.cxx\
  src/passes/ExtractOutputs.hxx \
  src/passes/GenerateAnalysisOverview.h src/passes/GenerateAnalysisOverview.cc \
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
  src/passes/ExtractStencil.h src/passes/ExtractStencil.cc \
  src/passes/MergeKernels.h src/passes/MergeKernels.cc \
  src/highlevel/HLAVisitor.hxx src/highlevel/HLAVisitor.cxx \
  src/highlevel/HighlevelAnalysis.hxx src/highlevel/HighlevelAnalysis.cxx \
  src/ife/IFEVisitor.hxx src/ife/IFEVisitor.cxx \
  src/ife/Segment.h src/ife/Configuration.h src/ife/Partition.h \
  src/DfeTopSortVisitor.hxx src/DfeTopSortVisitor.cxx \
  src/ife/IdlefunctionElimination.hxx src/ife/IdlefunctionElimination.cxx \
  src/xml/xmlParser.hxx src/xml/xmlParser.cxx

TESTS = \
  test/common.sh test/highlevel.sh test/applications.sh test/new.sh

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

run:
	./fastc test/testApplications/maxc/RTM.c
	dot -Tps analysis_overview.dot > 1.ps
	evince 1.ps
dot:
	dot -Tps do_forward_graph.dot > low_level.ps
	evince low_level.ps
.PHONY: test
