#ifndef DEF_NODE
#define DEF_NODE

#include <vector>

enum class State { SLEEP, SOURCE, INTERNAL, SINK, LEADER};

class Node
{
	public:
		Node(int id);
		int m_id;

		int addNeighbour(Node* n);
		void receive();

	private:
		State m_state = State::SLEEP;
		std::vector<Node*> m_neighbours;
		std::vector<int> m_parents;
		std::vector<int> m_children;
		std::vector<int> m_values;

		void send();
};

#endif