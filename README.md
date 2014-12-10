University of Ottawa - CSI5308 - Principles of Distributed Computing
Yoyo-Algorithm  Copyright (C) 2014  Arthur Bourjac

=================

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

 =================

Fall 2014 - Assignement 3 by Arthur Bourjac

This assignment is the implementation of the Yoyo algorithm with Pruning seen in class.

The aim of this project is to test the algorithm on multiple graphs to get an approximation of the complexity.
After running the algorithm on multiple graphs, you can use the data saved in the generated report as you like. For example to create a .csv file with graphs showing the link between the number of nodes and the message complexity.

=================

How to use : via the bash file "yoyo.sh"

Usage : yoyo.sh -f file | -d dir report | -r number report
	-f : process one graph (console display)
		"file" specifies the file for the graph
	-d : process multiple graphs (WARNING : appends)
		"dir" specifies the directory for the graphs
		"report" specifies the file in which to save
	-r : process random graphs (WARNING : appends)
		"number" specifies the number of graphs to process
		"report" specifies the file in which to save

=================

Special thanks to :
	*Paola Flocchini, PhD, professor at University of Ottawa
		for her class on Distributed Computin
		http://www.site.uottawa.ca/~flocchin

	*Fabien Viger
		for his tool Gengraph, included in this project, used to generate graphs.

=================

Any questions, remarks or comments are welcome, write to
	arthur [dot] bourjac [at] gmail [dot] com




