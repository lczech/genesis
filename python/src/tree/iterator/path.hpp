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
void PythonExportClass_::genesis::tree::IteratorPath(std::string name)
{

    // -------------------------------------------------------------------
    //     Class IteratorPath
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using IteratorPathType = IteratorPath<typename LinkType, typename NodeType, typename EdgeType>;

    pybind11::class_< IteratorPathType, std::shared_ptr<IteratorPathType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< NodeType &, NodeType & >(),
            pybind11::arg("start"),
            pybind11::arg("finish")
        )
        .def(
            pybind11::init< LinkType &, LinkType & >(),
            pybind11::arg("start"),
            pybind11::arg("finish")
        )
        .def(
            pybind11::init< IteratorPath const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "edge",
            ( EdgeType & ( IteratorPathType::* )(  ) const )( &IteratorPathType::edge )
        )
        .def(
            "finish_link",
            ( LinkType & ( IteratorPathType::* )(  ) const )( &IteratorPathType::finish_link )
        )
        .def(
            "finish_node",
            ( NodeType & ( IteratorPathType::* )(  ) const )( &IteratorPathType::finish_node )
        )
        .def(
            "is_last_common_ancestor",
            ( bool ( IteratorPathType::* )(  ) const )( &IteratorPathType::is_last_common_ancestor ),
            get_docstring("bool ::genesis::tree::IteratorPath::is_last_common_ancestor () const")
        )
        .def(
            "is_lca",
            ( bool ( IteratorPathType::* )(  ) const )( &IteratorPathType::is_lca ),
            get_docstring("bool ::genesis::tree::IteratorPath::is_lca () const")
        )
        .def(
            "link",
            ( LinkType & ( IteratorPathType::* )(  ) const )( &IteratorPathType::link )
        )
        .def(
            "node",
            ( NodeType & ( IteratorPathType::* )(  ) const )( &IteratorPathType::node )
        )
        .def(
            "start_link",
            ( LinkType & ( IteratorPathType::* )(  ) const )( &IteratorPathType::start_link )
        )
        .def(
            "start_node",
            ( NodeType & ( IteratorPathType::* )(  ) const )( &IteratorPathType::start_node )
        )

        // Operators

        .def( pybind11::self != pybind11::self )
        .def( pybind11::self == pybind11::self )
    ;
}

template<typename ElementType>
void python_export_function_tree_iterator_path_ElementType ()
{
    scope.def(
        "path",
        ( utils::Range< IteratorPath< TreeLink const, TreeNode const, TreeEdge const  > > ( * )( ElementType const &, ElementType const & ))( &::genesis::tree::path ),
            pybind11::arg("start"),
            pybind11::arg("finish")
    );

    scope.def(
        "path",
        ( utils::Range< IteratorPath< TreeLink, TreeNode, TreeEdge > > ( * )( ElementType &, ElementType & ))( &::genesis::tree::path ),
            pybind11::arg("start"),
            pybind11::arg("finish")
    );
}
