program: program.cpp RCUList.h
	g++ -std=c++11 -Wall -pthread -lurcu-memb -O3 program.cpp -o program

clean:
	rm program
