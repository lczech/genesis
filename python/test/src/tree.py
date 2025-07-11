import sys

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *

##################################################
################### BASICS #######################
"""
input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;"

tree = CommonTreeNewickReader().read( from_file('./test/src/tree/tree_py.txt') )
#tree = CommonTreeNewickReader().read( from_string(input) )

if 2 != degree( tree.root_node() ):
	print("Error at BASICS-1")
if 7 != leaf_node_count( tree ):
	print("Error at BASICS-2")
if 6 != inner_node_count( tree ):
	print("Error at BASICS-3")
if 13 != tree.node_count():
	print("Error at BASICS-4")
if not is_bifurcating( tree ):
	print("Error at BASICS-5")

if "R" != tree.root_node().data().name:
	print("Error at BASICS-6")
if not validate_topology( tree ):
	print("Error at BASICS-7")

#Tree copy_a{ tree }
copy_a = tree
if not validate_topology( copy_a ):
	print("Error at BASICS-8")

copy_b = tree
if not validate_topology( copy_b ):
	print("Error at BASICS-9")

##################################################
"""
print("\nDone")

class MyPythonNodeData(CommonNodeData):
	pass

t1 = Tree()
