#ifndef DEF_NODE
#define DEF_NODE

#include <vector>
#include <unordered_map>

enum class State {SLEEP, SOURCE, INTERNAL, SINK, LEADER};

class Node
{
	public:
		Node(int v_id);
		int Id();
		static unsigned long MsgCount();
		std::unordered_map<int,Node*>* Neighbours();
		std::unordered_map<int,Node*>* Parents();
		std::unordered_map<int,Node*>* Children();

		void addNeighbour(int p_id, Node* p_n);
		void startBuildingDAG();
		void yoyo();

	private:
		static unsigned long s_msgCount;
		State m_state = State::SLEEP;
		int m_id;
		std::unordered_map<int,Node*> m_neighbours;
		std::unordered_map<int,Node*> m_parents;
		std::unordered_map<int,Node*> m_children;		
		std::vector<int> m_values;

		void sendIdDAG(Node* p_to);
		void receiveIdDAG(Node* p_from);
};

#endif