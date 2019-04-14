program: program.cpp RCUList.h
	g++ -std=c++11 -Wall -pthread program.cpp -o program

clean:
	rm program