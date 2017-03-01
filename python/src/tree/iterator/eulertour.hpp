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
void PythonExportClass_::genesis::tree::IteratorEulertour(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorEulertour
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using IteratorEulertourType = IteratorEulertour<typename LinkType, typename NodeType, typename EdgeType>;

    pybind11::class_< IteratorEulertourType, std::shared_ptr<IteratorEulertourType> > ( scope, name.c_str() )
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
            pybind11::init< IteratorEulertour const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "edge",
            ( EdgeType & ( IteratorEulertourType::* )(  ) const )( &IteratorEulertourType::edge )
        )
        .def(
            "is_first_iteration",
            ( bool ( IteratorEulertourType::* )(  ) const )( &IteratorEulertourType::is_first_iteration )
        )
        .def(
            "link",
            ( LinkType & ( IteratorEulertourType::* )(  ) const )( &IteratorEulertourType::link )
        )
        .def(
            "node",
            ( NodeType & ( IteratorEulertourType::* )(  ) const )( &IteratorEulertourType::node )
        )
        .def(
            "start_link",
            ( LinkType & ( IteratorEulertourType::* )(  ) const )( &IteratorEulertourType::start_link )
        )
        .def(
            "start_node",
            ( NodeType & ( IteratorEulertourType::* )(  ) const )( &IteratorEulertourType::start_node )
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}

template<typename ElementType>
void python_export_function_tree_iterator_eulertour_ElementType ()
{
    scope.def(
        "eulertour",
        ( utils::Range< IteratorEulertour< TreeLink const, TreeNode const, TreeEdge const  > > ( * )( ElementType const & ))( &::genesis::tree::eulertour ),
            pybind11::arg("element")
    );

    scope.def(
        "eulertour",
        ( utils::Range< IteratorEulertour< TreeLink, TreeNode, TreeEdge > > ( * )( ElementType & ))( &::genesis::tree::eulertour ),
            pybind11::arg("element")
    );
}
