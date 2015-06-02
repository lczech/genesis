#ifndef GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_EDGE_H_
#define GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_EDGE_H_

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
void BoostPythonExport_TreeEdge (std::string name)
{
    typedef ::genesis::TreeEdge<NodeDataType, EdgeDataType> EdgeType;
    typedef ::genesis::TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef ::genesis::TreeNode<NodeDataType, EdgeDataType> NodeType;

    boost::python::class_< EdgeType, boost::noncopyable > ( name.c_str() )

        // Public Member Functions

        .def(
            "primary_link",
            ( LinkType * ( EdgeType::* )(  ) const )( &EdgeType::primary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            "Returns the link of this edge that points towards the root."
        )
        .def(
            "secondary_link",
            ( LinkType * ( EdgeType::* )(  ) const )( &EdgeType::secondary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            "Returns the link of this edge that points away from the root."
        )
        .def(
            "primary_node",
            ( NodeType * ( EdgeType::* )(  ) const )( &EdgeType::primary_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "secondary_node",
            ( NodeType * ( EdgeType::* )(  ) const )( &EdgeType::secondary_node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "index",
            ( size_t ( EdgeType::* )(  ) const )( &EdgeType::index ),
            "Returns the index of this Link."
        )
        .def(
            "dump",
            ( std::string ( EdgeType::* )(  ) const )( &EdgeType::dump )
        )

        // Operators

    ;

}

#endif // include guard
