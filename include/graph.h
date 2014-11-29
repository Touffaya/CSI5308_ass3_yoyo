#ifndef DEF_TREE
#define DEF_TREE

#include <string>
#include <unordered_map>

#include "node.h"

class Graph
{
	public:
		Graph(std::string p_filePath);
		int yoyo();

	private:
		std::unordered_map<int,Node> m_nodes;
		void buildDAG();
};

#endif