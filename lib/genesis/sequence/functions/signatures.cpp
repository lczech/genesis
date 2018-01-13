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

#include "genesis/sequence/functions/signatures.hpp"

#include "genesis/sequence/functions/codes.hpp"
#include "genesis/sequence/functions/signature_specifications.hpp"
#include "genesis/sequence/functions/stats.hpp"
#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/math/statistics.hpp"
#include "genesis/utils/tools/char_lookup.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Signature Counts and Frequencies
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
        auto const cur = settings.char_index( sequence[pos] );

        // Check if the char is valid in the alphabet
        if( cur == settings.InvalidCharIndex ) {
            switch( settings.unknown_char_behavior() ) {
                case SignatureSpecifications::UnknownCharBehavior::kSkip:
                    continue;
                case SignatureSpecifications::UnknownCharBehavior::kThrow:
                    throw std::runtime_error(
                        "Unknown Sequence char for kmer counting: '" +
                        std::string( 1, sequence[pos] ) + "'"
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
    Sequence const& sequence,
    SignatureSpecifications const& settings
) {
    // Get kmer counts. We need to do a full accumulation instead of using the sequence length,
    // because unknown chars might have been skipped while counting.
    auto const kmers = signature_counts( sequence, settings );
    auto const sum = static_cast<double>( std::accumulate( kmers.begin(), kmers.end(), 0 ));

    // Caluclate the frequencies.
    auto freqs = std::vector<double>( kmers.size() );
    for( size_t i = 0; i < kmers.size(); ++i ) {
        freqs[i] = static_cast<double>( kmers[i] ) / sum;
    }
    return freqs;
}

// =================================================================================================
//     Signature Symmetrized
// =================================================================================================

/**
 * @brief Local helper function that adds up the values for reverse complement k-mers.
 */
template<class T>
std::vector<T> signature_symmetrized_helper(
    std::vector<T> const& kmers,
    SignatureSpecifications const& settings
) {
    auto const& map = settings.kmer_combined_reverse_complement_map();
    assert( kmers.size() == map.size() );

    auto const size  = settings.kmer_reverse_complement_list_size();
    auto result = std::vector<T>( size, {} );

    for( size_t i = 0; i < kmers.size(); ++i ) {
        assert( map[i] < result.size() );
        result[ map[i] ] += kmers[i];
    }
    return result;
}

std::vector<size_t> signature_symmetrized_counts(
    Sequence const& sequence,
    SignatureSpecifications const& settings
) {
    return signature_symmetrized_helper(
        signature_counts( sequence, settings ),
        settings
    );
}

std::vector<double> signature_symmetrized_frequencies(
    Sequence const& sequence,
    SignatureSpecifications const& settings
) {
    return signature_symmetrized_helper(
        signature_frequencies( sequence, settings ),
        settings
    );
}

// =================================================================================================
//     Signature Ranks
// =================================================================================================

std::vector<size_t> signature_ranks(
    Sequence const& sequence,
    SignatureSpecifications const& settings
) {
    // We use frequencies, because the ranking expects a vec of double,
    // and not a vec of size_t as returned by the counts signature...
    return utils::ranking_standard( signature_frequencies( sequence, settings ) );
}

std::vector<size_t> signature_symmetrized_ranks(
    Sequence const& sequence,
    SignatureSpecifications const& settings
) {
    // We use frequencies, because the ranking expects a vec of double,
    // and not a vec of size_t as returned by the counts signature...
    return utils::ranking_standard( signature_symmetrized_frequencies( sequence, settings ) );
}

// =================================================================================================
//     Signature Min Max Reverse Complements
// =================================================================================================

/**
 * @brief Local helper function that returns a vector where the frequencies of the non-palindromic
 * kmers are combined using a functor.
 */
template<class Combinator>
std::vector<double> signature_complementarity_frequencies_helper(
    Sequence const& sequence,
    SignatureSpecifications const& settings,
    Combinator combinator
) {
    // Get indices first, so that the function fails early if we do not have nucleic acids.
    // Also, get size of the resulting list, and init the list.
    auto const& indices = settings.kmer_reverse_complement_indices();
    // auto const  size = settings.kmer_reverse_complement_list_size( false );
    // auto result = std::vector<double>( size, init );
    auto result = std::vector<double>();

    // Calculate freqs.
    auto const& freqs = signature_frequencies( sequence, settings );
    assert( freqs.size() == indices.size() );
    for( size_t i = 0; i < freqs.size(); ++i ) {

        // Skip palindromes.
        if( indices[i] == i ) {
            continue;
        }

        // Skip the second kmer. We do the comparison for the first one already.
        if( indices[i] < i ) {
            continue;
        }

        // Find the min or max of the two complementary kmers and store it.
        auto const combined = combinator( freqs[i], freqs[ indices[i] ] );
        result.push_back( combined );
    }
    return result;
}

std::vector<double> signature_minimal_complementarity_frequencies(
    Sequence const& sequence,
    SignatureSpecifications const& settings
) {
    return signature_complementarity_frequencies_helper(
        sequence,
        settings,
        []( double ff, double fc ){
            return std::min( ff, fc );
        }
    );
}

std::vector<double> signature_maximal_complementarity_frequencies(
    Sequence const& sequence,
    SignatureSpecifications const& settings
) {
    return signature_complementarity_frequencies_helper(
        sequence,
        settings,
        []( double ff, double fc ){
            return std::max( ff, fc );
        }
    );
}

std::vector<double> signature_reverse_identity_frequencies(
    Sequence const& sequence,
    SignatureSpecifications const& settings
){
    // Get indices first, so that the function fails early if we do not have nucleic acids.
    auto const& indices = settings.kmer_reverse_complement_indices();
    auto result = std::vector<double>();

    // Calculate freqs.
    auto const& freqs = signature_frequencies( sequence, settings );
    assert( freqs.size() == indices.size() );
    for( size_t i = 0; i < freqs.size(); ++i ) {

        // Only use palindromes, i.e., the ones where the reverse complement is the identity.
        if( indices[i] != i ) {
            continue;
        }
        result.push_back( freqs[i] );
    }
    return result;
}

// =================================================================================================
//     Signature Misc
// =================================================================================================

std::vector<double> signature_frequency_ratios_1(
    Sequence const& sequence,
    SignatureSpecifications const& settings
) {
    return signature_complementarity_frequencies_helper(
        sequence,
        settings,
        []( double ff, double fc ){
            assert( ff >= 0.0 && fc >= 0.0 );
            if( ff == 0.0 && fc == 0.0 ) {
                return 1.0;
            } else if( ff == 0.0 || fc == 0.0 ) {
                return 0.0;
            } else {
                return std::min( ff / fc, fc / ff );
            }
        }
    );
}

std::vector<double> signature_frequency_ratios_2(
    Sequence const& sequence,
    SignatureSpecifications const& settings
) {
    return signature_complementarity_frequencies_helper(
        sequence,
        settings,
        []( double ff, double fc ){
            assert( ff >= 0.0 && fc >= 0.0 );
            if( ff == 0.0 && fc == 0.0 ) {
                return 0.5;
            } else {
                return std::min( ff, fc ) / ( ff + fc );
            }
        }
    );
}

std::vector<double> signature_jensen_shannon(
    Sequence const& sequence,
    SignatureSpecifications const& settings
) {
    return signature_complementarity_frequencies_helper(
        sequence,
        settings,
        []( double ff, double fc ){
            assert( ff >= 0.0 && fc >= 0.0 );
            auto const s1 = ff * log( ( 2 * ff ) / ( ff + fc ) );
            auto const s2 = fc * log( ( 2 * fc ) / ( ff + fc ) );
            return s1 + s2;
        }
    );
}

// =================================================================================================
//     Kmer Strings
// =================================================================================================

/**
 * @brief Local helper function that writes one kmer string to a stream.
 */
void kmer_string_single_kmer(
    Sequence const&                sequence,
    SignatureSpecifications const& settings,
    size_t                         start,
    std::ostream&                  out
) {
    for( size_t i = start; i < start + settings.k(); ++i ) {

        // Check if the char is valid in the alphabet
        if( settings.char_index( sequence[ i ] ) == settings.InvalidCharIndex ) {
            switch( settings.unknown_char_behavior() ) {
                case SignatureSpecifications::UnknownCharBehavior::kSkip:
                    continue;
                case SignatureSpecifications::UnknownCharBehavior::kThrow:
                    throw std::runtime_error(
                        "Unknown Sequence char for kmer string: '" +
                        std::string( 1, sequence[ i ] ) + "'"
                    );
                default:
                    assert( false );
            }
        }

        out << sequence[ i ];
    }
}

/**
 * @brief Local helper function that writes an overlapping kmer string to a stream.
 */
void kmer_string_overlapping_line(
    Sequence const&                sequence,
    SignatureSpecifications const& settings,
    std::ostream&                  out
) {
    auto const k = settings.k();

    // Not even one k-mer: Do nothing.
    // We need this check, otherwise the subtraction of sizes later on gives an overflow.
    if( sequence.size() < k ) {
        return;
    }

    for( size_t i = 0; i < sequence.size() - k + 1; ++i ) {
        if( i > 0 ) {
            out << " ";
        }
        kmer_string_single_kmer( sequence, settings, i, out );
    }
}

/**
 * @brief Local helper function that does one line of a non overlapping kmer string.
 */
void kmer_strings_non_overlapping_line(
    Sequence const&                sequence,
    SignatureSpecifications const& settings,
    std::ostream&                  out,
    size_t                         offset
) {
    auto const k = settings.k();
    for( size_t i = offset; i + k - 1 < sequence.size(); i += k ) {
        if( i > offset ) {
            out << " ";
        }
        kmer_string_single_kmer( sequence, settings, i, out );
    }
}

std::string kmer_string_overlapping(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
) {
    std::ostringstream out;
    kmer_string_overlapping_line( sequence, settings, out );
    return out.str();
}

void kmer_string_overlapping(
    Sequence const&                sequence,
    SignatureSpecifications const& settings,
    std::ostream&                  out
) {
    if( sequence.size() < settings.k() ) {
        return;
    }

    kmer_string_overlapping_line( sequence, settings, out );
    out << "\n";
}

std::vector<std::string> kmer_strings_non_overlapping(
    Sequence const&                sequence,
    SignatureSpecifications const& settings
) {
    std::vector<std::string> result;
    for( size_t offset = 0; offset < settings.k(); ++offset ) {

        // Stop if we do not have enough sequence length left for this offset.
        // This only occurs if the sequence is short.
        if( offset + settings.k() > sequence.size() ) {
            break;
        }

        std::ostringstream out;
        kmer_strings_non_overlapping_line( sequence, settings, out, offset );
        result.push_back( out.str() );
    }
    return result;
}

void kmer_strings_non_overlapping(
    Sequence const&                sequence,
    SignatureSpecifications const& settings,
    std::ostream&                  out
) {
    for( size_t offset = 0; offset < settings.k(); ++offset ) {

        // Stop if we do not have enough sequence length left for this offset.
        // This only occurs if the sequence is short.
        if( offset + settings.k() > sequence.size() ) {
            break;
        }

        kmer_strings_non_overlapping_line( sequence, settings, out, offset );
        out << "\n";
    }
}

} // namespace sequence
} // namespace genesis
