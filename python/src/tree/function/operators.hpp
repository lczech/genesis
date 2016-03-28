/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::tree;

template<class TreeTypeL, class TreeTypeR>
void python_export_function_tree_function_operators_TreeTypeL_TreeTypeR ()
{
    boost::python::def(
        "equal",
        ( bool ( * )( const TreeTypeL &, const TreeTypeR & ))( &::genesis::tree::equal ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") ),
        get_docstring("bool ::genesis::tree::equal (const TreeTypeL & lhs, const TreeTypeR & rhs)")
    );

    boost::python::def(
        "equal",
        ( bool ( * )( const TreeTypeL &, const TreeTypeR &, std::function< bool(const typename TreeTypeL::NodeType &, const typename TreeTypeR::NodeType &) >, std::function< bool(const typename TreeTypeL::EdgeType &, const typename TreeTypeR::EdgeType &) > ))( &::genesis::tree::equal ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs"), boost::python::arg("node_comparator"), boost::python::arg("edge_comparator") ),
        get_docstring("bool ::genesis::tree::equal (const TreeTypeL & lhs, const TreeTypeR & rhs, std::function< bool(const typename TreeTypeL::NodeType &, const typename TreeTypeR::NodeType &) > node_comparator, std::function< bool(const typename TreeTypeL::EdgeType &, const typename TreeTypeR::EdgeType &) > edge_comparator)")
    );

    boost::python::def(
        "identical_topology",
        ( bool ( * )( const TreeTypeL &, const TreeTypeR & ))( &::genesis::tree::identical_topology ),
        ( boost::python::arg("lhs"), boost::python::arg("rhs") ),
        get_docstring("bool ::genesis::tree::identical_topology (const TreeTypeL & lhs, const TreeTypeR & rhs)")
    );
}

template<class TreeType>
void python_export_function_tree_function_operators_TreeType ()
{
    boost::python::def(
        "validate",
        ( bool ( * )( TreeType const & ))( &::genesis::tree::validate ),
        ( boost::python::arg("tree") ),
        get_docstring("bool ::genesis::tree::validate (TreeType const & tree)")
    );
}

