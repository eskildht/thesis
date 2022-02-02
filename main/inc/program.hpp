#include "parallelbplustree.hpp"
#include <random>

class Program {
	public:
		Program(const int order, const int threads, const int trees, const bool bloom, const int opDistrLow, const int opDistrHigh, const int buildDistrLow, const int buildDistrHigh);
		Program(const int order, const int opDistrLow, const int opDistrHigh, const int buildDistrLow, const int buildDistrHigh);
		void printTreeInfo();
		void buildRandomTree(const int numInserts, const bool show = false, const bool runAsOp = false);
		void searchTest(const int op, const int treeSize, const bool show);
		void deleteTest(const int op, const int treeSize, const bool show);
		void insertTest(const int op, const bool show);
		void updateTest(const int op, const int treeSize, const bool show);
		void updateOrInsertTest(const int op, const int treeSize, const bool show);
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
		std::chrono::duration<double, std::ratio<1, 1000000000>>::rep buildRandomBplustree(const int numInserts, std::uniform_int_distribution<> &distr);
		std::chrono::duration<double, std::ratio<1, 1000000000>>::rep buildRandomParallelBplustree(const int numInserts, std::uniform_int_distribution<> &distr);
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> searchBplustree(const int op);
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> searchParallelBplustree(const int op);
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> deleteBplustree(const int op);
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> deleteParallelBplustree(const int op);
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> updateBplustree(const int op);
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> updateParallelBplustree(const int op);
};
