#ifndef DEF_GRAPH
#define DEF_GRAPH

// #include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

#include "node.h"

class Node;

class Graph
{
	public:
		/* --------------- ATTRIBUTES --------------- */
		
		/* --------------- FUNCTIONS --------------- */
		Graph(std::string p_filePath);
		std::unordered_multimap<std::shared_ptr<Node>,unsigned long>* Nodes();
		std::unordered_multimap<std::shared_ptr<Node>,unsigned long>::iterator findNode(Node* p_node);
		unsigned long countNodes(Node* p_nodep_node);
		
		void yoyo();
		
		void buildDAG();
		void sendIdDAG(Node& p_from, unsigned long p_id, Node& p_to);
		void sendIdYoyo(Node& p_from, unsigned long p_id, Node& p_to);
		void sendAnswerYoyo(Node& p_from, bool p_answer, bool p_prune, Node& p_to);

		void assignLeader(Node& p_leader);

#ifndef NDEBUG
		std::mutex m_mtxDebug;
		void writeDebug(std::string p_msg);
#endif

	private:
		/* --------------- ATTRIBUTES --------------- */
		std::unordered_multimap<std::shared_ptr<Node>,unsigned long> m_nodes;
		unsigned long m_msgCount = 0;
		std::mutex m_mtxMsgCount;
		bool m_leaderIsFound = false;
		Node* m_leader;

		/* --------------- FUNCTIONS --------------- */
};

#endif