UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	std := c++20
endif
ifeq ($(UNAME), Linux)
	std :=c++2a
	libinc := -pthread
endif


# OPTIMIZED
optimized: main_optimized.o program_optimized.o node_optimized.o internalnode_optimized.o leafnode_optimized.o bplustree_optimized.o parallelbplustree_optimized.o
	g++ $(libinc) main_optimized.o program_optimized.o node_optimized.o internalnode_optimized.o leafnode_optimized.o bplustree_optimized.o parallelbplustree_optimized.o -o optimized -std=$(std)
	@echo "Optimized build compiled and linked"

main_optimized.o: ../main/src/main.cpp
	g++ -o main_optimized.o -c ../main/src/main.cpp -I ../main/inc -I ../bplustree/inc -I ../parallelbplustree/inc -std=$(std) -O2

program_optimized.o: ../main/src/program.cpp ../main/inc/program.hpp
	g++ -o program_optimized.o -c ../main/src/program.cpp -I ../main/inc -I ../bplustree/inc -I ../parallelbplustree/inc -std=$(std) -O2

node_optimized.o: ../bplustree/src/node.cpp ../bplustree/inc/node.hpp
	g++ -o node_optimized.o -c ../bplustree/src/node.cpp -I ../bplustree/inc -std=$(std) -O2

internalnode_optimized.o: ../bplustree/src/internalnode.cpp ../bplustree/inc/internalnode.hpp
	g++ -o internalnode_optimized.o -c ../bplustree/src/internalnode.cpp -I ../bplustree/inc -std=$(std) -O2

leafnode_optimized.o: ../bplustree/src/leafnode.cpp ../bplustree/inc/leafnode.hpp
	g++ -o leafnode_optimized.o -c ../bplustree/src/leafnode.cpp -I ../bplustree/inc -std=$(std) -O2

bplustree_optimized.o: ../bplustree/src/bplustree.cpp ../bplustree/inc/bplustree.hpp
	g++ -o bplustree_optimized.o -c ../bplustree/src/bplustree.cpp -I ../bplustree/inc -std=$(std) -O2

parallelbplustree_optimized.o: ../parallelbplustree/src/parallelbplustree.cpp ../parallelbplustree/inc/parallelbplustree.hpp
	g++ -o parallelbplustree_optimized.o -c ../parallelbplustree/src/parallelbplustree.cpp -I ../bplustree/inc -I ../parallelbplustree/inc -std=$(std) -O2


# DEBUG
debug: main_debug.o program_debug.o node_debug.o internalnode_debug.o leafnode_debug.o bplustree_debug.o parallelbplustree_debug.o
	g++ $(libinc) main_debug.o program_debug.o node_debug.o internalnode_debug.o leafnode_debug.o bplustree_debug.o parallelbplustree_debug.o -o debug -std=$(std)
	@echo "Debug build compiled and linked"

main_debug.o: ../main/src/main.cpp
	g++ -o main_debug.o -c ../main/src/main.cpp -I ../main/inc -I ../bplustree/inc -I ../parallelbplustree/inc -std=$(std) -g

program_debug.o: ../main/src/program.cpp ../main/inc/program.hpp
	g++ -o program_debug.o -c ../main/src/program.cpp -I ../main/inc -I ../bplustree/inc -I ../parallelbplustree/inc -std=$(std) -g

node_debug.o: ../bplustree/src/node.cpp ../bplustree/inc/node.hpp
	g++ -o node_debug.o -c ../bplustree/src/node.cpp -I ../bplustree/inc -std=$(std) -g

internalnode_debug.o: ../bplustree/src/internalnode.cpp ../bplustree/inc/internalnode.hpp
	g++ -o internalnode_debug.o -c ../bplustree/src/internalnode.cpp -I ../bplustree/inc -std=$(std) -g

leafnode_debug.o: ../bplustree/src/leafnode.cpp ../bplustree/inc/leafnode.hpp
	g++ -o leafnode_debug.o -c ../bplustree/src/leafnode.cpp -I ../bplustree/inc -std=$(std) -g


bplustree_debug.o: ../bplustree/src/bplustree.cpp ../bplustree/inc/bplustree.hpp
	g++ -o bplustree_debug.o -c ../bplustree/src/bplustree.cpp -I ../bplustree/inc -std=$(std) -g

parallelbplustree_debug.o: ../parallelbplustree/src/parallelbplustree.cpp ../parallelbplustree/inc/parallelbplustree.hpp
	g++ -o parallelbplustree_debug.o -c ../parallelbplustree/src/parallelbplustree.cpp -I ../bplustree/inc -I ../parallelbplustree/inc -std=$(std) -g


# TESTS
tests: optimized bplustree_test.o parallelbplustree_test.o
	g++ $(libinc) -lgtest -lgtest_main *_test.o node_optimized.o internalnode_optimized.o leafnode_optimized.o bplustree_optimized.o parallelbplustree_optimized.o -o tests -std=$(std)

bplustree_test.o: ../tests/bplustree_test.cpp
	g++ $(libinc) -o bplustree_test.o -c ../tests/bplustree_test.cpp -I ../bplustree/inc -std=$(std)

parallelbplustree_test.o: ../tests/parallelbplustree_test.cpp
	g++ $(libinc) -o parallelbplustree_test.o -c ../tests/parallelbplustree_test.cpp -I ../bplustree/inc -I ../parallelbplustree/inc -std=$(std)


# ALL
all: debug optimized tests


# CLEAN
clean: clean_debug clean_optimized

clean_debug:
	-rm *_debug.o debug

clean_optimized:
	-rm *_optimized.o optimized
