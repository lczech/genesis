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
            //~ "import_content",
            //~ ( void ( TreeType::* )( TreeType::LinkArray &, TreeType::NodeArray &, TreeType::EdgeArray & ))( &TreeType::import_content ),
            //~ ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") )
        //~ )
        //~ .def(
            //~ "export_content",
            //~ ( void ( TreeType::* )( TreeType::LinkArray &, TreeType::NodeArray &, TreeType::EdgeArray & ))( &TreeType::export_content ),
            //~ ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") )
        //~ )
        //~ .def(
            //~ "root_link",
            //~ ( TreeType::LinkType * ( TreeType::* )(  ) const )( &TreeType::root_link )
        //~ )
        //~ .def(
            //~ "root_node",
            //~ ( TreeType::NodeType * ( TreeType::* )(  ) const )( &TreeType::root_node )
        //~ )
        //~ .def(
            //~ "link_at",
            //~ ( TreeType::LinkType * ( TreeType::* )( size_t ) const )( &TreeType::link_at ),
            //~ ( boost::python::arg("index") )
        //~ )
        //~ .def(
            //~ "node_at",
            //~ ( TreeType::NodeType * ( TreeType::* )( size_t ) const )( &TreeType::node_at ),
            //~ ( boost::python::arg("index") )
        //~ )
        //~ .def(
            //~ "edge_at",
            //~ ( TreeType::EdgeType * ( TreeType::* )( size_t ) const )( &TreeType::edge_at ),
            //~ ( boost::python::arg("index") )
        //~ )
        .def(
            "link_count",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::link_count ),
            "Returns the number of Links of the Tree."
        )
        .def(
            "node_count",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::node_count ),
            "Returns the number of Nodes of the Tree."
        )
        .def(
            "edge_count",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::edge_count ),
            "Returns the number of Edges of the Tree."
        )
        //~ .def(
            //~ "find_node",
            //~ ( TreeType::NodeType * ( TreeType::* )( std::string ) const )( &TreeType::find_node ),
            //~ ( boost::python::arg("name") )
        //~ )
        .def(
            "max_rank",
            ( int ( TreeType::* )(  ) const )( &TreeType::max_rank )
        )
        .def(
            "is_bifurcating",
            ( bool ( TreeType::* )(  ) const )( &TreeType::is_bifurcating )
        )
        .def(
            "leaf_count",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::leaf_count )
        )
        .def(
            "inner_count",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::inner_count )
        )
        .def(
            "length",
            ( double ( TreeType::* )(  ) const )( &TreeType::length )
        )
        //~ .def(
            //~ "node_depth_matrix",
            //~ ( Matrix< int > * ( TreeType::* )(  ) const )( &TreeType::node_depth_matrix )
        //~ )
        //~ .def(
            //~ "node_depth_vector",
            //~ ( std::vector< int > ( TreeType::* )( const TreeType::NodeType * ) const )( &TreeType::node_depth_vector ),
            //~ ( boost::python::arg("node")=(const TreeType::NodeType *)(nullptr) )
        //~ )
        //~ .def(
            //~ "node_distance_matrix",
            //~ ( Matrix< double > * ( TreeType::* )(  ) const )( &TreeType::node_distance_matrix )
        //~ )
        //~ .def(
            //~ "node_distance_vector",
            //~ ( std::vector< double > ( TreeType::* )( const TreeType::NodeType * ) const )( &TreeType::node_distance_vector ),
            //~ ( boost::python::arg("node")=(const TreeType::NodeType *)(nullptr) )
        //~ )
        //~ .def(
            //~ "closest_leaf_depth_vector",
            //~ ( NodeIntVectorType ( TreeType::* )(  ) const )( &TreeType::closest_leaf_depth_vector )
        //~ )
        //~ .def(
            //~ "closest_leaf_distance_vector",
            //~ ( NodeDoubleVectorType ( TreeType::* )(  ) const )( &TreeType::closest_leaf_distance_vector )
        //~ )
        .def(
            "deepest_distance",
            ( double ( TreeType::* )(  ) const )( &TreeType::deepest_distance )
        )
        //~ .def(
            //~ "equal",
            //~ ( bool ( TreeType::* )( const TreeType &, const std::function< bool(TreeType::ConstIteratorPreorder &, TreeType::ConstIteratorPreorder &)> ) const )( &TreeType::equal ),
            //~ ( boost::python::arg("other"), boost::python::arg("comparator") )
        //~ )
        .def(
            "has_identical_topology",
            ( bool ( TreeType::* )( const TreeType & ) const )( &TreeType::has_identical_topology ),
            ( boost::python::arg("other") )
        )
        .def(
            "validate",
            ( bool ( TreeType::* )(  ) const )( &TreeType::validate )
        )
        .def(
            "dump",
            ( std::string ( TreeType::* )(  ) const )( &TreeType::dump )
        )
        .def(
            "dump_lists",
            ( std::string ( TreeType::* )(  ) const )( &TreeType::dump_lists )
        )
        //~ .def(
            //~ "equal",
            //~ ( bool ( TreeType::* )( const TreeType &, const TreeType &, const std::function< bool(TreeType::ConstIteratorPreorder &, TreeType::ConstIteratorPreorder &)> ))( &TreeType::equal ),
            //~ ( boost::python::arg("lhs"), boost::python::arg("rhs"), boost::python::arg("comparator") )
        //~ )
        //~ .staticmethod("equal")

        // Iterators


        //~ ( ::genesis::TreeType::IteratorEulertour ( TreeType::* )(  ) )( &TreeType::begin_eulertour )
        //~ ( ::genesis::TreeType::IteratorEulertour ( TreeType::* )(  ) )( &TreeType::end_eulertour )
        //~ .add_property(
            //~ "Eulertour",
            //~ boost::python::range (
                //~ ( ::genesis::TreeType::ConstIteratorEulertour ( TreeType::* )(  ) const )( &TreeType::begin_eulertour ),
                //~ ( ::genesis::TreeType::ConstIteratorEulertour ( TreeType::* )(  ) const )( &TreeType::end_eulertour )
            //~ )
        //~ )


        //~ .add_property(
            //~ "Eulertour",
            //~ boost::python::range ( &::genesis::TreeType::begin_eulertour, &::genesis::TreeType::end_eulertour )
        //~ )
        //~ .add_property(
            //~ "Preorder",
            //~ boost::python::range ( &::genesis::TreeType::begin_preorder, &::genesis::TreeType::end_preorder )
        //~ )
        //~ .add_property(
            //~ "Postorder",
            //~ boost::python::range ( &::genesis::TreeType::begin_postorder, &::genesis::TreeType::end_postorder )
        //~ )
        //~ .add_property(
            //~ "Levelorder",
            //~ boost::python::range ( &::genesis::TreeType::begin_levelorder, &::genesis::TreeType::end_levelorder )
        //~ )
        //~ .add_property(
            //~ "Links",
            //~ boost::python::range ( &::genesis::TreeType::begin_links, &::genesis::TreeType::end_links )
        //~ )
        //~ .add_property(
            //~ "Nodes",
            //~ boost::python::range ( &::genesis::TreeType::begin_nodes, &::genesis::TreeType::end_nodes )
        //~ )
        //~ .add_property(
            //~ "Edges",
            //~ boost::python::range ( &::genesis::TreeType::begin_edges, &::genesis::TreeType::end_edges )
        //~ )
    ;

}

#endif // include guard
