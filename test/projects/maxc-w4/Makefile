## Commands
JAVAC =maxjc -1.6 -cp $(MAXCOMPILERDIR)/lib/MaxCompiler.jar
JAVA  =maxJavaRun
FILEC =maxfilecompile

## Project properties
APP        =KernelFunc
BUILD_NAME =Builder
EXE_SIM    =$(APP)_Sim

ENGINE_PKG  =engine
BUILDER     =$(ENGINE_PKG)/AppManager
MAXFILE_SIM =$(BUILD_NAME)_SIM/results/$(BUILD_NAME)_SIM.max
MAXFILE_HW  =$(BUILD_NAME)_HW/results/$(BUILD_NAME)_HW.max
MAXFILE_OBJ    =$(BUILD_NAME).o
MAXFILE_OBJ_HW =$(BUILD_NAME)_HW.o

## Runtime Libraries
SIM_MAXELEROS_LIB ="$(MAXCOMPILERDIR)/lib/maxeleros-sim/lib/"
SIM_PRELOAD       ="$(SIM_MAXELEROS_LIB)/libmaxeleros.so"

## Compilation Flags
MAX_CFLAGS=-g -I${MAXCOMPILERDIR}/include -I${MAXELEROSDIR}/include \
           -DDESIGN_NAME=$(APP)

MAX_LFLAGS=-Wl,--hash-style=both\
           -L${MAXCOMPILERDIR}/lib -L${MAXELEROSDIR}/lib\
           -L/usr/local/lib\
           -lmaxcompilerrt -lmaxeleros -lm -lpthread -openmp

all: run-sim

maxc:
	../../../maxcc maxc/*.c
	rm -rf engine && mkdir engine
	mv *.java engine

$(BUILDER).class:
	$(JAVAC) engine/*.java


# --- Simulation ---

$(MAXFILE_SIM): $(BUILDER).class
	MAXAPPJCP=. $(JAVA) $(BUILDER) $(BUILD_NAME)_SIM SIM

$(MAXFILE_OBJ):  $(MAXFILE_SIM)
	$(FILEC) $< $@ $(APP)

$(APP)_sim.o: cpu/$(APP).c
	$(CC) $< $(MAX_CFLAGS) -D__SIM__ -c -o $@

$(EXE_SIM): $(MAXFILE_OBJ) $(APP)_sim.o
	$(CC) $^ $(MAX_LFLAGS) -o $@

run-sim: $(EXE_SIM)
	maxcompilersim -n $(USER)cc -c3424 restart
	LD_LIBRARY_PATH="$(SIM_MAXELEROS_LIB):$(LD_LIBRARY_PATH)" LD_PRELOAD=$(SIM_PRELOAD) ./$< "$(USER)cc0:$(USER)cc"
	maxcompilersim -n $(USER)cc -c3424 stop


# --- Hardware ---

$(MAXFILE_HW): $(BUILDER).class
	MAXSOURCEDIRS=engine MAXAPPJCP=. $(JAVA) $(BUILDER) $(BUILD_NAME)_HW HW

$(MAXFILE_OBJ_HW): $(MAXFILE_HW)
	$(FILEC) $< $@ $(APP)

$(APP)_hw.o: cpu/$(APP).c
	$(CC) $< $(MAX_CFLAGS) -c -o $@
#	$(FILEC) $(MAXFILE_HW) $(APP)_hw.o $(APP)

build-hw: $(MAXFILE_OBJ_HW) $(APP)_hw.o
	$(CC) -o hw src/MyApp_pkg/$(PRJ).o $(PRJ)_hw.o $(MAX_LFLAGS)

run-hw:
	LD_PRELOAD=/opt/maxeler/maxeleros/lib/libmaxeleros.so ./hw "/dev/maxeler0"


# --- Other Rules ---

gdb:
	maxcompilersim -n $(USER)cc -c3424 restart
	LD_LIBRARY_PATH="$(MAXCOMPILERDIR)/lib/maxeleros-sim/lib/:$(LD_LIBRARY_PATH)" LD_PRELOAD=$(MAXCOMPILERDIR)/lib/maxeleros-sim/lib/libmaxeleros.so gdb $< "$(USER)cc0:$(USER)cc"

clean:
	find . -name "*.class" -exec $(RM) {} \;
	find . -name "*.o" -exec $(RM) {} \;
	$(RM) -r SimBuilder $(EXE_SIM)


.PHONY: gdb maxc
