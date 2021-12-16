output: main.o node.o bplustree.o
	g++ main.o node.o bplustree.o -o output -std=c++20

main.o: main.cpp
	g++ -c main.cpp -std=c++20

node.o: node.cpp node.hpp
	g++ -c node.cpp -std=c++20

bplustree.o: bplustree.cpp bplustree.hpp
	g++ -c bplustree.cpp -std=c++20

clean:
	rm *.o output