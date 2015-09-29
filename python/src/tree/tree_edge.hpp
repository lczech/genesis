#ifndef GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_EDGE_H_
#define GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_EDGE_H_

/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/tree/tree_edge.hpp"
#include "lib/tree/tree_link.hpp"
#include "lib/tree/tree_node.hpp"

template <class NodeDataType, class EdgeDataType>
void BoostPythonExport_TreeEdge (std::string name)
{
    typedef ::genesis::TreeEdge<NodeDataType, EdgeDataType> EdgeType;
    typedef ::genesis::TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef ::genesis::TreeNode<NodeDataType, EdgeDataType> NodeType;

    boost::python::class_< EdgeType,  boost::noncopyable > ( name.c_str() )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( EdgeType::* )(  ) const )( &EdgeType::dump ),
            get_docstring("std::string ::genesis::TreeEdge::dump () const")
        )
        .def(
            "index",
            ( size_t ( EdgeType::* )(  ) const )( &EdgeType::index ),
            get_docstring("size_t ::genesis::TreeEdge::index () const")
        )
        .def(
            "primary_link",
            ( LinkType * ( EdgeType::* )(  ) const )( &EdgeType::primary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType * ::genesis::TreeEdge::primary_link () const")
        )
        .def(
            "primary_node",
            ( NodeType * ( EdgeType::* )(  ) const )( &EdgeType::primary_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType * ::genesis::TreeEdge::primary_node () const")
        )
        .def(
            "secondary_link",
            ( LinkType * ( EdgeType::* )(  ) const )( &EdgeType::secondary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType * ::genesis::TreeEdge::secondary_link () const")
        )
        .def(
            "secondary_node",
            ( NodeType * ( EdgeType::* )(  ) const )( &EdgeType::secondary_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType * ::genesis::TreeEdge::secondary_node () const")
        )

        // Operators

    ;

}

#endif // include guard
