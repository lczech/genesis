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
void PythonExportClass_::genesis::tree::IteratorPathSet(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorPathSet
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using IteratorPathSetType = IteratorPathSet<typename LinkType, typename NodeType, typename EdgeType>;

    pybind11::class_< IteratorPathSetType, std::shared_ptr<IteratorPathSetType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< NodeType &, NodeType &, NodeType & >(),
            pybind11::arg("start"),
            pybind11::arg("finish"),
            pybind11::arg("lca")
        )
        .def(
            pybind11::init< LinkType &, LinkType &, LinkType & >(),
            pybind11::arg("start"),
            pybind11::arg("finish"),
            pybind11::arg("lca")
        )
        .def(
            pybind11::init< IteratorPathSet const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "edge",
            ( EdgeType & ( IteratorPathSetType::* )(  ) const )( &IteratorPathSetType::edge )
        )
        .def(
            "finish_link",
            ( LinkType & ( IteratorPathSetType::* )(  ) const )( &IteratorPathSetType::finish_link )
        )
        .def(
            "finish_node",
            ( NodeType & ( IteratorPathSetType::* )(  ) const )( &IteratorPathSetType::finish_node )
        )
        .def(
            "is_last_common_ancestor",
            ( bool ( IteratorPathSetType::* )(  ) const )( &IteratorPathSetType::is_last_common_ancestor )
        )
        .def(
            "is_lca",
            ( bool ( IteratorPathSetType::* )(  ) const )( &IteratorPathSetType::is_lca )
        )
        .def(
            "lca_link",
            ( LinkType & ( IteratorPathSetType::* )(  ) const )( &IteratorPathSetType::lca_link )
        )
        .def(
            "lca_node",
            ( NodeType & ( IteratorPathSetType::* )(  ) const )( &IteratorPathSetType::lca_node )
        )
        .def(
            "link",
            ( LinkType & ( IteratorPathSetType::* )(  ) const )( &IteratorPathSetType::link )
        )
        .def(
            "node",
            ( NodeType & ( IteratorPathSetType::* )(  ) const )( &IteratorPathSetType::node )
        )
        .def(
            "start_link",
            ( LinkType & ( IteratorPathSetType::* )(  ) const )( &IteratorPathSetType::start_link )
        )
        .def(
            "start_node",
            ( NodeType & ( IteratorPathSetType::* )(  ) const )( &IteratorPathSetType::start_node )
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}

template<typename ElementType>
void python_export_function_tree_iterator_path_set_ElementType ()
{
    scope.def(
        "path_set",
        ( utils::Range< IteratorPathSet< TreeLink const, TreeNode const, TreeEdge const  > > ( * )( ElementType const &, ElementType const &, ElementType const & ))( &::genesis::tree::path_set ),
            pybind11::arg("start"),
            pybind11::arg("finish"),
            pybind11::arg("lca")
    );

    scope.def(
        "path_set",
        ( utils::Range< IteratorPathSet< TreeLink, TreeNode, TreeEdge > > ( * )( ElementType &, ElementType &, ElementType & ))( &::genesis::tree::path_set ),
            pybind11::arg("start"),
            pybind11::arg("finish"),
            pybind11::arg("lca")
    );
}
