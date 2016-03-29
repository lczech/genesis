/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

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
