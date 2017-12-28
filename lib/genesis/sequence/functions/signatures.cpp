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
#include "genesis/sequence/functions/signature_specifications.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/math/common.hpp"
#include "genesis/utils/tools/char_lookup.hpp"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <stdexcept>
#include <unordered_map>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Signatures
// =================================================================================================

std::vector<size_t> signature_counts(
    Sequence const& sequence,
    SignatureSpecifications const& settings
) {
    // Get alphabet.
    auto const& w = settings.alphabet();
    auto const ws = w.size();

    // Get the number of entries in the kmer list.
    size_t const p = settings.kmer_list_size();

    // Result vector. Count the occurance of each possible kmer.
    auto result = std::vector<size_t>( p, 0 );

    // If the sequence is not long enough and does not contain even one kmer, we are done already.
    if( sequence.size() < settings.k() ) {
        return result;
    }

    // Store the index of the count vector for the current kmer,
    // and the number of valid processed chars of the sequence.
    size_t index  = 0;
    size_t valids = 0;

    // Process the sequence.
    for( size_t pos = 0; pos < sequence.size(); ++pos ) {
        auto const cur = settings.index_of( sequence[pos] );

        // Check if the char is valid in the alphabet
        if( cur == settings.InvalidCharIndex ) {
            switch( settings.unknown_char_behavior() ) {
                case SignatureSpecifications::UnknownCharBehavior::kSkip:
                    continue;
                case SignatureSpecifications::UnknownCharBehavior::kThrow:
                    throw std::runtime_error(
                        "Unknown Sequence char for kmer counting: " + std::string( 1, cur )
                    );
                default:
                    assert( false );
            }
        }

        // Build up the index.
        index *= ws;
        index %= p;
        index += cur;
        ++valids;

        // Only if we already have seen enough valid chars for one k-mer length (or more),
        // store the kmer.
        if( valids >= settings.k() ) {
            assert( index < result.size() );
            ++result[ index ];
        }
    }

    return result;
}

std::vector<double> signature_frequencies(
    Sequence const& seq,
    SignatureSpecifications const& settings
) {
    // Get kmer counts. We need to do a full accumulation instead of using the sequence length,
    // because unknown chars might have been skipped while counting.
    auto const kmers = signature_counts( seq, settings );
    auto const sum = static_cast<double>( std::accumulate( kmers.begin(), kmers.end(), 0 ));

    // Caluclate the frequencies.
    auto freqs = std::vector<double>( kmers.size() );
    for( size_t i = 0; i < kmers.size(); ++i ) {
        freqs[i] = static_cast<double>( kmers[i] ) / sum;
    }
    return freqs;
}

} // namespace sequence
} // namespace genesis
