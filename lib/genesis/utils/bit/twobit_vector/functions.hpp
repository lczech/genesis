#ifndef GENESIS_UTILS_BIT_TWOBIT_VECTOR_FUNCTIONS_H_
#define GENESIS_UTILS_BIT_TWOBIT_VECTOR_FUNCTIONS_H_

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

#include "genesis/utils/bit/twobit_vector.hpp"
#include "genesis/utils/math/common.hpp"

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Strings
// =================================================================================================

TwobitVector::ValueType translate_from_nucleic_acid( char site );
char                    translate_to_nucleic_acid( TwobitVector::ValueType value );

TwobitVector from_nucleic_acids( std::string const& sequence );
std::string to_nucleic_acids( TwobitVector const& vec );

std::string bitstring( TwobitVector const& vec );
std::string bitstring( TwobitVector::WordType const& vec );

// =================================================================================================
//     K-mer counts
// =================================================================================================

/**
 * @brief Count the occurrences of k-mers in a TwobitVector.
 *
 * The function produces a vector of counts, in lexicograpical ordering of k-mers of the given size.
 * The optional template parameter `T` can be used to change the int type used for counting.
 */
template<typename T = size_t>
std::vector<T> kmer_occurrences( TwobitVector const& vec, size_t k )
{
    // Boundary check, just in case to spot accidental misuse. This would result in a humonguous
    // table anyway, so, unlikely that anyway will come even close to this any time soon.
    if( k > 32 ) {
        throw std::invalid_argument(
            "Cannot execute kmer_occurrences() with k > 32"
        );
    }
    if( !is_valid_int_pow( 4, k )) {
        throw std::invalid_argument(
            "Cannot execute kmer_occurrences() with k == " + std::to_string( k ) +
            " due to numerical overflow."
        );
    }

    // Set up the resulting vector, with entries for each possible k-mer.
    auto const res_size = int_pow( 4, k );
    auto res = std::vector<T>( res_size, T{} );

    // Iterate all k-mers of the given size with the vector, and accumulate their counts.
    TwobitVector::WordType kmer = 0;
    TwobitVector::WordType const mask = TwobitVector::ones_mask(k);
    for( size_t i = 0; i < vec.size(); ++i ) {
        // Shift the current value and remove first base.
        // Then, add the next value at the now empty right-most position.
        kmer <<= 2;
        kmer &= mask;
        kmer |= static_cast<TwobitVector::WordType>( vec.get( i ));

        // Once we have filled up a complete kmer, we start accumulating counts.
        // That is, in the first k-1 iterations, we have not completed a k-mer, so no counting yet.
        if( i + 1 >= k ) {
            assert( kmer < res_size );
            ++res[kmer];
        }
    }
    return res;
}

} // namespace utils
} // namespace genesis

#endif // include guard
