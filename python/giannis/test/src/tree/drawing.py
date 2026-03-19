import sys
import unittest

sys.path.append('./genesis_bindings')

from mylibgenesis.genesis.utils import *
from mylibgenesis.genesis.tree import *
from mylibgenesis.genesis.placement import *

class DrawingTest(unittest.TestCase):

    def test_drawing(self):
        #input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;"
        input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L,(M,N)O)R;"

        #tree = CommonTreeNewickReader().read( from_string( input ))
        tree = CommonTreeNewickReader().read( from_file( './test/src/tree/drawing_py.txt' ))
        #if not CommonTreeNewickReader().from_file( "/home/lucas/best_tree.newick", tree ):
        #   print("Error at DRAWING-1")

        #layout = RectangularLayout( tree, LayoutType.kPhylogram )
        layout = CircularLayout( tree, LayoutType.kPhylogram )
        #spreading = LayoutSpreading.kLeafNodesOnly
        #spreading = LayoutSpreading.kAllNodes
        spreading = LayoutSpreading.kAllNodesButRoot

        scheme = [
            "Crimson",
            "DarkCyan",
            "DarkGoldenRod",
            "DarkGreen",
            "DarkOrchid",
            "DeepPink",
            "DodgerBlue",
            "DimGray",
            "GreenYellow",
            "Indigo",
            "MediumVioletRed",
            "MidnightBlue",
            "Olive",
            "Orange",
            "OrangeRed",
            "Peru",
            "Purple",
            "SeaGreen",
            "DeepSkyBlue",
            "RoyalBlue",
            "SlateBlue",
            "Tomato",
            "YellowGreen"
        ]

        #Set colourful edges.
        strokes = []
        for i in range(0,tree.edge_count()):
            cv = scheme[ i % len(scheme) ]
            strokes.append( SvgStroke() )
            strokes[-1].color = color_from_name_web( cv )

            #node = tree.edge_at(i).secondary_link().node()
            #if is_leaf(node):
            #    node.data().name = cv

        layout.set_edge_strokes( strokes )

        #Set label alignment.
        layout.align_labels( True )
        layout.extra_spacer( 50.0 )
        spacer_stroke = SvgStroke( Color( 0.8, 0.8, 0.8 ))
        spacer_stroke.dash_array = [2.0, 0.5]
        spacer_stroke.dash_offset = 2.0
        layout.set_label_spacer_strokes( spacer_stroke, spreading )

        #Set colourful node shapes.
            #std::vector<utils::SvgGroup> node_shapes
            #node_shapes.resize( tree.node_count() )
        node_shapes = [None] * tree.node_count()
        for i in range(0,tree.node_count()):
            node_shapes.insert( i, SvgCircle( SvgPoint( 0, 0 ), 10, SvgStroke(), SvgFill( Color() ) ))
            #node_shapes.insert( i, SvgImage( "http://files.gamebanana.com/img/ico/sprays/516c32f08e03d.png", SvgPoint( -60, -60 ), SvgSize( 120, 120 ) ))
        #layout.set_node_shapes( node_shapes )

        #Set node spreading
        layout.inner_node_spreading( spreading )

        #Do the drawing.
        #from mylibgenesis.std import *
        #std::ostringstream out;
        #layout.to_svg_document().write( out );

        #LOG_DBG << out.str();
        #utils::file_write( out.str(), "/home/lucas/tree.svg" );

    def test_drawing_heat_tree(self):

        params = HeatTreeParameters()

        #input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;"
        input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L,(M,N)O)R;"
        #params.tree = CommonTreeNewickReader().read( from_string( input ))
        params.tree = CommonTreeNewickReader().read( from_file( './test/src/tree/drawing_py.txt' ))

        #row_count = params.tree.node_count()
        #row_count = params.tree.edge_count()
        row_count = leaf_node_count( params.tree )


        # Set colourful edges.
        #std::vector<utils::SvgStroke> strokes;
        for i in range(0,params.tree.edge_count()):
            ve = i / params.tree.edge_count()
            params.color_per_branch.append( Color( ve, 0.0, 0.0 ))

        # Make a matrix with the red-blue color plane.
        params.matrix = Matrix_genesis_utils_Color_t( row_count, 28 )
        for r in range( 0, params.matrix.rows()):
            for c in range( 0, params.matrix.cols()):
                vr = r / params.matrix.rows()
                vc = c / params.matrix.cols()
                params.matrix[ r, c ] = Color( 0.0, vr, vc )

        # Labels.
        for c in range( 0, params.matrix.cols()):
            params.column_labels.append( str( c ) )

        # Matrix color scale.
        matrix_color_map = ColorMap([ Color( 0.0, 1.0, 0.0 ), Color( 0.0, 0.0, 1.0 ) ])
        matrix_color_norm = ColorNormalizationLinear( 0.0, params.matrix.cols() )

        # Tree color scale.
        tree_color_map = ColorMap([ Color( 0.0, 0.0, 0.0 ), Color( 1.0, 0.0, 0.0 ) ])
        tree_color_norm = ColorNormalizationLinear( 0.0, row_count )

        doc = heat_tree( params, matrix_color_map, matrix_color_norm, tree_color_map, tree_color_norm)

        #std::ostringstream out;
        #doc.write( out );
        #LOG_DBG << out.str();
        #utils::file_write( out.str(), "/home/lucas/tree.svg" );

if __name__ == '__main__':
    unittest.main()
