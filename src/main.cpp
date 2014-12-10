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
#include <string>

#include "graph.h"

using namespace std;

int main(int argc, char* argv[])
{ 
	string option = argv[1];
	if (option == "-o") {
		Graph g(argv[2]);
		g.yoyo(true);	
	} else if (option == "-m") {
		fstream fs;
  		fs.open (argv[3], fstream::in | fstream::out | fstream::app);

		Graph g(argv[2]);
		g.yoyo(false);	

		fs << g.Nodes()->size() << ";" << g.MsgCount() << endl;
  		fs.close();
	}

	// if (argc == 3 && option == "-f") {
	// }
	// else if (argc == 4 && option == "-d") {
	// }
	// else if (argc == 4 && option == "-r") {

	// }
	// else {
	// 	cout << usage;
	// }

  	return 0;
}