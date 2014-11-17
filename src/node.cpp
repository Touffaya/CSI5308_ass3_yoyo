#include <iostream>

#include "node.h"

using namespace std;

Node::Node(int id)
{
	m_id = id;
	vector<Node*> m_neighbours();
	vector<int> m_parents();
	vector<int> m_children();
}

int Node::addNeighbour(Node* n)
{
	m_neighbours.push_back(n);
	return 0;
}