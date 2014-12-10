#ifndef DEF_NODE
#define DEF_NODE

#include <set>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

#include "graph.h"

enum class State {SOURCE, INTERNAL, SINK, IDLE, LEADER};

class Graph;

class Node
{
	public:
		/* --------------- ATTRIBUTES --------------- */

		/* --------------- FUNCTIONS --------------- */	
#ifndef NDEBUG
		static std::string threadIdToStr();
#endif

		Node(Graph& p_graph, unsigned long v_id);
		unsigned long Id();	
		
		void yoyo();
		void addNeighbour(Node& p_n);
		void startBuildingDAG();
		void receiveIdDAG(Node& p_from, unsigned long p_id);
		void receiveIdYoyo(Node& p_from, unsigned long p_id);
		void receiveAnswerYoyo(Node& p_from, bool p_answer, bool p_prune);
		
	private:
		/* --------------- ATRIBUTES --------------- */
		Graph* m_graph;

		State m_state;
		unsigned long m_id;
		
		std::multiset<Node*> m_neighbours;
		unsigned long m_parentsCount = 0;
		std::multiset<Node*> m_children;

#ifndef NDEBUG
		unsigned long d_childrenCount = 0;
		unsigned long d_parentsCount = 0;
#endif
		
		unsigned long m_valuesSent = 0;
		std::multimap<unsigned long,Node*> m_valuesReceived;
		bool m_answer = true;
		unsigned long m_answersReceived = 0;
		std::multiset<Node*> m_negativeAnswers;
		std::multiset<Node*> m_pruneAnswers;
		
		std::mutex m_mtxChildren;
		std::mutex m_mtxValues;
		std::condition_variable m_condValues;
		std::mutex m_mtxAnswers;
		std::condition_variable m_condAnswers;

		/* --------------- FUNCTIONS --------------- */		

		void processCaseSource();
		void processCaseInternal();
		void processCaseSink();
		bool processCaseLeader();
		bool processCaseIdle();
};

#endif