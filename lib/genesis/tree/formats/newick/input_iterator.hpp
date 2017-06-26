#ifndef GENESIS_TREE_FORMATS_NEWICK_INPUT_ITERATOR_H_
#define GENESIS_TREE_FORMATS_NEWICK_INPUT_ITERATOR_H_

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

#include "genesis/tree/tree.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <iterator>

namespace genesis {
namespace tree {

// =================================================================================================
//     Newick Input Iterator
// =================================================================================================

/**
 * @brief Iterate an input stream and parse it as Newick trees.
 *
 * This class allows to iterate over an input stream, iterpreting it as Newick trees, and
 * yielding one such tree per iteration step. This is useful for processing large files without
 * having to keep them fully in memory.
 *
 * Example:
 *
 *     auto it = NewickInputIterator( some_istream );
 *     while( it ) {
 *         Tree const& t = *it;
 *         std::cout << t.node_count() << "\n";
 *         ++it;
 *     }
 *
 * See NewickReader for a description of the expected format. In order to change the reading
 * behaviour, use the reader() function to access the internal NewickReader and change its
 * properties.
 */
class NewickInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = NewickInputIterator;
    using iterator_category = std::input_iterator_tag;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    NewickInputIterator()
        : reader_()
        , input_stream_( nullptr )
        , tree_()
    {}

    NewickInputIterator( utils::InputStream& in )
        : reader_()
        , input_stream_( &in )
        , tree_()
    {
        // Setting so that we only read one tree at a time.
        reader_.stop_at_semicolon( true );

        // Read first tree.
        increment();
    }

    NewickInputIterator( utils::InputStream& in, NewickReader const& reader )
        : reader_( reader )
        , input_stream_( &in )
        , tree_()
    {
        // Setting so that we only read one tree at a time.
        reader_.stop_at_semicolon( true );

        // Read first tree.
        increment();
    }

    ~NewickInputIterator() = default;

    NewickInputIterator( self_type const& ) = delete;
    NewickInputIterator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = delete;
    self_type& operator= ( self_type&& )      = default;

    // -------------------------------------------------------------------------
    //     Comparators
    // -------------------------------------------------------------------------

    bool operator == ( self_type const& other ) const
    {
        return input_stream_ == other.input_stream_;
    }

    bool operator != ( self_type const& other ) const
    {
        return !( *this == other );
    }

    /**
    * @brief Return true iff dereferencing is valid, i.e., iff there is a Tree available.
    */
    explicit operator bool() const
    {
        return good_;
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    Tree const& operator * () const
    {
        return dereference();
    }

    Tree const* operator -> () const
    {
        return &dereference();
    }

    Tree const& dereference() const
    {
        return tree_;
    }

    /**
     * @brief Return the NewickReader used for this iterator.
     *
     * Use this to change the reading behaviour of the iterator. See NewickReader for details.
     */
    NewickReader& reader()
    {
        return reader_;
    }

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    self_type& operator ++ ()
    {
        increment();
        return *this;
    }

    void increment()
    {
        // Check whether the input stream is good (not end-of-stream) and can be read from.
        // If not, we reached its end, so we stop reading in the next iteration.
        if( input_stream_ == nullptr || ! *input_stream_ ) {
            good_ = false;
            return;
        }

        // Read the next tree.
        tree_ = reader_.parse_single_tree( *input_stream_ );

        // Check whether we actually got a tree. We use empty as marker for this,
        // which is valid, as we can never read an actual empty tree from any input
        // (it always consists of at least the root node).
        if( tree_.empty() ) {
            good_ = false;
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    NewickReader        reader_;
    utils::InputStream* input_stream_;
    Tree                tree_;
    bool                good_ = true;
};

} // namespace tree
} // namespace genesis

#endif // include guard
