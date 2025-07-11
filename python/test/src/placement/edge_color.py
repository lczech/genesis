import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *
from mylibgenesis.genesis.placement import *


class TestPlacementTreeEdgeColor(unittest.TestCase):

	def test_CountGradientPhyloxml(self):

		infile = "./test/data/placement/test_a.jplace"

		map = JplaceReader().read( from_file( infile ))

		writer = CommonTreePhyloxmlWriter()
		color_plugin = PhyloxmlColorWriterPlugin()
		color_plugin.register_with( writer )

		color_plugin.edge_colors( placement_color_count_gradient( map ))
		out = ""
		writer.write( map.tree(), to_string( out ))

		# At least one element in the output should have the color for the edge with most placements.
		## self.assertTrue( out.find("<red>255</red>"), std::string::npos)
		# EXPECT_NE(std::string::npos, out.find("<red>255</red>")) << "Red edge not found.";

if __name__ == '__main__':
    unittest.main()
