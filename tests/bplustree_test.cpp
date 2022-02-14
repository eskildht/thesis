#include <gtest/gtest.h>
#include "bplustree.hpp"

class BplustreeTest : public ::testing::Test {
	protected:
		Bplustree tree;
		BplustreeTest() : tree(5) {
			for (int i = 0; i < 1000; i++) {
				tree.insert(i, i+1);
			}
		};
};

TEST(BplustreeInsertTest, InsertUniqueKeysTest) {
	Bplustree tree(5);
	for (int i = 0; i < 1000; i++) {
		tree.insert(i, i + 1);
		EXPECT_EQ(i + 1, tree.getNumKeysStored());
	}
}

TEST(BplustreeInsertTest, InsertDuplicateKeysTest) {
	Bplustree tree(5);
	for (int i = 0; i < 500; i++) {
		tree.insert(i, i + 1);
	}
	for (int i = 0; i < 500; i++) {
		tree.insert(i, i + 2);
		EXPECT_EQ(500, tree.getNumKeysStored());
	}
}

TEST_F(BplustreeTest, SearchForKeysInTreeTest) {
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

TEST_F(BplustreeTest, SearchForKeyNotInTreeTest) {
	const std::vector<int> *res = tree.search(1001);
	EXPECT_FALSE(res);
}

TEST_F(BplustreeTest, UpdateKeyInTreeTest) {
	tree.update(500, {1, 2});
	const std::vector<int> *res = tree.search(500);
	EXPECT_TRUE(res);
	EXPECT_EQ(res->size(), 2);
	EXPECT_EQ((*res)[0], 1);
	EXPECT_EQ((*res)[1], 2);
}

TEST_F(BplustreeTest, UpdateKeyNotInTreeTest) {
	tree.update(1001, {1, 2});
	const std::vector<int> *res = tree.search(1001);
	EXPECT_FALSE(res);
	tree.update(1001, {1, 2}, true);
	res = tree.search(1001);
	EXPECT_TRUE(res);
	EXPECT_EQ(res->size(), 2);
	EXPECT_EQ((*res)[0], 1);
	EXPECT_EQ((*res)[1], 2);
}

TEST_F(BplustreeTest, DeleteKeyInTreeTest) {
	const std::vector<int> *res = tree.search(500);
	EXPECT_TRUE(res);
	int oldSize = tree.getNumKeysStored();
	tree.remove(500);
	res = tree.search(500);
	EXPECT_FALSE(res);
	EXPECT_EQ(oldSize - 1, tree.getNumKeysStored());
}

TEST_F(BplustreeTest, DeleteKeyNotInTreeTest) {
	const std::vector<int> *res = tree.search(1001);
	EXPECT_FALSE(res);
	int oldSize = tree.getNumKeysStored();
	tree.remove(1001);
	res = tree.search(1001);
	EXPECT_FALSE(res);
	EXPECT_EQ(oldSize, tree.getNumKeysStored());
}
