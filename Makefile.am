AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = maxcc

maxcc_SOURCES = src/main.cxx src/main.hxx \
                src/AstToDfgVisitor.hxx src/ASTtoDFGVisitor.cxx \
                src/AstToMaxJVisitor.hxx src/AstToMaxJVisitor.cxx \
                src/DotDFSVisitor.hxx src/DotDFSVisitor.cxx \
                src/MaxJVisitor.cxx \
                src/DataFlowGraph/DataFlowGraph.hxx src/DataFlowGraph/DataFlowGraph.cxx \
                src/DataFlowGraph/InputNode.hxx src/DataFlowGraph/Node.hxx \
                src/DataFlowGraph/InputNode.cxx src/DataFlowGraph/Node.cxx \
                src/DataFlowGraph/BFSVisitor.cxx \
                src/DataFlowGraph/DFSVisitor.cxx \
                src/DataFlowGraph/SinkNode.hxx src/DataFlowGraph/SinkNode.cxx \
                src/DataFlowGraph/SourceNode.hxx src/DataFlowGraph/SourceNode.cxx \
                src/DataFlowGraph/DFSVisitor.cxx \
                src/Kernel.hxx src/Kernel.cxx

BUILT_SOURCES = src/precompiled.hxx.gch
src/precompiled.hxx.gch: src/precompiled.hxx
	$(CXXCOMPILE) -c $<

dist_noinst_SCRIPTS = autogen.sh
