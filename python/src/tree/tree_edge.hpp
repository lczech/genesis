/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "tree/tree_edge.hpp"

template <class NodeDataType, class EdgeDataType>
void PythonExportClass_TreeEdge(std::string name)
{

    // -------------------------------------------------------------------
    //     Class TreeEdge
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using TreeEdgeType = TreeEdge<class NodeDataType, class EdgeDataType>;

    boost::python::class_< TreeEdgeType > ( name.c_str(), boost::python::init<  >(  ) )
        .def( boost::python::init< size_t, LinkType *, LinkType * >(( boost::python::arg("index"), boost::python::arg("primary_link"), boost::python::arg("secondary_link") )) )
        .def( boost::python::init< TreeEdge const & >(( boost::python::arg("") )) )
        .def( boost::python::init< TreeEdge && >(( boost::python::arg("") )) )

        // Public Member Functions

        .def(
            "dump",
            ( std::string ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::dump ),
            get_docstring("std::string ::genesis::tree::TreeEdge::dump () const")
        )
        .def(
            "index",
            ( size_t ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::index ),
            get_docstring("size_t ::genesis::tree::TreeEdge::index () const")
        )
        .def(
            "primary_link",
            ( LinkType & ( TreeEdgeType::* )(  ))( &TreeEdgeType::primary_link ),
            get_docstring("LinkType & ::genesis::tree::TreeEdge::primary_link ()")
        )
        .def(
            "primary_link",
            ( LinkType const & ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::primary_link ),
            get_docstring("LinkType const & ::genesis::tree::TreeEdge::primary_link () const")
        )
        .def(
            "primary_node",
            ( NodeType & ( TreeEdgeType::* )(  ))( &TreeEdgeType::primary_node ),
            get_docstring("NodeType & ::genesis::tree::TreeEdge::primary_node ()")
        )
        .def(
            "primary_node",
            ( NodeType const & ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::primary_node ),
            get_docstring("NodeType const & ::genesis::tree::TreeEdge::primary_node () const")
        )
        .def(
            "reset_index",
            ( TreeEdge & ( TreeEdgeType::* )( size_t ))( &TreeEdgeType::reset_index ),
            ( boost::python::arg("val") )
        )
        .def(
            "reset_primary_link",
            ( TreeEdge & ( TreeEdgeType::* )( LinkType * ))( &TreeEdgeType::reset_primary_link ),
            ( boost::python::arg("val") )
        )
        .def(
            "reset_secondary_link",
            ( TreeEdge & ( TreeEdgeType::* )( LinkType * ))( &TreeEdgeType::reset_secondary_link ),
            ( boost::python::arg("val") )
        )
        .def(
            "secondary_link",
            ( LinkType & ( TreeEdgeType::* )(  ))( &TreeEdgeType::secondary_link ),
            get_docstring("LinkType & ::genesis::tree::TreeEdge::secondary_link ()")
        )
        .def(
            "secondary_link",
            ( LinkType const & ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::secondary_link ),
            get_docstring("LinkType const & ::genesis::tree::TreeEdge::secondary_link () const")
        )
        .def(
            "secondary_node",
            ( NodeType & ( TreeEdgeType::* )(  ))( &TreeEdgeType::secondary_node ),
            get_docstring("NodeType & ::genesis::tree::TreeEdge::secondary_node ()")
        )
        .def(
            "secondary_node",
            ( NodeType const & ( TreeEdgeType::* )(  ) const )( &TreeEdgeType::secondary_node ),
            get_docstring("NodeType const & ::genesis::tree::TreeEdge::secondary_node () const")
        )
    ;
}
