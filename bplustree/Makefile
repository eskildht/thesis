output: main.o internalnode.o leafnode.o node.o bplustree.o
	g++ main.o internalnode.o leafnode.o node.o bplustree.o -o output -std=c++20

main.o: src/main.cpp
	g++ -c src/main.cpp -I inc -std=c++20 -g

bplustree.o: src/bplustree.cpp inc/bplustree.hpp
	g++ -c src/bplustree.cpp -I inc -std=c++20 -g

internalnode.o: src/internalnode.cpp inc/internalnode.hpp
	g++ -c src/internalnode.cpp -I inc -std=c++20 -g

leafnode.o: src/leafnode.cpp inc/leafnode.hpp
	g++ -c src/leafnode.cpp -I inc -std=c++20 -g

node.o: src/node.cpp inc/node.hpp
	g++ -c src/node.cpp -I inc -std=c++20 -g

clean:
	rm *.o output
