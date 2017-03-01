/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::tree;

template <typename LinkType, typename NodeType, typename EdgeType>
void PythonExportClass_::genesis::tree::IteratorPreorder(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorPreorder
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using IteratorPreorderType = IteratorPreorder<typename LinkType, typename NodeType, typename EdgeType>;

    pybind11::class_< IteratorPreorderType, std::shared_ptr<IteratorPreorderType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Tree & >(),
            pybind11::arg("tree")
        )
        .def(
            pybind11::init< Tree const & >(),
            pybind11::arg("tree")
        )
        .def(
            pybind11::init< NodeType & >(),
            pybind11::arg("node")
        )
        .def(
            pybind11::init< LinkType & >(),
            pybind11::arg("link")
        )
        .def(
            pybind11::init< IteratorPreorder const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "edge",
            ( EdgeType & ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::edge )
        )
        .def(
            "is_first_iteration",
            ( bool ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::is_first_iteration )
        )
        .def(
            "link",
            ( LinkType & ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::link )
        )
        .def(
            "node",
            ( NodeType & ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::node )
        )
        .def(
            "start_link",
            ( LinkType & ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::start_link )
        )
        .def(
            "start_node",
            ( NodeType & ( IteratorPreorderType::* )(  ) const )( &IteratorPreorderType::start_node )
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}

template<typename ElementType>
void python_export_function_tree_iterator_preorder_ElementType ()
{
    scope.def(
        "preorder",
        ( utils::Range< IteratorPreorder< TreeLink const, TreeNode const, TreeEdge const  > > ( * )( ElementType const & ))( &::genesis::tree::preorder ),
            pybind11::arg("element")
    );

    scope.def(
        "preorder",
        ( utils::Range< IteratorPreorder< TreeLink, TreeNode, TreeEdge > > ( * )( ElementType & ))( &::genesis::tree::preorder ),
            pybind11::arg("element")
    );
}
