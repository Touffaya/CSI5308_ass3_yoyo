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
#include <string>

#include "graph.h"

using namespace std;

int main(int argc, char* argv[])
{ 
	string copyright = "Yoyo-Algorithm  Copyright (C) 2014  Arthur Bourjac\n";
    copyright += "This program comes with ABSOLUTELY NO WARRANTY;\n";
    copyright += "This is free software, and you are welcome to redistribute it\n";
    copyright += "under certain conditions;\n\n\n";
    cout << copyright;

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
			g.yoyo(true);	
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