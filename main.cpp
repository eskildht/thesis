#include "parallelbplustree.hpp"
#include <random>
#include <iostream>

int main() {
	ParallelBplustree tree(5, 4, 4, true);
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distr(1, 100);

	for(int i = 0; i < 100; i++) {
		int k = distr(gen);
		int v = distr(gen);
		tree.insert(k, v);
	}

	std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	tree.waitForWorkToFinish();
	std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::micro> μs_double = t2 - t1;

	std::cout << μs_double.count() << "μs spent waiting for work to finish\n";

	tree.show();
}
