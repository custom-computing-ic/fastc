# --- Commands ---
JAVAC=maxjc
JAVA =maxJavaRun
FILEC=maxfilecompile
CC=g++

# --- Project properties ---
APP        =RTM
BUILD_NAME =Builder
EXE_SIM    =$(APP)_Sim
EXE_HW     =$(APP)_Hw

HOST_DIR    =cpu
ENGINE_PKG  =engine
BUILDER     =$(ENGINE_PKG)/AppManager
MAXFILE_SIM =$(BUILD_NAME)_SIM/results/$(BUILD_NAME)_SIM.max
MAXFILE_HW  =$(BUILD_NAME)_HW/results/$(BUILD_NAME)_HW.max
MAXFILE_OBJ    =$(BUILD_NAME).o
MAXFILE_OBJ_HW =$(BUILD_NAME)_HW.o

# --- Runtime Libraries ---
SIM_MAXELEROS_LIB ="$(MAXCOMPILERDIR)/lib/maxeleros-sim/lib/"
SIM_PRELOAD       ="$(SIM_MAXELEROS_LIB)/libmaxeleros.so"

# --- Compilation Flags ---
MAX_CFLAGS=-g -I${MAXCOMPILERDIR}/include -I${MAXELEROSDIR}/include -DDESIGN_NAME=$(APP)

MAX_LFLAGS=-Wl,--hash-style=both\
 -L${MAXCOMPILERDIR}/lib -L${MAXELEROSDIR}/lib\
 -L/usr/local/lib\
 -lmaxcompilerrt -lmaxeleros -lm -lpthread -fopenmp

MAX_JFLAGS=-cp ${MAXCOMPILERDIR}/lib/MaxCompiler.jar -1.6 -d .

# --- Project Files ---
DESIGNFILES=\
 ${ENGINE_PKG}/${APP}.java \
 ${ENGINE_PKG}/Cmdwrite.java \
 ${ENGINE_PKG}/Cmdread.java \
 ${ENGINE_PKG}/AppManager.java \

HOSTCODE   =\
 ${HOST_DIR}/constructors.o\
 ${HOST_DIR}/computation.o \

maxc:
	rm -rf $(MAXCFILES)
	../../../maxcc maxc/kernels.c
	mkdir -p engine
	mv *.java engine

$(BUILDER).class:
	$(JAVAC) $(MAX_JFLAGS) $(ENGINE_PKG)/*.java

${HOST_DIR}/%.o: ${HOST_DIR}/%.c
	$(CC) $(MAX_CFLAGS) -c $< -o $@


# ---- Simulation ----

$(MAXFILE_SIM): $(BUILDER).class
	MAXAPPJCP=. $(JAVA) $(BUILDER) $(BUILD_NAME)_SIM SIM

$(MAXFILE_OBJ):  $(MAXFILE_SIM)
	$(FILEC) $< $@ $(APP)

$(APP)_sim.o: cpu/$(APP).c
	$(CXX) $< $(MAX_CFLAGS) -D__SIM__ -c -o $@

$(EXE_SIM): $(MAXFILE_OBJ) $(APP)_sim.o $(HOSTCODE)
	$(CXX) $^ $(MAX_LFLAGS) -o $@

run-sim: $(EXE_SIM)
	maxcompilersim -n $(USER)cc -c3424 restart
	LD_LIBRARY_PATH="$(SIM_MAXELEROS_LIB):$(LD_LIBRARY_PATH)" LD_PRELOAD=$(SIM_PRELOAD) ./$< "$(USER)cc0:$(USER)cc"
	maxcompilersim -n $(USER)cc -c3424 stop


# ---- Hardware ----

$(MAXFILE_HW): $(BUILDER).class
	MAXAPPJCP=. $(JAVA) $(BUILDER) $(BUILD_NAME)_HW HW

$(MAXFILE_OBJ_HW): $(MAXFILE_HW)
	$(FILEC) $< $@ $(APP)

$(APP)_hw.o:  cpu/$(APP).c
	$(CXX) $< $(MAX_CFLAGS) -c -o $@

$(EXE_HW): $(MAXFILE_OBJ_HW) $(APP)_hw.o $(HOSTCODE)
	$(CXX) $^ $(MAX_LFLAGS) -o $@

build-hw: $(EXE_HW)

run-hw:
	LD_PRELOAD=/opt/maxeler/maxeleros//lib/libmaxeleros.so $(EXE_HW) "/dev/maxeler0"


# --- Debug ---

debug:
	maxdebug -g test -d "$(USER)cc0:$(USER)cc" -n SimBuilder/results/SimBuilder.max
	dot -Tsvg test_x210cc0_fpga_a.dot > test_${USER}0_fpga_a.svg
	eog test_nx2100_fpga_a.svg

hwdebug:
	maxdebug -g test -d "/dev/maxeler0" -n HWBuilder/results/HWBuilder.max
	dot -Tsvg test_maxeler0_fpga_a.dot > test_${USER}0_fpga_a.svg
	eog test_nx2100_fpga_a.svg

# --- Other ---

clean:
	rm -f ${APP}_sim ${APP} hw MyApp_hw.o
	find . -name "*.o" -exec rm {} \;
	find . -name "*.class" -exec rm {} \;


distclean: clean
	rm -fr ${APP}_pkg SimBuilder HWBuilder
	rm -f SimBuilder.o HWBuilder.o ${APP}.o ${APP}_sim.o

.PHONY: maxc
