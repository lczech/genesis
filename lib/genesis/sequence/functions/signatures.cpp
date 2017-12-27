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
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/tools/char_lookup.hpp"

#include <algorithm>
#include <numeric>
#include <unordered_map>

namespace genesis {
namespace sequence {

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
    auto const w = normalize_code_alphabet( alphabet );

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
    auto const w = normalize_code_alphabet( alphabet );

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

// =================================================================================================
//     Signatures
// =================================================================================================

std::vector<size_t> kmer_reverse_complement_indices( size_t k )
{
    // Calculate a vector that maps from a kmer index according to kmer_list
    // to a position in [ 0 , s ), so that rev comp indices map to the same position.
    auto indices = std::vector<size_t>( utils::int_pow( 4, k ), 0 );

    // Store a list of kmers and their indices. it only stores one direction - that is,
    // if we see that the rev comp of a kmer is already in this map, we do not add anything.
    std::unordered_map<std::string, size_t> done;

    // Get all kmers as strings.
    auto const list = kmer_list( k, "ACGT" );
    assert( indices.size() == list.size() );

    // This uses string maps and is terribly slow.
    // But I am really lazy today and the function will not be called often.
    for( size_t i = 0; i < list.size(); ++i ) {
        auto const& seq = list[i];

        // We always encounter the alphabetically first entry fist.
        // So, we cannot have added its complement before.
        assert( done.count( seq ) == 0 );

        // If we have seen the rev comp of the current kmer before, reuse its index.
        // If not, make a new index, which is one higher than the currently highest index
        // (by using done.size() to get the number of current entries).
        auto const rev = reverse_complement( seq );
        if( done.count( rev ) == 0 ) {
            auto const ni = done.size();
            indices[ i ] = ni;
            done[ seq ]  = ni;
        } else {
            // We have seen the rev comp before, so we do not update the done list.
            indices[ i ] = done[ rev ];
        }
    }

    return indices;
}

size_t kmer_reverse_complement_size( size_t k )
{
    // Calcualtions according to: https://stackoverflow.com/a/40953130

    // Number of palindromic k-mers. For odd k, there are none, for even k, there are 4^(k/2) = 2^k
    auto const p = ( k % 2 == 1 ? 0 : utils::int_pow( 2, k ));

    // Number of entries needed to store rev comp kmers.
    return p + ( utils::int_pow( 4, k ) - p ) / 2;
}

std::vector<double> signature_frequencies( Sequence const& seq, size_t k )
{
    // Get kmer counts.
    auto const kmers = kmer_counts( seq, k );
    auto const sum = static_cast<double>( std::accumulate( kmers.begin(), kmers.end(), 0 ));

    // Caluclate their frequencies.
    auto freqs = std::vector<double>( kmers.size() );
    for( size_t i = 0; i < kmers.size(); ++i ) {
        freqs[i] = static_cast<double>( kmers[i] ) / sum;
    }
    return freqs;
}

} // namespace sequence
} // namespace genesis
