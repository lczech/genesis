#ifndef GENESIS_TREE_FORMATS_NEWICK_INPUT_ITERATOR_H_
#define GENESIS_TREE_FORMATS_NEWICK_INPUT_ITERATOR_H_

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

#include "genesis/tree/tree.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/utils/io/input_source.hpp"
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
 *     for( auto const& t : NewickInputIterator( from_file( "/path/to/trees.newick" ))) {
 *         std::cout << t.node_count() << "\n";
 *     }
 *
 * Alternatively, the following also works:
 *
 *     auto it = NewickInputIterator( from_file( "/path/to/trees.newick" ));
 *     while( it ) {
 *         Tree const& t = *it;
 *         std::cout << t.node_count() << "\n";
 *         ++it;
 *     }
 *
 * Use functions such as utils::from_file() and utils::from_string() to conveniently
 * get an input source that can be used here.
 *
 * See NewickReader for a description of the expected format. In order to change the reading
 * behaviour, a NewickReader object can be handed over from which the settings are copied.
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

    /**
     * @brief Create a default instance, with no input.
     */
    NewickInputIterator()
        : input_stream_( nullptr )
        , reader_()
        , tree_()
    {}

    /**
     * @brief Create an instance that reads from an input source, using a default NewickReader.
     */
    explicit NewickInputIterator( std::shared_ptr<utils::BaseInputSource> source )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , reader_()
        , tree_()
    {
        // Setting so that we only read one tree at a time.
        reader_.stop_after_semicolon( true );

        // Read first tree.
        increment();
    }

    /**
     * @brief Create an instance that reads from an input source,
     * using the settings of a given NewickReader.
     */
    NewickInputIterator( std::shared_ptr<utils::BaseInputSource> source, NewickReader const& settings )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , reader_( settings )
        , tree_()
    {
        // Setting so that we only read one tree at a time.
        reader_.stop_after_semicolon( true );

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

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    /**
     * @brief Beginning of the iterator.
     *
     * This is a bit uncommon, as the iterator provides its own begin() and end() functions.
     * We do this to allow the easy use case that is explained above.
     */
    self_type& begin()
    {
        return *this;
    }

    /**
     * @brief End of the iterator.
     *
     * @copydoc begin()
     */
    self_type end()
    {
        return self_type();
    }

    self_type& operator ++ ()
    {
        increment();
        return *this;
    }

    void increment()
    {
        // Check whether the input stream is good (not end-of-stream) and can be read from.
        // If not, we reached its end, so we stop reading in the next iteration.
        if( ! input_stream_ || ! *input_stream_ ) {
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

    std::shared_ptr<utils::InputStream> input_stream_;

    bool                good_ = true;
    NewickReader        reader_;
    Tree                tree_;
};

} // namespace tree
} // namespace genesis

#endif // include guard
