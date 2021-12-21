output: main.o node.o bplustree.o
	g++ main.o node.o bplustree.o -o output -std=c++20

main.o: src/main.cpp
	g++ -c src/main.cpp -I inc -std=c++20

node.o: src/node.cpp inc/node.hpp
	g++ -c src/node.cpp -I inc -std=c++20

bplustree.o: src/bplustree.cpp inc/bplustree.hpp
	g++ -c src/bplustree.cpp -I inc -std=c++20

clean:
	rm *.o output
