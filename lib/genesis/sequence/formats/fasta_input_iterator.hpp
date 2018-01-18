#ifndef GENESIS_SEQUENCE_FORMATS_FASTA_INPUT_ITERATOR_H_
#define GENESIS_SEQUENCE_FORMATS_FASTA_INPUT_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @brief Iterate an input stream and parse it as Fasta sequences.
 *
 * This class allows to iterate over an input stream, iterpreting it as Fasta sequences, and
 * yielding one such sequence per iteration step. This is useful for processing large files without
 * having to keep them fully in memory.
 *
 * Example:
 *
 *     auto it = FastaInputIterator();
 *     it.from_file( "/path/to/large_file.fasta" );
 *     while( it ) {
 *         std::cout << it->length() << "\n";
 *         ++it;
 *     }
 *
 * See FastaReader for a description of the expected format. In order to change the reading
 * behaviour, use the reader() function to access the internal FastaReaser and change its
 * properties.
 *
 * The copy constructur copies a pointer the the underlying stream.
 * Thus, when advancing the copy to the next Sequence, the original "skips" this Sequence,
 * as the stream then is at the next one.
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
     * @brief Create a default instance, using a default FastaReader.
     */
    FastaInputIterator()
        : reader_()
        , sequence_()
    {}

    /**
     * @brief Create an instance that copies the settings of a given FastaReader.
     */
    FastaInputIterator( FastaReader const& reader )
        : reader_( reader )
        , sequence_()
    {}

    ~FastaInputIterator() = default;

    FastaInputIterator( self_type const& ) = default;
    FastaInputIterator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    // -------------------------------------------------------------------------
    //     Reading
    // -------------------------------------------------------------------------

    /**
     * @brief Start reading from an input stream.
     *
     * This reads the first Sequence so that it is available for dereferencing.
     */
    self_type& from_stream( std::istream& input_stream )
    {
        // Create input stream and read first sequence.
        input_stream_ = std::make_shared<utils::InputStream>(
            utils::make_unique< utils::StreamInputSource >( input_stream )
        );
        increment();
        return *this;
    }

    /**
     * @brief Start reading from an input string.
     *
     * @copydetails from_stream()
     */
    self_type& from_file( std::string const& file_name )
    {
        // Create input stream and read first sequence.
        input_stream_ = std::make_shared<utils::InputStream>(
            utils::make_unique< utils::FileInputSource >( file_name )
        );
        increment();
        return *this;
    }

    /**
     * @brief Start reading from an input file.
     *
     * @copydetails from_stream()
     */
    self_type& from_string( std::string const& input_string )
    {
        // Create input stream and read first sequence.
        input_stream_ = std::make_shared<utils::InputStream>(
            utils::make_unique< utils::StringInputSource >( input_string )
        );
        increment();
        return *this;
    }

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

    std::shared_ptr<utils::InputStream> input_stream_;
    bool good_ = true;

    FastaReader reader_;
    Sequence    sequence_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
