#include "parallelbplustree.hpp"
#include <random>

class Program {
	public:
		Program(const int order, const int threads, const int trees, const bool bloom, const int opDistrLow, const int opDistrHigh, const int buildDistrLow, const int buildDistrHigh);
		Program(const int order, const int opDistrLow, const int opDistrHigh, const int buildDistrLow, const int buildDistrHigh);
		void printTreeInfo();
		void buildRandomTree(const int numInserts, const bool runAsOp);
		void buildTreeWithUniqueKeys(const int numInserts);
		void searchTest(const int op);
		void deleteTest(const int op);
		void insertTest(const int op, const bool runAsOp = false);
		void updateTest(const int op);
		void updateOrInsertTest(const int op);
		~Program();
	private:
		ParallelBplustree *pbtree;
		Bplustree *btree;
		std::mt19937_64 gen;
		const int opDistrLow;
		const int opDistrHigh;
		const int buildDistrLow;
		const int buildDistrHigh;
		std::uniform_int_distribution<> opDistr;
		std::uniform_int_distribution<> buildDistr;
		void printBplustreeInfo();
		void printParallelBplustreeInfo();
		std::chrono::duration<double, std::ratio<1, 1000>>::rep buildRandomBplustree(const int numInserts, std::uniform_int_distribution<> &distr);
		std::chrono::duration<double, std::ratio<1, 1000>>::rep buildRandomParallelBplustree(const int numInserts, std::uniform_int_distribution<> &distr);
};
