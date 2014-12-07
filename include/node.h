#ifndef DEF_NODE
#define DEF_NODE

#include <vector>
#include <unordered_set>

enum class State {SOURCE, INTERNAL, SINK, IDLE, LEADER};

class Node
{
	public:
		/* --------------- ATTRIBUTES --------------- */		
		Node(int v_id);
		int Id();

		/* --------------- FUNCTIONS --------------- */		
		static unsigned long MsgCount();
		std::unordered_multiset<Node*>* Neighbours();
		std::unordered_multiset<Node*>* Parents();
		std::unordered_multiset<Node*>* Children();
		void addNeighbour(Node* p_n);
		void startBuildingDAG();
		void yoyo();

	private:
		/* --------------- ATRIBUTES --------------- */
		static unsigned long s_msgCount;
		State m_state;
		int m_id;
		std::unordered_multiset<Node*> m_neighbours;
		std::unordered_multiset<Node*> m_parents;
		std::unordered_multiset<Node*> m_children;		
		std::vector<int> m_values;

		/* --------------- FUNCTIONS --------------- */
		static void sendIdDAG(Node* p_from, Node* p_to);
		void receiveIdDAG(Node* p_from);
		int processCaseSource();
		int processCaseInternal();
		int processCaseSink();
		int processCaseLeader();
		int processCaseIdle();
};

#endif