OPTIONS = -Di686 -Wall -openmp -O3

debug: OPTIONS += -DDEBUG -g
debug: mvo

all: mvo

mvo: src/main.cpp kernel.o 
	icpc $(OPTIONS) -o mvo src/main.cpp kernel.o

kernel.o: src/kernel.cpp inc/kernel.hpp
	icpc $(OPTIONS) -c src/kernel.cpp

clean:
	rm *.o mvo
