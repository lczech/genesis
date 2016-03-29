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

template<class TreeType>
void python_export_function_tree_default_functions_TreeType ()
{
    boost::python::def(
        "find_node",
        ( TreeType::NodeType * ( * )( TreeType &, const std::string &, bool ))( &::genesis::tree::find_node ),
        ( boost::python::arg("tree"), boost::python::arg("name"), boost::python::arg("replace_underscores")=(bool)(false) ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("TreeType::NodeType * ::genesis::tree::find_node (TreeType & tree, const std::string & name, bool replace_underscores=false)")
    );

    boost::python::def(
        "node_names",
        ( std::vector< std::string > ( * )( TreeType &, bool ))( &::genesis::tree::node_names ),
        ( boost::python::arg("tree"), boost::python::arg("leaves_only")=(bool)(false) ),
        get_docstring("std::vector< std::string > ::genesis::tree::node_names (TreeType & tree, bool leaves_only=false)")
    );
}
