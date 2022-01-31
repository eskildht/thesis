#include "parallelbplustree.hpp"

class Program {
	public:
		Program(const int order, const int threads, const int trees, const bool bloom);
		void printTreeInfo();
		void buildRandomTree(const int numInserts, const int distLower, const int distUpper);
		void buildTreeWithUniqueKeys(const int numInserts);
		void searchTest(const int op);
		void deleteTest(const int op);
		void insertTest(const int op);
		void updateTest(const int op);
		void updateOrInsertTest(const int op);
	private:
		ParallelBplustree tree;
};
