/*
	Yoyo-Algorithm : processing of yoyo-algorithm for message complexity.
    Copyright (C) 2014  Arthur Bourjac

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// #define NDEBUG
#include <iostream>
#include <algorithm>
#include <chrono>
#include <cassert>
#include <sstream>

#include "node.h"

using namespace std;

/* --------------- STATIC --------------- */
#ifndef NDEBUG
string Node::threadIdToStr()
{
	auto thr_id = this_thread::get_id();
	stringstream ss;
	ss << thr_id;
	return ss.str();
}
#endif

/* --------------- PUBLIC --------------- */

Node::Node(Graph& p_graph, unsigned long v_id)
{
	m_graph = &p_graph;
	assert(v_id >= 0);
	m_id = v_id;
	assert(m_neighbours.empty());
	assert(m_children.empty());
	assert(m_valuesReceived.empty());
	assert(m_negativeAnswers.empty());
	assert(m_pruneAnswers.empty());
}

unsigned long Node::Id()
{
	return m_id;
}


void Node::addNeighbour(Node& p_n)
{
	assert(m_graph->findNode(&p_n) != m_graph->Nodes()->end());
	assert(m_graph->countNodes(&p_n) == 1);
#ifndef NDEBUG
	unsigned long d_size = m_neighbours.size();
#endif
	assert(m_neighbours.count(&p_n) == 0);
	assert(m_neighbours.find(&p_n) == m_neighbours.end());
	m_neighbours.insert(&p_n);
	assert(m_neighbours.size() == d_size+1);
	assert(m_neighbours.count(&p_n) == 1);
	assert(m_neighbours.find(&p_n) != m_neighbours.end());
	assert(*m_neighbours.find(&p_n) == &p_n);
}

void Node::startBuildingDAG()
{	
	assert(!m_neighbours.empty());
	for (Node* neighbour : m_neighbours) {
		m_graph->sendIdDAG(*this,m_id,*neighbour);
	}
}

void  Node::receiveIdDAG(Node& p_from, unsigned long p_id)
{
	assert(m_graph->findNode(&p_from) != m_graph->Nodes()->end());
	assert(m_graph->countNodes(&p_from) == 1);
	assert(m_neighbours.count(&p_from) == 1);
	assert(m_neighbours.find(&p_from) != m_neighbours.end());
	assert(p_id >= 0);
	if (p_id < m_id) {
		++m_parentsCount;
	}
	else {
#ifndef NDEBUG
		unsigned long d_size = m_children.size();
#endif
		assert(m_children.count(&p_from) == 0);
		assert(m_children.find(&p_from) == m_children.end());
		m_children.insert(&p_from);
		assert(m_children.size() == d_size+1);
		assert(m_children.count(&p_from) == 1);
		assert(m_children.find(&p_from) != m_children.end());
		assert(*m_children.find(&p_from) == &p_from);
	}

	if (m_parentsCount+m_children.size() == m_neighbours.size()) {
		if (m_parentsCount == 0) {
			assert(m_parentsCount == 0 && !m_children.empty());
			m_state = State::SOURCE;
		}
		else if (m_children.empty()) {
			assert(m_parentsCount > 0 && m_children.empty());
			m_state = State::SINK;
		}
		else {
			assert(m_parentsCount > 0 && !m_children.empty());
			m_state = State::INTERNAL;
		}
	}
}

void  Node::receiveIdYoyo(Node& p_from, unsigned long p_id)
{
#ifndef NDEBUG
	/*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" receives id:"+to_string(p_id)+" from "+to_string(p_from.Id()));
#endif
	assert(m_graph->findNode(&p_from) != m_graph->Nodes()->end());
	assert(m_graph->countNodes(&p_from) == 1);
	assert(p_id >= 0);
#ifndef NDEBUG
	// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits for m_mtxValues");
#endif
	unique_lock<mutex> uniqueLockValues(m_mtxValues);
	assert(uniqueLockValues.owns_lock());
#ifndef NDEBUG
	// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxValues");
	unsigned long d_size = m_valuesReceived.size();
#endif
	m_valuesReceived.insert(make_pair(p_id,&p_from));
	assert(m_valuesReceived.size() == d_size+1);
	assert(m_valuesReceived.count(p_id) >= 1);
	assert(m_valuesReceived.find(p_id) != m_valuesReceived.end());
	uniqueLockValues.unlock();
	assert(!uniqueLockValues.owns_lock());
#ifndef NDEBUG
	// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") released m_mtxValues");
#endif
	m_condValues.notify_one();
#ifndef NDEBUG
	// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" notified m_condValues");
#endif
}

void Node::receiveAnswerYoyo(Node& p_from, bool p_answer, bool p_prune)
{
#ifndef NDEBUG
	// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" enters");
	/*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" receives "+(p_answer?"true":"false")+"/"+(p_prune?"true":"false")+" from "+to_string(p_from.Id()));
#endif
	assert(m_graph->findNode(&p_from) != m_graph->Nodes()->end());
	assert(m_graph->countNodes(&p_from) == 1);
#ifndef NDEBUG
	// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits for m_mtxAnswers");
#endif
	unique_lock<mutex> uniqueLockAnswers(m_mtxAnswers);
	assert(uniqueLockAnswers.owns_lock());
#ifndef NDEBUG
	// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxAnswers");
#endif

	m_answersReceived++;
	if (!p_answer) {
		m_answer = false;
#ifndef NDEBUG
		unsigned long d_size = m_negativeAnswers.size();
#endif
		assert(m_negativeAnswers.count(&p_from) == 0);
		assert(m_negativeAnswers.find(&p_from) == m_negativeAnswers.end());
		m_negativeAnswers.insert(&p_from);
		assert(m_negativeAnswers.size() == d_size+1);
		assert(m_negativeAnswers.count(&p_from) == 1);
		assert(m_negativeAnswers.find(&p_from) != m_negativeAnswers.end());
		assert(*m_negativeAnswers.find(&p_from) == &p_from);
	}
	if (p_prune) {
#ifndef NDEBUG
		unsigned long d_size = m_pruneAnswers.size();
#endif
		assert(m_pruneAnswers.count(&p_from) == 0);
		assert(m_pruneAnswers.find(&p_from) == m_pruneAnswers.end());
		m_pruneAnswers.insert(&p_from);
		assert(m_pruneAnswers.size() == d_size+1);
		assert(m_pruneAnswers.count(&p_from) == 1);
		assert(m_pruneAnswers.find(&p_from) != m_pruneAnswers.end());
		assert(*m_pruneAnswers.find(&p_from) == &p_from);
	}
	m_condAnswers.notify_one();
}

/* --------------- PRIVATE --------------- */

void Node::yoyo()
{
	bool done = false;
	while (!done) {
#ifndef NDEBUG
		cout << m_id << " processing" << endl;
#endif
		switch (m_state) {
			case State::SOURCE :
				processCaseSource();
				if (m_children.empty()) {
					m_state = (m_parentsCount > 0 ? State::SINK : State::LEADER);
				}
				else if (m_parentsCount) {
					m_state = State::INTERNAL;
				}
				break;
			case State::INTERNAL :
				processCaseInternal();
				assert(m_children.empty() == (m_parentsCount <= 0));
				if (m_children.empty() && m_parentsCount == 0)
					m_state = State::IDLE;
				break;
			case State::SINK :
				processCaseSink();
				assert(m_children.empty() == (m_parentsCount <= 0));
				m_state = ( m_parentsCount ? State::INTERNAL : State::IDLE);
				break;
			case State::LEADER :
				assert(m_children.empty() && m_parentsCount == 0);
				done = processCaseLeader();
				break;
			case State::IDLE :
				assert(m_children.empty() && m_parentsCount == 0);
				done = processCaseIdle();
				break;
		}
		this_thread::sleep_for(chrono::milliseconds{ 1000 });
	}
}

void Node::processCaseSource()
{
#ifndef NDEBUG
	bool d_cond = false;
#endif
	assert(!m_children.empty());
	if (m_valuesSent == 0) {
		assert(m_parentsCount == 0);
#ifndef NDEBUG
		d_cond = true;
		d_childrenCount = m_children.size();
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits for m_mtxChildren");
#endif
		unique_lock<mutex> uniqueLockChildren(m_mtxChildren);
		assert(uniqueLockChildren.owns_lock());

#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxChildren");
		unsigned long d_size = m_children.size();
#endif
		m_answer = true;
		m_answersReceived = 0;
		m_negativeAnswers.clear();
		m_pruneAnswers.clear();
		assert(m_negativeAnswers.empty());
		assert(m_pruneAnswers.empty());
		for (Node* child : m_children) {
			m_graph->sendIdYoyo(*this, m_id, *child);
			m_valuesSent++;
#ifndef NDEBUG
			 d_size--;
#endif
		}
		assert(d_size == 0 && m_valuesSent == d_childrenCount);
	}
	else {
		assert (m_valuesSent == d_childrenCount);
#ifndef NDEBUG
		d_cond = true;
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits for m_mtxAnswers");
#endif
		unique_lock<mutex> uniqueLockAnswers(m_mtxAnswers);
		assert(uniqueLockAnswers.owns_lock());
#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxAnswers");
		/*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits m_condAnswers");
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+ " : waits for answer from "+to_string((*m_children.begin())->Id()));
#endif
		while (m_answersReceived < m_valuesSent) {
#ifndef NDEBUG
			// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") released m_mtxAnswers (waiting)");
#endif
			m_condAnswers.wait(uniqueLockAnswers);
		}
		assert(m_answersReceived == m_valuesSent);
		assert(m_answersReceived == d_childrenCount);
		assert(uniqueLockAnswers.owns_lock());
#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxAnswers");
		m_answersReceived = 0;
#endif

		for (Node* toPrune : m_pruneAnswers) {
#ifndef NDEBUG
			unsigned long d_size = m_children.size();
#endif
			assert(m_children.find(toPrune) != m_children.end());
			assert(m_children.count(toPrune) == 1);
			m_children.erase(toPrune);
			assert(m_children.size() == d_size-1);
			assert(m_children.count(toPrune) == 0);
			assert(m_children.find(toPrune) == m_children.end());	
		}
		if (!m_answer) {
			for (Node* child : m_negativeAnswers) {
#ifndef NDEBUG
				unsigned long d_size = m_children.size();
#endif
				assert(m_children.find(child) != m_children.end());
				assert(m_children.count(child) == 1);
				m_children.erase(child);
				assert(m_children.size() == d_size-1);
				assert(m_children.count(child) == 0);
				assert(m_children.find(child) == m_children.end());
				m_parentsCount++;
			}
		}
		m_valuesSent = 0;
	}
	assert(d_cond);
}

void Node::processCaseInternal()
{
#ifndef NDEBUG
	bool d_cond = false;
#endif
	assert(m_parentsCount >= 0);
	assert(!m_children.empty());
	if (m_valuesSent == 0) {
#ifndef NDEBUG
		d_cond = true;
		d_childrenCount = m_children.size(); d_parentsCount = m_parentsCount;
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits for m_mtxValues");
#endif
		unique_lock<mutex> uniqueLockValues(m_mtxValues);
		assert(uniqueLockValues.owns_lock());
#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxValues");
		/*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits m_condValues");
#endif
		while (m_valuesReceived.size() < m_parentsCount) {
#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") released m_mtxValues (waiting)");
#endif
			m_condValues.wait(uniqueLockValues);
		}
		assert(m_valuesReceived.size() == m_parentsCount);
		assert(m_valuesReceived.size() == d_parentsCount);
		assert(uniqueLockValues.owns_lock());
#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxValues");
#endif
		
		m_answer = true;
		m_answersReceived = 0;
		m_negativeAnswers.clear();
		m_pruneAnswers.clear();
		assert(m_negativeAnswers.empty());
		assert(m_pruneAnswers.empty());

		auto candidatePair = *m_valuesReceived.cbegin();
		unsigned long candidateId = ( candidatePair.first < m_id ? candidatePair.first : m_id );
#ifndef NDEBUG
		unsigned long d_size = m_children.size();
		int d_i = 0;
#endif
		for (Node* child : m_children) {
			m_graph->sendIdYoyo(*this, candidateId, *child);
#ifndef NDEBUG
			d_i++;
#endif
			m_valuesSent++;
#ifndef NDEBUG
			d_size--;
#endif
		}
		assert(d_size == 0 && m_valuesSent == d_childrenCount);
	}

	else {		
		assert (m_valuesSent == d_childrenCount);
#ifndef NDEBUG
		d_cond = true;
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits for m_mtxAnswer");
#endif
		unique_lock<mutex> uniqueLockAnswers(m_mtxAnswers);
		assert(uniqueLockAnswers.owns_lock());
#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxAnswer");
		/*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits m_condAnswers");
#endif
		while (m_answersReceived < m_valuesSent) {
#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") released m_mtxAnswers (waiting)");
#endif
			m_condAnswers.wait(uniqueLockAnswers);
		}
		assert(m_answersReceived == m_valuesSent);
		assert(m_answersReceived == d_childrenCount);
		assert(uniqueLockAnswers.owns_lock());
#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxAnswer");
#endif
		
		
		for (Node* toPrune : m_pruneAnswers) {
#ifndef NDEBUG
			unsigned long d_size = m_children.size();
#endif
			assert(m_children.find(toPrune) != m_children.end());
			assert(m_children.count(toPrune) == 1);
			m_children.erase(toPrune);
			assert(m_children.size() == d_size-1);
			assert(m_children.count(toPrune) == 0);
			assert(m_children.find(toPrune) == m_children.end());		
		}
		if (!m_answer) {
			for (Node* child : m_negativeAnswers) {
#ifndef NDEBUG
				unsigned long d_size = m_children.size();
#endif
				assert(m_children.find(child) != m_children.end());
				assert(m_children.count(child) == 1);
				m_children.erase(child);
				assert(m_children.size() == d_size-1);
				assert(m_children.count(child) == 0);
				assert(m_children.find(child) == m_children.end());
				m_parentsCount++;
			}
		}
#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits for m_mtxValues");
#endif
		unique_lock<mutex> uniqueLockValues(m_mtxValues);
		assert(uniqueLockValues.owns_lock());
#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxValues");
#endif
		unsigned long min, countMin; bool answer, prune, firstIt = true;
		assert(m_valuesReceived.size() == d_parentsCount);
		min = m_valuesReceived.cbegin()->first;
		countMin = m_valuesReceived.count(min);
		for (auto parent : m_valuesReceived) {
			answer = (m_answer && parent.first == min);
			assert ((m_answer && parent.first == min && answer) || 
				(m_answer && parent.first > min && !answer) ||
				(!m_answer && !answer));
			prune = (answer && (!firstIt || (m_children.empty() && m_parentsCount==countMin)));
			assert((!answer && !prune) ||
				(answer && !firstIt && prune) ||
				(answer && firstIt && m_parentsCount == countMin && m_children.empty() && prune) ||
				(answer && firstIt && ((m_parentsCount > countMin)||!m_children.empty()) && !prune));
#ifndef NDEBUG
#endif
			m_graph->sendAnswerYoyo(*this, answer, prune, *parent.second);
			firstIt = false;

			if (prune) {
				m_parentsCount--;
			}
			else if (!answer) {
				m_parentsCount--;
#ifndef NDEBUG
				unsigned long d_size = m_children.size();
#endif
				assert(m_children.count(parent.second) == 0);
				assert(m_children.find(parent.second) == m_children.end());
				m_children.insert(parent.second);
				assert(m_children.size() == d_size+1);
				assert(m_children.count(parent.second) == 1);
				assert(m_children.find(parent.second) != m_children.end());
				assert(*m_children.find(parent.second) == parent.second);
			}
		}

		m_valuesReceived.clear();
		m_valuesSent = 0;
	}
	assert(d_cond);
}

void Node::processCaseSink()
{
#ifndef NDEBUG
	d_parentsCount = m_parentsCount;
	// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits for m_mtxValues");
#endif
	unique_lock<mutex> uniqueLockValues(m_mtxValues);
	assert(uniqueLockValues.owns_lock());
#ifndef NDEBUG
	// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxValues");
	/*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") waits m_condValues");
#endif
	while (m_valuesReceived.size() < m_parentsCount) {
#ifndef NDEBUG
		// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") released m_mtxValues (waiting)");
#endif
		m_condValues.wait(uniqueLockValues);
	}
	assert(m_valuesReceived.size() == m_parentsCount);
	assert(m_valuesReceived.size() == d_parentsCount);
	assert(uniqueLockValues.owns_lock());
#ifndef NDEBUG
	// /*if (threadIdToStr()=="5")*/ m_graph->writeDebug(string(typeid(*this).name())+"."+__func__+":"+to_string(__LINE__)+" -> "+to_string(m_id)+" ("+Node::threadIdToStr()+") owns m_mtxValues");
#endif

	unsigned long min = m_valuesReceived.cbegin()->first;
	if ((m_valuesReceived.size() == 1) || (m_valuesReceived.count(min) == m_valuesReceived.size())) {
		for (auto parent : m_valuesReceived) {
			m_graph->sendAnswerYoyo(*this, true, true, *parent.second);
			m_parentsCount--;
		}
	}
	else {
		bool answer, prune, firstIt = true;
		for (auto parent : m_valuesReceived) {
			answer = (parent.first == min);
			assert((parent.first == min && answer) || (parent.first > min && !answer));
			prune = (answer && !firstIt);
			assert((answer && firstIt && !prune) ||
				(answer && !firstIt && prune) ||
				(!answer && !prune));
			m_graph->sendAnswerYoyo(*this, answer, prune, *parent.second);
			firstIt = false;

			if (prune) {
				m_parentsCount--;
			}
			else if (!answer) {
				m_parentsCount--;
#ifndef NDEBUG
				unsigned long d_size = m_children.size();
#endif
				assert(m_children.count(parent.second) == 0);
				assert(m_children.find(parent.second) == m_children.end());
				m_children.insert(parent.second);
				assert(m_children.size() == d_size+1);
				assert(m_children.count(parent.second) == 1);
				assert(m_children.find(parent.second) != m_children.end());
				assert(*m_children.find(parent.second) == parent.second);

			}
		}
		m_valuesReceived.clear();
		m_valuesSent = 0;
	}
}

bool Node::processCaseLeader()
{
	m_graph->assignLeader(*this);
#ifndef NDEBUG
	m_graph->writeDebug(to_string(m_id)+" is the LEADER");
#endif
	return true;
}

bool Node::processCaseIdle()
{
#ifndef NDEBUG
	m_graph->writeDebug(to_string(m_id)+" became IDLE");

#endif
	return true;
}