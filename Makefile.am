AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = maxcc

maxcc_SOURCES = src/main.cxx src/main.hxx \
                src/AstToDfgVisitor.hxx src/ASTtoDFGVisitor.cxx \
                src/AstToMaxJVisitor.hxx src/AstToMaxJVisitor.cxx \
                src/DotDFSVisitor.hxx src/DotDFSVisitor.cxx \
                src/MaxJVisitor.cxx \
                src/DataFlowGraph/InputNode.hxx src/DataFlowGraph/Node.hxx \
                src/DataFlowGraph/BFSVisitor.cxx src/DataFlowGraph/DataFlowGraph.cxx \
                src/DataFlowGraph/DFSVisitor.cxx \
                src/DataFlowGraph/InputNode.cxx src/DataFlowGraph/Node.cxx \
                src/precompiled.hxx

dist_noinst_SCRIPTS = autogen.sh
