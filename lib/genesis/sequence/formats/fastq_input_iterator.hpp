#ifndef GENESIS_SEQUENCE_FORMATS_FASTQ_INPUT_ITERATOR_H_
#define GENESIS_SEQUENCE_FORMATS_FASTQ_INPUT_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

#include "genesis/sequence/formats/fastq_reader.hpp"
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
//     Fastq Input Iterator
// =================================================================================================

/**
 * @brief Iterate an input source and parse it as Fastq sequences.
 *
 * This class allows to iterate over an input source, interpreting it as Fastq sequences, and
 * yielding one such sequence per iteration step. This is useful for processing large files without
 * having to keep them fully in memory.
 *
 * Example:
 *
 *     for( auto const& s : FastqInputIterator( from_file( "/path/to/large_file.fastq" ))) {
 *         std::cout << s.length() << "\n";
 *     }
 *
 * Alternatively, the following also works:
 *
 *     auto it = FastqInputIterator( from_file( "/path/to/large_file.fastq" ));
 *     while( it ) {
 *         std::cout << it->length() << "\n";
 *         ++it;
 *     }
 *
 * Use functions such as utils::from_file() and utils::from_string() to conveniently
 * get an input source that can be used here.
 *
 * See FastqReader for a description of the expected format. In order to change the reading
 * behaviour, a FastqReader object can be handed over from which the settings are copied.
 *
 * The copy constructur copies a pointer to the underlying source.
 * Thus, when advancing the copy to the next Sequence, the original "skips" this Sequence,
 * as the source then is at the next one.
 *
 * Thread safety: No thread safety. The common use case for this iterator is to loop over a file.
 * Thus, guarding induces unnecessary overhead. If multiple threads read from this iterator, both
 * dereferencing and incrementing need to be guarded.
 */
class FastqInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = FastqInputIterator;
    using value_type        = Sequence;
    using pointer           = value_type*;
    using reference         = value_type&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default instance, with no input.
     */
    FastqInputIterator()
        : input_stream_( nullptr )
        , sequence_()
        , reader_()
    {}

    /**
     * @brief Create an instance that reads from an input source, using a default FastqReader.
     */
    explicit FastqInputIterator( std::shared_ptr<utils::BaseInputSource> source )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , sequence_()
        , reader_()
    {
        increment();
    }

    /**
     * @brief Create an instance that reads from an input source,
     * using the settings of a given FastqReader.
     */
    FastqInputIterator( std::shared_ptr<utils::BaseInputSource> source, FastqReader const& settings )
        : input_stream_( std::make_shared<utils::InputStream>( source ))
        , sequence_()
        , reader_( settings )
    {
        increment();
    }

    ~FastqInputIterator() = default;

    FastqInputIterator( self_type const& ) = default;
    FastqInputIterator( self_type&& )      = default;

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
        return FastqInputIterator();
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
    FastqReader reader_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
