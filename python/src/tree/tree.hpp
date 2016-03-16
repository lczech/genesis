#ifndef GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_H_
#define GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_H_

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/tree/tree.hpp"

#include "python/src/tree/bipartition_set.hpp"
#include "python/src/tree/tree_edge.hpp"
#include "python/src/tree/tree_link.hpp"
#include "python/src/tree/tree_node.hpp"

// #include "python/src/tree/iterators.hpp"


    /*
        TODO Phyloxml tree and other tree related classes are initilized using those hard coded lists
        of classes. instead, a more flexible approach could be to use variadic templates to achieve this:
        the tree export function takes the class types of all trees to be exported, defines the python
        bindings for them, and then delegates to all the other tree related exporters using the same
        template paramters.
    */

template <class NodeDataType, class EdgeDataType>
void BoostPythonExport_Tree (std::string name)
{
    using namespace genesis;
    typedef ::genesis::tree::Tree<NodeDataType, EdgeDataType> TreeType;

    // -------------------------------------------------------------------
    //     Export related classes
    // -------------------------------------------------------------------

    BoostPythonExport_TreeEdge<NodeDataType, EdgeDataType>(name + "Edge");
    BoostPythonExport_TreeLink<NodeDataType, EdgeDataType>(name + "Link");
    BoostPythonExport_TreeNode<NodeDataType, EdgeDataType>(name + "Node");

    BoostPythonExport_BipartitionSet<TreeType>("BipartitionSet" + name);

    // BoostPythonExport_TreeIterators<TreeType>(name);

    // -------------------------------------------------------------------
    //     Class Tree
    // -------------------------------------------------------------------

    // boost::python::class_< TreeType, std::shared_ptr<TreeType> > ( name.c_str(), boost::python::init<  >(  ) )
    boost::python::class_< TreeType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< const TreeType & >(( boost::python::arg("other") )) )

        // Public Member Functions

        .def(
            "clear",
            ( void ( TreeType::* )(  ))( &TreeType::clear ),
            get_docstring("void ::genesis::Tree::clear ()")
        )
        // .def(
        //     "closest_leaf_depth_vector",
        //     ( NodeIntVectorType ( TreeType::* )(  ) const )( &TreeType::closest_leaf_depth_vector ),
        //     get_docstring("NodeIntVectorType TreeType::closest_leaf_depth_vector () const")
        // )
        // .def(
        //     "closest_leaf_distance_vector",
        //     ( NodeDoubleVectorType ( TreeType::* )(  ) const )( &TreeType::closest_leaf_distance_vector ),
        //     get_docstring("NodeDoubleVectorType TreeType::closest_leaf_distance_vector () const")
        // )
        // .def(
        //     "deepest_distance",
        //     ( double ( TreeType::* )(  ) const )( &TreeType::deepest_distance ),
        //     get_docstring("double ::genesis::Tree::deepest_distance () const")
        // )
        // .def(
        //     "dump",
        //     ( std::string ( TreeType::* )(  ) const )( &TreeType::dump ),
        //     get_docstring("std::string ::genesis::Tree::dump () const")
        // )
        // .def(
        //     "dump_lists",
        //     ( std::string ( TreeType::* )(  ) const )( &TreeType::dump_lists ),
        //     get_docstring("std::string ::genesis::Tree::dump_lists () const")
        // )
        .def(
            "edge_at",
            ( typename TreeType::EdgeType * ( TreeType::* )( size_t ) )( &TreeType::edge_at ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            ( boost::python::arg("index") )
        )
        .def(
            "edge_count",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::edge_count ),
            get_docstring("size_t ::genesis::Tree::edge_count () const")
        )
        // .def(
        //     "equal",
        //     ( bool ( * )( const TreeType &, const TreeType &, const std::function< bool(TreeType::ConstIteratorPreorder &, TreeType::ConstIteratorPreorder &)> ))( &TreeType::equal ),
        //     ( boost::python::arg("lhs"), boost::python::arg("rhs"), boost::python::arg("comparator") ),
        //     get_docstring("static bool ::genesis::Tree::equal (const ::genesis::Tree & lhs, const ::genesis::Tree & rhs, const std::function< bool(::genesis::Tree::ConstIteratorPreorder &, ::genesis::Tree::ConstIteratorPreorder &)> comparator)")
        // )
        // .staticmethod("equal")
        // .def(
        //     "equal",
        //     ( bool ( TreeType::* )( const TreeType &, const std::function< bool(TreeType::ConstIteratorPreorder &, TreeType::ConstIteratorPreorder &)> ) const )( &TreeType::equal ),
        //     ( boost::python::arg("other"), boost::python::arg("comparator") ),
        //     get_docstring("bool ::genesis::Tree::equal (const ::genesis::Tree & other, const std::function< bool(::genesis::Tree::ConstIteratorPreorder &, ::genesis::Tree::ConstIteratorPreorder &)> comparator) const")
        // )
        // .def(
        //     "export_content",
        //     ( void ( TreeType::* )( LinkArray &, NodeArray &, EdgeArray & ))( &TreeType::export_content ),
        //     ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") ),
        //     get_docstring("void ::genesis::Tree::export_content (LinkArray & links, NodeArray & nodes, EdgeArray & edges)")
        // )
        // .def(
        //     "find_node",
        //     ( typename TreeType::NodeType * ( TreeType::* )( std::string ) const )( &TreeType::find_node ),
        //     boost::python::return_value_policy<boost::python::reference_existing_object>(),
        //     ( boost::python::arg("name") ),
        //     get_docstring("NodeType * ::genesis::Tree::find_node (std::string name) const")
        // )
        // .def(
        //     "identical_topology",
        //     ( bool ( TreeType::* )( const TreeType & ) const )( &TreeType::identical_topology ),
        //     ( boost::python::arg("other") ),
        //     get_docstring("bool ::genesis::Tree::identical_topology (const TreeType & other) const")
        // )
        // .def(
        //     "import_content",
        //     ( void ( TreeType::* )( LinkArray &, NodeArray &, EdgeArray & ))( &TreeType::import_content ),
        //     ( boost::python::arg("links"), boost::python::arg("nodes"), boost::python::arg("edges") ),
        //     get_docstring("void ::genesis::Tree::import_content (LinkArray & links, NodeArray & nodes, EdgeArray & edges)")
        // )
        // .def(
        //     "inner_count",
        //     ( size_t ( TreeType::* )(  ) const )( &TreeType::inner_count ),
        //     get_docstring("size_t ::genesis::Tree::inner_count () const")
        // )
        // .def(
        //     "is_bifurcating",
        //     ( bool ( TreeType::* )(  ) const )( &TreeType::is_bifurcating ),
        //     get_docstring("bool ::genesis::Tree::is_bifurcating () const")
        // )
        // .def(
        //     "leaf_count",
        //     ( size_t ( TreeType::* )(  ) const )( &TreeType::leaf_count ),
        //     get_docstring("size_t ::genesis::Tree::leaf_count () const")
        // )
        // .def(
        //     "length",
        //     ( double ( TreeType::* )(  ) const )( &TreeType::length ),
        //     get_docstring("double ::genesis::Tree::length () const")
        // )
        .def(
            "link_at",
            ( typename TreeType::LinkType * ( TreeType::* )( size_t ) )( &TreeType::link_at ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            ( boost::python::arg("index") )
        )
        .def(
            "link_count",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::link_count ),
            get_docstring("size_t ::genesis::Tree::link_count () const")
        )
        // .def(
        //     "max_rank",
        //     ( int ( TreeType::* )(  ) const )( &TreeType::max_rank ),
        //     get_docstring("int ::genesis::Tree::max_rank () const")
        // )
        .def(
            "node_at",
            ( typename TreeType::NodeType * ( TreeType::* )( size_t ) )( &TreeType::node_at ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            ( boost::python::arg("index") )
        )
        .def(
            "node_count",
            ( size_t ( TreeType::* )(  ) const )( &TreeType::node_count ),
            get_docstring("size_t ::genesis::Tree::node_count () const")
        )
        // .def(
        //     "node_depth_matrix",
        //     ( Matrix< int > * ( TreeType::* )(  ) const )( &TreeType::node_depth_matrix ),
        //     get_docstring("Matrix< int > * ::genesis::Tree::node_depth_matrix () const")
        // )
        // .def(
        //     "node_depth_vector",
        //     ( std::vector< int > ( TreeType::* )( const NodeType * ) const )( &TreeType::node_depth_vector ),
        //     ( boost::python::arg("node")=(const NodeType *)(nullptr) ),
        //     get_docstring("std::vector< int > ::genesis::Tree::node_depth_vector (const NodeType * node=nullptr) const")
        // )
        // .def(
        //     "node_distance_matrix",
        //     ( Matrix< double > * ( TreeType::* )(  ) const )( &TreeType::node_distance_matrix ),
        //     get_docstring("Matrix< double > * ::genesis::Tree::node_distance_matrix () const")
        // )
        // .def(
        //     "node_distance_vector",
        //     ( std::vector< double > ( TreeType::* )( const NodeType * ) const )( &TreeType::node_distance_vector ),
        //     ( boost::python::arg("node")=(const NodeType *)(nullptr) ),
        //     get_docstring("std::vector< double > ::genesis::Tree::node_distance_vector (const NodeType * node=nullptr) const")
        // )
        .def(
            "root_link",
            ( typename TreeType::LinkType * ( TreeType::* )(  ) )( &TreeType::root_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "root_node",
            ( typename TreeType::NodeType * ( TreeType::* )(  ) )( &TreeType::root_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "swap",
            ( void ( TreeType::* )( TreeType & ))( &TreeType::swap ),
            ( boost::python::arg("other") ),
            get_docstring("void ::genesis::Tree::swap (::genesis::Tree & other)")
        )
        // .def(
        //     "validate",
        //     ( bool ( TreeType::* )(  ) const )( &TreeType::validate )
        // )

        // Iterators

        // ( ::genesis::TreeType::IteratorEulertour ( TreeType::* )(  ) )( &TreeType::begin_eulertour )
        // ( ::genesis::TreeType::IteratorEulertour ( TreeType::* )(  ) )( &TreeType::end_eulertour )
        // .add_property(
        //     "eulertour",
        //     boost::python::range (
        //         &TreeType::begin_eulertour,
        //         &TreeType::end_eulertour
        //         // ( TreeType::ConstIteratorEulertour ( * )(  ) const )( &TreeType::begin_eulertour ),
        //         // ( TreeType::ConstIteratorEulertour ( TreeType::* )(  ) const )( &TreeType::end_eulertour )
        //     )
        // )


        // .add_property(
            // "Eulertour",
            // boost::python::range ( &::genesis::TreeType::begin_eulertour, &::genesis::TreeType::end_eulertour )
        // )
        // .add_property(
            // "Preorder",
            // boost::python::range ( &::genesis::TreeType::begin_preorder, &::genesis::TreeType::end_preorder )
        // )
        // .add_property(
            // "Postorder",
            // boost::python::range ( &::genesis::TreeType::begin_postorder, &::genesis::TreeType::end_postorder )
        // )
        // .add_property(
            // "Levelorder",
            // boost::python::range ( &::genesis::TreeType::begin_levelorder, &::genesis::TreeType::end_levelorder )
        // )
        // .add_property(
            // "Links",
            // boost::python::range ( &::genesis::TreeType::begin_links, &::genesis::TreeType::end_links )
        // )
        // .add_property(
            // "Nodes",
            // boost::python::range ( &::genesis::TreeType::begin_nodes, &::genesis::TreeType::end_nodes )
        // )
        // .add_property(
            // "Edges",
            // boost::python::range ( &::genesis::TreeType::begin_edges, &::genesis::TreeType::end_edges )
        // )
    ;

}

#endif // include guard
