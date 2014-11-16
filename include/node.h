#ifndef DEF_NODE
#define DEF_NODE

#include <vector>

enum class State { SOURCE, INTERNAL, SINK, LEADER};

class Node
{
	public:
		Node();

		void receive();

	private:
		int id;
		std::vector<Node> neighbours;
		std::vector<Node> parents;
		std::vector<Node> children;
		std::vector<int> values;

		void send();
};

#endif