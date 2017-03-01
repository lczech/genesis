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
void PythonExportClass_::genesis::tree::IteratorPostorder(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorPostorder
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using IteratorPostorderType = IteratorPostorder<typename LinkType, typename NodeType, typename EdgeType>;

    pybind11::class_< IteratorPostorderType, std::shared_ptr<IteratorPostorderType> > ( scope, name.c_str() )
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
            pybind11::init< IteratorPostorder const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "edge",
            ( EdgeType & ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::edge )
        )
        .def(
            "is_last_iteration",
            ( bool ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::is_last_iteration )
        )
        .def(
            "link",
            ( LinkType & ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::link )
        )
        .def(
            "node",
            ( NodeType & ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::node )
        )
        .def(
            "start_link",
            ( LinkType & ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::start_link )
        )
        .def(
            "start_node",
            ( NodeType & ( IteratorPostorderType::* )(  ) const )( &IteratorPostorderType::start_node )
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}

template<typename ElementType>
void python_export_function_tree_iterator_postorder_ElementType ()
{
    scope.def(
        "postorder",
        ( utils::Range< IteratorPostorder< TreeLink const, TreeNode const, TreeEdge const  > > ( * )( ElementType const & ))( &::genesis::tree::postorder ),
            pybind11::arg("element")
    );

    scope.def(
        "postorder",
        ( utils::Range< IteratorPostorder< TreeLink, TreeNode, TreeEdge > > ( * )( ElementType & ))( &::genesis::tree::postorder ),
            pybind11::arg("element")
    );
}
