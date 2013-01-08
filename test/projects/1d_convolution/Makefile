## Commands
JAVAC =maxjc -1.6 -cp $(MAXCOMPILERDIR)/lib/MaxCompiler.jar
JAVA  =maxJavaRun
FILEC =maxfilecompile

## Project properties
APP        =Convolution1d
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


#MAX_JFLAGS=-cp ${MAXCOMPILERDIR}/lib/MaxCompiler.jar -1.6 -d .
# MAX_CFLAGS=-O3 -I${MAXCOMPILERDIR}/include -I${MAXELEROSDIR}/include -DDESIGN_NAME=${PRJ} -I/usr/local/include\ -Di686
#MAX_LFLAGS=-Wl,--hash-style=both -L${MAXCOMPILERDIR}/lib -L${MAXCOMPILERDIR}/lib/maxeleros-sim/lib -L/usr/local/lib

MAXCFILES=engine/Cmdread.java engine/Cmdwrite.java engine/Convolution1d.java

maxc:
	rm -rf $(MAXCFILES)
	../../../maxcc maxc/kernels.c
	mkdir -p engine
	mv *.java engine

$(BUILDER).class:
	$(JAVAC) $(ENGINE_PKG)/*.java

cpu/%.o: cpu/%.cpp
	$(CXX) $(MAX_CFLAGS) -c $< -o $@

# --- Simulation ---

$(MAXFILE_SIM): $(BUILDER).class
	MAXAPPJCP=. $(JAVA) $(BUILDER) $(BUILD_NAME)_SIM SIM

$(MAXFILE_OBJ):  $(MAXFILE_SIM)
	$(FILEC) $< $@ $(APP)

$(APP)_sim.o: cpu/$(APP).c
	$(CXX) $< $(MAX_CFLAGS) -D__SIM__ -c -o $@

$(EXE_SIM): $(MAXFILE_OBJ) $(APP)_sim.o cpu/kernel.o
	$(CXX) $^ $(MAX_LFLAGS) -o $@

run-sim: $(EXE_SIM)
	maxcompilersim -n $(USER)cc -c3424 restart
	LD_LIBRARY_PATH="$(SIM_MAXELEROS_LIB):$(LD_LIBRARY_PATH)" LD_PRELOAD=$(SIM_PRELOAD) ./$< "$(USER)cc0:$(USER)cc"
	maxcompilersim -n $(USER)cc -c3424 stop

# ---- Simulation ----

#SimBuilder/scratch/SimBuilder.max: ${DESIGNFILES} src/MyApp_pkg/SimBuilder.java
#	${JAVAC} ${MAX_JFLAGS} $^
#	MAXAPPJCP=. ${JAVA} ${PRJ}_pkg.SimBuilder

#SimBuilder.o: SimBuilder/scratch/SimBuilder.max
#	${FILEC} SimBuilder/results/SimBuilder.max SimBuilder.o ${PRJ}

#${PRJ}_sim.o: src/MyApp_pkg/main.cpp
#	${CC} $< ${MAX_CFLAGS} -D__SIM__ -c -o $@

#${PRJ}_sim: SimBuilder.o ${PRJ}_sim.o $(HOSTCODE)
#	${CC} -o ${PRJ}_sim ${PRJ}_sim.o SimBuilder.o $(HOSTCODE) ${MAX_LFLAGS}

#sim: ${PRJ}_sim

#run-sim: sim
#	maxcompilersim -n $(USER)cc -c3424 restart
#	LD_LIBRARY_PATH="$(MAXCOMPILERDIR)/lib/maxeleros-sim/lib/:$(LD_LIBRARY_PATH)" LD_PRELOAD=$(MAXCOMPILERDIR)/lib/maxeleros-sim/lib/libmaxeleros.so ./MyApp_sim "$(USER)cc0:$(USER)cc"
#	maxcompilersim -n $(USER)cc -c3424 stop

debug:
	maxdebug -g test -d "$(USER)cc0:$(USER)cc" -n SimBuilder/results/SimBuilder.max
	dot -Tsvg test_x210cc0_fpga_a.dot > test_${USER}0_fpga_a.svg
	eog test_nx2100_fpga_a.svg

hwdebug:
	maxdebug -g test -d "/dev/maxeler1" -n HWBuilder/results/HWBuilder.max
	dot -Tsvg test_x210cc0_fpga_a.dot > test_${USER}0_fpga_a.svg
	eog test_nx2100_fpga_a.svg

# ---- Hardware ----

HWBuilder/scratch/HWBuilder.max: src/MyApp_pkg/$(PRJ).java src/MyApp_pkg/HWBuilder.java ${DESIGNFILES}
	  MAXSOURCEDIRS=src $(JAVAC) $(MAX_JFLAGS) $?
		MAXSOURCEDIRS=src MAXAPPJCP=. $(JAVA) $(PRJ)_pkg.HWBuilder

par:HWBuilder/scratch/HWBuilder.max

#we do not generate the max file automatically
$(PRJ)_hw.o:
	  $(FILEC) HWBuilder/results/HWBuilder.max $(PRJ)_hw.o $(PRJ)

hw: src/MyApp_pkg/main.o $(PRJ)_hw.o $(HOSTCODE)
	  $(CC) -o hw src/MyApp_pkg/main.o $(PRJ)_hw.o $(HOSTCODE) $(MAX_LFLAGS)


runhw:
	LD_PRELOAD=/opt/maxeler/maxeleros/lib/libmaxeleros.so ./hw "/dev/maxeler0"

# ----

# clean:
#	rm -f ${PRJ}_sim ${PRJ} hw MyApp_hw.o

clean:
	rm -f ${PRJ}_sim ${PRJ} hw MyApp_hw.o
	rm -fr ${PRJ}_pkg SimBuilder HWBuilder
	rm -f SimBuilder.o HWBuilder.o ${PRJ}.o ${PRJ}_sim.o

deploy: maxc
	./deploy.sh

.PHONY: maxc
