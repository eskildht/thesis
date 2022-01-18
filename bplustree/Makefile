all: optimized debug

optimized: main_optimized.o bplustree_optimized.o internalnode_optimized.o leafnode_optimized.o node_optimized.o 
	@echo "Optimized build compiled and linked"
	g++ main_optimized.o internalnode_optimized.o leafnode_optimized.o node_optimized.o bplustree_optimized.o -o optimized -std=c++20

debug: main_debug.o bplustree_debug.o internalnode_debug.o leafnode_debug.o node_debug.o 
	@echo "Debug build compiled and linked"
	g++ main_debug.o internalnode_debug.o leafnode_debug.o node_debug.o bplustree_debug.o -o debug -std=c++20

main_debug.o: src/main.cpp
	g++ -o main_debug.o -c src/main.cpp -I inc -std=c++20 -g

bplustree_debug.o: src/bplustree.cpp inc/bplustree.hpp
	g++ -o bplustree_debug.o -c src/bplustree.cpp -I inc -std=c++20 -g

internalnode_debug.o: src/internalnode.cpp inc/internalnode.hpp
	g++ -o internalnode_debug.o -c src/internalnode.cpp -I inc -std=c++20 -g

leafnode_debug.o: src/leafnode.cpp inc/leafnode.hpp
	g++ -o leafnode_debug.o -c src/leafnode.cpp -I inc -std=c++20 -g

node_debug.o: src/node.cpp inc/node.hpp
	g++ -o node_debug.o -c src/node.cpp -I inc -std=c++20 -g

main_optimized.o: src/main.cpp
	g++ -o main_optimized.o -c src/main.cpp -I inc -std=c++20 -O2

bplustree_optimized.o: src/bplustree.cpp inc/bplustree.hpp
	g++ -o bplustree_optimized.o -c src/bplustree.cpp -I inc -std=c++20 -O2

internalnode_optimized.o: src/internalnode.cpp inc/internalnode.hpp
	g++ -o internalnode_optimized.o -c src/internalnode.cpp -I inc -std=c++20 -O2

leafnode_optimized.o: src/leafnode.cpp inc/leafnode.hpp
	g++ -o leafnode_optimized.o -c src/leafnode.cpp -I inc -std=c++20 -O2

node_optimized.o: src/node.cpp inc/node.hpp
	g++ -o node_optimized.o -c src/node.cpp -I inc -std=c++20 -O2

clean: clean_debug clean_optimized

clean_debug:
	rm *_debug.o debug

clean_optimized:
	rm *_optimized.o optimized
