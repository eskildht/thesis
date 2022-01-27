#include "parallelbplustree.hpp"

class MainPB {
	public:
		MainPB(const int order, const int threads, const int trees, const bool bloom);
		void printTreeInfo();
		void buildRandomTree(const int numInserts, const int distLower, const int distUpper);
		void searchTest(const int op);
		void deleteTest(const int op);
		void insertTest(const int op);
		void updateTest(const int op);
	private:
		ParallelBplustree tree;
};
