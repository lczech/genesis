#ifndef GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_NODE_H_
#define GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_NODE_H_

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "tree/tree_edge.hpp"
#include "tree/tree_link.hpp"
#include "tree/tree_node.hpp"

template <class NodeDataType, class EdgeDataType>
void BoostPythonExport_TreeNode (std::string name)
{
    typedef ::genesis::TreeEdge<NodeDataType, EdgeDataType> EdgeType;
    typedef ::genesis::TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef ::genesis::TreeNode<NodeDataType, EdgeDataType> NodeType;

    boost::python::class_< NodeType, boost::noncopyable > ( name.c_str() )

        // Public Member Functions

        .def(
            "primary_link",
            ( LinkType * ( NodeType::* )(  ) const )( &NodeType::primary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            "Returns the link of this node that points towards the root."
        )
        .def(
            "link",
            ( LinkType * ( NodeType::* )(  ) const )( &NodeType::link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            "Returns the link of this node that points towards the root."
        )
        .def(
            "index",
            ( size_t ( NodeType::* )(  ) const )( &NodeType::index ),
            "Returns the index of this link."
        )
        .def(
            "rank",
            ( int ( NodeType::* )(  ) const )( &NodeType::rank )
        )
        .def(
            "is_leaf",
            ( bool ( NodeType::* )(  ) const )( &NodeType::is_leaf )
        )
        .def(
            "is_inner",
            ( bool ( NodeType::* )(  ) const )( &NodeType::is_inner )
        )
        .def(
            "dump",
            ( std::string ( NodeType::* )(  ) const )( &NodeType::dump )
        )

        // Operators


        // Iterators

        // .add_property(
        //     "links",
        //     boost::python::range ( &NodeType::begin_links, &NodeType::end_links )
        // )
    ;

}

#endif // include guard
