# Makefile ---- Xinyu 2012

PRJ=MyApp
BOARD_MODEL=

CC=g++
JAVAC=maxjc
JAVA=maxJavaRun
FILEC=maxfilecompile

HOST_DIR=cpu
ENGINE_DIR=engine_orig
ENGINE_PKG=${ENGINE_DIR}

MAX_JFLAGS=-cp ${MAXCOMPILERDIR}/lib/MaxCompiler.jar -1.6 -d .
MAX_CFLAGS=-O3 -Iinc -fopenmp\
					 -Iinc -I${MAXCOMPILERDIR}/include -I${MAXELEROSDIR}/include\
					 -DDESIGN_NAME=${PRJ}\
					 -I./includes\
					 -I/usr/local/include
MAX_LFLAGS=-Wl,--hash-style=both\
					 -L${MAXCOMPILERDIR}/lib\
					 -L${MAXCOMPILERDIR}/lib/maxeleros-sim/lib\
					 -L/usr/local/lib\
					 -lmaxcompilerrt -lmaxeleros -lm -lpthread -fopenmp

DESIGNFILES=\
 ${ENGINE_DIR}/${PRJ}.java \
 ${ENGINE_DIR}/Cmdwrite.java \
 ${ENGINE_DIR}/Cmdread.java \
 ${ENGINE_DIR}/AppManager.java \

HOSTCODE   =\
 ${HOST_DIR}/constructors.o\
 ${HOST_DIR}/computation.o \
 # ./src/MyApp_pkg/max_util.o


all: sim hw
# ---- HostCode   ----

${HOST_DIR}/%.o: ${HOST_DIR}/%.c
	$(CC) $(MAX_CFLAGS) -c $< -o $@

# ---- Simulation ----


SimBuilder/scratch/SimBuilder.max: ${DESIGNFILES}
	${JAVAC} ${MAX_JFLAGS} $^
	MAXAPPJCP=. ${JAVA} ${ENGINE_PKG}.AppManager SimBuilder SIM

SimBuilder.o: SimBuilder/scratch/SimBuilder.max
	${FILEC} SimBuilder/results/SimBuilder.max SimBuilder.o ${PRJ}

${PRJ}_sim.o: ${HOST_DIR}/${PRJ}.c
	${CC} $< ${MAX_CFLAGS} -D__SIM__ -c -o $@

${PRJ}_sim: SimBuilder.o ${PRJ}_sim.o $(HOSTCODE)
	${CC} -o ${PRJ}_sim ${PRJ}_sim.o SimBuilder.o $(HOSTCODE) ${MAX_LFLAGS}

sim: ${PRJ}_sim

run-sim: sim
	maxcompilersim -n $(USER)cc -c3424 restart
	#LD_LIBRARY_PATH="$(MAXCOMPILERDIR)/lib/maxeleros-sim/lib:$LD_LIBRARY_PATH" 
	LD_PRELOAD=$(MAXCOMPILERDIR)/lib/maxeleros-sim/lib/libmaxeleros.so ./MyApp_sim "$(USER)cc0:$(USER)cc"
	maxcompilersim -n $(USER)cc -c3424 stop

debug:
	maxdebug -g test -d "$(USER)cc0:$(USER)cc" -n SimBuilder/results/SimBuilder.max
	dot -Tsvg test_x210cc0_fpga_a.dot > test_${USER}0_fpga_a.svg
	eog test_nx2100_fpga_a.svg 

hwdebug:
	maxdebug -g test -d "/dev/maxeler0" -n HWBuilder/results/HWBuilder.max
	dot -Tsvg test_maxeler0_fpga_a.dot > test_${USER}0_fpga_a.svg
	eog test_nx2100_fpga_a.svg 

# ---- Hardware ----

HWBuilder/scratch/HWBuilder.max: src/MyApp_pkg/$(PRJ).java src/MyApp_pkg/HWBuilder.java ${DESIGNFILES}
	  MAXSOURCEDIRS=src $(JAVAC) $(MAX_JFLAGS) $?
		MAXSOURCEDIRS=src MAXAPPJCP=. $(JAVA) $(PRJ)_pkg.HWBuilder

par:HWBuilder/scratch/HWBuilder.max

#we do not generate the max file automatically
$(PRJ)_hw.o:
	  $(FILEC) HWBuilder/results/HWBuilder.max $(PRJ)_hw.o $(PRJ)

hw: src/MyApp_pkg/$(PRJ).o $(PRJ)_hw.o $(HOSTCODE)
	  $(CC) -o hw src/MyApp_pkg/$(PRJ).o $(PRJ)_hw.o $(HOSTCODE) $(MAX_LFLAGS)


runhw:
	LD_PRELOAD=/opt/maxeler/maxeleros//lib/libmaxeleros.so ./hw "/dev/maxeler0"

# ----

clean:
	rm -f ${PRJ}_sim ${PRJ} hw MyApp_hw.o
	find . -name "*.o" -exec rm {} \;


distclean: clean
	rm -fr ${PRJ}_pkg SimBuilder HWBuilder
	rm -f SimBuilder.o HWBuilder.o ${PRJ}.o ${PRJ}_sim.o
