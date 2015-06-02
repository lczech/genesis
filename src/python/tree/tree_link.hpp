#ifndef GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_LINK_H_
#define GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_LINK_H_

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
void BoostPythonExport_TreeLink (std::string name)
{
    typedef ::genesis::TreeEdge<NodeDataType, EdgeDataType> EdgeType;
    typedef ::genesis::TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef ::genesis::TreeNode<NodeDataType, EdgeDataType> NodeType;

    boost::python::class_< LinkType, boost::noncopyable > ( name.c_str() )

        // Public Member Functions

        .def(
            "next",
            ( LinkType * ( LinkType::* )(  ) const )( &LinkType::next ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            "Returns a pointer to the next link within the node."
        )
        .def(
            "prev",
            ( LinkType * ( LinkType::* )(  ) const )( &LinkType::prev ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            "Returns a pointer to the previous link within the node."
        )
        .def(
            "outer",
            ( LinkType * ( LinkType::* )(  ) const )( &LinkType::outer ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            "Returns a pointer to the link of the adjacent node."
        )
        .def(
            "edge",
            ( EdgeType * ( LinkType::* )(  ) const )( &LinkType::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            "Returns a pointer to the TreeEdge containing the data of this link's edge."
        )
        .def(
            "node",
            ( NodeType * ( LinkType::* )(  ) const )( &LinkType::node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            "Returns a pointer to the TreeNode containing the data of this link's node."
        )
        .def(
            "index",
            ( size_t ( LinkType::* )(  ) const )( &LinkType::index ),
            "Returns the index of this Link."
        )
        .def(
            "is_leaf",
            ( bool ( LinkType::* )(  ) const )( &LinkType::is_leaf ),
            "Returns true iff the node of this link is a leaf node."
        )
        .def(
            "is_inner",
            ( bool ( LinkType::* )(  ) const )( &LinkType::is_inner ),
            "Returns true iff the node of this link is an inner node."
        )
        .def(
            "dump",
            ( std::string ( LinkType::* )(  ) const )( &LinkType::dump ),
            "Returns a string containing dump information about this link."
        )

        // Operators

    ;

}

#endif // include guard
