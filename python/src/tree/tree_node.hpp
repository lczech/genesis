/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <class NodeDataType, class EdgeDataType>
void PythonExportClass_TreeNode(std::string name)
{

    // -------------------------------------------------------------------
    //     Class TreeNode
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using TreeNodeType = TreeNode<class NodeDataType, class EdgeDataType>;

    boost::python::class_< TreeNodeType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< size_t, LinkType * >(( boost::python::arg("index"), boost::python::arg("primary_link") )) )
        .def( boost::python::init< TreeNode const & >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( TreeNodeType::* )(  ) const )( &TreeNodeType::dump ),
            get_docstring("std::string ::genesis::tree::TreeNode::dump () const")
        )
        .def(
            "index",
            ( size_t ( TreeNodeType::* )(  ) const )( &TreeNodeType::index ),
            get_docstring("size_t ::genesis::tree::TreeNode::index () const")
        )
        .def(
            "is_inner",
            ( bool ( TreeNodeType::* )(  ) const )( &TreeNodeType::is_inner ),
            get_docstring("bool ::genesis::tree::TreeNode::is_inner () const")
        )
        .def(
            "is_leaf",
            ( bool ( TreeNodeType::* )(  ) const )( &TreeNodeType::is_leaf ),
            get_docstring("bool ::genesis::tree::TreeNode::is_leaf () const")
        )
        .def(
            "link",
            ( LinkType & ( TreeNodeType::* )(  ))( &TreeNodeType::link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType & ::genesis::tree::TreeNode::link ()")
        )
        .def(
            "link",
            ( LinkType const & ( TreeNodeType::* )(  ) const )( &TreeNodeType::link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType const & ::genesis::tree::TreeNode::link () const")
        )
        .def(
            "primary_link",
            ( LinkType & ( TreeNodeType::* )(  ))( &TreeNodeType::primary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType & ::genesis::tree::TreeNode::primary_link ()")
        )
        .def(
            "primary_link",
            ( LinkType const & ( TreeNodeType::* )(  ) const )( &TreeNodeType::primary_link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>(),
            get_docstring("LinkType const & ::genesis::tree::TreeNode::primary_link () const")
        )
        .def(
            "rank",
            ( size_t ( TreeNodeType::* )(  ) const )( &TreeNodeType::rank ),
            get_docstring("size_t ::genesis::tree::TreeNode::rank () const")
        )
        .def(
            "reset_index",
            ( TreeNode & ( TreeNodeType::* )( size_t ))( &TreeNodeType::reset_index ),
            ( boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
        .def(
            "reset_primary_link",
            ( TreeNode & ( TreeNodeType::* )( LinkType * ))( &TreeNodeType::reset_primary_link ),
            ( boost::python::arg("val") ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
    ;
}
