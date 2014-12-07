#include <iostream>
#include <algorithm>

#include "node.h"

using namespace std;

/* --------------- STATIC --------------- */

unsigned long Node::s_msgCount = 0;

void Node::sendIdDAG(Node* p_from, Node* p_to)
{
	s_msgCount++;
	p_to->receiveIdDAG(p_from);
}

/* --------------- PUBLIC --------------- */

Node::Node(int v_id)
{
	m_id = v_id;
	unordered_multiset<Node*> m_neighbours();
	unordered_multiset<Node*> m_parents();
	unordered_multiset<Node*> m_children();
}

int Node::Id()
{
	return m_id;
}

unsigned long Node::MsgCount()
{
	return s_msgCount;
}

unordered_multiset<Node*>* Node::Neighbours()
{
	return &m_neighbours;
}

unordered_multiset<Node*>* Node::Parents()
{
	return &m_parents;
}

unordered_multiset<Node*>* Node::Children()
{
	return &m_children;
}

/* --------------- PRIVATE --------------- */

void Node::addNeighbour(Node* p_n)
{
	m_neighbours.insert(p_n);
}

void Node::startBuildingDAG()
{	
	for (Node* neighbour : m_neighbours) {
		Node::sendIdDAG(this,neighbour);
	}
}

void  Node::receiveIdDAG(Node* p_from)
{
	if (p_from->Id() < m_id) {
		m_parents.insert(p_from);
	} else {
		m_children.insert(p_from);
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

void Node::yoyo()
{
	// while(true) {
		switch (m_state) {
			case State::SOURCE :
				processCaseSource();
				break;
			case State::INTERNAL :
				processCaseInternal();
				break;
			case State::SINK :
				processCaseSink();
				break;
			case State::LEADER :
				processCaseLeader();
				break;
			case State::IDLE :
				processCaseIdle();
				break;
		}
	// }
}

int Node::processCaseSource()
{
	if (m_children.size() == 0) {
		m_state = State::LEADER;
		return 1;
	}

	return 0;
}

int Node::processCaseInternal()
{
	return 0;
}

int Node::processCaseSink()
{
	return 0;
}

int Node::processCaseLeader()
{
	return 0;
}

int Node::processCaseIdle()
{
	return 0;
}