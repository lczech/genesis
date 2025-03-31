#ifndef GENESIS_SEQUENCE_FORMATS_FAI_INPUT_STREAM_H_
#define GENESIS_SEQUENCE_FORMATS_FAI_INPUT_STREAM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Fai Input Stream
// =================================================================================================

/**
 * @brief Stream through an input source and parse it as fai records, i.e., fasta indices.
 *
 * This class allows to iterate over an input fai file (or other source), and yielding one fai
 * record per iteration step. This is useful for processing large files without having to keep
 * them fully in memory.
 *
 * Example:
 *
 *     for( auto const& s : FaiInputStream( from_file( "/path/to/large_file.fasta.fai" ))) {
 *         std::cout << s.length << "\n";
 *     }
 *
 * Use functions such as utils::from_file() and utils::from_string() to conveniently
 * get an input source that can be used here.
 *
 * Thread safety: No thread safety. The common use case for this iterator is to loop over a file.
 * Thus, guarding induces unnecessary overhead. If multiple threads read from this iterator, both
 * dereferencing and incrementing need to be guarded.
 *
 * We internally use std::string_view if possible, and hence once the iterator is incremented,
 * all records of previous iterations are considered invalidated.
 */
class FaiInputStream
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    /**
     * @brief One record in a fai index.
     *
     * See https://www.htslib.org/doc/faidx.html
     */
    struct Record
    {
        /**
         * @brief Line number in the fai file itself
         */
        size_t line;

        /**
         * @brief Name of this reference sequence
         */
        #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
            std::string_view name;
        #else
            std::string name;
        #endif

        /**
         * @brief Total length of this reference sequence, in bases
         */
        size_t length;

        /**
         * @brief Offset in the FASTA/FASTQ file of this sequence's first base
         */
        size_t offset;

        /**
         * @brief The number of bases on each line
         */
        size_t linebases;

        /**
         * @brief The number of bytes in each line, including the newline
         */
        size_t linewidth;

        /**
         * @brief Offset of sequence's first quality within the FASTQ file
         */
        size_t qualoffset;
    };

    using self_type         = FaiInputStream;
    using value_type        = Record;
    using pointer           = value_type*;
    using reference         = value_type&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

public:

    /**
     * @brief Internal iterator over the fai records.
     *
     * This is the class that does the actual work. Use the dereference `operator*()`
     * and `operator->()` to get the current Record of the iteration.
     */
    class Iterator
    {
        // -------------------------------------------------------------------------
        //     Typedefs and Enums
        // -------------------------------------------------------------------------

    public:

        using self_type         = FaiInputStream::Iterator;
        using value_type        = Record;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

    private:

        Iterator() = default;

        Iterator( FaiInputStream const* parent )
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

        friend FaiInputStream;

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        value_type const* operator->() const
        {
            return &record_;
        }

        value_type* operator->()
        {
            return &record_;
        }

        value_type const& operator*() const
        {
            return record_;
        }

        value_type& operator*()
        {
            return record_;
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
         * FaiInputStream instance will compare equal, as long as neither of them is
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
                record_ = Record();
                return;
            }

            // Get the next line (moves the inut stream), and split the line in the record.
            // Depending on the standard, we can use a view, which is cheaper.
            #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
                auto const line = input_stream_->get_line_view();
                utils::split_view( line, col_buffer_, '\t', false );
            #else
                auto const line = input_stream_->get_line();
                utils::split( line, col_buffer_, '\t', false );
            #endif
            ++line_cnt_;

            // Input error checks.
            if( col_buffer_.size() != 5 && col_buffer_.size() != 6 ) {
                throw std::runtime_error(
                    "Invalid sequence fai file: Line " + std::to_string( line_cnt_ ) +
                    " has " + std::to_string( col_buffer_.size() ) +
                    " columns instead of the expected 5 or 6 columns."
                );
            }

            // Now we know that we have a valid line.
            // Set up the record with its values
            assert( col_buffer_.size() == 5 || col_buffer_.size() == 6 );
            try {
                #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
                    record_.line   = line_cnt_;
                    record_.name   = col_buffer_[0];
                    record_.length = utils::convert_from_chars<size_t>( col_buffer_[1] );
                    if( ! parent_->only_name_and_length_ ) {
                        record_.offset     = utils::convert_from_chars<size_t>( col_buffer_[2] );
                        record_.linebases  = utils::convert_from_chars<size_t>( col_buffer_[3] );
                        record_.linewidth  = utils::convert_from_chars<size_t>( col_buffer_[4] );
                        if( col_buffer_.size() == 6 ) {
                            record_.qualoffset = utils::convert_from_chars<size_t>( col_buffer_[5] );
                        }
                    }
                #else
                    record_.line   = line_cnt_;
                    record_.name   = col_buffer_[0];
                    record_.length = std::stoull( col_buffer_[1] );
                    if( ! parent_->only_name_and_length_ ) {
                        record_.offset     = std::stoull( col_buffer_[2] );
                        record_.linebases  = std::stoull( col_buffer_[3] );
                        record_.linewidth  = std::stoull( col_buffer_[4] );
                        if( col_buffer_.size() == 6 ) {
                            record_.qualoffset = std::stoull( col_buffer_[5] );
                        }
                    }
                #endif
            } catch(...) {
                throw std::runtime_error(
                    "Invalid sequence fai file: Line " + std::to_string( line_cnt_ ) +
                    " contains a record with non-numerical entries."
                );
            }

            // We are a bit pedantic here, and throw in case of empty results.
            // Technically, those could actually occur in the data, but that would be some weird edge
            // case anyway, that would lead to other problems downstream.
            if( record_.name.empty() || record_.length == 0 ) {
                throw std::runtime_error(
                    "Invalid sequence fai file: Line " + std::to_string( line_cnt_ ) +
                    " contains a record with invalid empty NAME or zero LENGTH fields."
                );
            }
        }

        // -------------------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------------------

    private:

        // Parent. If null, this indicates the end of the input and that we are done iterating.
        FaiInputStream const* parent_ = nullptr;

        // Data stream to read from, and number of lines read.
        std::shared_ptr<utils::InputStream> input_stream_;
        size_t line_cnt_ = 0;

        // The record that we parse the input into and expose to the user.
        Record record_;

        // We want to avoid re-allocation of the splitting per line.
        // Keep a buffer into which we can split the elements of each line.
        #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
            std::vector<std::string_view> col_buffer_;
        #else
            std::vector<std::string> col_buffer_;
        #endif
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
    FaiInputStream()
        : input_source_( nullptr )
    {}

    /**
     * @brief Create an instance that reads from an input source, using a default FastaReader or
     * FastqReader.
     */
    explicit FaiInputStream(
        std::shared_ptr<utils::BaseInputSource> source
    )
        : input_source_( source )
    {}

    ~FaiInputStream() = default;

    FaiInputStream( self_type const& ) = default;
    FaiInputStream( self_type&& )      = default;

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

    /**
     * @brief Set if only to parse the `NAME` and `LENGTH` fields of each fai record.
     *
     * In some applications, it might not be necessary to parse all elements of a fai record.
     * If only the first two are needed, this setting can save a bit of processing time.
     */
    void only_name_and_length( bool value )
    {
        only_name_and_length_ = value;
    }

    bool only_name_and_length() const
    {
        return only_name_and_length_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::shared_ptr<utils::BaseInputSource> input_source_;
    bool only_name_and_length_ = false;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
