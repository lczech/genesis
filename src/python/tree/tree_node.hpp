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

const char* get_docstring (const std::string& signature);

template <class NodeDataType, class EdgeDataType>
void BoostPythonExport_TreeNode (std::string name)
{
    typedef ::genesis::TreeEdge<NodeDataType, EdgeDataType> EdgeType;
    typedef ::genesis::TreeLink<NodeDataType, EdgeDataType> LinkType;
    typedef ::genesis::TreeNode<NodeDataType, EdgeDataType> NodeType;

    // boost::python::class_< NodeType, boost::python::bases<NodeDataType>, boost::noncopyable > ( name.c_str() )
    boost::python::class_< NodeType, boost::python::bases<NodeDataType> > ( name.c_str() )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( NodeType::* )(  ) const )( &NodeType::dump ),
            get_docstring("std::string ::genesis::TreeNode::dump () const")
        )
        .def(
            "index",
            ( size_t ( NodeType::* )(  ) const )( &NodeType::index ),
            get_docstring("size_t ::genesis::TreeNode::index () const")
        )
        .def(
            "is_inner",
            ( bool ( NodeType::* )(  ) const )( &NodeType::is_inner ),
            get_docstring("bool ::genesis::TreeNode::is_inner () const")
        )
        .def(
            "is_leaf",
            ( bool ( NodeType::* )(  ) const )( &NodeType::is_leaf ),
            get_docstring("bool ::genesis::TreeNode::is_leaf () const")
        )
        .def(
            "link",
            ( LinkType * ( NodeType::* )(  ) const )( &NodeType::link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType * ::genesis::TreeNode::link () const")
        )
        .def(
            "primary_link",
            ( LinkType * ( NodeType::* )(  ) const )( &NodeType::primary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType * ::genesis::TreeNode::primary_link () const")
        )
        .def(
            "rank",
            ( int ( NodeType::* )(  ) const )( &NodeType::rank ),
            get_docstring("int ::genesis::TreeNode::rank () const")
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
