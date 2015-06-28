#ifndef GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_LINK_H_
#define GENESIS_BOOST_PYTHON_EXPORT_TREE_TREE_LINK_H_

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
void BoostPythonExport_TreeLink (std::string name)
{
    typedef ::genesis::TreeEdge<NodeDataType, EdgeDataType> EdgeType;
    typedef ::genesis::TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef ::genesis::TreeNode<NodeDataType, EdgeDataType> NodeType;

    boost::python::class_< LinkType, boost::noncopyable > ( name.c_str() )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( LinkType::* )(  ) const )( &LinkType::dump ),
            get_docstring("std::string ::genesis::TreeLink::dump () const")
        )
        .def(
            "edge",
            ( EdgeType * ( LinkType::* )(  ) const )( &LinkType::edge ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("EdgeType * ::genesis::TreeLink::edge () const")
        )
        .def(
            "index",
            ( size_t ( LinkType::* )(  ) const )( &LinkType::index ),
            get_docstring("size_t ::genesis::TreeLink::index () const")
        )
        .def(
            "is_inner",
            ( bool ( LinkType::* )(  ) const )( &LinkType::is_inner ),
            get_docstring("bool ::genesis::TreeLink::is_inner () const")
        )
        .def(
            "is_leaf",
            ( bool ( LinkType::* )(  ) const )( &LinkType::is_leaf ),
            get_docstring("bool ::genesis::TreeLink::is_leaf () const")
        )
        .def(
            "next",
            ( LinkType * ( LinkType::* )(  ) const )( &LinkType::next ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType * ::genesis::TreeLink::next () const")
        )
        .def(
            "node",
            ( NodeType * ( LinkType::* )(  ) const )( &LinkType::node ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("NodeType * ::genesis::TreeLink::node () const")
        )
        .def(
            "outer",
            ( LinkType * ( LinkType::* )(  ) const )( &LinkType::outer ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType * ::genesis::TreeLink::outer () const")
        )
        .def(
            "prev",
            ( LinkType * ( LinkType::* )(  ))( &LinkType::prev ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType * ::genesis::TreeLink::prev ()")
        )

        // Operators

    ;

}

#endif // include guard
