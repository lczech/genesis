#ifndef GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_H_
#define GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_H_

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "tree/tree.hpp"

template <class NodeDataType, class EdgeDataType>
void BoostPythonExport_Tree (std::string name)
{
    using namespace boost::python;
    using namespace genesis;

    typedef Tree<NodeDataType, EdgeDataType> TreeType;

    boost::python::class_< TreeType > ( name.c_str() )

        // Public Member Functions

        .def(
            "clear",
            ( void ( TreeType::* )(  ))( &TreeType::clear )
        )
        .def(
            "swap",
            ( void ( TreeType::* )( TreeType & ))( &TreeType::swap ),
            ( boost::python::arg("other") )
        )
        //~ .def(
            //~ "Import",
            //~ ( void ( TreeType::* )( TreeType::LinkArray &, TreeType::NodeArray &, TreeType::EdgeArray & ))( &TreeType::Import ),
            //~ ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") )
        //~ )
        //~ .def(
            //~ "Export",
            //~ ( void ( TreeType::* )( TreeType::LinkArray &, TreeType::NodeArray &, TreeType::EdgeArray & ))( &TreeType::Export ),
            //~ ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") )
        //~ )
        //~ .def(
            //~ "RootLink",
            //~ ( TreeType::LinkType * ( TreeType::* )(  ) const )( &TreeType::RootLink )
        //~ )
        //~ .def(
            //~ "RootNode",
            //~ ( TreeType::NodeType * ( TreeType::* )(  ) const )( &TreeType::RootNode )
        //~ )
        //~ .def(
            //~ "LinkAt",
            //~ ( TreeType::LinkType * ( TreeType::* )( size_t ) const )( &TreeType::LinkAt ),
            //~ ( boost::python::arg("index") )
        //~ )
        //~ .def(
            //~ "NodeAt",
            //~ ( TreeType::NodeType * ( TreeType::* )( size_t ) const )( &TreeType::NodeAt ),
            //~ ( boost::python::arg("index") )
        //~ )
        //~ .def(
            //~ "EdgeAt",
            //~ ( TreeType::EdgeType * ( TreeType::* )( size_t ) const )( &TreeType::EdgeAt ),
            //~ ( boost::python::arg("index") )
        //~ )
        .def(
            "LinkCount",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::LinkCount ),
            "Returns the number of Links of the Tree."
        )
        .def(
            "NodeCount",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::NodeCount ),
            "Returns the number of Nodes of the Tree."
        )
        .def(
            "EdgeCount",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::EdgeCount ),
            "Returns the number of Edges of the Tree."
        )
        //~ .def(
            //~ "FindNode",
            //~ ( TreeType::NodeType * ( TreeType::* )( std::string ) const )( &TreeType::FindNode ),
            //~ ( boost::python::arg("name") )
        //~ )
        .def(
            "MaxRank",
            ( int ( TreeType::* )(  ) const )( &TreeType::MaxRank )
        )
        .def(
            "IsBifurcating",
            ( bool ( TreeType::* )(  ) const )( &TreeType::IsBifurcating )
        )
        .def(
            "LeafCount",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::LeafCount )
        )
        .def(
            "InnerCount",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::InnerCount )
        )
        .def(
            "Length",
            ( double ( TreeType::* )(  ) const )( &TreeType::Length )
        )
        //~ .def(
            //~ "NodeDepthMatrix",
            //~ ( Matrix< int > * ( TreeType::* )(  ) const )( &TreeType::NodeDepthMatrix )
        //~ )
        //~ .def(
            //~ "NodeDepthVector",
            //~ ( std::vector< int > ( TreeType::* )( const TreeType::NodeType * ) const )( &TreeType::NodeDepthVector ),
            //~ ( boost::python::arg("node")=(const TreeType::NodeType *)(nullptr) )
        //~ )
        //~ .def(
            //~ "NodeDistanceMatrix",
            //~ ( Matrix< double > * ( TreeType::* )(  ) const )( &TreeType::NodeDistanceMatrix )
        //~ )
        //~ .def(
            //~ "NodeDistanceVector",
            //~ ( std::vector< double > ( TreeType::* )( const TreeType::NodeType * ) const )( &TreeType::NodeDistanceVector ),
            //~ ( boost::python::arg("node")=(const TreeType::NodeType *)(nullptr) )
        //~ )
        //~ .def(
            //~ "ClosestLeafDepthVector",
            //~ ( NodeIntVectorType ( TreeType::* )(  ) const )( &TreeType::ClosestLeafDepthVector )
        //~ )
        //~ .def(
            //~ "ClosestLeafDistanceVector",
            //~ ( NodeDoubleVectorType ( TreeType::* )(  ) const )( &TreeType::ClosestLeafDistanceVector )
        //~ )
        .def(
            "DeepestDistance",
            ( double ( TreeType::* )(  ) const )( &TreeType::DeepestDistance )
        )
        //~ .def(
            //~ "Equal",
            //~ ( bool ( TreeType::* )( const TreeType &, const std::function< bool(TreeType::ConstIteratorPreorder &, TreeType::ConstIteratorPreorder &)> ) const )( &TreeType::Equal ),
            //~ ( boost::python::arg("other"), boost::python::arg("comparator") )
        //~ )
        .def(
            "HasIdenticalTopology",
            ( bool ( TreeType::* )( const TreeType & ) const )( &TreeType::HasIdenticalTopology ),
            ( boost::python::arg("other") )
        )
        .def(
            "Validate",
            ( bool ( TreeType::* )(  ) const )( &TreeType::Validate )
        )
        .def(
            "Dump",
            ( std::string ( TreeType::* )(  ) const )( &TreeType::Dump )
        )
        .def(
            "DumpLists",
            ( std::string ( TreeType::* )(  ) const )( &TreeType::DumpLists )
        )
        //~ .def(
            //~ "DumpEulertour",
            //~ ( std::string ( TreeType::* )(  ) const )( &TreeType::DumpEulertour )
        //~ )
        //~ .def(
            //~ "Equal",
            //~ ( bool ( TreeType::* )( const TreeType &, const TreeType &, const std::function< bool(TreeType::ConstIteratorPreorder &, TreeType::ConstIteratorPreorder &)> ))( &TreeType::Equal ),
            //~ ( boost::python::arg("lhs"), boost::python::arg("rhs"), boost::python::arg("comparator") )
        //~ )
        //~ .staticmethod("Equal")

        // Iterators


        //~ ( ::genesis::TreeType::IteratorEulertour ( TreeType::* )(  ) )( &TreeType::BeginEulertour )
        //~ ( ::genesis::TreeType::IteratorEulertour ( TreeType::* )(  ) )( &TreeType::EndEulertour )
        //~ .add_property(
            //~ "Eulertour",
            //~ boost::python::range (
                //~ ( ::genesis::TreeType::ConstIteratorEulertour ( TreeType::* )(  ) const )( &TreeType::BeginEulertour ),
                //~ ( ::genesis::TreeType::ConstIteratorEulertour ( TreeType::* )(  ) const )( &TreeType::EndEulertour )
            //~ )
        //~ )


        //~ .add_property(
            //~ "Eulertour",
            //~ boost::python::range ( &::genesis::TreeType::BeginEulertour, &::genesis::TreeType::EndEulertour )
        //~ )
        //~ .add_property(
            //~ "Preorder",
            //~ boost::python::range ( &::genesis::TreeType::BeginPreorder, &::genesis::TreeType::EndPreorder )
        //~ )
        //~ .add_property(
            //~ "Postorder",
            //~ boost::python::range ( &::genesis::TreeType::BeginPostorder, &::genesis::TreeType::EndPostorder )
        //~ )
        //~ .add_property(
            //~ "Levelorder",
            //~ boost::python::range ( &::genesis::TreeType::BeginLevelorder, &::genesis::TreeType::EndLevelorder )
        //~ )
        //~ .add_property(
            //~ "Links",
            //~ boost::python::range ( &::genesis::TreeType::BeginLinks, &::genesis::TreeType::EndLinks )
        //~ )
        //~ .add_property(
            //~ "Nodes",
            //~ boost::python::range ( &::genesis::TreeType::BeginNodes, &::genesis::TreeType::EndNodes )
        //~ )
        //~ .add_property(
            //~ "Edges",
            //~ boost::python::range ( &::genesis::TreeType::BeginEdges, &::genesis::TreeType::EndEdges )
        //~ )
    ;

}

#endif // include guard
