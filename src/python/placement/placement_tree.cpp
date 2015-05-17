/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "placement/placement_tree.hpp"
#include "tree/newick_processor.hpp"

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
            "DumpLists",
            ( std::string ( PlacementTree::* )(  ) const )( &PlacementTree::DumpLists )
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

        // Iterators


        //~ ( ::genesis::PlacementTree::IteratorEulertour ( PlacementTree::* )(  ) )( &PlacementTree::BeginEulertour )
        //~ ( ::genesis::PlacementTree::IteratorEulertour ( PlacementTree::* )(  ) )( &PlacementTree::EndEulertour )
        //~ .add_property(
            //~ "Eulertour",
            //~ boost::python::range (
                //~ ( ::genesis::PlacementTree::ConstIteratorEulertour ( PlacementTree::* )(  ) const )( &PlacementTree::BeginEulertour ),
                //~ ( ::genesis::PlacementTree::ConstIteratorEulertour ( PlacementTree::* )(  ) const )( &PlacementTree::EndEulertour )
            //~ )
        //~ )


        //~ .add_property(
            //~ "Eulertour",
            //~ boost::python::range ( &::genesis::PlacementTree::BeginEulertour, &::genesis::PlacementTree::EndEulertour )
        //~ )
        //~ .add_property(
            //~ "Preorder",
            //~ boost::python::range ( &::genesis::PlacementTree::BeginPreorder, &::genesis::PlacementTree::EndPreorder )
        //~ )
        //~ .add_property(
            //~ "Postorder",
            //~ boost::python::range ( &::genesis::PlacementTree::BeginPostorder, &::genesis::PlacementTree::EndPostorder )
        //~ )
        //~ .add_property(
            //~ "Levelorder",
            //~ boost::python::range ( &::genesis::PlacementTree::BeginLevelorder, &::genesis::PlacementTree::EndLevelorder )
        //~ )
        //~ .add_property(
            //~ "Links",
            //~ boost::python::range ( &::genesis::PlacementTree::BeginLinks, &::genesis::PlacementTree::EndLinks )
        //~ )
        //~ .add_property(
            //~ "Nodes",
            //~ boost::python::range ( &::genesis::PlacementTree::BeginNodes, &::genesis::PlacementTree::EndNodes )
        //~ )
        //~ .add_property(
            //~ "Edges",
            //~ boost::python::range ( &::genesis::PlacementTree::BeginEdges, &::genesis::PlacementTree::EndEdges )
        //~ )
    ;

}

void BoostPythonExport_PlacementTree_NewickProcessor()
{
    // -------------------------------------------------------------------
    //     Class NewickProcessor
    // -------------------------------------------------------------------

    boost::python::class_< ::genesis::NewickProcessor > ( "PlacementTreeNewickProcessor" )

        // Public Member Functions

        .def(
            "FromFile",
            ( bool ( * )( const std::string, ::genesis::PlacementTree& ))( &::genesis::NewickProcessor::FromFile ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .staticmethod("FromFile")
        .def(
            "FromString",
            ( bool ( * )( const std::string, ::genesis::PlacementTree& ))( &::genesis::NewickProcessor::FromString ),
            ( boost::python::arg("ts"), boost::python::arg("tree") )
        )
        .staticmethod("FromString")

        .def(
            "ToFile",
            ( bool ( * )( const std::string, const ::genesis::PlacementTree& ))( &::genesis::NewickProcessor::ToFile ),
            ( boost::python::arg("fn"), boost::python::arg("tree") )
        )
        .staticmethod("ToFile")
        .def(
            "ToString",
            ( std::string ( * )( const ::genesis::PlacementTree& ))( &::genesis::NewickProcessor::ToString ),
            ( boost::python::arg("tree") )
        )
        .staticmethod("ToString")

        // Public Member Variables

        .def_readwrite("print_names",          &::genesis::NewickProcessor::print_names)
        .def_readwrite("print_branch_lengths", &::genesis::NewickProcessor::print_branch_lengths)
        .def_readwrite("print_comments",       &::genesis::NewickProcessor::print_comments)
        .def_readwrite("print_tags",           &::genesis::NewickProcessor::print_tags)

    ;
}
