#include <gtest/gtest.h>
#include "bplustree.hpp"

TEST(BplustreeTest, InsertUniqueKeysTest) {
	Bplustree tree(5);
	for (int i = 0; i < 1000; i++) {
		tree.insert(i, i + 1);
		EXPECT_EQ(i + 1, tree.getNumKeysStored());
	}
}

TEST(BplustreeTest, InsertDuplicateKeysTest) {
	Bplustree tree(5);
	for (int i = 0; i < 500; i++) {
		tree.insert(i, i + 1);
	}
	for (int i = 0; i < 500; i++) {
		tree.insert(i, i + 2);
		EXPECT_EQ(500, tree.getNumKeysStored());
	}
}
