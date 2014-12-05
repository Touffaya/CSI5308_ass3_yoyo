#include <iostream>

#include "graph.h"

using namespace std;

int main(int argc, char* argv[])
{ 
 	// if (argc != 3) 
 	// {
 	// 	cout << "Usage : yoyo [-d dir | -f file | -a number]" << endl;
 	// 	cout << "Use with either" << endl;
 	// 	cout << "\t-d : directory where the graph files are" << endl;
 	// 	cout << "\t-f : specify the file to process only one graph" << endl;
 	// 	cout << "\t-a : process number of auto-generated graphs" << endl;
 	// }

	Graph g(argv[1]);
	g.yoyo();

	cout << "End main" << endl;

  return 0;
}