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

using namespace ::genesis::placement;

template<class TreeSetType>
void python_export_function_tree_function_tree_set_TreeSetType ()
{
    boost::python::def(
        "find_tree",
        ( TreeSetType::TreeType * ( * )( TreeSetType &, std::string const & ))( &::genesis::placement::find_tree ),
        ( boost::python::arg("tset"), boost::python::arg("name") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("TreeSetType::TreeType * ::genesis::placement::find_tree (TreeSetType & tset, std::string const & name)")
    );

    boost::python::def(
        "average_branch_length_tree",
        ( TreeSetType::TreeType ( * )( TreeSetType const & ))( &::genesis::placement::average_branch_length_tree ),
        ( boost::python::arg("tset") ),
        get_docstring("TreeSetType::TreeType ::genesis::placement::average_branch_length_tree (TreeSetType const & tset)")
    );

    boost::python::def(
        "find_tree",
        ( TreeSetType::TreeType const * ( * )( TreeSetType const &, std::string const & ))( &::genesis::placement::find_tree ),
        ( boost::python::arg("tset"), boost::python::arg("name") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("TreeSetType::TreeType const * ::genesis::placement::find_tree (TreeSetType const & tset, std::string const & name)")
    );

    boost::python::def(
        "all_equal",
        ( bool ( * )( TreeSetType const & ))( &::genesis::placement::all_equal ),
        ( boost::python::arg("tset") ),
        get_docstring("bool ::genesis::placement::all_equal (TreeSetType const & tset)")
    );

    boost::python::def(
        "all_equal",
        ( bool ( * )( TreeSetType const &, std::function< bool(typename TreeSetType::TreeType::NodeType const &, typename TreeSetType::TreeType::NodeType const &)>, std::function< bool(typename TreeSetType::TreeType::EdgeType const &, typename TreeSetType::TreeType::EdgeType const &)> ))( &::genesis::placement::all_equal ),
        ( boost::python::arg("tset"), boost::python::arg("node_comparator"), boost::python::arg("edge_comparator") ),
        get_docstring("bool ::genesis::placement::all_equal (TreeSetType const & tset, std::function< bool(typename TreeSetType::TreeType::NodeType const &, typename TreeSetType::TreeType::NodeType const &)> node_comparator, std::function< bool(typename TreeSetType::TreeType::EdgeType const &, typename TreeSetType::TreeType::EdgeType const &)> edge_comparator)")
    );

    boost::python::def(
        "all_identical_topology",
        ( bool ( * )( TreeSetType const & ))( &::genesis::placement::all_identical_topology ),
        ( boost::python::arg("tset") ),
        get_docstring("bool ::genesis::placement::all_identical_topology (TreeSetType const & tset)")
    );
}
