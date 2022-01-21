all: debug optimized


# DEBUG
debug: main_debug.o node_debug.o internalnode_debug.o leafnode_debug.o bplustree_debug.o parallelbplustree_debug.o
	g++ main_debug.o node_debug.o internalnode_debug.o leafnode_debug.o bplustree_debug.o parallelbplustree_debug.o -o debug -std=c++20
	@echo "Debug build compiled and linked"

main_debug.o: ../main.cpp
	g++ -o main_debug.o -c ../main.cpp -I ../bplustree/inc -I ../parallelbplustree/inc -std=c++20 -g

node_debug.o: ../bplustree/src/node.cpp ../bplustree/inc/node.hpp
	g++ -o node_debug.o -c ../bplustree/src/node.cpp -I ../bplustree/inc -std=c++20 -g

internalnode_debug.o: ../bplustree/src/internalnode.cpp ../bplustree/inc/internalnode.hpp
	g++ -o internalnode_debug.o -c ../bplustree/src/internalnode.cpp -I ../bplustree/inc -std=c++20 -g

leafnode_debug.o: ../bplustree/src/leafnode.cpp ../bplustree/inc/leafnode.hpp
	g++ -o leafnode_debug.o -c ../bplustree/src/leafnode.cpp -I ../bplustree/inc -std=c++20 -g

bplustree_debug.o: ../bplustree/src/bplustree.cpp ../bplustree/inc/bplustree.hpp
	g++ -o bplustree_debug.o -c ../bplustree/src/bplustree.cpp -I ../bplustree/inc -std=c++20 -g

parallelbplustree_debug.o: ../parallelbplustree/src/parallelbplustree.cpp ../parallelbplustree/inc/parallelbplustree.hpp
	g++ -o parallelbplustree_debug.o -c ../parallelbplustree/src/parallelbplustree.cpp -I ../bplustree/inc -I ../parallelbplustree/inc -std=c++20 -g


# OPTIMIZED
optimized: main_optimized.o node_optimized.o internalnode_optimized.o leafnode_optimized.o bplustree_optimized.o parallelbplustree_optimized.o
	g++ main_optimized.o node_optimized.o internalnode_optimized.o leafnode_optimized.o bplustree_optimized.o parallelbplustree_optimized.o -o optimized -std=c++20
	@echo "Optimized build compiled and linked"

main_optimized.o: ../main.cpp
	g++ -o main_optimized.o -c ../main.cpp -I ../bplustree/inc -I ../parallelbplustree/inc -std=c++20 -O2

node_optimized.o: ../bplustree/src/node.cpp ../bplustree/inc/node.hpp
	g++ -o node_optimized.o -c ../bplustree/src/node.cpp -I ../bplustree/inc -std=c++20 -O2

internalnode_optimized.o: ../bplustree/src/internalnode.cpp ../bplustree/inc/internalnode.hpp
	g++ -o internalnode_optimized.o -c ../bplustree/src/internalnode.cpp -I ../bplustree/inc -std=c++20 -O2

leafnode_optimized.o: ../bplustree/src/leafnode.cpp ../bplustree/inc/leafnode.hpp
	g++ -o leafnode_optimized.o -c ../bplustree/src/leafnode.cpp -I ../bplustree/inc -std=c++20 -O2

bplustree_optimized.o: ../bplustree/src/bplustree.cpp ../bplustree/inc/bplustree.hpp
	g++ -o bplustree_optimized.o -c ../bplustree/src/bplustree.cpp -I ../bplustree/inc -std=c++20 -O2

parallelbplustree_optimized.o: ../parallelbplustree/src/parallelbplustree.cpp ../parallelbplustree/inc/parallelbplustree.hpp
	g++ -o parallelbplustree_optimized.o -c ../parallelbplustree/src/parallelbplustree.cpp -I ../bplustree/inc -I ../parallelbplustree/inc -std=c++20 -O2


# CLEAN
clean: clean_debug clean_optimized

clean_debug:
	rm *_debug.o debug

clean_optimized:
	rm *_optimized.o optimized
