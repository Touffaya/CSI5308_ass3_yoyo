#include <iostream>
#include <algorithm>
#include <chrono>

#include "node.h"

using namespace std;

/* --------------- STATIC --------------- */

unsigned long Node::s_msgCount = 0;
mutex Node::s_mtxMsgCount;

void Node::sendIdDAG(Node* p_from, unsigned long p_id, Node* p_to)
{
	s_msgCount++;
	p_to->receiveIdDAG(p_from, p_id);
}

void Node::sendIdYoyo(Node* p_from, unsigned long p_id, Node* p_to)
{
	s_mtxMsgCount.lock();
	s_msgCount++;
	s_mtxMsgCount.unlock();
	p_to->receiveIdYoyo(p_from, p_id);
}

void Node::sendAnswerYoyo(Node* p_from, bool p_answer, Node* p_to, bool p_prune)
{
	s_mtxMsgCount.lock();
	s_msgCount++;
	s_mtxMsgCount.unlock();
	p_to->receiveAnswerYoyo(p_from, p_answer, p_prune);
}

/* --------------- PUBLIC --------------- */

Node::Node(unsigned long v_id)
{
	m_id = v_id;
	multiset<Node*> m_neighbours();
	multiset<Node*> m_children();
	multimap<unsigned long,Node*> m_valuesReceived;
	multiset<Node*> m_negativeAnswers;
}

unsigned long Node::Id()
{
	return m_id;
}

unsigned long Node::MsgCount()
{
	return s_msgCount;
}

std::thread Node::execute()
{
	thread m_thread(&Node::yoyo,this);
	return m_thread;
}

void Node::join()
{
	m_thread.join();
}

/* --------------- PRIVATE --------------- */

void Node::addNeighbour(Node* p_n)
{
	m_neighbours.insert(p_n);
}

void Node::startBuildingDAG()
{	
	for (Node* neighbour : m_neighbours) {
		Node::sendIdDAG(this,m_id,neighbour);
	}
}

void  Node::receiveIdDAG(Node* p_from, unsigned long p_id)
{
	if (p_id < m_id) {
		m_parentsCount++;
	}
	else {
		m_children.insert(p_from);
	}

	if (m_parentsCount+m_children.size() == m_neighbours.size()) {
		if (m_parentsCount == 0) {
			m_state = State::SOURCE;
		}
		else if (m_children.empty()) {
			m_state = State::SINK;
		}
		else {
			m_state = State::INTERNAL;
		}
	}
}

void  Node::receiveIdYoyo(Node* p_from, unsigned long p_id)
{
	m_mtxValues.lock();
	m_valuesReceived.insert(make_pair(p_id,p_from));
	m_mtxValues.unlock();
	m_condValues.notify_one();
}

void Node::receiveAnswerYoyo(Node* p_from, bool p_answer, bool p_prune)
{
	lock_guard<mutex> guardState(m_mtxState);
	m_mtxAnswers.lock();
	m_answersReceived++;
	if (!p_answer) {
		m_answer = p_answer;
		m_negativeAnswers.insert(p_from);
	} else if (p_prune) {
		lock_guard<mutex> guardChildren(m_mtxChildren);
		m_children.erase(p_from);
		if (m_children.empty()) {
			// if (m_state == State::SOURCE) m_state = State::LEADER;
			// else {
			// 	m_state = State::SINK;
			// 	m_valuesSent = 0;
			// }
			m_state = (m_state == State::SOURCE ? State::LEADER : State::SINK);
		}
	}
	m_mtxAnswers.unlock();
	m_condAnswers.notify_one();
}

void Node::yoyo()
{
	bool done = false;
	while(!done) {
		m_mtxState.lock();
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
				done = processCaseLeader();
				break;
			case State::IDLE :
				done = processCaseIdle();
				break;
		}
		m_mtxState.unlock();
		this_thread::sleep_for(chrono::milliseconds{ 500 });
	}
}

void Node::processCaseSource()
{
	if (m_valuesSent == 0) {
		lock_guard<mutex> guardChildren(m_mtxChildren);
		for (Node* child : m_children) {
			Node::sendIdYoyo(this, m_id, child);
			m_valuesSent++;
		}
		m_answersReceived = 0;
		m_answer = true;
		m_negativeAnswers.clear();
	}
	else {
		unique_lock<mutex> uniqueLockAnswers(m_mtxAnswers);
		while (m_answersReceived < m_valuesSent) {
			m_condAnswers.wait(uniqueLockAnswers);
		}

		if (!m_answer) {
			for (Node* child : m_negativeAnswers) {
				m_children.erase(child);
				m_parentsCount++;
			}
			m_state = (m_children.empty() ? State::SINK : State::INTERNAL);
		}
		m_valuesSent = 0;
	}
}

void Node::processCaseInternal()
{
	if (m_valuesSent == 0) {
		unique_lock<mutex> uniqueLockValues(m_mtxValues);
		while (m_valuesReceived.size() < m_parentsCount) {
			m_condValues.wait(uniqueLockValues);
		}
		
		auto candidate = *m_valuesReceived.begin();
		unsigned long candidateId = ( candidate.first < m_id ? candidate.first : m_id );
		for (Node* child : m_children) {
			Node::sendIdYoyo(this, candidateId, child);
			m_valuesSent++;
		}

		m_valuesReceived.clear();
		m_answer = true;
		m_answersReceived = 0;
		m_negativeAnswers.clear();
	}

	else {
		unique_lock<mutex> uniqueLockAnswers(m_mtxAnswers);
		while (m_answersReceived < m_valuesSent) {
			m_condAnswers.wait(uniqueLockAnswers);
		}
		
		if (!m_answer) {
			for (Node* child : m_negativeAnswers) {
				m_children.erase(child);
				m_parentsCount++;
			}
		}

		unsigned long min; bool answer, prune, firstIt = true;
		for (auto parent : m_valuesReceived) {
			if (firstIt) min = parent.first;
			answer = (m_answer && parent.first == min);
			prune = (answer && !firstIt);
			Node::sendAnswerYoyo(this, answer, parent.second, prune);
			firstIt = false;

			if (prune) {
				m_parentsCount--;
			}
			else if (!answer) {
				m_parentsCount--;
				m_children.insert(parent.second);
			}
		}

		m_valuesSent = 0;
	}
}

void Node::processCaseSink()
{
	unique_lock<mutex> uniqueLockValues(m_mtxValues);
	while (m_valuesReceived.size() < m_parentsCount ) {
		m_condValues.wait(uniqueLockValues);
	}

	unsigned long min = m_valuesReceived.cbegin()->first;
	if (m_valuesReceived.count(min) == m_valuesReceived.size()) {
		for (auto parent : m_valuesReceived) {
			Node::sendAnswerYoyo(this, true, parent.second, true);
		}
		m_parentsCount = 0;
		m_state = State::IDLE;
	}
	else {
		bool answer, prune, firstIt = true;
		for (auto parent : m_valuesReceived) {
			answer = (parent.first == min);
			prune = (answer && !firstIt);
			Node::sendAnswerYoyo(this, answer, parent.second, prune);
			firstIt = false;

			if (prune) {
				m_parentsCount--;
			}
			else if (!answer) {
				m_parentsCount--;
				m_children.insert(parent.second);
			}
		}
		m_state = State::INTERNAL;
	}
}

bool Node::processCaseLeader()
{
	return false;
}

bool Node::processCaseIdle()
{
	return false;
}