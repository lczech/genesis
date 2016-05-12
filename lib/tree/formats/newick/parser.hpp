#ifndef GENESIS_TREE_FORMATS_NEWICK_PARSER_H_
#define GENESIS_TREE_FORMATS_NEWICK_PARSER_H_

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
 * @ingroup tree
 */

#include "tree/formats/newick/lexer.hpp"

#include <string>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class  NewickBroker;

// =================================================================================================
//     Newick Parser and Generator
// =================================================================================================

bool parse_newick_tree (
    NewickLexer::iterator      & ct,
    NewickLexer::iterator const& end,
    NewickBroker&                broker
);

std::string generate_newick_tree (
    NewickBroker const& broker
);

} // namespace tree
} // namespace genesis

#endif // include guard
