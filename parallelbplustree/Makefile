all: debug

debug: main_debug.o parallelbplustree_debug.o
	g++ *_debug.o ../bplustree/leafnode_debug.o ../bplustree/internalnode_debug.o ../bplustree/node_debug.o ../bplustree/bplustree_debug.o -o debug -std=c++20
	@echo "Debug build compiled and linked"

main_debug.o: src/main.cpp
	g++ -o main_debug.o -c src/main.cpp -I inc -I ../bplustree/inc -std=c++20 -g

parallelbplustree_debug.o: src/parallelbplustree.cpp inc/parallelbplustree.hpp
	g++ -o parallelbplustree_debug.o -c src/parallelbplustree.cpp -I inc -I ../bplustree/inc -std=c++20 -g

clean: clean_debug

clean_debug:
	rm *_debug.o debug
