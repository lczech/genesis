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

#include "genesis/tree/formats/newick/reader.hpp"

#include "genesis/tree/formats/newick/broker.hpp"
#include "genesis/tree/tree_set.hpp"
#include "genesis/tree/tree.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/std.hpp"

#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <deque>
#include <memory>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Reading a single Tree
// =================================================================================================

Tree NewickReader::from_stream( std::istream& input_stream ) const
{
    utils::InputStream it( utils::make_unique< utils::StreamInputSource >( input_stream ));
    return parse_single_tree( it );
}

Tree NewickReader::from_file( std::string const& filename ) const
{
    utils::InputStream it( utils::make_unique< utils::FileInputSource >( filename ));
    return parse_single_tree( it );
}

Tree NewickReader::from_string( std::string const& tree_string ) const
{
    utils::InputStream it( utils::make_unique< utils::StringInputSource >( tree_string ));
    return parse_single_tree( it );
}

// =================================================================================================
//     Reading into a TreeSet
// =================================================================================================

void NewickReader::from_stream(
    std::istream& input_stream,
    TreeSet&           tree_set,
    std::string const& default_name
) const {
    utils::InputStream it( utils::make_unique< utils::StreamInputSource >( input_stream ));
    parse_multiple_trees( it, tree_set, default_name );
}

void NewickReader::from_file (
    std::string const& filename,
    TreeSet&           tree_set,
    std::string const& default_name
) const {
    utils::InputStream it( utils::make_unique< utils::FileInputSource >( filename ));
    parse_multiple_trees( it, tree_set, default_name );
}

void NewickReader::from_string (
    std::string const& tree_string,
    TreeSet&           tree_set,
    std::string const& default_name
) const {
    utils::InputStream it( utils::make_unique< utils::StringInputSource >( tree_string ));
    parse_multiple_trees( it, tree_set, default_name );
}

// =================================================================================================
//     Reading multiple input sources
// =================================================================================================

void NewickReader::from_files (
    std::vector<std::string> const& filenames,
    TreeSet&                        tree_set
) const {
    for( auto const& fn : filenames ) {
        from_file( fn, tree_set );
    }
}

void NewickReader::from_strings (
    std::vector<std::string> const& tree_strings,
    TreeSet&                        tree_set,
    std::string const&              default_name
) const {
    for( auto const& ts : tree_strings ) {
        from_string (ts, tree_set, default_name);
    }
}

// =================================================================================================
//     Parse Single Tree
// =================================================================================================

Tree NewickReader::parse_single_tree( utils::InputStream& input_stream ) const
{
    // Get name and tree, only use tree.
    auto tree = parse_named_tree( input_stream ).second;

    // If we just read this tree, continue until end of stream.
    if( ! stop_at_semicolon_ ) {
        parse_trailing_input_( input_stream );
    }

    // Return resulting tree.
    return tree;
}

// =================================================================================================
//     Parse Multiple Trees
// =================================================================================================

void NewickReader::parse_multiple_trees(
    utils::InputStream& input_stream,
    TreeSet&            tree_set,
    std::string const&  default_name
) const {
    // Count how many unnamed trees we have seen.
    size_t unnamed_ctr = 0;

    while( input_stream ) {

        // Get name and tree.
        auto named_tree = parse_named_tree( input_stream );

        // If there are no trees left, we are done.
        if( named_tree.first.empty() && named_tree.second.empty() ) {
            return;
        }

        // Fill in default name if needed.
        if( named_tree.first.empty() ) {
            named_tree.first = default_name + std::to_string( unnamed_ctr );
            ++unnamed_ctr;
        }

        // Store it in the TreeSet, without any copy steps.
        tree_set.add( std::move( named_tree.first ), std::move( named_tree.second ));
    }
}

// =================================================================================================
//     Parse Named Tree
// =================================================================================================

std::pair< std::string, Tree > NewickReader::parse_named_tree( utils::InputStream& input_stream ) const
{
    // Helper function for valid tree name chars.
    auto is_valid_tree_name_char = [&]( char c ){
        return   isprint(c)
            && ! isspace(c)
            && c != ';'
            && c != '('
            && c != ')'
            && c != '='
        ;
    };

    // Skip leading stuff.
    while( input_stream ) {

        // Whitespaces.
        utils::skip_while( input_stream, isspace );

        // No input, return empty tree.
        // We can never read an empty tree from an input, so this is useful to distungish
        // whether we were able to read a tree from the input.
        if( ! input_stream ) {
            return {};
        }

        // Skip comments.
        if( *input_stream == '[' ) {
            ++input_stream;
            utils::read_until( input_stream, ']' );

            if( !input_stream ) {
                throw std::runtime_error(
                    "Reached unexpected end of Newick tree at " + input_stream.at()
                );
            }
            assert( *input_stream == ']' );
            ++input_stream;

            continue;
        }

        // If neither applies, we are done here.
        break;
    }
    assert( input_stream );

    // Get the name of the current tree, if there is one.
    std::string name = "";
    if( *input_stream != '(' ) {

        // Distinguish between names in quotes, and those without.
        // Names without quotes cannot contain certain chars, see is_valid_tree_name_char().
        if( *input_stream == '"' || *input_stream == '\'' ) {
            name = utils::parse_quoted_string( input_stream, false, true, false );
        } else {
            name =  utils::read_while( input_stream, is_valid_tree_name_char );
        }

        // Always allow white spaces...
        utils::skip_while( input_stream, isspace );

        // After a name, we expect an equals sign.
        if( *input_stream != '=' ) {
            throw std::runtime_error(
                "Invalid character '" + std::string( 1, *input_stream ) + "' at "
                + input_stream.at() + ". Expecting '='."
            );
        }

        assert( *input_stream == '=' );
        ++input_stream;

        // After a name, there has to be something.
        if( ! input_stream ) {
            throw std::runtime_error( "Unexpected end of tree at " + input_stream.at() + "." );
        }
    }

    // Parse the tree and return it.
    return { name, broker_to_tree_( parse_tree_to_broker_( input_stream )) };
}

// =================================================================================================
//     Parse Trailing Input
// =================================================================================================

void NewickReader::parse_trailing_input_( utils::InputStream& input_stream ) const
{
    // Check for more data after the semicolon. We cannot do this check in the parsing function,
    // as there are cases where we read a Newick tree as part of another file (e.g, Nexus or Jplace),
    // where it is natural that there is more data after the tree finished.
    Token ct;
    ct.type = TokenType::kUnknown;
    while( input_stream && ct.type != TokenType::kEnd ) {
        ct = get_next_token_( input_stream );
        if( ct.type != TokenType::kEnd && ct.type != TokenType::kComment ) {
            throw std::runtime_error( "Tree contains more data after the semicolon at " + ct.at() );
        }
    }
}

// =================================================================================================
//     Token Lexing
// =================================================================================================

NewickReader::Token NewickReader::get_next_token_( utils::InputStream& input_stream ) const
{
    // Prepare result token.
    Token result;

    // Shorthand.
    auto& is = input_stream;

    // Helper function to distinguish valid chars in a Newick name string.
    // According to http://evolution.genetics.washington.edu/phylip/newicktree.html :
    // "A name can be any string of printable characters except blanks, colons, semicolons,
    // parentheses, and square brackets." Well, they forgot to mention commas here.
    // But we knew before that Newick is not a good format anyway...
    // Also, if enable_tags_ is true, we do not allow {}, as those are used for tags.
    auto is_valid_name_char = [&]( char c ){
        return   isprint(c)
            && ! isspace(c)
            && c != ':'
            && c != ';'
            && c != '('
            && c != ')'
            && c != '['
            && c != ']'
            && c != ','
            && ( ! enable_tags_ || ( c != '{' && c != '}' ))
        ;
    };

    // Skip initial whitespace, then set the current position in the stream.
    // This is where the token begins.
    utils::skip_while( is, isspace );
    result.line = is.line();
    result.column = is.column();

    // Find token type and text from the stream.
    if( !is ) {
        result.type = TokenType::kEnd;

    } else  if( *is == '(' ) {
        result.type = TokenType::kOpeningParenthesis;
        ++is;

    } else if( *is == ')' ) {
        result.type = TokenType::kClosingParenthesis;
        ++is;

    } else if( *is == ',' ) {
        result.type = TokenType::kComma;
        ++is;

    } else if( *is == ';' ) {
        result.type = TokenType::kSemicolon;
        ++is;

    } else if( *is == '=' ) {
        result.type = TokenType::kEquals;
        ++is;

    } else if( *is == '[' ) {
        result.type = TokenType::kComment;
        ++is;
        result.text = utils::read_until( is, ']' );

        if( !is ) {
            throw std::runtime_error( "Reached unexpected end of Newick tree at " + is.at() );
        }
        assert( *is == ']' );
        ++is;

    } else if( *is == ':' ) {
        result.type = TokenType::kValue;
        ++is;
        result.text = utils::parse_number_string( is );

    } else if( *is == '{' && enable_tags_ ) {
        result.type = TokenType::kTag;
        ++is;
        result.text = utils::read_until( is, '}' );

        if( !is ) {
            throw std::runtime_error( "Reached unexpected end of Newick tree at " + is.at() );
        }
        assert( *is == '}' );
        ++is;

    } else if( *is == '"' || *is == '\''  ) {
        result.type = TokenType::kString;
        result.text = utils::parse_quoted_string( is, false, true, false );

    } else if( is_valid_name_char( *is )) {
        result.type = TokenType::kString;
        result.text = utils::read_while( is, is_valid_name_char );

    } else {
        result.type = TokenType::kUnknown;
    }

    return result;
}

// =================================================================================================
//     Parse Tree into Broker
// =================================================================================================

NewickBroker NewickReader::parse_tree_to_broker_( utils::InputStream& input_stream ) const
{
    // Create result broker.
    NewickBroker broker;

    // Create a node that is currently being populated with data.
    // This is copied into the broker whenever we finish a tree node.
    NewickBrokerElement node;

    // How deep is the current token nested in the tree?
    int depth = 0;

    // Was it closed at some point? We want to avoid a tree like "()();" to be parsed!
    bool closed = false;

    // Store current token, start with an invalid value to indicate problems.
    Token ct;
    ct.type = TokenType::kEnd;

    // Store previous token.
    // In the beginning of the loop, we set pt to ct, so that in the first iteration we have
    // pt == TokenType::kEnd. This is used as indicator that we are in the first iteration.
    Token pt;

    // --------------------------------------------------------------
    //     Loop over lexer tokens and check if it...
    // --------------------------------------------------------------

    while( input_stream ) {
        // Init the previous token to what the current token (of the previous iteration) was.
        // In the first iteration, this inits to the kEnd token.
        // Then, get the next token.
        pt = ct;
        ct = get_next_token_( input_stream );

        // Treat some special error cases.
        if( ct.type == TokenType::kUnknown ) {
            throw std::runtime_error(
                "Invalid characters at " + ct.at() + ": '" + ct.text + "'."
            );
        }
        if( ct.type == TokenType::kEnd ) {
            break;
        }

        // ------------------------------------------------------
        //     is bracket '('  ==>  begin of subtree
        // ------------------------------------------------------

        if( ct.type == TokenType::kOpeningParenthesis ) {
            if( pt.type != TokenType::kEnd                && !(
                pt.type == TokenType::kOpeningParenthesis ||
                pt.type == TokenType::kComma              ||
                pt.type == TokenType::kComment
            )) {
                throw std::runtime_error(
                    "Invalid characters at " + ct.at() + ": '" + ct.text + "'."
                );
            }

            if (closed) {
                throw std::runtime_error(
                    "Tree was already closed. Cannot reopen it with '(' at " + ct.at() + "."
                );
            }

            ++depth;
            continue;
        }

        // ------------------------------------------------------
        //     Prepare for all other tokens.
        // ------------------------------------------------------

        // if we reach this, the previous condition is not fullfilled (otherwise, the
        // `continue` statement just above would
        // have been called). so we have a token other than '(', which means we should already
        // be somewhere in the tree (or a comment). check, if that is true.
        if( pt.type == TokenType::kEnd ) {

            // If it is a comment before the start of the tree, we cannot attach it to any node,
            // so just skip it and reset the current token to end, so that the next iteration
            // starts fresh.
            if( ct.type == TokenType::kComment ) {
                ct.type = TokenType::kEnd;
                continue;
            }

            throw std::runtime_error( "Tree does not start with '(' at " + ct.at() + "." );
        }

        // if we reached this point in code, this means that ct != begin, so it is not the first
        // iteration in this loop. this means that pt was already set in the loop header (at least
        // once), which means it now points to a valid token.
        assert( pt.type != TokenType::kEnd );

        // set up the node that will be filled with data now.
        // We use depth == -1 as an indicator whether it is already initialized.
        // If it is already initialized, this means we are adding more information to it, e.g.
        // a branch length or a tag. so we do not need to create it.
        // however, if this node is not initialized, this means we saw a token before that finished
        // a node and pushed it to the stack (either closing bracket or comma), so we need to init
        // it here.
        if( node.depth == -1 ) {
            node.depth = depth;
        }

        // ------------------------------------------------------
        //     is symbol or string  ==>  label
        // ------------------------------------------------------

        if( ct.type == TokenType::kString ) {
            if (!(
                pt.type == TokenType::kOpeningParenthesis ||
                pt.type == TokenType::kClosingParenthesis ||
                pt.type == TokenType::kComma              ||
                pt.type == TokenType::kComment
            )) {
                throw std::runtime_error(
                    "Invalid characters at " + ct.at() + ": '" + ct.text + "'."
                );
            }

            // populate the node
            node.name = ct.text;
            continue;
        }

        // ------------------------------------------------------
        //     is number  ==>  branch length
        // ------------------------------------------------------

        if( ct.type == TokenType::kValue ) {
            if (!(
                pt.type == TokenType::kOpeningParenthesis ||
                pt.type == TokenType::kClosingParenthesis ||
                pt.type == TokenType::kString             ||
                pt.type == TokenType::kComma              ||
                pt.type == TokenType::kComment
            )) {
                throw std::runtime_error(
                    "Invalid characters at " + ct.at() + ": '" + ct.text + "'."
                );
            }

            // populate the node
            node.values.push_back( ct.text );
            continue;
        }

        // ------------------------------------------------------
        //     is tag {}  ==>  tag
        // ------------------------------------------------------

        if( ct.type == TokenType::kTag ) {
            // in some newick extensions, a tag has a semantic meaning that belongs to the
            // current node/edge, thus we need to store it

            // populate the node
            node.tags.push_back(ct.text);
            continue;
        }

        // ------------------------------------------------------
        //     is comment []  ==>  comment
        // ------------------------------------------------------

        if( ct.type == TokenType::kComment ) {
            // in some newick extensions, a comment has a semantic meaning that belongs to the
            // current node/edge, thus we need to store it

            // populate the node
            node.comments.push_back(ct.text);
            continue;
        }

        // ------------------------------------------------------
        //     is comma ','  ==>  next subtree
        // ------------------------------------------------------

        if( ct.type == TokenType::kComma ) {
            if (!(
                pt.type == TokenType::kOpeningParenthesis ||
                pt.type == TokenType::kClosingParenthesis ||
                pt.type == TokenType::kString             ||
                pt.type == TokenType::kComma              ||
                pt.type == TokenType::kValue              ||
                pt.type == TokenType::kTag                ||
                pt.type == TokenType::kComment
            )) {
                throw std::runtime_error( "Invalid ',' at " + ct.at() + "." );
            }

            // Store and finish the current node. Then, make a new, uninitialized one.
            broker.push_top( node );
            node = NewickBrokerElement();
            continue;
        }

        // ------------------------------------------------------
        //     is bracket ')'  ==>  end of subtree
        // ------------------------------------------------------

        if( ct.type == TokenType::kClosingParenthesis ) {
            if (depth == 0) {
                throw std::runtime_error( "Too many ')' at " + ct.at() + "." );
            }
            if (!(
                pt.type == TokenType::kOpeningParenthesis ||
                pt.type == TokenType::kClosingParenthesis ||
                pt.type == TokenType::kString             ||
                pt.type == TokenType::kComma              ||
                pt.type == TokenType::kValue              ||
                pt.type == TokenType::kTag                ||
                pt.type == TokenType::kComment
            )) {
                throw std::runtime_error( "Invalid ')' at " + ct.at() + ": '" + ct.text + "'." );
            }

            // Store and finish the current node. Then, make a new, uninitialized one.
            broker.push_top( node );
            node = NewickBrokerElement();

            // decrease depth and check if this was the parenthesis that closed the tree
            --depth;
            if (depth == 0) {
                closed = true;
            }
            continue;
        }

        // ------------------------------------------------------
        //     is semicolon ';'  ==>  end of tree
        // ------------------------------------------------------

        if( ct.type == TokenType::kSemicolon ) {
            if (depth != 0) {
                throw std::runtime_error(
                    "Not enough ')' in tree before closing it with ';' at " + ct.at() + "."
                );
            }
            if (!(
                pt.type == TokenType::kClosingParenthesis ||
                pt.type == TokenType::kString             ||
                pt.type == TokenType::kValue              ||
                pt.type == TokenType::kTag                ||
                pt.type == TokenType::kComment
            )) {
                throw std::runtime_error( "Invalid ';' at " + ct.at() + ": '" + ct.text + "'." );
            }

            // Store and finish the current node. Then, make a new, uninitialized one.
            broker.push_top( node );
            node = NewickBrokerElement();
            break;
        }

        // If we reach this part of the code, all checkings for token types are done.
        // as we check for every type that NewickLexer yields, and we use a continue or break
        // in each of them, we should never reach this point, unless we forgot a type!
        assert(false);
    }

    // Tree has to finish with semicolon. Particularly ct.type == TokenType::kEnd is not allowed
    // to happen here!
    if( ct.type != TokenType::kSemicolon ) {
        throw std::runtime_error( "Tree does not finish with a semicolon." );
    }

    return broker;
}

// =================================================================================================
//     Broker to Tree
// =================================================================================================

Tree NewickReader::broker_to_tree_ (
    NewickBroker const& broker
) const {
    Tree tree;

    auto& links = tree.expose_link_container();
    auto& nodes = tree.expose_node_container();
    auto& edges = tree.expose_edge_container();

    std::vector< TreeLink* > link_stack;

    // we need the ranks (number of immediate children) of all nodes
    broker.assign_ranks();

    // Call all prepare plugins
    for( auto const& prepare_plugin : prepare_reading_plugins ) {
        prepare_plugin( broker, tree );
    }

    // iterate over all nodes of the tree broker
    for (auto b_itr = broker.cbegin(); b_itr != broker.cend(); ++b_itr) {
        NewickBrokerElement const& broker_node = *b_itr;

        // create the tree node for this broker node
        auto cur_node_u  = utils::make_unique< TreeNode >();
        auto cur_node    = cur_node_u.get();
        cur_node->reset_index( nodes.size() );

        // Create data pointer, if there is a suitable function. Otherwise, data is
        // simply a nullptr, i.e., there is no data.
        if( create_node_data_plugin ) {
            create_node_data_plugin( *cur_node );
        }

        // Call all node plugins.
        for( auto const& node_plugin : element_to_node_plugins ) {
            node_plugin( broker_node, *cur_node );
        }

        // Add the node.
        nodes.push_back(std::move(cur_node_u));

        // create the link that points towards the root.
        // this link is created for every node, root, inner and leaves.
        auto up_link_u  = utils::make_unique< TreeLink >();
        auto up_link    = up_link_u.get();
        up_link->reset_node( cur_node );
        cur_node->reset_primary_link( up_link );
        up_link->reset_index( links.size() );
        links.push_back(std::move(up_link_u));

        // establish the link towards the root
        if (link_stack.empty()) {
            // if the link stack is empty, we are currently at the very beginning of this loop,
            // which means we are at the root itself. in this case, make the "link towards the root"
            // point to itself.
            up_link->reset_outer( up_link );
        } else {
            // if we are however in some other node (leaf or inner, but not the root), we establish
            // the link "upwards" to the root, and back from there.
            up_link->reset_outer( link_stack.back() );
            link_stack.back()->reset_outer( up_link );

            // also, create an edge that connects both nodes
            auto up_edge = utils::make_unique< TreeEdge >(
                edges.size(),
                link_stack.back(),
                up_link
            );

            up_link->reset_edge( up_edge.get() );
            link_stack.back()->reset_edge( up_edge.get() );

            // Create data pointer, if there is a suitable function. Otherwise, data is
            // simply a nullptr, i.e., there is no data.
            if( create_edge_data_plugin ) {
                create_edge_data_plugin( *up_edge );
            }

            // Call all edge plugins.
            for( auto const& edge_plugin : element_to_edge_plugins ) {
                edge_plugin( broker_node, *up_edge );
            }

            // Add the edge.
            edges.push_back(std::move(up_edge));

            // we can now delete the head of the stack, because we just estiablished its "downlink"
            // and thus are done with it
            link_stack.pop_back();
        }

        // in the following, we create the links that will connect to the nodes' children.
        // for leaf nodes, this makes the next pointer point to the node itself (the loop
        // is never executed in this case, as leaves have rank 0).
        // for inner nodes, we create as many "down" links as they have children. each of them
        // is pushed to the stack, so that for the next broker nodes they are available as
        // reciever for the "up" links.
        // in summary, make all next pointers of a node point to each other in a circle.
        auto prev_link = up_link;
        for (int i = 0; i < broker_node.rank(); ++i) {
            auto down_link = utils::make_unique< TreeLink >();
            prev_link->reset_next( down_link.get() );
            prev_link = down_link.get();

            down_link->reset_node( cur_node );
            down_link->reset_index( links.size() );
            link_stack.push_back(down_link.get());
            links.push_back(std::move(down_link));
        }
        prev_link->reset_next( up_link );
    }

    // we pushed elements to the link_stack for all children of the nodes and popped them when we
    // were done processing those children, so there should be no elements left. this assumes that
    // NewickBroker.assign_ranks() does its job properly!
    assert(link_stack.empty());

    // now delete the uplink of the root, in order to make the tree fully unrooted.
    // (we do that after the tree creation, as it is way easier this way)
    assert( &links.front()->outer() == links.front().get() );
    auto next = &links.front()->next();
    while( &next->next() != links.front().get() ) {
        next = &next->next();
    }
    next->reset_next( &next->next().next() );
    assert( &next->next() == &links.front()->next() );
    links.erase(links.begin());
    for (size_t i = 0; i < links.size(); ++i) {
        links[i]->reset_index(i);
    }
    next->node().reset_primary_link( &next->next() );

    // Call all finish plugins.
    for( auto const& finish_plugin : finish_reading_plugins ) {
        finish_plugin( broker, tree );
    }

    return tree;
}

// =================================================================================================
//     Settings
// =================================================================================================

NewickReader& NewickReader::enable_tags( bool value )
{
    enable_tags_ = value;
    return *this;
}

bool NewickReader::enable_tags() const
{
    return enable_tags_;
}

NewickReader& NewickReader::stop_at_semicolon( bool value )
{
    stop_at_semicolon_ = value;
    return *this;
}

bool NewickReader::stop_at_semicolon() const
{
    return stop_at_semicolon_;
}

} // namespace tree
} // namespace genesis
