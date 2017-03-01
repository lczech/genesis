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
void PythonExportClass_::genesis::tree::IteratorNodeLinks(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorNodeLinks
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using IteratorNodeLinksType = IteratorNodeLinks<typename LinkType, typename NodeType, typename EdgeType>;

    pybind11::class_< IteratorNodeLinksType, std::shared_ptr<IteratorNodeLinksType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
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
            pybind11::init< IteratorNodeLinks const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "edge",
            ( EdgeType & ( IteratorNodeLinksType::* )(  ) const )( &IteratorNodeLinksType::edge )
        )
        .def(
            "is_first_iteration",
            ( bool ( IteratorNodeLinksType::* )(  ) const )( &IteratorNodeLinksType::is_first_iteration )
        )
        .def(
            "link",
            ( LinkType & ( IteratorNodeLinksType::* )(  ) const )( &IteratorNodeLinksType::link )
        )
        .def(
            "node",
            ( NodeType & ( IteratorNodeLinksType::* )(  ) const )( &IteratorNodeLinksType::node )
        )
        .def(
            "start_link",
            ( LinkType & ( IteratorNodeLinksType::* )(  ) const )( &IteratorNodeLinksType::start_link )
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}

template<typename ElementType>
void python_export_function_tree_iterator_node_links_ElementType ()
{
    scope.def(
        "node_links",
        ( utils::Range< IteratorNodeLinks< TreeLink const, TreeNode const, TreeEdge const  > > ( * )( ElementType const & ))( &::genesis::tree::node_links ),
            pybind11::arg("element")
    );

    scope.def(
        "node_links",
        ( utils::Range< IteratorNodeLinks< TreeLink, TreeNode, TreeEdge > > ( * )( ElementType & ))( &::genesis::tree::node_links ),
            pybind11::arg("element")
    );
}
