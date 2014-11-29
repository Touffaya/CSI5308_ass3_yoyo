#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <sys/param.h>


#include "graph.h"

using namespace std;

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
			Node newNode(id);
			m_nodes.insert(make_pair(id,newNode));
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
				m_nodes.at(id).addNeighbour(link,&(m_nodes.at(link)));
			}
		}
	}
}

int Graph::yoyo()
{
	cout << "Processing algorithm Yo-Yo..." << endl;
	buildDAG();
	return 0;
}

void Graph::buildDAG()
{
	for (pair<int,Node> pair : m_nodes) {
		m_nodes.at(pair.first).startBuildingDAG();
	}

	// for (pair<int,Node> node : m_nodes) {
	// 	cout << node.first << endl << "neighbours : ";
	// 	for (pair<int,Node*> neighb : node.second.m_neighbours) {
	// 		cout << neighb.first << ", ";
	// 	}
	// 	cout << endl << "parents : ";
	// 	for (pair<int,Node*> parent : node.second.m_parents) {
	// 		cout << parent.first << ", ";
	// 	}
	// 	cout << endl << "children : ";
	// 	for (pair<int,Node*> child : node.second.m_children) {
	// 		cout << child.first << ", ";
	// 	}
	// 	cout << endl;
	// }

	cout << "Construction DAG : " << Node::getCountMsg() << " messages." << endl;
}
