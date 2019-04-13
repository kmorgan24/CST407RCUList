program: program.cpp RCUList.h
	g++ -std=c++11 -Wall program.cpp -o program

clean:
	rm program