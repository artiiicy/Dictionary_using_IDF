step1:	step1.o dictionary.o
	g++ -o step1 step1.o dictionary.o
step3:	step3.o dictionary.o
	g++ -o step3 step3.o dictionary.o
step4:	step4.o dictionary.o
	g++ -o step4 step4.o dictionary.o
printdict:	printdict.o dictionary.o
	g++ -o printdict printdict.o dictionary.o
consult:	consult.o dictionary.o
	g++ -o consult consult.o dictionary.o
dictionary.o step1.o step3.o step4.o printdict.o consult.o: dictionary.h
