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

TEST(BplustreeTest, SearchForKeysInTreeTest) {
	Bplustree tree(5);
	for (int i = 0; i < 500; i++) {
		tree.insert(i, i + 1);
	}
	const std::vector<int> *res;
	for (int i = 0; i < 30; i += 10) {
		res = tree.search(i);
		EXPECT_TRUE(res);
		EXPECT_EQ(res->size(), 1);
		EXPECT_EQ((*res)[0], i + 1);
	}
	tree.insert(300, 302);
	res = tree.search(300);
	EXPECT_EQ(res->size(), 2);
	EXPECT_EQ((*res)[0], 301);
	EXPECT_EQ((*res)[1], 302);
}

TEST(BplustreeTest, SearchForKeyNotInTreeTest) {
	Bplustree tree(5);
	for (int i = 0; i < 500; i++) {
		tree.insert(i, i + 1);
	}
	const std::vector<int> *res = tree.search(501);
	EXPECT_FALSE(res);
}
