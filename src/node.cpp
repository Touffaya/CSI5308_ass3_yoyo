#include <iostream>
#include <algorithm>

#include "node.h"

using namespace std;

unsigned long Node::s_msgCount = 0;

Node::Node(int v_id)
{
	m_id = v_id;
	unordered_map<int,Node*> m_neighbours();
	unordered_map<int,Node*> m_parents();
	unordered_map<int,Node*> m_children();
}

int Node::getId()
{
	return m_id;
}

unsigned long Node::getCountMsg()
{
	return s_msgCount;
}

void Node::addNeighbour(int p_id,Node* p_n)
{
	m_neighbours.insert(make_pair(p_id,p_n));
}

void Node::startBuildingDAG()
{	
	for (pair<int,Node*> pair : m_neighbours) {
		sendIdDAG(pair.second);
	}

}

void Node::sendIdDAG(Node* p_to)
{
	s_msgCount++;
	(*p_to).receiveIdDAG(this);
}

void  Node::receiveIdDAG(Node* p_from)
{
	if (p_from->getId() < m_id) {
		m_parents.insert(make_pair(p_from->getId(),p_from));
	} else {
		m_children.insert(make_pair(p_from->getId(),p_from));
	}

	if (m_parents.size()+m_children.size() == m_neighbours.size()) {
		if (m_parents.empty()) {
			m_state = State::SOURCE;
		} else if (m_children.empty()) {
			m_state = State::SINK; 
		} else {
			m_state = State::INTERNAL;
		}
	}
}