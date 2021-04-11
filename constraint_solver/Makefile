VPATH = ../index/include/:../index/src/:../utility/include/:../utility/src/:../

objects = constraint_solver.o AbstractISR.o IndexConstructor.o \
	Dictionary.o

test : $(objects)
	g++ -g -o -Wall test $(objects)

constraint_solver : 
	g++ -g -o -Wall constraint_solver $(objects)

constraint_solver.o : constraint_solver.h AbstractISR.h ISR.h Vector.h \
Dictionary.h
	g++ -c constraint_solver.cpp AbstractISR.cpp ISR.cpp Vector.cpp

AbstractISR.o : AbstractISR.h

IndexConstructor.o : IndexConstructor.h

Dictionary.o : Dictionary.h

clean :
	-rm -f constraint_solver *.o