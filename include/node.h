#ifndef DEF_NODE
#define DEF_NODE

#include <set>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

enum class State {SOURCE, INTERNAL, SINK, IDLE, LEADER};

class Node
{
	public:
		/* --------------- ATTRIBUTES --------------- */

		/* --------------- FUNCTIONS --------------- */	
		static unsigned long MsgCount();
		
		Node(unsigned long v_id);
		unsigned long Id();	
		
		std::thread execute();
		void join();
		
		void addNeighbour(Node* p_n);
		void startBuildingDAG();

	private:
		/* --------------- ATRIBUTES --------------- */
		static unsigned long s_msgCount;
		static std::mutex s_mtxMsgCount;

		std::thread m_thread;
		State m_state;
		unsigned long m_id;
		
		std::multiset<Node*> m_neighbours;
		unsigned long m_parentsCount = 0;
		std::multiset<Node*> m_children;
		
		unsigned long m_valuesSent = 0;
		std::multimap<unsigned long,Node*> m_valuesReceived;
		bool m_answer = true;
		unsigned long m_answersReceived = 0;
		std::multiset<Node*> m_negativeAnswers;
		
		std::mutex m_mtxState;
		std::mutex m_mtxChildren;
		std::mutex m_mtxValues;
		std::condition_variable m_condValues;
		std::mutex m_mtxAnswers;
		std::condition_variable m_condAnswers;

		/* --------------- FUNCTIONS --------------- */
		static void sendIdDAG(Node* p_from, unsigned long p_id, Node* p_to);
		static void sendIdYoyo(Node* p_from, unsigned long p_id, Node* p_to);
		static void sendAnswerYoyo(Node* p_from, bool p_answer, Node* p_to, bool p_prune);
		
		void yoyo();

		void receiveIdDAG(Node* p_from, unsigned long p_id);
		void receiveIdYoyo(Node* p_from, unsigned long p_id);
		void receiveAnswerYoyo(Node* p_from, bool p_answer, bool p_prune);
		
		void processCaseSource();
		void processCaseInternal();
		void processCaseSink();
		bool processCaseLeader();
		bool processCaseIdle();
};

#endif