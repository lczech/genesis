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
 * @ingroup sequence
 */

#include "genesis/sequence/functions/stats.hpp"

#include "genesis/sequence/functions/codes.hpp"
#include "genesis/sequence/functions/functions.hpp"
#include "genesis/sequence/printers/simple.hpp"
#include "genesis/sequence/counts.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/text/style.hpp"
#include "genesis/utils/tools/char_lookup.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <numeric>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Characteristics
// =================================================================================================

// -------------------------------------------------------------------------
//     Site Histogram
// -------------------------------------------------------------------------

std::map<char, size_t> site_histogram( Sequence const& seq )
{
    std::map<char, size_t> result;
    for( auto const& s : seq ) {
        ++result[s];
    }
    return result;
}
std::map<char, size_t> site_histogram( SequenceSet const& set )
{
    std::map<char, size_t> result;
    for( auto const& seq : set ) {
        for( auto const& s : seq ) {
            ++result[s];
        }
    }
    return result;
}

// -------------------------------------------------------------------------
//     Base Frequencies
// -------------------------------------------------------------------------

/**
 * @brief Local helper function that turns a site histogram into base frequencies.
 */
std::map<char, double> base_frequencies_accumulator(
    std::map<char, size_t> const& sitehistogram,
    std::string            const& plain_chars
) {
    // Prepare result (do it here to facilitate copy elision).
    std::map<char, double> result;

    // Calculate sum of raw counts of all chars given in plain_chars.
    size_t sum = 0;
    for( auto const& shp : sitehistogram ) {
        if( plain_chars.find( shp.first ) != std::string::npos ) {
            sum += shp.second;
        }
    }

    // Make relative.
    for( auto const& pc : plain_chars ) {
        if( sitehistogram.count( pc )) {
            result[pc] = static_cast<double>( sitehistogram.at(pc) ) / static_cast<double>( sum );
        }
    }
    return result;
}

std::map<char, double> base_frequencies( Sequence const& seq, std::string const& plain_chars )
{
    auto const sh = site_histogram( seq );
    return base_frequencies_accumulator( sh, plain_chars );
}

std::map<char, double> base_frequencies( SequenceSet const& set, std::string const& plain_chars )
{
    auto const sh = site_histogram( set );
    return base_frequencies_accumulator( sh, plain_chars );
}

// -------------------------------------------------------------------------
//     Char counting and validation
// -------------------------------------------------------------------------

size_t count_chars( SequenceSet const& set, std::string const& chars )
{
    // Init array to false, then set all necessary chars to true.
    auto lookup = utils::CharLookup<bool>( false );
    lookup.set_selection_upper_lower( chars, true );

    size_t counter = 0;
    for( auto& s : set ) {
        for( auto& c : s ) {
            // get rid of this check and leave it to the parser/lexer/stream iterator
            if( c < 0 ) {
                continue;
            }
            assert( c >= 0 );
            if( lookup[ c ] ) {
                ++counter;
            }
        }
    }
    return counter;
}

// -------------------------------------------------------------------------
//     Gap Counting
// -------------------------------------------------------------------------

double gapyness( SequenceSet const& set, std::string const& gap_chars )
{
    size_t gaps = count_chars( set, gap_chars );
    size_t len  = total_length( set );
    if( len == 0 ) {
        return 0.0;
    }

    double ret = static_cast<double>( gaps ) / static_cast<double>( len );
    assert( 0.0 <= ret && ret <= 1.0 );
    return ret;
}

size_t gap_site_count( SequenceCounts const& counts )
{
    size_t res = 0;
    for( size_t site_idx = 0; site_idx < counts.length(); ++site_idx ) {
        bool all_gap_site = true;

        for( size_t char_idx = 0; char_idx < counts.characters().size(); ++char_idx ) {
            all_gap_site &= ( counts.count_at( char_idx, site_idx ) == 0 );
        }

        if( all_gap_site ) {
            ++res;
        }
    }
    return res;
}

// =================================================================================================
//     K-mers
// =================================================================================================

std::vector<size_t> kmer_counts( Sequence const& seq, size_t k )
{
    // Make a very generous check whether we can actually calculate the wanted power.
    // We are going to calculate the power 4^k, which is 2^(2k), so test for this limit.
    // We will almost surely run out of memory for even way smaller k, but at least this is the
    // upper limit. Also, sutract one last bit, because we need it for programming convenience.
    // Coule be done in a different way, but the size is big enough anyway.
    if( k > sizeof(size_t) * 8 / 2 - 1 ) {
        throw std::invalid_argument( "Cannot store kmers for such large k." );
    }
    if( k == 0 ) {
        throw std::invalid_argument( "Invalid k==0 for kmer list." );
    }

    // Result vector. Count the occurance of each possible kmer.
    auto result = std::vector<size_t>( utils::int_pow( 4, k ), 0 );

    if( seq.size() < k ) {
        return result;
    }

    auto char_to_num = []( char c ){
        switch( c ) {
            case 'a':
            case 'A':
                return 0;

            case 'c':
            case 'C':
                return 1;

            case 'g':
            case 'G':
                return 2;

            case 't':
            case 'T':
                return 3;

            default:
                throw std::runtime_error( "Sequence contains other chars than 'ACGT'." );
        }
    };

    // Current position in the sequence, and the idx of the count vector for the current kmer.
    size_t pos = 0;
    size_t idx = 0;

    size_t const mask = ( 1 << ( k * 2 )) - 1;
    // LOG_DBG2 << "mask " << mask;

    // Init with the first kmer.
    for( ; pos < k; ++pos ) {
        idx <<= 2;
        idx |= char_to_num( seq[pos] );

        // LOG_DBG2 << seq[pos] << " ("<< char_to_num( seq[pos] ) << ")" << " --> " << idx;
    }
    ++result[ idx ];

    // LOG_DBG2 << "---";

    // Process the rest of the sequence.
    for( ; pos < seq.size(); ++pos ) {
        // LOG_DBG2 << "1 " << idx;
        idx <<= 2;
        // LOG_DBG2 << "2 " << idx;
        idx &= mask;
        // LOG_DBG2 << "3 " << idx;
        idx |= char_to_num( seq[pos] );
        // LOG_DBG2 << "4 " << idx;

        // LOG_DBG2 << seq[pos] << " ("<< char_to_num( seq[pos] ) << ")" << " --> " << idx;
        ++result[ idx ];
    }

    return result;
}

std::vector<size_t> kmer_counts( Sequence const& seq, size_t k, std::string const& alphabet )
{
    // Normalize alphabet.
    auto const w = normalize_codes( alphabet );

    // Size check.
    if( w.size() == 0 ) {
        throw std::invalid_argument( "Invalid alphabet for kmer list." );
    }
    if( k == 0 ) {
        throw std::invalid_argument( "Invalid k==0 for kmer list." );
    }
    if( ! utils::is_valid_int_pow( w.size(), k ) ) {
        throw std::invalid_argument( "Cannot store kmers for such large k." );
    }

    // Get the number of entries in the kmer list.
    size_t const p = utils::int_pow( w.size(), k );

    // Result vector. Count the occurance of each possible kmer.
    auto result = std::vector<size_t>( p, 0 );

    // If the sequence is not long enough and does not contain even one kmer, we are done already.
    if( seq.size() < k ) {
        return result;
    }

    // Build lookup from sequence chars to index. Use w.size() as invalid char indicator.
    auto lookup = utils::CharLookup<size_t>( w.size() );
    for( size_t i = 0; i < w.size(); ++i ) {
        lookup.set_char_upper_lower( w[i], i );
    }

    // Store the index of the count vector for the current kmer,
    // and the number of valid processed chars of the sequence.
    size_t index  = 0;
    size_t valids = 0;

    // Process the sequence.
    for( size_t pos = 0; pos < seq.size(); ++pos ) {
        auto const cur = lookup[ seq[pos] ];

        // Check if the char is valid in the alphabet, and build up the index.
        if( cur < w.size() ) {
            index *= w.size();
            index %= p;
            index += cur;
            ++valids;

            // Only if we already have seen enough valid chars for one k-mer length (or more),
            // store the kmer.
            if( valids >= k ) {
                assert( index < p );
                ++result[ index ];
            }
        }
    }

    return result;
}

std::vector<std::string> kmer_list( size_t k, std::string const& alphabet )
{
    // Normalize alphabet.
    auto const w = normalize_codes( alphabet );

    // Size check.
    if( w.size() == 0 ) {
        throw std::invalid_argument( "Invalid alphabet for kmer list." );
    }
    if( k == 0 ) {
        throw std::invalid_argument( "Invalid k==0 for kmer list." );
    }
    if( ! utils::is_valid_int_pow( w.size(), k ) ) {
        throw std::invalid_argument( "Cannot store kmers for such large k." );
    }

    // Get the number of entries in the kmer list.
    size_t const p = utils::int_pow( w.size(), k );

    // Result vector. List all kmers.
    auto result = std::vector<std::string>( p );
    for( size_t i = 0; i < p; ++i ) {

        // Start with a dummy kmer of the correct size and the current index.
        auto kmer = std::string( k, '#' );
        assert( kmer.size() == k );
        size_t c = i;

        // Fill the kmer from right to left, using conversion of c to base w.size().
        for( size_t j = 0; j < k; ++j ) {
            assert(( 1 + j <= k ) && ( k - 1 - j < kmer.size() ));

            kmer[ k - 1 - j ] = w[ c % w.size() ];
            c /= w.size();
        }

        // In the last loop iteration, we processed all bits of c, so there should be nothing left.
        assert( c == 0 );

        result[ i ] = kmer;
    }

    return result;
}

} // namespace sequence
} // namespace genesis
