/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/tree/printer/detailed.hpp"

#include "genesis/tree/tree.hpp"

// TODO used for conversion - ensure typesafety!
#include "genesis/tree/default/tree.hpp"

#include "genesis/tree/function/distances.hpp"

#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/text/style.hpp"

#include <assert.h>
#include <sstream>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Print Detailed
// =================================================================================================

void PrinterDetailed::print (
    std::ostream&   out,
    Tree const& tree
)  const {
    std::vector<size_t> depth = node_path_length_vector(tree);
    std::vector<size_t> done;

    // Prepare text color styles.
    auto node_color = utils::Style( "red", true );
    auto link_color = utils::Style( "blue" );
    auto next_color = utils::Style( "green" );
    auto edge_color = utils::Style( "cyan" );

    if( ! use_color_ ) {
        node_color.enabled( false );
        link_color.enabled( false );
        next_color.enabled( false );
        edge_color.enabled( false );
    }

    // prepare link so that we point to the root link. this will ensure that the order of nodes
    // displayed by this funtion is the one expected by the user. usually, we would go into
    // the first branch immediately, but then there would be no way of first nicely displaying
    // the information about the root node. so we need to do it a bit more complex than the
    // usual iteration...
    auto l = &tree.root_link();
    while( &l->next() != &tree.root_link() ) {
        l = &l->next();
    }

    // do an euler tour traversal over all links. (we cannot use the iterator here, as
    // we need each link on its own, and not each node as the iterator gives)
    do {
        auto& n = l->node();
        std::string indent = std::string(4 * depth[n.index()], ' ');
        if( ! utils::contains( done, n.index() )) {
            out << indent
                // TODO typesafety!
                << node_color( "Node " + std::to_string(n.index()) + ": \"" + n.data<DefaultNodeData>().name + "\"" )
                << "\n";
        }
        done.push_back(n.index());

        // dont display the next link when we are at the first iteration.
        if( &l->next() == &tree.root_link() ) {
            l = &l->next();
        } else {
            out << indent;
            out << "    " << link_color( "Link " + std::to_string( l->index() ));
            l = &l->next();
            out << " " << next_color(">") << " "
                << link_color( "Link " + std::to_string( l->index() )) << "\n";
        }

        out << indent;
        out << " -- " << link_color( "Link " + std::to_string( l->index() ));
        out << " -- " << edge_color( "Edge " + std::to_string( l->edge().index() ));
        l = &l->outer();
        out << " --> " << link_color( "Link " + std::to_string( l->index() )) << "\n";
    } while( &l->next() != &tree.root_link() );

    // output the last next link back to the root, because we skipped this in the loop
    // (the one that was skipped in the beginning).
    out << "    " << link_color( "Link " + std::to_string( l->index() ));
    l = &l->next();
    out << " " << next_color(">") << " "
        << link_color( "Link " + std::to_string( l->index() )) << "\n";
}

std::string PrinterDetailed::print (
    Tree const& tree
)  const {
    std::ostringstream res;
    print( res, tree );
    return res.str();
}

} // namespace tree
} // namespace genesis
