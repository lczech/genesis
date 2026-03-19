//ignoring tests in comments


////////// utils
math/bitvector.py STREAMS - difficulties translating c++ to python code - __REPR__
interoperatability between numpy and matrix
why ::genesis::utils::operator<< won't compile for template parameter ::genesis::utils::Color


////////// sequence
fasta, fastq, phylip, print tests have to_string-related errors
try to write advanced and rangedBased tests in fasta.py


////////// taxonomy
all tests work in python :)


////////// placement
! from_string bug is temporarily solved by reading from a file that contains the string (files ending in "\_py.txt") !
edge_color test has to_string-related errors
edge_color bind CommonTreePhyloxmlWriter (tree related)
manipulation.py - unordered_map bug


////////// tree
tree/drawing.py - end of tests, find a way to bind ostream
tree/drawing.py - tests don't check anything, they just execute commands

tree/manipulation.py - levelorder convertion compilation error

tree/mass_tree.py - operators.hpp convert bug needs BaseNode/EdgeData (+add placement bindings)

tree/printer.py - check it with c++ tests


////////// population
utils/IntervalTree many bugs
