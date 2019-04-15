program: program.cpp RCUList.h
	g++ -std=c++11 -Wall -pthread -lurcu-memb -g program.cpp -o program

clean:
	rm program
