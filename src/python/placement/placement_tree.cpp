/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "placement/placement_tree.hpp"

void BoostPythonExport_PlacementTree()
{
    using namespace boost::python;
    using namespace genesis;


    boost::python::class_< PlacementTree > ( "PlacementTree" )

        // Public Member Functions

        .def(
            "clear",
            ( void ( PlacementTree::* )(  ))( &PlacementTree::clear )
        )
        .def(
            "swap",
            ( void ( PlacementTree::* )( PlacementTree & ))( &PlacementTree::swap ),
            ( boost::python::arg("other") )
        )
        .def(
            "Import",
            ( void ( PlacementTree::* )( PlacementTree::LinkArray &, PlacementTree::NodeArray &, PlacementTree::EdgeArray & ))( &PlacementTree::Import ),
            ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") )
        )
        .def(
            "Export",
            ( void ( PlacementTree::* )( PlacementTree::LinkArray &, PlacementTree::NodeArray &, PlacementTree::EdgeArray & ))( &PlacementTree::Export ),
            ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") )
        )
        //~ .def(
            //~ "RootLink",
            //~ ( PlacementTree::LinkType * ( PlacementTree::* )(  ) const )( &PlacementTree::RootLink )
        //~ )
        //~ .def(
            //~ "RootNode",
            //~ ( PlacementTree::NodeType * ( PlacementTree::* )(  ) const )( &PlacementTree::RootNode )
        //~ )
        //~ .def(
            //~ "LinkAt",
            //~ ( PlacementTree::LinkType * ( PlacementTree::* )( size_t ) const )( &PlacementTree::LinkAt ),
            //~ ( boost::python::arg("index") )
        //~ )
        //~ .def(
            //~ "NodeAt",
            //~ ( PlacementTree::NodeType * ( PlacementTree::* )( size_t ) const )( &PlacementTree::NodeAt ),
            //~ ( boost::python::arg("index") )
        //~ )
        //~ .def(
            //~ "EdgeAt",
            //~ ( PlacementTree::EdgeType * ( PlacementTree::* )( size_t ) const )( &PlacementTree::EdgeAt ),
            //~ ( boost::python::arg("index") )
        //~ )
        .def(
            "LinkCount",
            ( size_t ( PlacementTree::* )(  ) const )( &PlacementTree::LinkCount ),
            "Returns the number of Links of the Tree."
        )
        .def(
            "NodeCount",
            ( size_t ( PlacementTree::* )(  ) const )( &PlacementTree::NodeCount ),
            "Returns the number of Nodes of the Tree."
        )
        .def(
            "EdgeCount",
            ( size_t ( PlacementTree::* )(  ) const )( &PlacementTree::EdgeCount ),
            "Returns the number of Edges of the Tree."
        )
        //~ .def(
            //~ "BeginEulertour",
            //~ ( IteratorEulertour ( PlacementTree::* )(  ))( &PlacementTree::BeginEulertour )
        //~ )
        //~ .def(
            //~ "BeginEulertour",
            //~ ( IteratorEulertour ( PlacementTree::* )( const PlacementTree::LinkType * ))( &PlacementTree::BeginEulertour ),
            //~ ( boost::python::arg("link") )
        //~ )
        //~ .def(
            //~ "BeginEulertour",
            //~ ( IteratorEulertour ( PlacementTree::* )( const PlacementTree::NodeType * ))( &PlacementTree::BeginEulertour ),
            //~ ( boost::python::arg("node") )
        //~ )
        //~ .def(
            //~ "EndEulertour",
            //~ ( IteratorEulertour ( PlacementTree::* )(  ))( &PlacementTree::EndEulertour )
        //~ )
        //~ .def(
            //~ "BeginEulertour",
            //~ ( ConstIteratorEulertour ( PlacementTree::* )(  ) const )( &PlacementTree::BeginEulertour )
        //~ )
        //~ .def(
            //~ "BeginEulertour",
            //~ ( ConstIteratorEulertour ( PlacementTree::* )( const PlacementTree::LinkType * ) const )( &PlacementTree::BeginEulertour ),
            //~ ( boost::python::arg("link") )
        //~ )
        //~ .def(
            //~ "BeginEulertour",
            //~ ( ConstIteratorEulertour ( PlacementTree::* )( const PlacementTree::NodeType * ) const )( &PlacementTree::BeginEulertour ),
            //~ ( boost::python::arg("node") )
        //~ )
        //~ .def(
            //~ "EndEulertour",
            //~ ( ConstIteratorEulertour ( PlacementTree::* )(  ) const )( &PlacementTree::EndEulertour )
        //~ )
        //~ .def(
            //~ "BeginPreorder",
            //~ ( IteratorPreorder ( PlacementTree::* )(  ))( &PlacementTree::BeginPreorder )
        //~ )
        //~ .def(
            //~ "BeginPreorder",
            //~ ( IteratorPreorder ( PlacementTree::* )( const PlacementTree::LinkType * ))( &PlacementTree::BeginPreorder ),
            //~ ( boost::python::arg("link") )
        //~ )
        //~ .def(
            //~ "BeginPreorder",
            //~ ( IteratorPreorder ( PlacementTree::* )( const PlacementTree::NodeType * ))( &PlacementTree::BeginPreorder ),
            //~ ( boost::python::arg("node") )
        //~ )
        //~ .def(
            //~ "EndPreorder",
            //~ ( IteratorPreorder ( PlacementTree::* )(  ))( &PlacementTree::EndPreorder )
        //~ )
        //~ .def(
            //~ "BeginPreorder",
            //~ ( ConstIteratorPreorder ( PlacementTree::* )(  ) const )( &PlacementTree::BeginPreorder )
        //~ )
        //~ .def(
            //~ "BeginPreorder",
            //~ ( ConstIteratorPreorder ( PlacementTree::* )( const PlacementTree::LinkType * ) const )( &PlacementTree::BeginPreorder ),
            //~ ( boost::python::arg("link") )
        //~ )
        //~ .def(
            //~ "BeginPreorder",
            //~ ( ConstIteratorPreorder ( PlacementTree::* )( const PlacementTree::NodeType * ) const )( &PlacementTree::BeginPreorder ),
            //~ ( boost::python::arg("node") )
        //~ )
        //~ .def(
            //~ "EndPreorder",
            //~ ( ConstIteratorPreorder ( PlacementTree::* )(  ) const )( &PlacementTree::EndPreorder )
        //~ )
        //~ .def(
            //~ "BeginPostorder",
            //~ ( IteratorPostorder ( PlacementTree::* )(  ))( &PlacementTree::BeginPostorder )
        //~ )
        //~ .def(
            //~ "BeginPostorder",
            //~ ( IteratorPostorder ( PlacementTree::* )( const PlacementTree::LinkType * ))( &PlacementTree::BeginPostorder ),
            //~ ( boost::python::arg("link") )
        //~ )
        //~ .def(
            //~ "BeginPostorder",
            //~ ( IteratorPostorder ( PlacementTree::* )( const PlacementTree::NodeType * ))( &PlacementTree::BeginPostorder ),
            //~ ( boost::python::arg("node") )
        //~ )
        //~ .def(
            //~ "EndPostorder",
            //~ ( IteratorPostorder ( PlacementTree::* )(  ))( &PlacementTree::EndPostorder )
        //~ )
        //~ .def(
            //~ "BeginPostorder",
            //~ ( ConstIteratorPostorder ( PlacementTree::* )(  ) const )( &PlacementTree::BeginPostorder )
        //~ )
        //~ .def(
            //~ "BeginPostorder",
            //~ ( ConstIteratorPostorder ( PlacementTree::* )( const PlacementTree::LinkType * ) const )( &PlacementTree::BeginPostorder ),
            //~ ( boost::python::arg("link") )
        //~ )
        //~ .def(
            //~ "BeginPostorder",
            //~ ( ConstIteratorPostorder ( PlacementTree::* )( const PlacementTree::NodeType * ) const )( &PlacementTree::BeginPostorder ),
            //~ ( boost::python::arg("node") )
        //~ )
        //~ .def(
            //~ "EndPostorder",
            //~ ( ConstIteratorPostorder ( PlacementTree::* )(  ) const )( &PlacementTree::EndPostorder )
        //~ )
        //~ .def(
            //~ "BeginLevelorder",
            //~ ( IteratorLevelorder ( PlacementTree::* )(  ))( &PlacementTree::BeginLevelorder )
        //~ )
        //~ .def(
            //~ "BeginLevelorder",
            //~ ( IteratorLevelorder ( PlacementTree::* )( const PlacementTree::LinkType * ))( &PlacementTree::BeginLevelorder ),
            //~ ( boost::python::arg("link") )
        //~ )
        //~ .def(
            //~ "BeginLevelorder",
            //~ ( IteratorLevelorder ( PlacementTree::* )( const PlacementTree::NodeType * ))( &PlacementTree::BeginLevelorder ),
            //~ ( boost::python::arg("node") )
        //~ )
        //~ .def(
            //~ "EndLevelorder",
            //~ ( IteratorLevelorder ( PlacementTree::* )(  ))( &PlacementTree::EndLevelorder )
        //~ )
        //~ .def(
            //~ "BeginLevelorder",
            //~ ( ConstIteratorLevelorder ( PlacementTree::* )(  ) const )( &PlacementTree::BeginLevelorder )
        //~ )
        //~ .def(
            //~ "BeginLevelorder",
            //~ ( ConstIteratorLevelorder ( PlacementTree::* )( const PlacementTree::LinkType * ) const )( &PlacementTree::BeginLevelorder ),
            //~ ( boost::python::arg("link") )
        //~ )
        //~ .def(
            //~ "BeginLevelorder",
            //~ ( ConstIteratorLevelorder ( PlacementTree::* )( const PlacementTree::NodeType * ) const )( &PlacementTree::BeginLevelorder ),
            //~ ( boost::python::arg("node") )
        //~ )
        //~ .def(
            //~ "EndLevelorder",
            //~ ( ConstIteratorLevelorder ( PlacementTree::* )(  ) const )( &PlacementTree::EndLevelorder )
        //~ )
        //~ .def(
            //~ "BeginLinks",
            //~ ( IteratorLinks ( PlacementTree::* )(  ))( &PlacementTree::BeginLinks )
        //~ )
        //~ .def(
            //~ "EndLinks",
            //~ ( IteratorLinks ( PlacementTree::* )(  ))( &PlacementTree::EndLinks )
        //~ )
        //~ .def(
            //~ "BeginLinks",
            //~ ( ConstIteratorLinks ( PlacementTree::* )(  ) const )( &PlacementTree::BeginLinks )
        //~ )
        //~ .def(
            //~ "EndLinks",
            //~ ( ConstIteratorLinks ( PlacementTree::* )(  ) const )( &PlacementTree::EndLinks )
        //~ )
        //~ .def(
            //~ "BeginNodes",
            //~ ( IteratorNodes ( PlacementTree::* )(  ))( &PlacementTree::BeginNodes )
        //~ )
        //~ .def(
            //~ "EndNodes",
            //~ ( IteratorNodes ( PlacementTree::* )(  ))( &PlacementTree::EndNodes )
        //~ )
        //~ .def(
            //~ "BeginNodes",
            //~ ( ConstIteratorNodes ( PlacementTree::* )(  ) const )( &PlacementTree::BeginNodes )
        //~ )
        //~ .def(
            //~ "EndNodes",
            //~ ( ConstIteratorNodes ( PlacementTree::* )(  ) const )( &PlacementTree::EndNodes )
        //~ )
        //~ .def(
            //~ "BeginEdges",
            //~ ( IteratorEdges ( PlacementTree::* )(  ))( &PlacementTree::BeginEdges )
        //~ )
        //~ .def(
            //~ "EndEdges",
            //~ ( IteratorEdges ( PlacementTree::* )(  ))( &PlacementTree::EndEdges )
        //~ )
        //~ .def(
            //~ "BeginEdges",
            //~ ( ConstIteratorEdges ( PlacementTree::* )(  ) const )( &PlacementTree::BeginEdges )
        //~ )
        //~ .def(
            //~ "EndEdges",
            //~ ( ConstIteratorEdges ( PlacementTree::* )(  ) const )( &PlacementTree::EndEdges )
        //~ )
        //~ .def(
            //~ "FindNode",
            //~ ( PlacementTree::NodeType * ( PlacementTree::* )( std::string ) const )( &PlacementTree::FindNode ),
            //~ ( boost::python::arg("name") )
        //~ )
        .def(
            "MaxRank",
            ( int ( PlacementTree::* )(  ) const )( &PlacementTree::MaxRank )
        )
        .def(
            "IsBifurcating",
            ( bool ( PlacementTree::* )(  ) const )( &PlacementTree::IsBifurcating )
        )
        .def(
            "LeafCount",
            ( size_t ( PlacementTree::* )(  ) const )( &PlacementTree::LeafCount )
        )
        .def(
            "InnerCount",
            ( size_t ( PlacementTree::* )(  ) const )( &PlacementTree::InnerCount )
        )
        .def(
            "Length",
            ( double ( PlacementTree::* )(  ) const )( &PlacementTree::Length )
        )
        //~ .def(
            //~ "NodeDepthMatrix",
            //~ ( Matrix< int > * ( PlacementTree::* )(  ) const )( &PlacementTree::NodeDepthMatrix )
        //~ )
        //~ .def(
            //~ "NodeDepthVector",
            //~ ( std::vector< int > ( PlacementTree::* )( const PlacementTree::NodeType * ) const )( &PlacementTree::NodeDepthVector ),
            //~ ( boost::python::arg("node")=(const PlacementTree::NodeType *)(nullptr) )
        //~ )
        //~ .def(
            //~ "NodeDistanceMatrix",
            //~ ( Matrix< double > * ( PlacementTree::* )(  ) const )( &PlacementTree::NodeDistanceMatrix )
        //~ )
        //~ .def(
            //~ "NodeDistanceVector",
            //~ ( std::vector< double > ( PlacementTree::* )( const PlacementTree::NodeType * ) const )( &PlacementTree::NodeDistanceVector ),
            //~ ( boost::python::arg("node")=(const PlacementTree::NodeType *)(nullptr) )
        //~ )
        //~ .def(
            //~ "ClosestLeafDepthVector",
            //~ ( NodeIntVectorType ( PlacementTree::* )(  ) const )( &PlacementTree::ClosestLeafDepthVector )
        //~ )
        //~ .def(
            //~ "ClosestLeafDistanceVector",
            //~ ( NodeDoubleVectorType ( PlacementTree::* )(  ) const )( &PlacementTree::ClosestLeafDistanceVector )
        //~ )
        .def(
            "DeepestDistance",
            ( double ( PlacementTree::* )(  ) const )( &PlacementTree::DeepestDistance )
        )
        .def(
            "Equal",
            ( bool ( PlacementTree::* )( const PlacementTree &, const std::function< bool(PlacementTree::ConstIteratorPreorder &, PlacementTree::ConstIteratorPreorder &)> ) const )( &PlacementTree::Equal ),
            ( boost::python::arg("other"), boost::python::arg("comparator") )
        )
        .def(
            "HasIdenticalTopology",
            ( bool ( PlacementTree::* )( const PlacementTree & ) const )( &PlacementTree::HasIdenticalTopology ),
            ( boost::python::arg("other") )
        )
        .def(
            "Validate",
            ( bool ( PlacementTree::* )(  ) const )( &PlacementTree::Validate )
        )
        .def(
            "Dump",
            ( std::string ( PlacementTree::* )(  ) const )( &PlacementTree::Dump )
        )
        .def(
            "DumpLinks",
            ( std::string ( PlacementTree::* )(  ) const )( &PlacementTree::DumpLinks )
        )
        .def(
            "DumpNodes",
            ( std::string ( PlacementTree::* )(  ) const )( &PlacementTree::DumpNodes )
        )
        .def(
            "DumpEdges",
            ( std::string ( PlacementTree::* )(  ) const )( &PlacementTree::DumpEdges )
        )
        //~ .def(
            //~ "DumpEulertour",
            //~ ( std::string ( PlacementTree::* )(  ) const )( &PlacementTree::DumpEulertour )
        //~ )
        //~ .def(
            //~ "Equal",
            //~ ( bool ( PlacementTree::* )( const PlacementTree &, const PlacementTree &, const std::function< bool(PlacementTree::ConstIteratorPreorder &, PlacementTree::ConstIteratorPreorder &)> ))( &PlacementTree::Equal ),
            //~ ( boost::python::arg("lhs"), boost::python::arg("rhs"), boost::python::arg("comparator") )
        //~ )
        //~ .staticmethod("Equal")

    ;

}
