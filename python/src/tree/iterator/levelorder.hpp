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
void PythonExportClass_::genesis::tree::IteratorLevelorder(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorLevelorder
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using IteratorLevelorderType = IteratorLevelorder<typename LinkType, typename NodeType, typename EdgeType>;

    pybind11::class_< IteratorLevelorderType, std::shared_ptr<IteratorLevelorderType> > ( scope, name.c_str() )
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
            pybind11::init< IteratorLevelorder const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "depth",
            ( int ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::depth )
        )
        .def(
            "edge",
            ( EdgeType & ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::edge )
        )
        .def(
            "is_first_iteration",
            ( bool ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::is_first_iteration )
        )
        .def(
            "link",
            ( LinkType & ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::link )
        )
        .def(
            "node",
            ( NodeType & ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::node )
        )
        .def(
            "start_link",
            ( LinkType & ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::start_link )
        )
        .def(
            "start_node",
            ( NodeType & ( IteratorLevelorderType::* )(  ) const )( &IteratorLevelorderType::start_node )
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}

template<typename ElementType>
void python_export_function_tree_iterator_levelorder_ElementType ()
{
    scope.def(
        "levelorder",
        ( utils::Range< IteratorLevelorder< TreeLink const, TreeNode const, TreeEdge const  > > ( * )( ElementType const & ))( &::genesis::tree::levelorder ),
            pybind11::arg("element")
    );

    scope.def(
        "levelorder",
        ( utils::Range< IteratorLevelorder< TreeLink, TreeNode, TreeEdge > > ( * )( ElementType & ))( &::genesis::tree::levelorder ),
            pybind11::arg("element")
    );
}
