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

template<class Tree>
void python_export_function_tree_function_functions_Tree ()
{
    boost::python::def(
        "is_bifurcating",
        ( bool ( * )( Tree const & ))( &::genesis::tree::is_bifurcating ),
        ( boost::python::arg("tree") ),
        get_docstring("bool ::genesis::tree::is_bifurcating (Tree const & tree)")
    );

    boost::python::def(
        "max_rank",
        ( int ( * )( Tree const & ))( &::genesis::tree::max_rank ),
        ( boost::python::arg("tree") ),
        get_docstring("int ::genesis::tree::max_rank (Tree const & tree)")
    );

    boost::python::def(
        "inner_node_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::inner_node_count ),
        ( boost::python::arg("tree") ),
        get_docstring("size_t ::genesis::tree::inner_node_count (Tree const & tree)")
    );

    boost::python::def(
        "leaf_node_count",
        ( size_t ( * )( Tree const & ))( &::genesis::tree::leaf_node_count ),
        ( boost::python::arg("tree") ),
        get_docstring("size_t ::genesis::tree::leaf_node_count (Tree const & tree)")
    );
}
