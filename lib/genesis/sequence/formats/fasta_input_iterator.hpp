#ifndef GENESIS_SEQUENCE_FORMATS_FASTA_INPUT_ITERATOR_H_
#define GENESIS_SEQUENCE_FORMATS_FASTA_INPUT_ITERATOR_H_

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
 * @ingroup sequence
 */

#include "genesis/sequence/formats/fasta_reader.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <cstddef>
#include <iterator>
#include <memory>
#include <sstream>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Fasta Input Iterator
// =================================================================================================

/**
 * @brief Iterate an input source and parse it as Fasta sequences.
 *
 * This class allows to iterate over an input source, interpreting it as Fasta sequences, and
 * yielding one such sequence per iteration step. This is useful for processing large files without
 * having to keep them fully in memory.
 *
 * Example:
 *
 *     for( auto const& s : FastaInputIterator( from_file( "/path/to/large_file.fasta" ))) {
 *         std::cout << s.length() << "\n";
 *     }
 *
 * Alternatively, the following also works:
 *
 *     auto it = FastaInputIterator( from_file( "/path/to/large_file.fasta" ) );
 *     while( it ) {
 *         std::cout << it->length() << "\n";
 *         ++it;
 *     }
 *
 * Use functions such as utils::from_file() and utils::from_string() to conveniently
 * get an input source that can be used here.
 *
 * See FastaReader for a description of the expected format. In order to change the reading
 * behaviour, a FastaReader object can be handed over from which the settings are copied.
 *
 * The copy constructur copies a pointer to the underlying source.
 * Thus, when advancing the copy to the next Sequence, the original "skips" this Sequence,
 * as the source then is at the next one.
 *
 * Thread safety: No thread safety. The common use case for this iterator is to loop over a file.
 * Thus, guarding induces unnecessary overhead. If multiple threads read from this iterator, both
 * dereferencing and incrementing need to be guarded.
 */
class FastaInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = FastaInputIterator;
    using value_type        = Sequence;
    using pointer           = value_type&;
    using reference         = value_type*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default instance, with no input.
     */
    FastaInputIterator()
        : input_stream_( nullptr )
        , sequence_()
        , reader_()
    {}

    /**
     * @brief Create an instance that reads from an input source, using a default FastaReader.
     */
    explicit FastaInputIterator( std::shared_ptr<utils::BaseInputSource> source )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , sequence_()
        , reader_()
    {
        increment();
    }

    /**
     * @brief Create an instance that reads from an input source,
     * using the settings of a given FastaReader.
     */
    FastaInputIterator( std::shared_ptr<utils::BaseInputSource> source, FastaReader const& settings )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , sequence_()
        , reader_( settings )
    {
        increment();
    }

    ~FastaInputIterator() = default;

    FastaInputIterator( self_type const& ) = default;
    FastaInputIterator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
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

    value_type const& operator * () const
    {
        return sequence_;
    }

    value_type const* operator -> () const
    {
        return &sequence_;
    }

    value_type const& dereference() const
    {
        return sequence_;
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

    self_type operator ++(int)
    {
        auto r = *this;
        increment();
        return r;
    }

    void increment()
    {
        // Check whether the input stream is good (not end-of-stream) and can be read from.
        // If not, we reached its end, so we stop reading in the next iteration.
        if( ! input_stream_ || ! *input_stream_ ) {
            good_ = false;
            input_stream_ = nullptr;
            sequence_ = Sequence();
            return;
        }

        reader_.parse_sequence( *input_stream_, sequence_ );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    bool good_ = true;
    std::shared_ptr<utils::InputStream> input_stream_;

    Sequence    sequence_;
    FastaReader reader_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
