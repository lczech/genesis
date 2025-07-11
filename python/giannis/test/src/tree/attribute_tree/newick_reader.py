import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *

class NewickReaderTest(unittest.TestCase):

	def count_attribute_tree_data( self, tree: Tree):
		node_attr_cnt = 0
		for node in tree.nodes():
			data = node.data()
			node_attr_cnt = node_attr_cnt + len(data.attributes)

		edge_attr_cnt = 0
		for edge in tree.edges():
			data = edge.data()
			edge_attr_cnt = edge_attr_cnt + len(data.attributes)

		return (node_attr_cnt, edge_attr_cnt)

	def print_attribute_tree_data( self, tree: Tree):
		#LOG_DBG << "Nodes";
		for node in tree.nodes():
			data = node.data()
			#LOG_DBG1 << "node " << data.name;
			for m in data.attributes:
				True
				#LOG_DBG2 << m.first << " --> " << m.second;

		#LOG_DBG << "Edges";
		for edge in tree.edges():
			data = edge.data();
			#LOG_DBG1 << "edge";
			for m in data.attributes:
				True
				#LOG_DBG2 << m.first << " --> " << m.second;

	def test_indexed_newick_reader_index(self):

		#Read and process tree.
		infile = "test/data/tree/indexed_attributes_0.newick"

		reader = IndexedAttributeTreeNewickReader()
		reader.add_attribute( IndexedAttributeTreeNewickReader.Source.kComment, 0, IndexedAttributeTreeNewickReader.Target.kEdge, "bootstrap")

		tree = reader.read( from_file( infile ))

		counts = self.count_attribute_tree_data( tree )
		self.assertEqual(0,counts[0])
		self.assertEqual(3,counts[1])

		#print_attribute_tree_data( tree );

	def test_indexed_newick_reader_catch_all(self):

		# Read and process tree.
		infile = "test/data/tree/indexed_attributes_1.newick"

		reader = IndexedAttributeTreeNewickReader()
		reader.add_catch_all( IndexedAttributeTreeNewickReader.Source.kComment, IndexedAttributeTreeNewickReader.Target.kEdge, "comment_")

		tree = reader.read( from_file( infile ))

		node_attr_cnt = 0
		for node in tree.nodes():
			data = node.data();
			node_attr_cnt = node_attr_cnt + len(data.attributes)
		self.assertEqual(0,node_attr_cnt)

		edge_attr_cnt = 0
		for edge in tree.edges():
			data = edge.data()
			edge_attr_cnt += len(data.attributes)
		self.assertEqual(12,edge_attr_cnt)

		counts = self.count_attribute_tree_data( tree )
		self.assertEqual(0,counts[0])
		self.assertEqual(12,counts[1])

	def test_keyed_newick_reader_keys(self):

		#Read and process tree.
		infile = "test/data/tree/keyed_attributes_0.newick"

		reader = KeyedAttributeTreeNewickReader()
		reader.add_attribute( "bs",     KeyedAttributeTreeNewickReader.Target.kEdge )
		reader.add_attribute( "!color", KeyedAttributeTreeNewickReader.Target.kEdge, "color" )

		tree = reader.read( from_file( infile ))

		counts = self.count_attribute_tree_data( tree )
		self.assertEqual(0,counts[0])
		self.assertEqual(4,counts[1])

	def test_keyed_newick_reader_catch_all(self):

		# Read and process tree.
		infile = "test/data/tree/keyed_attributes_0.newick"

		reader = KeyedAttributeTreeNewickReader()
		reader.add_catch_all( KeyedAttributeTreeNewickReader.Target.kEdge )

		tree = reader.read( from_file( infile ))

		counts = self.count_attribute_tree_data( tree )
		self.assertEqual(0,counts[0])
		self.assertEqual(4,counts[1])

		# print_attribute_tree_data( tree );

	def test_keyed_newick_reader_nhx(self):

		# Read and process tree.
		infile = "test/data/tree/keyed_attributes_1.newick"

		reader = KeyedAttributeTreeNewickReader()
		reader.add_nhx_attributes()
		# reader.add_catch_all( KeyedAttributeTreeNewickReader::Target::kNode );

		tree = reader.read( from_file( infile ))

		counts = self.count_attribute_tree_data( tree )
		self.assertEqual(25,counts[0])
		self.assertEqual(1,counts[1])

		# print_attribute_tree_data( tree );

if __name__ == '__main__':
	unittest.main()
