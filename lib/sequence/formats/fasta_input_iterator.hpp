#ifndef GENESIS_SEQUENCE_FORMATS_FASTA_INPUT_ITERATOR_H_
#define GENESIS_SEQUENCE_FORMATS_FASTA_INPUT_ITERATOR_H_

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
 * @ingroup sequence
 */

#include "sequence/sequence.hpp"
#include "sequence/formats/fasta_reader.hpp"
#include "utils/io/input_stream.hpp"

#include <iterator>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Fasta Input Iterator
// =================================================================================================

/**
 * @brief Iterate an input stream and parse it as fasta sequences.
 *
 * This class allows to iterate over an input stream, iterpreting it as fasta sequences, and
 * yielding one such sequence per iteration step. This is useful for processing large files without
 * having to keep them fully in memory.
 *
 * Example:
 *
 *     auto it = FastaInputIterator( some_istream );
 *     while( it ) {
 *         std::cout << it->length() << "\n";
 *         ++it;
 *     }
 *
 * See FastaReader for a description of the expected format. In order to change the reading
 * behaviour, use the reader() function to access the internal FastaReaser and change its
 * properties.
 */
class FastaInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = FastaInputIterator;
    using iterator_category = std::input_iterator_tag;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    FastaInputIterator()
        : reader_()
        , input_stream_( nullptr )
        , sequence_()
    {}

    FastaInputIterator( utils::InputStream& in )
        : reader_()
        , input_stream_( &in )
        , sequence_()
    {
        // Read first sequence.
        increment();
    }

    FastaInputIterator( utils::InputStream& in, FastaReader const& reader )
        : reader_( reader )
        , input_stream_( &in )
        , sequence_()
    {
        // Read first sequence.
        increment();
    }

    ~FastaInputIterator() = default;

    FastaInputIterator( self_type const& ) = delete;
    FastaInputIterator( self_type&& )      = default;

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
    * @brief Return true iff dereferencing is valid, i.e., iff there is a Sequence available.
    */
    explicit operator bool() const
    {
        return good_;
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    Sequence const& operator * () const
    {
        return dereference();
    }

    Sequence const* operator -> () const
    {
        return &dereference();
    }

    Sequence const& dereference() const
    {
        return sequence_;
    }

    /**
     * @brief Return the FastaReader used for this iterator.
     *
     * Use this to change the reading behaviour of the iterator. See FastaReader for details.
     */
    FastaReader& reader()
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

        reader_.parse_sequence( *input_stream_, sequence_ );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    FastaReader         reader_;
    utils::InputStream* input_stream_;
    Sequence            sequence_;
    bool                good_ = true;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
