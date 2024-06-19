#ifndef GENESIS_SEQUENCE_FORMATS_FASTX_INPUT_STREAM_H_
#define GENESIS_SEQUENCE_FORMATS_FASTX_INPUT_STREAM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "genesis/sequence/formats/fasta_reader.hpp"
#include "genesis/sequence/formats/fastq_reader.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>
#include <sstream>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward declarations and typedefs
// =================================================================================================

template<class Reader>
class FastxInputStream;

using FastaInputStream = FastxInputStream<FastaReader>;
using FastqInputStream = FastxInputStream<FastqReader>;

// =================================================================================================
//     Fasta and Fastq Input Stream
// =================================================================================================

/**
 * @brief Stream through an input source and parse it as Fasta or Fastq sequences.
 *
 * This class allows to iterate over an input source, interpreting it as Fasta or Fastq sequences,
 * and yielding one such sequence per iteration step. This is useful for processing large files
 * without having to keep them fully in memory.
 *
 * Example:
 *
 *     for( auto const& s : FastaInputStream( from_file( "/path/to/large_file.fastq" ))) {
 *         std::cout << s.length() << "\n";
 *     }
 *
 * Use functions such as utils::from_file() and utils::from_string() to conveniently
 * get an input source that can be used here.
 *
 * See FastaReader and FastqReader for a description of the expected formats. In order to change
 * the reading behaviour, a reader object can be handed over from which the settings are copied.
 * The reader instance can also be modified before starting the iteration, if needed.
 *
 * Thread safety: No thread safety. The common use case for this iterator is to loop over a file.
 * Thus, guarding induces unnecessary overhead. If multiple threads read from this iterator, both
 * dereferencing and incrementing need to be guarded.
 */
template<class Reader>
class FastxInputStream
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = FastxInputStream;
    using value_type        = Sequence;
    using pointer           = value_type*;
    using reference         = value_type&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

public:

    /**
     * @brief Internal iterator over the sequences.
     *
     * This is the class that does the actual work. Use the dereference `operator*()`
     * and `operator->()` to get the current Sequence of the iteration.
     */
    class Iterator
    {
        // -------------------------------------------------------------------------
        //     Typedefs and Enums
        // -------------------------------------------------------------------------

    public:

        using self_type         = FastxInputStream::Iterator;
        using value_type        = Sequence;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

    private:

        Iterator() = default;

        Iterator( FastxInputStream const* parent )
            : parent_( parent )
        {
            // Safeguard
            if( ! parent_ ) {
                return;
            }

            // Start reading from the input source into a stream.
            input_stream_ = std::make_shared<utils::InputStream>( parent_->input_source_ );

            // Start streaming the data
            increment_();
        }

    public:

        ~Iterator() = default;

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        friend FastxInputStream;

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        value_type const* operator->() const
        {
            return &sequence_;
        }

        value_type* operator->()
        {
            return &sequence_;
        }

        value_type const& operator*() const
        {
            return sequence_;
        }

        value_type& operator*()
        {
            return sequence_;
        }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++ ()
        {
            increment_();
            return *this;
        }

        // self_type operator ++(int)
        // {
        //     auto cpy = *this;
        //     increment_();
        //     return cpy;
        // }

        /**
         * @brief Compare two iterators for equality.
         *
         * Any two iterators that are created by calling begin() on the same
         * FastxInputStream instance will compare equal, as long as neither of them is
         * past-the-end. A valid (not past-the-end) iterator and an end() iterator will not compare
         * equal; all past-the-end iterators compare equal, independently from which parent they
         * were created.
         */
        bool operator==( self_type const& it ) const
        {
            return parent_ == it.parent_;
        }

        bool operator!=( self_type const& it ) const
        {
            return !(*this == it);
        }

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

    private:

        // ---------------------------------------------
        //     Increment and Processing Samples
        // ---------------------------------------------

        void increment_()
        {
            assert( parent_ );

            // Check whether the input stream is good (not end-of-stream) and can be read from.
            // If not, we reached its end, so we stop reading in the next iteration.
            if( ! input_stream_ || ! *input_stream_ ) {
                parent_ = nullptr;
                input_stream_ = nullptr;
                sequence_ = Sequence();
                return;
            }

            parent_->reader_.parse_sequence( *input_stream_, sequence_ );
        }

        // -------------------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------------------

    private:

        // Parent. If null, this indicates the end of the input and that we are done iterating.
        FastxInputStream const* parent_ = nullptr;

        // Data stream to read from.
        std::shared_ptr<utils::InputStream> input_stream_;

        // The sequence that we parse the input into and expose to the user.
        Sequence sequence_;
    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default instance, with no input.
     */
    FastxInputStream()
        : input_source_( nullptr )
        , reader_()
    {}

    /**
     * @brief Create an instance that reads from an input source, using a default FastaReader or
     * FastqReader.
     */
    explicit FastxInputStream(
        std::shared_ptr<utils::BaseInputSource> source
    )
        : input_source_( source )
        , reader_()
    {}

    /**
     * @brief Create an instance that reads from an input source,
     * using the settings of a given FastaReader oder FastqReader.
     */
    FastxInputStream(
        std::shared_ptr<utils::BaseInputSource> source,
        Reader const& reader
    )
        : input_source_( source )
        , reader_( reader )
    {}

    ~FastxInputStream() = default;

    FastxInputStream( self_type const& ) = default;
    FastxInputStream( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    Iterator begin() const
    {
        return Iterator( this );
    }

    Iterator end() const
    {
        return Iterator();
    }

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    std::shared_ptr<utils::BaseInputSource> input_source() const
    {
        return input_source_;
    }

    Reader& reader()
    {
        return reader_;
    }

    Reader const& reader() const
    {
        return reader_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::shared_ptr<utils::BaseInputSource> input_source_;
    Reader reader_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
