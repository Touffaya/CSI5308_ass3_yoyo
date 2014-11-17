#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <sys/param.h>

#include "graph.h"

using namespace std;

Graph::Graph(string filePath)
{
	unordered_map<int,Node> m_nodes;

	cout << "Trying to open " << filePath << " ..." << endl;

	string cwd;
	char *temp = new char[MAXPATHLEN];
	getcwd(temp, MAXPATHLEN);
	if(temp != NULL) {
		cwd = temp;
		delete[] temp;
	}

  	string path = cwd+filePath;
	ifstream file(path);
	if(!file)
	{
		cout << "ERROR: Can't open file " << path << endl;
	}
	else
	{
		cout << "File opened." << endl;
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

	// file = ifstream(path);
	file.clear();
	file.seekg(0, ios::beg);
	if(!file)
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
				m_nodes.at(id).addNeighbour(&(m_nodes.at(link)));
				// cout << &(m_nodes.at(link)) << endl;
			}
		};
	}
}