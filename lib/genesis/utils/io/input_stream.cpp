/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/io/input_stream.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>

#if defined(GENESIS_AVX) || defined(GENESIS_AVX2) || defined(GENESIS_AVX512)

    #include <immintrin.h>

#endif

namespace genesis {
namespace utils {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

InputStream& InputStream::operator= ( InputStream&& other )
{
    if( this == &other ) {
        return *this;
    }

    input_reader_ = std::move( other.input_reader_ );
    source_name_  = std::move( other.source_name_ );

    // Need to free our current buffer.
    if( buffer_ ) {
        delete[] buffer_;
    }

    // Move the data.
    buffer_   = other.buffer_;
    data_pos_ = other.data_pos_;
    data_end_ = other.data_end_;
    current_  = other.current_;
    line_     = other.line_;
    column_   = other.column_;

    // Set the other in a valid but empty state and avoid double freeing of the buffer.
    other.buffer_   = nullptr;
    other.data_pos_ = 0;
    other.data_end_ = 0;
    other.current_  = '\0';
    other.line_     = 0;
    other.column_   = 0;

    return *this;
}

// =================================================================================================
//     Char Operations
// =================================================================================================

char InputStream::read_char_or_throw( char const criterion )
{
    // Check char and move to next.
    if( data_pos_ >= data_end_ || current_ != criterion ) GENESIS_UNLIKELY {
        throw std::runtime_error(
            std::string("In ") + source_name() + ": " +
            "Expecting " + char_to_hex( criterion ) + " at " + at() + ", " +
            "but received " + char_to_hex( current_ ) + " instead."
        );
    }
    assert( good() && current_ == criterion );
    operator++();
    return criterion;
}

char InputStream::read_char_or_throw( std::function<bool (char)> criterion )
{
    // Check char and move to next.
    if( data_pos_ >= data_end_ || !criterion( current_ )) GENESIS_UNLIKELY {
        throw std::runtime_error(
            std::string("In ") + source_name() + ": " +
            "Unexpected char " + char_to_hex( current_ ) + " at " + at() + "."
        );
    }
    assert( good() );
    auto const chr = current_;
    operator++();
    return chr;
}

// =================================================================================================
//     Line Operations
// =================================================================================================

// -------------------------------------------------------------------------
//     get_line
// -------------------------------------------------------------------------

void InputStream::get_line( std::string& target )
{
    // Check edge case.
    if( data_pos_ >= data_end_ ) {
        return;
    }

    // Loop until we find the end of the line. As this can be longer than one block,
    // we might need to update the blocks and store the results in between.
    while( true ) {
        // Read data if necessary. After this, we are guaranteed to have data_pos_ in the first block.
        if( data_pos_ >= BlockLength ) GENESIS_UNLIKELY {
            update_blocks_();
        }
        assert( data_pos_ < BlockLength );

        // Store the starting position, so that we can copy from there once we found the end.
        size_t const start_pos = data_pos_;

        // Move data_pos_ to the end of the line or end of the buffered data.
        move_to_line_or_buffer_end_();
        assert(
            data_pos_ == data_end_ ||
            data_pos_ == start_pos + BlockLength ||
            buffer_[ data_pos_ ] == '\n' ||
            buffer_[ data_pos_ ] == '\r'
        );

        // Store what we have so far.
        assert( data_pos_ >= start_pos );
        target.append( buffer_ + start_pos, data_pos_ - start_pos );

        // If the line is not yet finished, we need an extra round. Start the loop again.
        if( data_pos_ - start_pos >= BlockLength ) {
            continue;
        }

        // In all other cases, we stop here.
        break;
    }

    // Some safty.
    assert( data_pos_ <= data_end_ );
    assert( data_pos_ < 2 * BlockLength );

    // Check all cases that can occur.
    if( data_pos_ == data_end_ ) {

        // Files might be missing the line break at the end of the last line.
        // We catch this case here, so that we can be sure that the next conditions
        // are actually valid when accessing the buffer.
        // But we don't need to do anything in this case.

    } else if( buffer_[ data_pos_ ] == '\n' ) {
        ++data_pos_;

    } else if( buffer_[ data_pos_ ] == '\r' ) {
        ++data_pos_;

        // Treat stupid Windows \r\n lines breaks.
        // We already moved past the \r, so check the next char.
        if( data_pos_ < data_end_ && buffer_[ data_pos_ ] == '\n' ) {
            ++data_pos_;
        }
    } else {
        // We have checked all cases where the loop above can terminate.
        // So this should not happen.
        assert( false );
    }

    // Set char and counters. It checks for end of the file,
    // so this is safe if we are past the end already.
    set_current_char_();
    ++line_;
    column_ = 1;
}

// -------------------------------------------------------------------------
//     move_to_line_or_buffer_end_
// -------------------------------------------------------------------------

bool InputStream::move_to_line_or_buffer_end_()
{
    // The caller needs to guarantee that data_pos_ in the first block by calling update_blocks_()
    assert( data_pos_ < BlockLength );

    // We need to stop before the end of the data, and before the end of the second block.
    // As a safeguard, we are not reading more than one block length away from the current pos.
    auto const stop_pos = std::min( data_end_, data_pos_ + BlockLength );

    // Pick the fastest implementation available to move data_pos_ as close as possible
    // to the next nl or cr character, without overshooting the end of the data or block.
    #if defined(GENESIS_AVX512)
        approach_line_or_buffer_end_avx512_( stop_pos );
    #elif defined(GENESIS_AVX2)
        approach_line_or_buffer_end_avx2_( stop_pos );
    #else
        approach_line_or_buffer_end_naive_( stop_pos );
    #endif

    // The above approach loops might end with data_pos_ somewhere before the exact line break.
    // In those cases, we now need to walk the rest by foot, and examine char by char.
    while(
        data_pos_ < stop_pos &&
        buffer_[ data_pos_ ] != '\n' &&
        buffer_[ data_pos_ ] != '\r'
    ) {
        ++data_pos_;
    }

    // Now we are either at the new line character, or at the end of the current data.
    // We return if we found a new line char as our result here, i.e., we were successful.
    assert( data_pos_ == stop_pos || buffer_[ data_pos_ ] == '\n' || buffer_[ data_pos_ ] == '\r' );
    return data_pos_ < stop_pos;
}

// -------------------------------------------------------------------------
//     approach_line_or_buffer_end_avx512_
// -------------------------------------------------------------------------

void InputStream::approach_line_or_buffer_end_avx512_( size_t const stop_pos )
{
    // Avoid compiler complaints when below code is not processed.
    (void) stop_pos;

    #if defined(GENESIS_AVX512)

        // UNTESTED and hence unused - we never set the GENESIS_AVX512 definition

        static auto const all_nl = _mm512_set1_epi8('\n');
        static auto const all_cr = _mm512_set1_epi8('\r');

        // Process 64 bytes at a time using AVX-512
        while( data_pos_ + 64 <= stop_pos ) {
            auto data_64bytes = _mm512_loadu_si512(
                reinterpret_cast<__m512i const*>( buffer_ + data_pos_ )
            );

            // Compare each byte in the chunk with '\n' and '\r'
            auto nl_pos = _mm512_cmpeq_epi8_mask(data_64bytes, all_nl);
            auto cr_pos = _mm512_cmpeq_epi8_mask(data_64bytes, all_cr);

            // Combine the results of the comparisons.
            // If this is non-zero, we have found a nl or cr character.
            auto nr_pos = nl_pos | cr_pos;

            // Check if any of the comparisons were true.
            if( nr_pos != 0 ) {
                // Find the position of the first set bit, using intrinsics.
                int offset = _tzcnt_u64(nr_pos);
                data_pos_ += offset;
                break;
            }

            data_pos_ += 64;
        }

    #endif
}

// -------------------------------------------------------------------------
//     approach_line_or_buffer_end_avx2_
// -------------------------------------------------------------------------

void InputStream::approach_line_or_buffer_end_avx2_( size_t const stop_pos )
{
    // Avoid compiler complaints when below code is not processed.
    (void) stop_pos;

    #if defined(GENESIS_AVX2)

        // 32 byte masks where each byte is new line or carriage return.
        static auto const all_nl = _mm256_set1_epi8( '\n' );
        static auto const all_cr = _mm256_set1_epi8( '\r' );

        // Load chunks of 32 bytes and loop until one of them contains nl or cr,
        // or we reach the end of what we can currently process.
        int mask = 0;
        bool aligned = reinterpret_cast<uintptr_t>( buffer_ + data_pos_ ) % 32 == 0;
        while( data_pos_ + 32 <= stop_pos ) {

            // Load 32 bytes of data. We first do an unaligned load for the first iteration,
            // and then move forward to the next alignment boundary, so that subsequent
            // iterations can use aligned load. On average this will double check 16 bytes,
            // which might be slower when the data consists of many very short lines.
            // But typically, that is not the case, and then this gives significant speedup.
            __m256i data_chunk;
            if( aligned ) {
                assert( reinterpret_cast<uintptr_t>( buffer_ + data_pos_ ) % 32 == 0 );
                data_chunk = _mm256_load_si256(
                    reinterpret_cast<__m256i const*>( buffer_ + data_pos_ )
                );
            } else {
                data_chunk = _mm256_loadu_si256(
                    reinterpret_cast<__m256i const*>( buffer_ + data_pos_ )
                );
            }

            // Compare the data with the masks, setting bits where they match,
            // and combining them into one mask that we then evaluate.
            auto const nl_pos = _mm256_cmpeq_epi8( data_chunk, all_nl );
            auto const cr_pos = _mm256_cmpeq_epi8( data_chunk, all_cr );
            auto const nr_pos = _mm256_or_si256( nl_pos, cr_pos );

            // Get a bit mask that is set wherever nl or cr are.
            // If there is a bit set, we are done with the loop.
            mask = _mm256_movemask_epi8( nr_pos );
            if( mask != 0 ) {
                break;
            }
            if( aligned ) {
                data_pos_ += 32;
            } else {
                auto const remainder = reinterpret_cast<uintptr_t>( buffer_ + data_pos_ ) % 32;
                data_pos_ += 32 - remainder;
                aligned = true;
            }
        }

        // If we have builtin capabilities to find the first set bit, we use it.
        // This brings data_pos_ to where the nl or cr is, so that the below slow loop
        // will not run. If we do not have the builtin, we instead use the loop below
        // to find the exact position of the char.
        #if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

            // If we found a new line, use the mask to get position of the first set bit.
            // This is where the nl or cr character is located, so we move there.
            if( mask != 0 ) {
                auto const offset = __builtin_ctz(mask);
                data_pos_ += offset;
                assert( data_pos_ <= stop_pos );
                assert( buffer_[ data_pos_ ] == '\n' || buffer_[ data_pos_ ] == '\r' );
            } else {
                assert( data_pos_ + 32 > stop_pos );
            }

        #endif // defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

    #endif // defined(GENESIS_AVX2)
}

// -------------------------------------------------------------------------
//     approach_line_or_buffer_end_naive_
// -------------------------------------------------------------------------

void InputStream::approach_line_or_buffer_end_naive_( size_t const stop_pos )
{
    // 8-fold loop unrolling, to help the compiler.
    // It gives some speedup, in particular if the reading is used in a parser that also
    // does other things with the data. In a stand-alone line reader, it still gives
    // a slight advantage.
    while(
        data_pos_ + 7 < stop_pos &&
        buffer_[ data_pos_ + 0 ] != '\n' &&
        buffer_[ data_pos_ + 0 ] != '\r' &&
        buffer_[ data_pos_ + 1 ] != '\n' &&
        buffer_[ data_pos_ + 1 ] != '\r' &&
        buffer_[ data_pos_ + 2 ] != '\n' &&
        buffer_[ data_pos_ + 2 ] != '\r' &&
        buffer_[ data_pos_ + 3 ] != '\n' &&
        buffer_[ data_pos_ + 3 ] != '\r' &&
        buffer_[ data_pos_ + 4 ] != '\n' &&
        buffer_[ data_pos_ + 4 ] != '\r' &&
        buffer_[ data_pos_ + 5 ] != '\n' &&
        buffer_[ data_pos_ + 5 ] != '\r' &&
        buffer_[ data_pos_ + 6 ] != '\n' &&
        buffer_[ data_pos_ + 6 ] != '\r' &&
        buffer_[ data_pos_ + 7 ] != '\n' &&
        buffer_[ data_pos_ + 7 ] != '\r'
    ) {
        data_pos_ += 8;
    }

    // Alternative version that uses 64bit words instead, and hence works without AVX.
    // Uses macros from https://graphics.stanford.edu/~seander/bithacks.html

    // static auto const nmask = ~static_cast<uint64_t>(0) / 255U * '\n';
    // static auto const rmask = ~static_cast<uint64_t>(0) / 255U * '\r';

    // #define haszero(v) (((v) - static_cast<uint64_t>(0x0101010101010101)) & ~(v) & static_cast<uint64_t>(0x8080808080808080))
    // #define hasvalue(x,n) (haszero((x) ^ (~static_cast<uint64_t>(0) / 255U * (n))))
    //
    // auto const* buffc = reinterpret_cast<uint64_t const*>( buffer_ + data_pos_ );
    // size_t i = 0;
    // while( true ) {
    //     bool const e = i*8 >= data_end_;
    //     bool const b = i*8 - start >= BlockLength;
    //
    //     // bool const n = buffc[i] ^ nmask;
    //     // bool const r = buffc[i] ^ rmask;
    //     bool const n = hasvalue( buffc[i], '\n' );
    //     bool const r = hasvalue( buffc[i], '\r' );
    //
    //     if( e | b | n | r ) {
    //         break;
    //     }
    //
    //     ++i;
    // }
    // data_pos_ += i*8;
    //
    // #undef haszero
    // #undef hasvalue
}

// =================================================================================================
//     Buffer Access
// =================================================================================================

void InputStream::jump_unchecked( size_t n )
{
    // Safety first! We do a single check here, so that in the default case,
    // we only branch once - assuming that the compiler doesn't optimize that even better anway.
    if( data_pos_ + n >= data_end_ ) {
        if( data_pos_ + n == data_end_ ) {
            // Lazy approach to make sure that all functions are called as expected
            // when reaching the end of the input data.
            assert( data_pos_ < data_end_ );
            assert( n > 0 );
            data_pos_ += n - 1;
            column_   += n - 1;
            advance();
            return;
        }

        // We try to jump past the end
        assert( data_pos_ + n > data_end_ );
        throw std::runtime_error(
            "Invalid InputStream jump to position after buffer end."
        );
    }

    // Update the position as neeeded.
    data_pos_ += n;
    column_ += n;
    if( data_pos_ >= BlockLength ) {
        update_blocks_();
    }
    set_current_char_();
}

// =================================================================================================
//     Internal Members
// =================================================================================================

// -------------------------------------------------------------------------
//     init_
// -------------------------------------------------------------------------

void InputStream::init_( std::shared_ptr<BaseInputSource> input_source )
{
    // Set to empty defaults if there is no input.
    if( input_source == nullptr ) {
        source_name_ = "invalid source";

        buffer_   = nullptr;
        data_pos_ = 0;
        data_end_ = 0;

        current_ = '\0';
        line_    = 0;
        column_  = 0;
        return;
    }

    // We use three buffer blocks: The first two for the current block/line.
    // The max line length is one buffer length, so the beginning of the line is always
    // in the first block, while its end can reach into the second block, but never exeed it.
    // The third block is for the async reading.
    buffer_ = new char[ 3 * BlockLength ];

    try {
        // Set source name.
        source_name_ = input_source->source_name();

        // Read up to two blocks.
        data_pos_ = 0;
        data_end_ = input_source->read( buffer_, 2 * BlockLength );

        // Skip UTF-8 BOM, if found.
        if( data_end_  >= 3      &&
            buffer_[0] == '\xEF' &&
            buffer_[1] == '\xBB' &&
            buffer_[2] == '\xBF'
        ) {
            data_pos_ = 3;
        }

        // If there was no data, set to "empty" values.
        if( data_pos_ == data_end_ ) {
            reset_();

        // If there is data, set char value.
        } else {
            set_current_char_();
        }

        // If there is more data after the two blocks that we just read, start the
        // reading process (possibly async).
        if( data_end_ == 2 * BlockLength ) {

            // Create the reader. We need to do this explictily,
            // as we use a unique ptr to make this class movable.
            input_reader_ = utils::make_unique<InputReader>();

            input_reader_->init( input_source );
            input_reader_->start_reading( buffer_ + 2 * BlockLength, BlockLength );
        }

    } catch( ... ) {
        delete[] buffer_;
        throw;
    }
}

// -------------------------------------------------------------------------
//     update_blocks_
// -------------------------------------------------------------------------

void InputStream::update_blocks_()
{
    // This function is only called locally in contexts where we already know that we need to
    // update the blocks. We only assert this here again, meaning that we expect the caller
    // functions to check for this already. Handling it this way ensures that the function
    // jump is only made when necesary.
    assert( data_pos_ >= BlockLength );

    // Furthermore, the callers need to check the following condition. So, if it breaks, this
    // function is invalidly called from somewhere else.
    assert( data_pos_ <  data_end_ );

    // If this assertion breaks, someone tempered with our internal invariants.
    assert( data_end_ <= BlockLength * 2 );

    // Move the second to the first block.
    std::memcpy( buffer_, buffer_ + BlockLength, BlockLength );
    data_pos_ -= BlockLength;
    data_end_ -= BlockLength;

    // If we are not yet at the end of the data, start the reader again:
    // Copy the third block to the second, and read into the third one.
    if( input_reader_ && input_reader_->valid() ) {
        data_end_ += input_reader_->finish_reading();
        std::memcpy( buffer_ + BlockLength, buffer_ + 2 * BlockLength, BlockLength );
        input_reader_->start_reading( buffer_ + 2 * BlockLength, BlockLength );
    }

    // After the update, the current position needs to be within the first block.
    assert( data_pos_ < BlockLength );
}

// -------------------------------------------------------------------------
//     set_current_char_
// -------------------------------------------------------------------------

void InputStream::set_current_char_()
{
    // Check end of stream conditions.
    if( data_pos_ >= data_end_ ) GENESIS_UNLIKELY {
        // We do not expect to overshoot. Let's assert this, but if it still happens
        // (in release build), we can also cope, and will just set \0 as the current char.
        assert( data_pos_ == data_end_ );

        if( data_pos_ == data_end_ && data_pos_ > 0 && buffer_[ data_pos_ - 1 ] != '\n' ) {
            // If this is the end of the data, but there was no closing \n, add one.
            buffer_[ data_pos_ ] = '\n';
            ++data_end_;
        } else {
            // If we reached the end, do not fully reset the line and column counters.
            // They might be needed in some parser.
            current_ = '\0';
            return;
        }
    }

    // Treat stupid Windows and Mac lines breaks. Set them to \n, so that downstream parsers
    // don't have to deal with this.
    if( buffer_[ data_pos_ ] == '\r' ) {
        buffer_[ data_pos_ ] = '\n';

        // If this is a Win line break \r\n, skip one of them, so that only a single \n
        // is visible to the outside. We do not treat \n\r line breaks properly here!
        // If any system still uses those, we'd have to change code here.
        if( data_pos_ + 1 < data_end_ && buffer_[ data_pos_ + 1 ] == '\n' ) {
            ++data_pos_;
        }
    }

    // Set the char.
    current_ = buffer_[ data_pos_ ];
}

} // namespace utils
} // namespace genesis
