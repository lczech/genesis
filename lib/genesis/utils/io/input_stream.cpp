/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
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

// For C++17, we have a little speedup in the integer parsing part.
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

    #include <charconv>

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
//     Line Operations
// =================================================================================================

void InputStream::get_line( std::string& target )
{
    // Check edge case.
    if( data_pos_ >= data_end_ ) {
        return;
    }

    // Loop until we find the end of the line. As this can be longer than one block,
    // we might need to update the blocks and store the results in between.
    while( true ) {
        // Read data if necessary.
        if( data_pos_ >= BlockLength ) {
            update_blocks_();
        }
        assert( data_pos_ < BlockLength );

        // Store the starting position, so that we can copy from there once we found the end.
        size_t const start = data_pos_;

        // Read until the end of the line, but also stop before the end of the data,
        // and after we read a full block. End of data: we are done anyway.
        // End of block: need to read the next one first, so loop again.
        auto const stop = std::min( data_end_, start + BlockLength );

        // 8-fold loop unrolling. Yes, the compiler does not do that.
        // It gives some speedup, in particular if the reading is used in a parser that also
        // does other things with the data. In a stand-alone line reader, it still gives
        // a slight advantage.
        while(
            data_pos_ + 7 < stop &&
            buffer_[ data_pos_ + 0 ] != '\n'    &&
            buffer_[ data_pos_ + 0 ] != '\r'    &&
            buffer_[ data_pos_ + 1 ] != '\n'    &&
            buffer_[ data_pos_ + 1 ] != '\r'    &&
            buffer_[ data_pos_ + 2 ] != '\n'    &&
            buffer_[ data_pos_ + 2 ] != '\r'    &&
            buffer_[ data_pos_ + 3 ] != '\n'    &&
            buffer_[ data_pos_ + 3 ] != '\r'    &&
            buffer_[ data_pos_ + 4 ] != '\n'    &&
            buffer_[ data_pos_ + 4 ] != '\r'    &&
            buffer_[ data_pos_ + 5 ] != '\n'    &&
            buffer_[ data_pos_ + 5 ] != '\r'    &&
            buffer_[ data_pos_ + 6 ] != '\n'    &&
            buffer_[ data_pos_ + 6 ] != '\r'    &&
            buffer_[ data_pos_ + 7 ] != '\n'    &&
            buffer_[ data_pos_ + 7 ] != '\r'
        ) {
            data_pos_ += 8;
        }

        // Working AVX version. Not worth the trouble as of now. Keeping it here for reference.

        // #ifdef GENESIS_AVX
        //     #include <immintrin.h>
        // #endif
        //
        // auto b = _mm256_loadu_si256(( __m256i const* )( buffer_ + data_pos_ ));
        //
        // static auto const n = _mm256_set1_epi8( '\n' );
        // static auto const r = _mm256_set1_epi8( '\r' );
        //
        // auto bn = _mm256_cmpeq_epi8( b, n );
        // auto br = _mm256_cmpeq_epi8( b, r );
        //
        // while(
        //     data_pos_ + 32 <= stop &&
        //     _mm256_testz_si256( bn, bn ) &&
        //     _mm256_testz_si256( bn, bn )
        // ) {
        //     data_pos_ += 32;
        //     b = _mm256_loadu_si256(( __m256i const* )( buffer_ + data_pos_ ));
        //     bn = _mm256_cmpeq_epi8( b, n );
        //     br = _mm256_cmpeq_epi8( b, r );
        // }

        // Alternative version taht uses 64bit words instead, and hence works without AVX.
        // Uses macros from https://graphics.stanford.edu/~seander/bithacks.html

        // static auto const nmask = ~static_cast<uint64_t>(0) / 255U * '\n';
        // static auto const rmask = ~static_cast<uint64_t>(0) / 255U * '\r';
        //
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

        // The above loop ends with data_pos_ somewhere before the exact line break.
        // We now need to walk the rest by foot, and examine char by char.
        while(
            data_pos_ < stop &&
            buffer_[ data_pos_ ] != '\n' &&
            buffer_[ data_pos_ ] != '\r'
        ) {
            ++data_pos_;
        }

        // Store what we have so far.
        target.append( buffer_ + start, data_pos_ - start );

        // If the line is not yet finished, we need an extra round. Start the loop again.
        assert( data_pos_ >= start );
        if( data_pos_ - start >= BlockLength ) {
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

// =================================================================================================
//     Parsing
// =================================================================================================

// Only use intrinsics version for the compilers that support them!
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

size_t InputStream::parse_unsigned_integer_gcc_intrinsic_()
{
    // This function only works on little endian systems (I think).
    // We do not check this here, as so far, no one has tried to run our code on any machine
    // that is not little endian. So we are good for now. In case this code needs to be adapted
    // to big endian as well: I think the only change required is the `chunk <<= ...` that needs
    // to turn into a right shift instead. Not entirely sure though.
    // Also, in this function, we make use of the fact that our internal buffer is always way larger
    // than any valid integer input. That is, we may read from after the block end, or even the
    // stream end, but we have enough buffer for this to be okay (after all, we are just reading
    // eight bytes here). We then check for this later.

    // Copy 8 bytes into a chunk that we process as one unit.
    std::uint64_t chunk = 0;
    std::memcpy( &chunk, &buffer_[ data_pos_ ], sizeof( chunk ));

    // Helper macro functions to check whether a word has bytes that are less than or greater
    // than some specified value, and mark these bytes.
    // http://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord
    // http://graphics.stanford.edu/~seander/bithacks.html#HasMoreInWord
    auto const zero = static_cast<uint64_t>(0);
    #define hasless(x,n) (((x)-~zero/255*(n))&~(x)&~zero/255*128)
    #define hasmore(x,n) ((((x)+~zero/255*(127-(n)))|(x))&~zero/255*128)

    // Get all positions that are not digits, by marking a bit in their respective byte.
    auto const l = hasless( chunk, '0' );
    auto const m = hasmore( chunk, '9' );
    auto const p = l | m;

    // Example:
    // String "167\n253\n" turns into chunk c (on little endian systems)
    //         \n        3        5        2       \n        7        6        1
    // c 00001010 00110011 00110101 00110010 00001010 00110111 00110110 00110001
    // l 10000000 00000000 00000000 00000000 10000000 00000000 00000000 00000000
    // m 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    // p 10000000 00000000 00000000 00000000 10000000 00000000 00000000 00000000
    //   ^                                   ^
    // with the two '\n' bytes marked.

    #undef hasless
    #undef hasmore

    // Find the index of the first byte that is not a digit. We first get the bit position
    // using an intrinsic, and then divite by 8 to get the byte. The branching to select the
    // correct intrinsic should be resolved at compile time already.
    // We are using __builtin_ffs and its variants:
    // Returns one plus the index of the least significant 1-bit of x, or if x is zero, returns zero.
    // https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html#Other-Builtins
    int idx = 0;
    if( sizeof(int) == sizeof(std::uint64_t) ) {
        idx = __builtin_ffs(p) / 8;
    } else if( sizeof(long) == sizeof(std::uint64_t) ) {
        idx = __builtin_ffsl(p) / 8;
    } else if( sizeof(long long) == sizeof(std::uint64_t) ) {
        idx = __builtin_ffsll(p) / 8;
    } else {
        static_assert(
            ( sizeof(int) == sizeof(std::uint64_t) ) ||
            ( sizeof(long) == sizeof(std::uint64_t) ) ||
            ( sizeof(long long) == sizeof(std::uint64_t) ),
            "No compilter intrinsic __builtin_ffs[l][l] for std::uint64_t"
        );
        throw std::runtime_error(
            "No compilter intrinsic __builtin_ffs[l][l] for std::uint64_t"
        );
    }
    assert( 0 <= idx && idx <= 8 );

    // Not needed but kept for reference: Mask out all bits that we do not want.
    // auto const mask = ~(~zero << ((idx-1)*8));
    // chunk &= mask;

    // On little endian systems, we need to move the actual data chars that we want to parse to the
    // left-most position for the following code to work. So, for our example from above, we need
    // to move the "xxxx x761" in the chunk so that we get "7610 0000".
    chunk <<= (8 * ( 8 - idx + 1 ));

    // Now use an O(log(n)) method of computing the result, where we combine adjacent parts into
    // numbers, first 2 bytes, then 4 bytes, then all 8 bytes. Inspired by parse_8_chars() from
    // https://kholdstare.github.io/technical/2020/05/26/faster-integer-parsing.html

    // 1-byte mask trick (works on 4 pairs of single digits)
    std::uint64_t lower_digits = (chunk & 0x0f000f000f000f00) >> 8;
    std::uint64_t upper_digits = (chunk & 0x000f000f000f000f) * 10;
    chunk = lower_digits + upper_digits;

    // 2-byte mask trick (works on 2 pairs of two digits)
    lower_digits = (chunk & 0x00ff000000ff0000) >> 16;
    upper_digits = (chunk & 0x000000ff000000ff) * 100;
    chunk = lower_digits + upper_digits;

    // 4-byte mask trick (works on pair of four digits)
    lower_digits = (chunk & 0x0000ffff00000000) >> 32;
    upper_digits = (chunk & 0x000000000000ffff) * 10000;
    chunk = lower_digits + upper_digits;

    // Edge cases. We treat them at the end, so that in the standard cases, the processor
    // does not come to a grinding halt when trying to figure out if these cases apply;
    // this might be premature optimization, but in our tests, it made the function slightly faster.
    // If the returned index is 0, there was no non-digit byte in the chunk,
    // so we run the naive loop instead. We could also call this function here again recursively,
    // summing up parts of large numbers. But that would mean that we need to do overflow
    // detection and all that, and currently, this does not seem needed. Let's be lazy today.
    // Furthermore, if the 8 bytes that we process here are at the end of the stream, we cannot
    // confidently use them, in cases for example where the stream ends in a number, but does
    // not have a new line char at the end. So in that case, better parse naievely.
    // Lastly, if the index is 1, the first byte is not a digit, which is an error, as this function
    // is only called from parsers that expect a number.
    if( idx == 0 || data_pos_ + 8 >= data_end_ ) {
        return parse_unsigned_integer_naive_();
    }
    if( idx == 1 ) {
        throw std::runtime_error(
            "Expecting integer in " + source_name() + " at " + at() + "."
        );
    }

    // Now move as far as needed in the buffer...
    data_pos_ += idx - 1;
    column_   += idx - 1;
    set_current_char_();

    // ...and finally initiate the next block if needed.
    if( data_pos_ >= BlockLength ) {
        update_blocks_();
    }
    assert( data_pos_ < BlockLength );

    return chunk;
}

size_t InputStream::parse_unsigned_integer_from_chars_()
{

    // Re-implementation of the gcc from_chars() code.
    // https://github.com/gcc-mirror/gcc/blob/12bb62fbb47bd2848746da53c72ed068a4274daf/libstdc++-v3/include/std/charconv
    // Currently not in use and not well tested!

    // Prepare. We alias T, in case we want to refactor to a template function at some point.
    using T = size_t;
    using namespace utils;
    T x = 0;

    int const base = 10;
    auto raise_and_add_ = [base]( T& val, unsigned char c ) {
        return !(
            __builtin_mul_overflow( val, base, &val ) ||
            __builtin_add_overflow( val, c, &val )
        );
    };

    auto from_chars_digit_ = [&]( char const*& first, char const* last, T& val ) {
        while( first != last ) {
            char const c = *first;
            if( is_digit(c) ) {
                if( !raise_and_add_(val, c - '0') ) {
                    return false;
                }
                first++;
            } else {
                return true;
            }
        }
        return true;
    };

    char const* start = &buffer_[ data_pos_ ];
    char const* end   = &buffer_[ data_end_ ];
    auto const valid = from_chars_digit_( start, end, x );
    auto const dist = start - &buffer_[ data_pos_ ];

    if( dist == 0 ) {
        throw std::runtime_error(
            "Expecting integer in " + source_name() + " at " + at() + "."
        );
    } else if( !valid ) {
        throw std::overflow_error(
            "Numerical overflow in " + source_name() + " at " + at() + "."
        );
    } else if( std::is_signed<T>::value ) {
        assert( false );
        // T tmp;
        // if (__builtin_mul_overflow(x, sign, &tmp)) {
        //     throw std::overflow_error(
        //         "Numerical overflow in " + source_name() + " at " + at() + "."
        //     );
        // }
    }

    // Move to where we the parsing left us.
    data_pos_ += dist;
    column_   += dist;
    set_current_char_();

    // Now finally initiate the next block if needed.
    if( data_pos_ >= BlockLength ) {
        update_blocks_();
    }
    assert( data_pos_ < BlockLength );

    return x;
}

#endif // defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

size_t InputStream::parse_unsigned_integer_std_from_chars_()
{
    // Uses the C++17 std::from_chars() function.
    // Currently not in use and not well tested!

    // Prepare. We alias T, in case we want to refactor to a template function at some point.
    using T = size_t;
    using namespace utils;
    T x = 0;

    // Fastest method accoing to
    // https://www.fluentcpp.com/2018/07/27/how-to-efficiently-convert-a-string-to-an-int-in-c/
    // is from_chars(), so let's us it!

    auto const conv = std::from_chars( &buffer_[ data_pos_ ], &buffer_[ data_end_ ], x );

    // How many chars did we consume?
    auto const dist = conv.ptr - &buffer_[ data_pos_ ];

    // Check that we processed at least one digit, as this function is only called when the
    // input format requires an integer. This is equivalent to the check in the non C++17 version
    // below for data_pos_ >= data_end_ || ! is_digit( current_ )
    if( dist == 0 ) {
        throw std::runtime_error(
            "Expecting integer in " + source_name() + " at " + at() + "."
        );
    }

    if( conv.ec != std::errc() ) {
        if( conv.ec == std::errc::result_out_of_range ) {
            throw std::overflow_error(
                "Numerical overflow in " + source_name() + " at " + at() + "."
            );
        } else if( conv.ec == std::errc::invalid_argument ) {
            // Cannot happen, as we above checked that there is at least one digit.
            assert( false );
        } else {
            // Cannot happen, as we caught every case of `ec`.
            assert( false );
        }

        // In either case, we need to stop here.
        throw std::overflow_error(
            "Integer parsing error in " + source_name() + " at " + at() + "."
        );
    }

    // Move to where we the parsing left us.
    column_   += dist;
    data_pos_ += dist;

    // Now finally initiate the next block if needed.
    if( data_pos_ >= BlockLength ) {
        update_blocks_();
    }
    assert( data_pos_ < BlockLength );

    // Finally we also need to update the char so that new lines are taken care of.
    set_current_char_();

    return x;
}

#endif // ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

size_t InputStream::parse_unsigned_integer_naive_()
{
    // Prepare. We alias T, in case we want to refactor to a template function at some point.
    using T = size_t;
    using namespace utils;
    T x = 0;

    if( data_pos_ >= data_end_ || ! is_digit( current_ ) ) {
        throw std::runtime_error(
            "Expecting integer in " + source_name() + " at " + at() + "."
        );
    }

    while(( data_pos_ < data_end_ ) && is_digit( current_ )) {
        T y = current_ - '0';

        if( x > ( std::numeric_limits<T>::max() - y ) / 10 ) {
            throw std::overflow_error(
                "Numerical overflow in " + source_name() + " at " + at() + "."
            );
        }

        x = 10 * x + y;

        // In the original function that was not part of this class, we simply called
        // advance() here, to move to the next char. However, here, we already know that
        // we have data_pos_ < data_end_, and that we do not have a new line.
        // Furthermore, we also can ignore the update for block length while in this loop
        // (or maybe even completely), as it does not matter much if we move a bit into the
        // second block before starting the reading thread again. This loop here cannot
        // iterate that many times anyway before we overflow the interger.
        // So let's simply move on to the next char.
        // advance();
        assert( data_pos_ < data_end_ );
        assert( current_ != '\n' );
        ++column_;
        ++data_pos_;
        current_ = buffer_[ data_pos_ ];
    }

    // Now finally initiate the next block if needed.
    if( data_pos_ >= BlockLength ) {
        update_blocks_();
    }
    assert( data_pos_ < BlockLength );

    // Finally we also need to update the char so that new lines are taken care of.
    set_current_char_();

    return x;
}

size_t InputStream::parse_unsigned_integer_size_t_()
{
    // Select the fastest alternative available for a given compiler and C++ version.
    #if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

        // If we have GCC or Clang, use our own handcrafted fast-as-hell implementation.
        return parse_unsigned_integer_gcc_intrinsic_();

    // #elif ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    //
    //     // Otherwise, if this is C++17, at least use its own fast version,
    //     // that can use some compiler intrinsics itself.
    //     return parse_unsigned_integer_std_from_chars_();

    #else

        // If neither, just use the slow, naive loop.
        return parse_unsigned_integer_naive_();

    #endif
}

// =================================================================================================
//     Internal Members
// =================================================================================================

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
        // reading process (possibly async, if pthreads is available).
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

} // namespace utils
} // namespace genesis
