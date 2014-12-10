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

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/param.h>
#include <cassert>
#include <vector>
#include <ctime>

#include "graph.h"

using namespace std;

/* --------------- PUBLIC --------------- */

Graph::Graph(string p_filePath)
{
	unordered_multimap<unsigned long,Node*> temp_nodes;
		
	cout << "Opening " << p_filePath << "...";

	string cwd;
	char *temp = new char[MAXPATHLEN];
	getcwd(temp, MAXPATHLEN);
	if (temp != NULL) {
		cwd = temp;
		delete[] temp;
	}

  	string path = cwd+"/"+p_filePath;
	ifstream file(path);
	if (!file) {
		cout << endl << "ERROR: Can't open file " << path << endl;
		exit(0);
	}
	else {
		cout << "done.\n" << endl;
		string line;
	   	while (getline(file, line))
		{
			istringstream iss(line);
			unsigned long id(0);
			if (!(iss >> id)) {break;}
			shared_ptr<Node> ptrN = std::make_shared<Node>(*this,id);
			m_nodes.insert(make_pair(ptrN,id));
			temp_nodes.insert(make_pair(id,ptrN.get()));
		};
	}

	file.clear();
	file.seekg(0, ios::beg);
	if (!file) {
		cout << endl << "ERROR: Can't open file " << path << endl;
		exit(0);
	}
	else {
		string line;
		while (getline(file, line)) {	
			istringstream iss(line);
			unsigned long id(0), link(0);			
			if (!(iss >> id)) {break;}
			while (iss >> link)
			{
				temp_nodes.find(id)->second->addNeighbour(*(temp_nodes.find(link)->second));
			}
		}
	}
}

unordered_multimap<shared_ptr<Node>,unsigned long>* Graph::Nodes()
{
	return &m_nodes;
}

unordered_multimap<shared_ptr<Node>,unsigned long>::iterator Graph::findNode(Node* p_node)
{
	for (auto it = m_nodes.begin(); it != m_nodes.end(); it++) {
		if (it->first.get() == p_node) {
			return it;
		}
	}
	return m_nodes.end();
}

unsigned long Graph::countNodes(Node* p_node)
{
	unsigned long cpt = 0;
	for (auto node : m_nodes) {
		if (node.first.get() == p_node) cpt++;
	}
	return cpt;
}


void Graph::yoyo(bool p_verb)
{
	cout << "Processing algorithm Yo-Yo...";
	buildDAG();
	time_t start, end;

	time(&start);
	if (p_verb) cout << "Starts at " << asctime(localtime(&start));;
	vector<thread> v_threads(m_nodes.size());
	for (auto node : m_nodes) {
		v_threads.push_back(thread(&Node::yoyo,node.first.get()));
	}
	for (thread& node_thr : v_threads) {
#ifndef NDEBUG
	   cout << "thread "<< node_thr.get_id() <<" joining" << endl;
#endif
	    if (node_thr.joinable()) node_thr.join();
	}
	time(&end);
	cout << "done" << endl << endl;
	if (p_verb) cout << "... ends at " << asctime(localtime(&end));
	if (p_verb) cout << "Processing time : " << to_string(difftime(end,start)) << " secondes" << endl;;
	if (p_verb) cout << "Leader " << m_leader->Id() << " found in " << m_msgCount << " messages." << endl;
}

void Graph::assignLeader(Node& p_leader)
{
	m_leader = &p_leader;
}

unsigned long Graph::MsgCount()
{
	return m_msgCount;
}

void Graph::buildDAG()
{
	for (auto pair : m_nodes) {
		pair.first.get()->startBuildingDAG();
	}
}

void Graph::sendIdDAG(Node& p_from, unsigned long p_id, Node& p_to)
{
	assert(p_id >= 0);
	assert(this->findNode(&p_from) != m_nodes.end());
	assert(this->countNodes(&p_from) == 1);
	assert(this->findNode(&p_from)->second == p_id);
	assert(this->findNode(&p_from)->first.get()->Id() == p_id);
	assert(this->findNode(&p_to) != m_nodes.end());
	assert(this->countNodes(&p_to) == 1);
	p_to.receiveIdDAG(p_from, p_id);
}

void Graph::sendIdYoyo(Node& p_from, unsigned long p_id, Node& p_to)
{
#ifndef NDEBUG
	// if (p_from.Id()) writeDebug(string(__func__)+" -> "+to_string(p_from.Id())+" ("+Node::threadIdToStr()+") sends id:"+to_string(p_id)+" to "+to_string(p_to.Id())+" for Yoyo");
#endif
	assert(p_id >= 0);
	assert(this->findNode(&p_from) != m_nodes.end());
	assert(this->countNodes(&p_from) == 1);
	assert(this->findNode(&p_to) != m_nodes.end());
	assert(this->countNodes(&p_to) == 1);
#ifndef NDEBUG
	// if (p_from.Id()) writeDebug(string(__func__)+" -> "+to_string(p_from.Id())+" ("+Node::threadIdToStr()+") waits for m_mtxMsgCount");
#endif
	lock_guard<mutex> uniqueLockCount(m_mtxMsgCount);
	// assert(uniqueLockCount.owns_lock());
#ifndef NDEBUG
	// if (p_from.Id()) writeDebug(string(__func__)+" -> "+to_string(p_from.Id())+" ("+Node::threadIdToStr()+") owns m_mtxMsgCount");
#endif
#ifndef NDEBUG
	unsigned long d_count = m_msgCount;
#endif
	m_msgCount++;
	assert(m_msgCount == d_count+1);
	// uniqueLockCount.unlock();
	// assert(!uniqueLockCount.owns_lock());
#ifndef NDEBUG
	// if (p_from.Id()) writeDebug(string(__func__)+" -> "+to_string(p_from.Id())+" ("+Node::threadIdToStr()+") released m_mtxMsgCount");
#endif
	p_to.receiveIdYoyo(p_from, p_id);
}

void Graph::sendAnswerYoyo(Node& p_from, bool p_answer, bool p_prune, Node& p_to)
{
#ifndef NDEBUG
	// if (p_from.Id()) writeDebug(string(__func__)+" -> "+to_string(p_from.Id())+" ("+Node::threadIdToStr()+") sends "+(p_answer?"true":"false")+"/"+(p_prune?"true":"false")+" to "+to_string(p_to.Id())+" for Yoyo");
#endif
	assert(this->findNode(&p_from) != m_nodes.end());
	assert(this->countNodes(&p_from) == 1);
	assert(this->findNode(&p_to) != m_nodes.end());
	assert(this->countNodes(&p_to) == 1);
#ifndef NDEBUG
	// if (p_from.Id()) writeDebug(string(__func__)+" -> "+to_string(p_from.Id())+" ("+Node::threadIdToStr()+") waits for m_mtxMsgCount");
#endif
	unique_lock<mutex> uniqueLockCount(m_mtxMsgCount);
	assert(uniqueLockCount.owns_lock());
#ifndef NDEBUG
	// if (p_from.Id()) writeDebug(string(__func__)+" -> "+to_string(p_from.Id())+" ("+Node::threadIdToStr()+") owns m_mtxMsgCount");
#endif
#ifndef NDEBUG
	unsigned long d_count = m_msgCount;
#endif
	m_msgCount++;
	assert(m_msgCount == d_count+1);
	uniqueLockCount.unlock();
	assert(!uniqueLockCount.owns_lock());
#ifndef NDEBUG
	// if (p_from.Id()) writeDebug(string(__func__)+" -> "+to_string(p_from.Id())+" ("+Node::threadIdToStr()+") released m_mtxMsgCount");
#endif
	p_to.receiveAnswerYoyo(p_from, p_answer, p_prune);
}

#ifndef NDEBUG
void Graph::writeDebug(string p_msg)
{
	// lock_guard<mutex> guardDebug(m_mtxDebug);
	// cout << p_msg << endl;
}
#endif