#include "parallelbplustree.hpp"

int main() {
	ParallelBplustree tree(5, 4);
	tree.insert(1, 1);
	tree.insert(2, 2);
	tree.insert(3, 3);
	tree.insert(4, 4);
	tree.insert(5, 5);
	tree.insert(6, 6);
	tree.insert(7, 7);
	tree.insert(8, 8);
	tree.insert(9, 9);
	tree.insert(10, 1);
	tree.insert(11, 2);
	tree.insert(12, 3);
	tree.insert(13, 4);
	tree.insert(14, 5);
	tree.insert(15, 6);
	tree.insert(16, 7);
	tree.insert(17, 8);
	tree.insert(18, 9);
	tree.insert(19, 10);
	tree.insert(20, 11);
	tree.insert(21, 12);
	tree.insert(22, 13);
	tree.insert(23, 14);
	tree.show();
}
