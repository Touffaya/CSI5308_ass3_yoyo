COPYRIGHT:
see file GNU_GPL.txt

SHORT DESCRIPTION:
"distrib n alpha min max" generates a sample of n integers taken in [min,max] from a
power-law distribution of exponent alpha.
"distrib n alpha min max z" generates a sample of n integers taken in [min,max] from a
heavy-tailed distribution of exponent alpha and average z.
"graph file" reads the degree distribution given in "file" and generates a random simple
connected graph with this degree sequence.
 
EXAMPLE:
$ distrib 10000 2.5 1 1000 5 > degs
$ graph degs > mygraph

EXAMPLE #2:
$ distrib -v 10000 2.5 1 1000 5 | graph -vv  > mygraph

HELP:
You can get some short help with bad arguments or with the -? option, like
$ distrib -?
$ graph -?

PLAYING:
To compare the computation time with the Gkantsidis heuristics:
$ distrib -v 10000 6 2.5 > degrees
$ graph -vv -t degrees > mygraph
$ graph -vv -g -t degrees > mygraph

FORMAT:
distrib output format :
 each line is <degree> <nb_vertices_with_this_degree>
graph output format :
 each line is <vertex> <neighbour 1> <neighbour 2> ...

COMPATIBILITY:
The two executables should work properly on any Windows platform.

DISCLAIMER:
I do NOT warrantee that these programs will be harmless to your computer,
or will not cause any data loss. Use them at your own risk !
However, if you encounter any problem by using this software, feel
free to contact me (address below)

Any questions, remarks or comments are welcome, write to
 fabien [dot] viger [at] ens [dot] fr

November 10th, 2004
