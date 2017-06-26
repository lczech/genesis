#ifndef GENESIS_UTILS_IO_INPUT_BUFFER_H_
#define GENESIS_UTILS_IO_INPUT_BUFFER_H_

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
 * @ingroup utils
 */

#include "genesis/utils/io/input_reader.hpp"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace genesis {
namespace utils {

// =================================================================================================
//     Input Buffer
// =================================================================================================

class InputBuffer
{
public:

    // -------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------

    /**
     * @brief Block length for internal buffering.
     *
     * The buffer uses three blocks of this size (4MB each).
     */
    static const size_t BlockLength = 1 << 22;

    using self_type         = InputBuffer;
    using value_type        = char;

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    InputBuffer()
        : source_name_( "invalid source" )
        , buffer_(   nullptr )
        , data_pos_( 0 )
        , data_end_( 0 )
    {}

    explicit InputBuffer( std::unique_ptr<BaseInputSource> input_source )
    {
        init_( std::move( input_source ));
    }

    ~InputBuffer()
    {
        delete[] buffer_;
        buffer_ = nullptr;
    }

    InputBuffer(self_type const&) = delete;
    InputBuffer(self_type&&)      = delete;

    self_type& operator= (self_type const&) = delete;
    self_type& operator= (self_type&&)      = delete;

    // -------------------------------------------------------------
    //     Reading
    // -------------------------------------------------------------

    operator bool() const
    {
        return data_pos_ < data_end_;
    }

    char peek( size_t ahead = 1 )
    {
        if( ahead > BlockLength ) {
            throw std::runtime_error(
                "Cannot peek ahead more than one block length of the Input Buffer."
            );
        }

        // Common use case: we can securely peek ahead.
        // Make sure we do not peek at data behind the end.
        if( data_pos_ + ahead < data_end_ ) {
            return buffer_[ data_pos_ + ahead ];
        } else {

            // Read blocks if necessary. Now we are surely in the first block.
            update_blocks_();
            assert( data_pos_ < BlockLength );

            // Try again. If we still cannot peek ahead, we are at the end of the stream.
            if( data_pos_ + ahead < data_end_ ) {
                return buffer_[ data_pos_ + ahead ];
            } else {
                return '\0';
            }
        }
    }

    size_t read( char* target, size_t size )
    {
        // Shortcut for most common use case: We are in the first block, and have enough buffer
        // to return the whole amount of requested data.
        if( data_pos_ < BlockLength && size < data_end_ - data_pos_ ) {
            std::memcpy( target, buffer_ + data_pos_, size );
            data_pos_ += size;
            return size;
        }

        // Keep track of what we still need to do and what we already did.
        size_t done_reading = 0;
        size_t yet_to_read  = size;

        // Read blocks if necessary. Now we are surely in the first block.
        update_blocks_();
        assert( data_pos_ < BlockLength );

        // Read data that is too big for one block, as long as there are more blocks.
        while( yet_to_read > BlockLength && data_end_ == 2 * BlockLength ) {

            // Read one block.
            std::memcpy( target + done_reading, buffer_ + data_pos_, BlockLength );
            data_pos_    += BlockLength;

            // Update our track keeping.
            done_reading += BlockLength;
            yet_to_read  -= BlockLength;

            // Update the blocks.
            update_blocks_();
            assert( data_pos_ < BlockLength );
            assert( data_pos_ < data_end_ );
        }

        // Get how much data is still buffered.
        size_t const buffered = data_end_ - data_pos_;

        // Don't read more than there is.
        if( yet_to_read > buffered ) {
            yet_to_read = buffered;
        }

        // Saftey. Never read more than there is.
        assert( yet_to_read <= buffered );
        assert( yet_to_read <= BlockLength );

        // Read rest.
        std::memcpy( target + done_reading, buffer_ + data_pos_, yet_to_read );
        data_pos_    += yet_to_read;

        // Update our track keeping.
        done_reading += yet_to_read;

        return done_reading;
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Refill the buffer blocks if necessary.
     */
    void update_blocks_()
    {
        // Nothing to do. We are already at the end of the input.
        if( data_pos_ == data_end_ ) {
            return;
        }
        assert( data_pos_ < data_end_ );

        // If this assertion breaks, someone tempered with our internal invariants.
        assert( data_end_ <= BlockLength * 2 );

        // If we are past the first block, we need to load more data into the blocks.
        if( data_pos_ >= BlockLength ) {

            // Move the second to the first block.
            std::memcpy( buffer_, buffer_ + BlockLength, BlockLength );
            data_pos_ -= BlockLength;
            data_end_ -= BlockLength;

            // If we are not yet at the end of the data, start the reader again:
            // Copy the third block to the second, and read into the third one.
            if( input_reader_.valid() ) {
                data_end_ += input_reader_.finish_reading();
                std::memcpy( buffer_ + BlockLength, buffer_ + 2 * BlockLength, BlockLength );
                input_reader_.start_reading( buffer_ + 2 * BlockLength, BlockLength );
            }
        }

        // After the update, the current position needs to be within the first block.
        assert( data_pos_ < BlockLength );
    }

    /**
     * @brief Init the buffers and the state of this object.
     */
    void init_( std::unique_ptr<BaseInputSource> input_source )
    {
        // Set to empty defaults if there is no input.
        if( input_source == nullptr ) {
            source_name_ = "invalid source";

            buffer_   = nullptr;
            data_pos_ = 0;
            data_end_ = 0;

            return;
        }

        // We use three buffer blocks:
        // The first two for the current blocks, and the third for the async reading.
        buffer_ = new char[ 3 * BlockLength ];

        try {
            // Set source name.
            source_name_ = input_source->source_name();

            // Read up to two blocks.
            data_pos_ = 0;
            data_end_ = input_source->read( buffer_, 2 * BlockLength );

            // If there is more data after the two blocks that we just read, start the
            // reading process (possibly async, if pthreads is available), into the third block.
            if( data_end_ == 2 * BlockLength ) {
                input_reader_.init( std::move( input_source ));
                input_reader_.start_reading( buffer_ + 2 * BlockLength, BlockLength );
            }

        } catch( ... ) {
            delete[] buffer_;
            throw;
        }
    }

    // -------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------

private:

    // Input data comes from here...
    InputReader input_reader_;
    std::string source_name_;

    // ...and is buffered here.
    char*  buffer_;

    // Current position in the buffer. It mostly is in the first block. Once we move into the
    // second block when advancing this position, the next call of update_blocks_() will move
    // it back to the first block.
    size_t data_pos_;

    // Past-the-end position in the blocks. While reading longer input, it stays at the first byte
    // of the third bufer, i.e., one past the end of the second block. Only when approaching the
    // end of the input, it moves closer to zero.
    size_t data_end_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
