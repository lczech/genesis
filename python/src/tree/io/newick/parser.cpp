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

PYTHON_EXPORT_FUNCTIONS(tree_io_newick_parser_export, "tree")
{

    boost::python::def(
        "parse_newick_tree",
        ( bool ( * )( NewickLexer::iterator &, const NewickLexer::iterator &, NewickBroker & ))( &::genesis::tree::parse_newick_tree ),
        ( boost::python::arg("ct"), boost::python::arg("end"), boost::python::arg("broker") ),
        get_docstring("bool ::genesis::tree::parse_newick_tree (NewickLexer::iterator & ct, const NewickLexer::iterator & end, NewickBroker & broker)")
    );

    // boost::python::def(
    //     "element_to_string",
    //     ( std::string ( * )( NewickBrokerElement const & ))( &::genesis::tree::element_to_string ),
    //     ( boost::python::arg("bn") )
    // );

    boost::python::def(
        "generate_newick_tree",
        ( std::string ( * )( NewickBroker const & ))( &::genesis::tree::generate_newick_tree ),
        ( boost::python::arg("broker") )
    );

    // boost::python::def(
    //     "to_string_rec",
    //     ( std::string ( * )( NewickBroker const &, size_t ))( &::genesis::tree::to_string_rec ),
    //     ( boost::python::arg("broker"), boost::python::arg("pos") )
    // );
}
