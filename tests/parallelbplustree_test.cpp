#include <gtest/gtest.h>
#include "parallelbplustree.hpp"

class ParallelBplustreeBloomEnabledTest : public ::testing::Test {
	protected:
		ParallelBplustree tree;
		ParallelBplustreeBloomEnabledTest() : tree(5, std::thread::hardware_concurrency(), std::thread::hardware_concurrency(), true) {
			for (int i = 0; i < 1000; i++) {
				tree.insert(i, i+1);
			}
			tree.waitForWorkToFinish();
		};
};

class ParallelBplustreeBloomDisabledTest : public ::testing::Test {
	protected:
		ParallelBplustree tree;
		ParallelBplustreeBloomDisabledTest() : tree(5, std::thread::hardware_concurrency(), std::thread::hardware_concurrency(), false) {
			for (int i = 0; i < 1000; i++) {
				tree.insert(i, i+1);
			}
			tree.waitForWorkToFinish();
		};
};

TEST_F(ParallelBplustreeBloomEnabledTest, SearchForKeysInTreeTest) {
	std::future<std::vector<std::future<const std::vector<int> *>>> futRes;
	std::vector<std::future<const std::vector<int> *>> wrapperRes;
	const std::vector<int> *resPart;
	for (int i = 0; i < 30; i += 10) {
		std::vector<const std::vector<int> *> validKeysInTrees;
		futRes = tree.search(i);
		wrapperRes = futRes.get();
		for (int j = 0; j < wrapperRes.size(); j++) {
			resPart = wrapperRes[j].get();
			if (resPart) {
				validKeysInTrees.push_back(resPart);
			}
		}
		EXPECT_EQ(validKeysInTrees.size(), 1);
		EXPECT_EQ((*(validKeysInTrees[0]))[0], i + 1);
	}
}

TEST_F(ParallelBplustreeBloomEnabledTest, SearchForKeyNotInTreeTest) {
	std::vector<std::future<const std::vector<int> *>> wrapperRes = tree.search(1001).get();
	for (int i = 0; i < wrapperRes.size(); i++) {
		const std::vector<int> *resPart = wrapperRes[i].get();
		EXPECT_FALSE(resPart);
	}
}
