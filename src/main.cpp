#include <iostream>
#include <string>

#include "graph.h"

using namespace std;

int main(int argc, char* argv[])
{ 
	string usage = "Usage : yoyo -f file | -d dir | -r number\n";
 	usage += "Use with either\n";
	usage += "\t-f : process one graph (specify the file)\n";
	usage += "\t-d : process multiple graphs (specify the directory)\n";
	usage += "\t-r : process random graphs (specify the number of graphs)\n";

 	if (argc != 3) {
 		cout << usage;
 	}
 	else {
 		string option = argv[1];
 		if (option == "-f") {
			Graph g(argv[2]);
			/* debug */
			// for (auto node : *g.Nodes()) {
			// 	cout << node.first.get()->Id() << " / " << node.second << endl;
			// }
			/* end debug */
			g.yoyo();	
 		}
 		else if (option == "-d") {

 		}
 		else if (option == "-r") {

 		}
 		else {
 			cout << usage;
 		}
 	}

  	return 0;
}