#ifndef DEF_TREE
#define DEF_TREE

#include <string>
#include <unordered_map>
#include <memory>

#include "node.h"

class Graph
{
	public:
		Graph(std::string p_filePath);
		std::unordered_map<int,std::shared_ptr<Node>>* Nodes();

		int yoyo();

	private:
		std::unordered_map<int,std::shared_ptr<Node>> m_nodes;
		int m_leaderId;

		void buildDAG();
};

#endif