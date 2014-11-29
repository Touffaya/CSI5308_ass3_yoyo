#include <iostream>

#include "graph.h"

using namespace std;

int main(int argc, char* argv[])
{ 
 	// if (argc != 3 && argc != 5) 
 	// {
 	// 	cout << "Usage : yoyo [-n number -d dir | -f file]" << endl;
 	// 	cout << "Use with either" << endl;
 	// 	cout << "\t-n : specify the number of graphs to process" << endl;
 	// 	cout << "\twhere -d is the directory where the graph files are" << endl;
 	// 	cout << "\t-f : specify the file to process only one graph" << endl;
 	// }

  	Graph g(argv[1]);
  	g.yoyo();

  	return 0;
}