#include "parallelbplustree.hpp"
#include <random>

class Program {
	public:
		Program(const int order, const int threads, const int trees, const bool bloom, const int distrLow, const int distrHigh);
		Program(const int order, const int distrLow, const int distrHigh);
		void printTreeInfo();
		void buildRandomTree(const int numInserts, const int distLower, const int distUpper);
		void buildTreeWithUniqueKeys(const int numInserts);
		void searchTest(const int op);
		void deleteTest(const int op);
		void insertTest(const int op);
		void updateTest(const int op);
		void updateOrInsertTest(const int op);
		~Program();
	private:
		ParallelBplustree *pbtree;
		Bplustree *btree;
		std::mt19937_64 gen;
		std::uniform_int_distribution<> distr;
		void printBplustreeInfo();
		void printParallelBplustreeInfo();
		void buildRandomBplustree(const int numInserts, const int distLower, const int distUpper);
		void buildRandomParallelBplustree(const int numInserts, const int distLower, const int distUpper);
};
