run: mailDB main.cpp 
	g++ -std=c++11 main.cpp mailDB.o -o $@

mailDB: mailDB.h mailDB.cpp 
	g++ -std=c++11 mailDB.cpp -c

clean:
	rm -f mailDB.o run