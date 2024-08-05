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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/format/sync_reader.hpp"

#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Reading & Parsing
// =================================================================================================

// -------------------------------------------------------------------------
//     read_header
// -------------------------------------------------------------------------

std::vector<std::string> SyncReader::read_header(
    utils::InputStream& input_stream
) const {
    // Check that there is a header; if not, we just leave again, without any sample names.
    std::vector<std::string> result;
    auto& it = input_stream;
    if( !it || *it != '#' ) {
        return result;
    }

    // Move to the content, potentially skipping a tab there.
    ++it;
    if( it && *it == '\t' ) {
        ++it;
    }

    // Now we can read the rest of the line, and for simplicity just split later.
    // Check that the fixed columns are there as expected.
    result = utils::split( it.get_line() );
    if( result.size() < 3 || result[0] != "chr" || result[1] != "pos" || result[2] != "ref" ) {
        throw std::runtime_error(
            "Malformed sync " + it.source_name() + ": Header row provided (starting with '#'), " +
            "but the first three entries are not \"chr\", \"pos\", \"ref\"."
        );
    }
    result.erase( result.begin(), result.begin() + 3 );
    return result;
}

std::vector<std::string> SyncReader::read_header(
    utils::InputStream& input_stream,
    std::vector<bool> const& sample_filter
) const {
    // The header only has to be read once, so we do not need to be overly efficient here.
    // Simply call the unfiltered function, and then subset later;
    // no need to replicate the parsing code here.
    std::vector<std::string> result;
    auto all_sample_names = read_header( input_stream );
    if( all_sample_names.empty() ) {
        return result;
    }

    // Now subset to the entries that we actually want.
    if( sample_filter.size() != all_sample_names.size() ) {
        throw std::runtime_error(
            "Malformed sync " + input_stream.source_name() +
            ": Number of sample names in header (" +
            std::to_string( all_sample_names.size() ) +
            ") does not match number of samples in filter (" +
            std::to_string( sample_filter.size() ) + ")"
        );
    }
    for( size_t i = 0; i < sample_filter.size(); ++i ) {
        if( sample_filter[i] ) {
            result.push_back( all_sample_names[i] );
        }
    }
    return result;
}

// -------------------------------------------------------------------------
//     read
// -------------------------------------------------------------------------

std::vector<Variant> SyncReader::read(
    std::shared_ptr< utils::BaseInputSource > source
) const {
    std::vector<Variant> result;
    utils::InputStream it( source );

    // Potentially read the header (and discard it, as our Variant does not store sample names).
    read_header( it );

    // Read until end of input, pushing copies into the result
    // (moving would not reduce the number of times that we need to allocate memory here).
    Variant variant;
    while( parse_line_( it, variant, {}, false )) {
        result.push_back( variant );
    }
    return result;
}

std::vector<Variant> SyncReader::read(
    std::shared_ptr< utils::BaseInputSource > source,
    std::vector<bool> const&                  sample_filter
) const {
    std::vector<Variant> result;
    utils::InputStream it( source );

    // Potentially read the header (and discard it, as our Variant does not store sample names).
    read_header( it, sample_filter );

    // Read until end of input
    Variant variant;
    while( parse_line_( it, variant, sample_filter, true )) {
        result.push_back( variant );
    }
    return result;
}

// -------------------------------------------------------------------------
//     parse_line
// -------------------------------------------------------------------------

bool SyncReader::parse_line(
    utils::InputStream& input_stream,
    Variant&            variant
) const {
    return parse_line_( input_stream, variant, {}, false );
}

bool SyncReader::parse_line(
    utils::InputStream&      input_stream,
    Variant&                 variant,
    std::vector<bool> const& sample_filter
) const {
    return parse_line_( input_stream, variant, sample_filter, true );
}

// =================================================================================================
//     Internal Parsing
// =================================================================================================

// -------------------------------------------------------------------------
//     parse_line_
// -------------------------------------------------------------------------

bool SyncReader::parse_line_(
    utils::InputStream&      input_stream,
    Variant&                 variant,
    std::vector<bool> const& sample_filter,
    bool                     use_sample_filter
) const {
    using namespace genesis::utils;

    // Shorthand.
    auto& it = input_stream;
    if( !it ) {
        variant = Variant{};
        return false;
    }

    // Read fixed columns for chromosome and position.
    variant.chromosome = utils::read_until( it, []( char c ){ return c == '\t' || c == '\n'; });
    if( variant.chromosome.empty() ) {
        throw std::runtime_error(
            "Malformed sync " + it.source_name() + " at " + it.at() +
            ": empty chromosome name"
        );
    }
    it.read_char_or_throw( '\t' );
    variant.position = parse_unsigned_integer<size_t>( it );
    if( variant.position == 0 ) {
        throw std::runtime_error(
            "Malformed sync " + it.source_name() + " at " + it.at() +
            ": chromosome position == 0"
        );
    }
    it.read_char_or_throw( '\t' );
    if( !it || *it == '\n' ) {
        throw std::runtime_error(
            std::string("In ") + it.source_name() + ": Unexpected end of line at " + it.at()
        );
    }

    // Read and check fixed column for the reference base.
    auto const rb = to_upper( *it );
    if( ! is_valid_base_or_n( rb ) && rb != '.' && rb != '*' ) {
        throw std::runtime_error(
            std::string("In ") + it.source_name() + ": Invalid reference base char " +
            char_to_hex(rb) + " at " + it.at()
        );
    }
    variant.reference_base = rb;
    ++it;

    // Read the samples. We switch once for the first line, and thereafter check that we read the
    // same number of samples each time.
    size_t src_index = 0;
    if( variant.samples.empty() ) {
        while( it && *it != '\n' ) {
            if( !use_sample_filter || ( src_index < sample_filter.size() && sample_filter[src_index] )) {
                variant.samples.emplace_back();
                parse_sample_( it, variant.samples.back() );
            } else {
                skip_sample_( it );
            }
            ++src_index;
        }
    } else {
        // Here we need two indices, one over the samples in the file (source),
        // and one for the samples that we are writing in our Variant (destination).
        size_t dst_index = 0;
        while( it && *it != '\n' ) {
            // Parse or skip, depending on filter.
            if(
                ( ! use_sample_filter ) ||
                ( src_index < sample_filter.size() && sample_filter[src_index] )
            ) {
                // If the numbers do not match, go straight to the error check and throw.
                if( dst_index >= variant.samples.size() ) {
                    break;
                }
                assert( dst_index < variant.samples.size() );
                parse_sample_( it, variant.samples[dst_index] );
                ++dst_index;
            } else {
                skip_sample_( it );
            }
            ++src_index;
        }

        // Need to have the exact size of samples in the line.
        if( dst_index != variant.samples.size() ) {
            throw std::runtime_error(
                "Malformed sync " + it.source_name() + " at " + it.at() +
                ": Line with different number of samples. Expecting " +
                std::to_string( variant.samples.size() ) + " samples based on previous lines, "
                "but found " + std::to_string( dst_index ) + " (non-filtered) samples."
            );
        }
    }
    if( use_sample_filter && src_index != sample_filter.size() ) {
        throw std::runtime_error(
            "Malformed sync " + it.source_name() + " at " + it.at() +
            ": Number of samples in the line (" + std::to_string( src_index ) +
            ") does not match the number of filter entries (" +
            std::to_string( sample_filter.size() ) + ")."
        );
    }

    // Sync does not have alt bases, so try to get one based on counts.
    // Excluding the ref base, we use the base of the remaining three that has the highest total
    // count across all samples, unless all of them are zero, in which case we do not set the
    // alt base. We also skip cases where the ref is not in ACGT, as then alt is also meaningless.
    if( guess_alt_base_ ) {
        variant.alternative_base = guess_alternative_base( variant, true );
    } else {
        variant.alternative_base = 'N';
    }

    // Set the status of the Variant. If all samples are missing, so is this Variant.
    variant.status.reset();
    size_t missing_count = 0;
    for( auto const& sample : variant.samples ) {
        if( sample.status.is( SampleCountsFilterTag::kMissing )) {
            ++missing_count;
        }
    }
    if( missing_count == variant.samples.size() ) {
        variant.status.set( VariantFilterTag::kMissing );
    }

    // We reched the end of the line or of the whole input.
    // Move to the beginning of the next line.
    assert( !it || *it == '\n' );
    ++it;
    return true;
}

// -------------------------------------------------------------------------
//     parse_sample_gcc_intrinsic_
// -------------------------------------------------------------------------

// Only use intrinsics version for the compilers that support them!
#if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

void SyncReader::parse_sample_gcc_intrinsic_(
    utils::InputStream& input_stream,
    SampleCounts&       sample
) const {
    using namespace genesis::utils;
    auto& it = input_stream;
    auto const buff = it.buffer();

    // We can only run this function if the buffer is guaranteed to contain at least 6 integers
    // of the largest size that we can process here (8 bytes in bulk, with 7 of them for the digits,
    // and one for the delimiter). If the buffer is smaller, because we are near
    // the end of the file, we switch to the slow function instead.
    // This check is conservative, as in most cases, we won't have numbers that long in the data.
    // But for those last few entries in a large file, this does not really matter, so let's play
    // it safe!
    if( buff.second < 6 * 8 ) {
        parse_sample_simple_( it, sample );
        return;
    }

    // This function adapts a lot of the ideas from our
    // InputStream::parse_unsigned_integer_intrinsic_() function. See there for details on the
    // techniques being used here. We here only provide shortened comments on the bit tricks.

    // We define a chunk to represent one count number, ACGT and N and D (deletions),
    // which starts with a chunk that contains 8 bytes from the input stream, which will then be
    // shortened and processed as needed to only contain the actual digits, and is then finally
    // turned into the integer representation, step by step.
    struct Chunk
    {
        // The data to process
        uint64_t data = 0;

        // How many bytes are actually digits? This is stored as the number of digits plus one.
        size_t length = 0;

        // Where in the buffer does this chunk (this sequence of digits) start?
        size_t offset = 0;
    };

    // Function to get a chunk, that is one set of chars representing a number. We here get 8 byte
    // in bulk, and later check that those contain the delimiter to the next number or the end of
    // the sample.
    // If not, we only find out at the end, after having done all the parsing work, and will thus
    // have wasted timed, but this only ever occurrs in cases with counts >= 10,000,000 (more than
    // 7 digits, so that there is no delimiter within the 8 bytes), which should be rare in practice,
    // and in which case we can live with the waste.
    auto get_chunk_ = []( char const* buffer, size_t offset )
    {
        // Prepare a new chunk and store its offset.
        Chunk chunk;
        chunk.offset = offset;

        // Copy 8 bytes into the chunk that we process as one unit.
        std::memcpy( &chunk.data, buffer + offset, sizeof( chunk.data ));

        // Helper macro functions to check whether a word has bytes that are less than or greater
        // than some specified value, and mark these bytes.
        // http://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord
        // http://graphics.stanford.edu/~seander/bithacks.html#HasMoreInWord
        auto const zero = static_cast<uint64_t>(0);
        #define genesis_sync_reader_hasless(x,n) (((x)-~zero/255*(n))&~(x)&~zero/255*128)
        #define genesis_sync_reader_hasmore(x,n) ((((x)+~zero/255*(127-(n)))|(x))&~zero/255*128)

        // Get all positions that are not digits, by marking a bit in their respective byte.
        auto const l = genesis_sync_reader_hasless( chunk.data, '0' );
        auto const m = genesis_sync_reader_hasmore( chunk.data, '9' );
        auto const p = l | m;

        #undef genesis_sync_reader_hasless
        #undef genesis_sync_reader_hasmore

        // Find the index of the first byte that is not a digit.
        // The length is stored plus one here, due to how __builtin_ffs works. We need this later
        // to check for the edge case (no delimiter found - the word contains only digits).
        if( sizeof(int) == sizeof(std::uint64_t) ) {
            chunk.length = __builtin_ffs(p) / 8;
        } else if( sizeof(long) == sizeof(std::uint64_t) ) {
            chunk.length = __builtin_ffsl(p) / 8;
        } else if( sizeof(long long) == sizeof(std::uint64_t) ) {
            chunk.length = __builtin_ffsll(p) / 8;
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

        return chunk;
    };

    // Do the minimal amount of work that is necessary to get all chunks into position.
    // That is, in this part, one line depends on the output of the previous, as we have to move
    // forward in the buffer depending on how many digits we found.
    // By doing the minimal work here (that is, not yet unpacking the chunk data into actual
    // integers), we can maximize the CPU pipeline parallel part later (that does the unpacking).
    // We start with offset 1, to skip the inital tab. We check later that there actually is a tab.
    // The allele frequencies are stored in the order `A:T:C:G:N:del`,
    // see https://sourceforge.net/p/popoolation2/wiki/Tutorial/
    auto a_chunk = get_chunk_( buff.first, 1 );
    auto t_chunk = get_chunk_( buff.first, a_chunk.offset + a_chunk.length );
    auto c_chunk = get_chunk_( buff.first, t_chunk.offset + t_chunk.length );
    auto g_chunk = get_chunk_( buff.first, c_chunk.offset + c_chunk.length );
    auto n_chunk = get_chunk_( buff.first, g_chunk.offset + g_chunk.length );
    auto d_chunk = get_chunk_( buff.first, n_chunk.offset + n_chunk.length );

    // This has to follow from the logic of the above.
    assert( a_chunk.offset == 1 );
    assert( t_chunk.offset == a_chunk.offset + a_chunk.length );
    assert( c_chunk.offset == t_chunk.offset + t_chunk.length );
    assert( g_chunk.offset == c_chunk.offset + c_chunk.length );
    assert( n_chunk.offset == g_chunk.offset + g_chunk.length );
    assert( d_chunk.offset == n_chunk.offset + n_chunk.length );

    // Function to process a chunk, that is, one number that is meant to be a count in the file.
    // See InputStream::parse_unsigned_integer_intrinsic_() for details.
    auto process_chunk_ = []( Chunk& chunk )
    {
        // We need to move the actual data chars that we want to parse to the left-most
        // position for the following code to work.
        chunk.data <<= (8 * ( 8 - chunk.length + 1 ));

        // 1-byte mask trick (works on 4 pairs of single digits)
        std::uint64_t lower_digits = (chunk.data & 0x0f000f000f000f00) >> 8;
        std::uint64_t upper_digits = (chunk.data & 0x000f000f000f000f) * 10;
        chunk.data = lower_digits + upper_digits;

        // 2-byte mask trick (works on 2 pairs of two digits)
        lower_digits = (chunk.data & 0x00ff000000ff0000) >> 16;
        upper_digits = (chunk.data & 0x000000ff000000ff) * 100;
        chunk.data = lower_digits + upper_digits;

        // 4-byte mask trick (works on pair of four digits)
        lower_digits = (chunk.data & 0x0000ffff00000000) >> 32;
        upper_digits = (chunk.data & 0x000000000000ffff) * 10000;
        chunk.data = lower_digits + upper_digits;

        // Check that we got at least one digit, and at most 7.
        // When there was no digit at all, the first char is a non-digit, meaning that length == 1.
        // When there were only digits (all 8 bytes), __builtin_ffs returned length == 0 instead,
        // as then no delimiter was found at all. Check both cases at once, by length > 1.
        // Also, we assert that the intrinsic did not return anything too large.
        assert( chunk.length <= 8 );
        return ( chunk.length > 1 );
    };

    // Now do the bulk processing, using CPU-level pipeline parallelization by offering all
    // chunks to the CPU at once, with no dependencies between them. Any reasonable compiler will
    // make use of this fact. This gives ~25% speedup in our tests compared to the already fast
    // InputStream::parse_unsigned_integer_intrinsic_() function.
    bool good = true;
    good &= process_chunk_( a_chunk );
    good &= process_chunk_( t_chunk );
    good &= process_chunk_( c_chunk );
    good &= process_chunk_( g_chunk );
    good &= process_chunk_( n_chunk );
    good &= process_chunk_( d_chunk );

    // We have now processed all chunk data, which now contain the actual numbers.
    sample.a_count = a_chunk.data;
    sample.t_count = t_chunk.data;
    sample.c_count = c_chunk.data;
    sample.g_count = g_chunk.data;
    sample.n_count = n_chunk.data;
    sample.d_count = d_chunk.data;

    // At the end do the error check, so that we are not wasting cycles to wait for the result
    // of this check in the standard (non-error) case first. If this fails, no problem, we have
    // not yet moved in the buffer, so just run the slow version on the same data again,
    // to get proper parsing (for cases with more than 7 digits) or proper error reporting.
    // We here check that the sample was delimited by a tab, that all number conversions were good
    // (that is, they contained at least one digit, and at most 7), and were all delimited by colons.
    // We also already asserted above that all offsets are at least 1, so that the subtraction
    // of 1 here works without wrapping around the unsigned int.
    if(
        ( *it != '\t' ) ||
        ! good ||
        buff.first[ t_chunk.offset - 1 ] != ':' ||
        buff.first[ c_chunk.offset - 1 ] != ':' ||
        buff.first[ g_chunk.offset - 1 ] != ':' ||
        buff.first[ n_chunk.offset - 1 ] != ':' ||
        buff.first[ d_chunk.offset - 1 ] != ':'
    ) {
        // Repeat slowly to throw error at the correct position.
        parse_sample_simple_( it, sample );
        return;
    }

    // If we are here, we have read a full sample with no error. This means that there were at least
    // 6 digits, 5 colons, and the inital tab, so 12 chars in total that we jump.
    assert( d_chunk.offset + d_chunk.length - 1 >= 12 );

    // Also, just because we can, assert all offsets...
    assert( a_chunk.offset ==  1 );
    assert( t_chunk.offset >=  3 );
    assert( c_chunk.offset >=  5 );
    assert( g_chunk.offset >=  7 );
    assert( n_chunk.offset >=  9 );
    assert( d_chunk.offset >= 11 );

    // ...and lengths. Again, lengths are plus one, due to how __builtin_ffs works.
    assert( a_chunk.length >=  2 );
    assert( t_chunk.length >=  2 );
    assert( c_chunk.length >=  2 );
    assert( g_chunk.length >=  2 );
    assert( n_chunk.length >=  2 );
    assert( d_chunk.length >=  2 );

    // We can only process data with 7 or fewer digits. Let's assert this.
    // Potential ways this could fail are, e.g., if somehome we produced random data or max int by
    // subtracting one from zero, or accessed uninitialized memory, or some other horrible error.
    assert( sample.a_count < 10000000 );
    assert( sample.t_count < 10000000 );
    assert( sample.c_count < 10000000 );
    assert( sample.g_count < 10000000 );
    assert( sample.n_count < 10000000 );
    assert( sample.d_count < 10000000 );

    // Jump to the position after the last entry.
    it.jump_unchecked( d_chunk.offset + d_chunk.length - 1 );
}

#endif // defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

// -------------------------------------------------------------------------
//     parse_sample_simple_
// -------------------------------------------------------------------------

void SyncReader::parse_sample_simple_(
    utils::InputStream& input_stream,
    SampleCounts&       sample
) const {
    using namespace genesis::utils;
    auto& it = input_stream;
    it.read_char_or_throw( '\t' );

    // The allele frequencies are stored in the order `A:T:C:G:N:del`,
    // see https://sourceforge.net/p/popoolation2/wiki/Tutorial/
    sample.a_count = parse_unsigned_integer<size_t>( it );
    it.read_char_or_throw( ':' );
    sample.t_count = parse_unsigned_integer<size_t>( it );
    it.read_char_or_throw( ':' );
    sample.c_count = parse_unsigned_integer<size_t>( it );
    it.read_char_or_throw( ':' );
    sample.g_count = parse_unsigned_integer<size_t>( it );
    it.read_char_or_throw( ':' );
    sample.n_count = parse_unsigned_integer<size_t>( it );
    it.read_char_or_throw( ':' );
    sample.d_count = parse_unsigned_integer<size_t>( it );
}

// -------------------------------------------------------------------------
//     parse_sample_
// -------------------------------------------------------------------------

void SyncReader::parse_sample_(
    utils::InputStream& input_stream,
    SampleCounts&       sample
) const {
    using namespace genesis::utils;
    auto& it = input_stream;
    auto const buff = it.buffer();

    // Reset the filter status of the sample, in case it was set to not passing previously.
    sample.status.reset();

    // We have two special cases that we want to check: all single digits (in which case we can
    // speed up the parsing by a lot!), and the missing data annotation format of Kapun.
    // Both consist of the ?:?:?:?:?:? pattern with single characters in between the colons.
    // We here check that part first, and then continue checking for the two special cases,
    // in order to avoid having to double check the shared aspects of the pattern.
    if(
        buff.second >= 12        &&
        buff.first[  0 ] == '\t' &&
        buff.first[  2 ] == ':'  &&
        buff.first[  4 ] == ':'  &&
        buff.first[  6 ] == ':'  &&
        buff.first[  8 ] == ':'  &&
        buff.first[ 10 ] == ':'
    ) {
        // We find that almost all entries in real world data are single digits.
        // Then, an entry has 11 chars: "0:0:6:0:0:0". Use this fact for super-charging the parsing.
        // We check that all chars are exactly as we expect them. At the end, we only need to check that
        // at position 12 there is no digit, that is, that the number is done and does not have any more
        // digits. The check whether that char is valid in the context of the file is then done later
        // in the next parsing step after finishing this function.
        if(
            buff.second >= 13              &&
            is_digit(   buff.first[  1 ] ) &&
            is_digit(   buff.first[  3 ] ) &&
            is_digit(   buff.first[  5 ] ) &&
            is_digit(   buff.first[  7 ] ) &&
            is_digit(   buff.first[  9 ] ) &&
            is_digit(   buff.first[ 11 ] ) &&
            ! is_digit( buff.first[ 12 ] )
        ) {
            // Convert single digits from ASCII to their int value.
            sample.a_count = buff.first[  1 ] - '0';
            sample.t_count = buff.first[  3 ] - '0';
            sample.c_count = buff.first[  5 ] - '0';
            sample.g_count = buff.first[  7 ] - '0';
            sample.n_count = buff.first[  9 ] - '0';
            sample.d_count = buff.first[ 11 ] - '0';

            // Jump to the position after the last entry.
            it.jump_unchecked( 12 );
            return;
        }

        // If activated, allow the missing site notation `.:.:.:.:.:.` of Kapun.
        if(
            allow_missing_           &&
            buff.first[  1 ] == '.'  &&
            buff.first[  3 ] == '.'  &&
            buff.first[  5 ] == '.'  &&
            buff.first[  7 ] == '.'  &&
            buff.first[  9 ] == '.'  &&
            buff.first[ 11 ] == '.'
        ) {
            // Set everything to zero and signal zero counts or missing data.
            sample.a_count = 0;
            sample.t_count = 0;
            sample.c_count = 0;
            sample.g_count = 0;
            sample.n_count = 0;
            sample.d_count = 0;
            sample.status.set( SampleCountsFilterTag::kMissing );

            // Jump to the position after the last entry.
            it.jump_unchecked( 12 );
            return;
        }
    }

    // If it's not the simply one-digit format, select the fastest alternative algorithm
    // available for the given compiler.
    #if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

        parse_sample_gcc_intrinsic_( it, sample );

    #else

        parse_sample_simple_( it, sample );

    #endif
}

// -------------------------------------------------------------------------
//     skip_sample_
// -------------------------------------------------------------------------

void SyncReader::skip_sample_(
    utils::InputStream& input_stream
) const {
    using namespace genesis::utils;

    // The skip functions are slow, because they need char by char access to the input stream.
    // Need to fix this at some point. For now, just read into an unused dummy.
    // Not worth bothering with this too much now, as this is really fast anyway.
    SampleCounts dummy;
    parse_sample_( input_stream, dummy );

    // Simply skip everything.
    // input_stream.read_char_or_throw( '\t' );
    // skip_while( input_stream, is_digit );
    // input_stream.read_char_or_throw( ':' );
    // skip_while( input_stream, is_digit );
    // input_stream.read_char_or_throw( ':' );
    // skip_while( input_stream, is_digit );
    // input_stream.read_char_or_throw( ':' );
    // skip_while( input_stream, is_digit );
    // input_stream.read_char_or_throw( ':' );
    // skip_while( input_stream, is_digit );
    // input_stream.read_char_or_throw( ':' );
    // skip_while( input_stream, is_digit );
}

} // namespace population
} // namespace genesis
