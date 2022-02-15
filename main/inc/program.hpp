#include "parallelbplustree.hpp"
#include <random>

//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET       "\033[0m"
#define RED         "\033[31m"        /* Red */
#define GREEN       "\033[32m"        /* Green */
#define YELLOW      "\033[33m"        /* Yellow */
#define MAGENTA     "\033[35m"        /* Magenta */
#define CYAN        "\033[36m"        /* Cyan */

class Program {
	public:
		Program(
				const std::string treeType,
				const int order,
				const int threads,
				const int trees,
				const bool bloom,
				const int op,
				const int opDistrLow,
				const int opDistrHigh,
				const int buildDistrLow,
				const int buildDistrHigh,
				const bool show,
				const bool batch,
				const int treeSize,
				const std::string test
				);
		~Program();
		void runTest();
	private:
		ParallelBplustree *pbtree;
		Bplustree *btree;
		std::mt19937_64 gen;
		const int op;
		const int opDistrLow;
		const int opDistrHigh;
		const int buildDistrLow;
		const int buildDistrHigh;
		const int treeSize;
		const bool show;
		const bool batch;
		const std::string test;
		std::uniform_int_distribution<> opDistr;
		std::uniform_int_distribution<> buildDistr;
		void printTreeInfo();
		void buildRandomTree(const bool runAsOp = false);
		void searchTest();
		void deleteTest();
		void insertTest();
		void updateTest();
		void printBplustreeInfo();
		void printParallelBplustreeInfo();
		std::chrono::duration<double, std::ratio<1, 1000000000>>::rep buildRandomBplustree(const int numInserts, std::uniform_int_distribution<> &distr);
		std::chrono::duration<double, std::ratio<1, 1000000000>>::rep buildRandomParallelBplustree(const int numInserts, std::uniform_int_distribution<> &distr);
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> searchBplustree();
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> searchParallelBplustree();
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> deleteBplustree();
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> deleteParallelBplustree();
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> updateBplustree();
		std::tuple<std::chrono::duration<double, std::ratio<1, 1000000000>>::rep, int, int> updateParallelBplustree();
};
