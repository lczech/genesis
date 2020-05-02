/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/tree/printer/compact.hpp"

#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/iterator/preorder.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <sstream>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Print Compact
// =================================================================================================

void PrinterCompact::print (
    std::ostream&   out,
    Tree const& tree,
    std::function<std::string (
        TreeNode const& node,
        TreeEdge const& edge
    )> const print_line
) {
    // Edge case: print nothing
    if( limit_ == 0 ) {
        return;
    }

    // Stores a count of how many child nodes each node has left for viewing.
    auto ranks   = std::vector<size_t>( tree.node_count(), 0 );

    // Store the current stack of parents while traversing.
    auto parents = std::vector<size_t>();

    // How many lines have been printed yet.
    // If this reaches the limit_, we print one for line for the ellipsis, then stop.
    long count = 0;

    for( auto it : preorder(tree) ) {
        if( limit_ > 0 && count > limit_ ) {
            break;
        }

        // Index of current and its parent node.
        size_t cur_idx = it.node().index();
        size_t par_idx = it.link().outer().node().index();

        // Set parent stack correctly (including current node), and store current rank.
        while (!parents.empty() && parents.back() != par_idx) {
            parents.pop_back();
        }
        parents.push_back(cur_idx);
        ranks[cur_idx] = degree( it.node() ) - 1;

        // The root node is special: We have to account for one more child, as it does not have a
        // parent. Also, we do not draw any lines or indention for the root.
        if (it.is_first_iteration()) {
            ++ranks[cur_idx];
            out << print_line( it.node(), it.edge() ) << "\n";
            ++count;
            continue;
        }

        // This point in code is reached for all nodes but the root. Thus, we already have at least
        // the root and the current node added to the parents stack. Also, the second but last
        // element will be the parent of the current node, and the last one the node itself.
        assert( parents.size() >= 2 );
        assert( parents[ parents.size() - 2 ] == par_idx );
        assert( parents[ parents.size() - 1 ] == cur_idx );

        // Draw indention lines for all non-immediate parents of the current node. If their rank
        // is zero, there will no other children follow, so do not draw a line then.
        for (size_t i = 0; i < parents.size() - 2; ++i) {
            if (ranks[parents[i]] > 0) {
                if( limit_ > 0 && count == limit_ ) {
                    out << "¦   ";
                } else {
                    out << "│   ";
                }
            } else {
                out << "    ";
            }
        }

        // We are about to draw a child of the parent. Prior to drawing, we need to reduce the
        // parents rank counter. If it then is zero, the current node is the last child of its
        // parent (which is drawn differently).
        // Also assert that it is not zero already, because this would mean that we are currently
        // processing more children of the parent than its rank indicated.
        assert(ranks[par_idx] > 0);
        --ranks[par_idx];

        if( limit_ > 0 && count == limit_ ) {

            // If this is the "extra" line to be printed after the main part,
            // use a broken bar to indicate ellipsis.
            out << "¦   ";
        } else {

            // Draw the lines down from the immediate parent of the current node.
            if (ranks[par_idx] > 0) {
                out << "├── ";
            } else {
                out << "└── ";
            }

            // Print the actual information about the current node.
            out << print_line( it.node(), it.edge() ) << "\n";
        }

        ++count;
    }
}

std::string PrinterCompact::print (
    Tree const& tree,
    std::function<std::string (
        TreeNode const& node,
        TreeEdge const& edge
    )> const print_line
) {
    std::ostringstream res;
    print( res, tree, print_line );
    return res.str();
}

std::string PrinterCompact::print( Tree const& tree )
{
    auto print_line = [] ( TreeNode const& node, TreeEdge const& edge )
    {
        std::string result;
        if( edge.has_data() ) {
            result += utils::to_string_nice( edge.data<CommonEdgeData>().branch_length );
        }
        if( edge.has_data() && node.has_data() && ! node.data<CommonNodeData>().name.empty() ) {
            result += " ";
        }
        if( node.has_data() ) {
            result += node.data<CommonNodeData>().name;
        }
        return result;
    };
    return print(tree, print_line);
}

} // namespace tree
} // namespace genesis
