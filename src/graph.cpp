#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <sys/param.h>


#include "graph.h"

using namespace std;

/* --------------- PUBLIC --------------- */

Graph::Graph(string p_filePath)
{
	cout << "Trying to open " << p_filePath << " ..." << endl;

	string cwd;
	char *temp = new char[MAXPATHLEN];
	getcwd(temp, MAXPATHLEN);
	if (temp != NULL) {
		cwd = temp;
		delete[] temp;
	}

  	string path = cwd+"/"+p_filePath;
	ifstream file(path);
	if (!file)
	{
		cout << "ERROR: Can't open file " << path << endl;
	}
	else
	{
		cout << "File opened.\n" << endl;
		string line;
	   	while (getline(file, line))
		{
			istringstream iss(line);
			int id(0);
			if (!(iss >> id)) {break;}
			shared_ptr<Node> ptrN = std::make_shared<Node>(id);
			m_nodes.insert(make_pair(id,ptrN));
		};
	}

	file.clear();
	file.seekg(0, ios::beg);
	if (!file)
	{
		cout << "ERROR: Can't open file " << path << endl;
	}
	else
	{
		string line;
	   	while (getline(file, line))
		{	
			istringstream iss(line);
			int id(0), link(0);
			if (!(iss >> id)) {break;}
			while (iss >> link)
			{
				m_nodes.at(id).get()->addNeighbour(m_nodes.at(link).get());
			}
		}
	}

	m_leaderId = -1;
}

unordered_map<int,shared_ptr<Node>>* Graph::Nodes()
{
	return &m_nodes;
}

int Graph::yoyo()
{
	cout << "Processing algorithm Yo-Yo..." << endl;
	buildDAG();

	vector<thread> thrs;
	for (pair<int, shared_ptr<Node>> node : m_nodes) {
		thrs.push_back(thread(&Node::yoyo,node.second.get()));
	}

	for  (thread &thr : thrs) {
		thr.join();
	}

	return 0;
}

/* --------------- PRIVATE --------------- */

void Graph::buildDAG()
{

	for (pair<int,shared_ptr<Node>> pair : m_nodes) {
		Nodes()->at(pair.first).get()->startBuildingDAG();
	}

	cout << "\t- Construction DAG : " << Node::MsgCount() << " messages." << endl;
}