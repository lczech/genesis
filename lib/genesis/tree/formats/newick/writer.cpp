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

#include "genesis/tree/formats/newick/writer.hpp"

#include "genesis/tree/formats/newick/broker.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/iterator/postorder.hpp"
#include "genesis/tree/tree_set.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/output_stream.hpp"

#include <cassert>
#include <deque>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Writing
// =================================================================================================

void NewickWriter::write( Tree const& tree, std::shared_ptr<utils::BaseOutputTarget> target ) const
{
    write( tree_to_broker( tree ), target );
}

void NewickWriter::write( TreeSet const& tree_set, std::shared_ptr<utils::BaseOutputTarget> target, bool with_names ) const
{
    auto& os = target->ostream();
    for( size_t i = 0; i < tree_set.size(); ++i ) {

        // Write the name if wanted. We here ignore that this makes the line longer,
        // and so the first line might exceed line_length a bit more. This is just a convenience
        // anyway.
        if( with_names ) {
            auto const& name = tree_set.name_at(i);
            bool need_qmarks = force_quot_marks_;
            need_qmarks |= ( std::string::npos != name.find_first_of( " :;()[],=" ));
            need_qmarks |= ( write_tags_ && std::string::npos != name.find_first_of( "{}" ));

            if( need_qmarks ) {
                os << quotation_mark_ << name << quotation_mark_;
            } else {
                os << name;
            }
            os << " = ";
        }

        write( tree_to_broker( tree_set.at(i) ), target );
    }
}

std::string NewickWriter::to_string( Tree const& tree ) const
{
    std::ostringstream oss;
    write( tree, utils::to_stream( oss ));
    return oss.str();
}

// =================================================================================================
//     Intermediate Functions
// =================================================================================================

NewickBroker NewickWriter::tree_to_broker( Tree const& tree ) const
{
    NewickBroker broker;
    for( auto const& prepare_plugin : prepare_writing_plugins ) {
        prepare_plugin( tree, broker );
    }

    // store the depth from each node to the root. this is needed to assign levels of depth
    // to the nodes for the broker.
    auto depth = node_path_length_vector(tree);

    // now fill the broker with nodes via postorder traversal, so that the root is put on top last.
    broker.clear();
    for( auto it : postorder(tree) ) {
        NewickBrokerElement bn;
        bn.depth = depth[ it.node().index() ];

        for( auto const& node_plugin : node_to_element_plugins ) {
            node_plugin( it.node(), bn );
        }
        // only write edge data to the broker element if it is not the last iteration.
        // the last iteration is the root, which usually does not have edge information in newick.
        // caveat: for the root node, the edge will point to an arbitrary edge away from the root.
        if( !it.is_last_iteration() ) {
            for( auto const& edge_plugin : edge_to_element_plugins ) {
                edge_plugin( it.edge(), bn );
            }
        }

        broker.push_top(bn);
    }

    broker.assign_ranks();
    for( auto const& finish_plugin : finish_writing_plugins ) {
        finish_plugin( tree, broker );
    }
    return broker;
}

void NewickWriter::write( NewickBroker const& broker, std::shared_ptr<utils::BaseOutputTarget> target ) const
{

    auto& os = target->ostream();

    // Assertion helpers: how many parenthesis were written?
    size_t op = 0;
    size_t cp = 0;

    // Iterate broker in reverse order, because Newick...
    size_t prev_depth = 0;
    size_t cur_length = 0;
    for( long pos = broker.size() - 1; pos >= 0; --pos ) {
        auto const& elem = broker[pos];
        if( elem.depth < 0 ) {
            throw std::runtime_error( "Invalid NewickBroker: Depth < 0." );
        }

        // Opening parenthesis.
        // We open as many as needed to get to the depth of the current element.
        // They will all be closed when processing the respective parent elements.
        assert( elem.depth >= 0 );
        for( size_t i = prev_depth; i < static_cast<size_t>( elem.depth ); ++i ) {
            os << "(";
            cur_length += 1;
            ++op;
        }

        // Write the NewickBrokerElement to the stream.
        cur_length += write_( broker[pos], os );

        // Stop if it is the root. Don't have to write parenthesis or commas after the root element.
        if( pos == 0 ) {
            continue;
        }

        // Closing parenthesis or comma for next element.
        // Even for "empty" elements (e.g., inner nodes with no names), this is called,
        // which ensures correct nesting.
        if( broker[ pos - 1 ].depth == elem.depth - 1 ) {
            os << ")";
            cur_length += 1;
            ++cp;
        } else {
            os << ",";
            cur_length += 1;
        }
        prev_depth = elem.depth;

        // Line length check.
        if( line_length_ > 0 && cur_length >= line_length_ ) {
            os << "\n";
            cur_length = 0;
        }
    }

    // Have to have written as many opening as closing parenthesis.
    // Use void casts to avoid compiler warnings in release mode.
    assert( op == cp );
    (void) op;
    (void) cp;

    os << ";";
    if( trailing_new_line_ ) {
        os << "\n";
    }
}

// =================================================================================================
//     Internal Functions
// =================================================================================================

size_t NewickWriter::write_( NewickBrokerElement const& bn, std::ostream& os ) const
{
    size_t length = 0;

    // Write name.
    if( write_names_ ) {
        // Find out whether we need to put this name in quotation marks.
        // According to http://evolution.genetics.washington.edu/phylip/newicktree.html :
        // "A name can be any string of printable characters except blanks, colons, semicolons,
        // parentheses, and square brackets." Well, they forgot to mention commas here.
        // But we knew before that Newick is not a good format anyway...
        // Also, if write_tags_ is true, we also quote {}, as those are used for tags.
        bool need_qmarks = force_quot_marks_;
        need_qmarks |= ( std::string::npos != bn.name.find_first_of( " :;()[]," ));
        need_qmarks |= ( write_tags_ && std::string::npos != bn.name.find_first_of( "{}" ));

        if( need_qmarks ) {
            os << quotation_mark_ << bn.name << quotation_mark_;
            length += 2 + bn.name.size();
        } else {
            os << bn.name;
            length += bn.name.size();
        }
    }

    // Write values (":...")
    if( write_values_ ) {
        for( std::string const& v : bn.values ) {
            os << ":" << v;
            length += 1 + v.size();
        }
    }

    // Write comments ("[...]")
    if( write_comments_ ) {
        for( std::string const& c : bn.comments ) {
            os << "[" << c << "]";
            length += 1 + c.size();
        }
    }

    // Write tags ("{...}")
    if( write_tags_ ) {
        for( std::string const& t : bn.tags ) {
            os << "{" << t << "}";
            length += 1 + t.size();
        }
    }

    return length;
}

// std::string NewickWriter::to_string_rec_( NewickBroker const& broker, size_t pos ) const
// {
//     // Old, recursive, slow version. Not used any more.
//
//     // check if it is a leaf, stop recursion if so.
//     if (broker[pos].rank() == 0) {
//         return element_to_string_(broker[pos]);
//     }
//
//     // recurse over all children of the current node. while doing so, build a stack of the resulting
//     // substrings in reverse order. this is because newick stores the nodes kind of "backwards",
//     // by starting at a leaf node instead of the root.
//     std::deque<std::string> children;
//     for (size_t i = pos + 1; i < broker.size() && broker[i].depth > broker[pos].depth; ++i) {
//         // skip if not immediate children (those will be called in later recursion steps)
//         if (broker[i].depth > broker[pos].depth + 1) {
//             continue;
//         }
//
//         // do the recursion step for this child, add the result to a stack
//         children.push_front(to_string_rec_(broker, i));
//     }
//
//     // build the string by iterating the stack
//     std::ostringstream out;
//     out << "(";
//     for( size_t i = 0; i < children.size(); ++i ) {
//         if (i>0) {
//             out << ",";
//         }
//         out << children[i];
//     }
//     out << ")" << element_to_string_(broker[pos]);
//     return out.str();
// }

} // namespace tree
} // namespace genesis
