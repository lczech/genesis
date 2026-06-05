#ifndef GENESIS_UTIL_CONTAINER_BITPACKED_PAIR_VECTOR_IO_H_
#define GENESIS_UTIL_CONTAINER_BITPACKED_PAIR_VECTOR_IO_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2026 Lucas Czech

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
 * @brief Binary file I/O helpers for BitpackedPairVector.
 *
 * Provides write() and read() free functions that serialise and deserialise a
 * BitpackedPairVector to/from a binary FILE stream. The element count (size) is not
 * written; callers must store and supply it separately (e.g. via a file footer).
 *
 * On-disk layout (all values native-endian uint64):
 * @code
 *   [width_a — 1 × uint64]
 *   [width_b — 1 × uint64]
 *   [raw storage words — ceil(size * (width_a + width_b) / 64) × uint64]
 * @endcode
 *
 * @file
 * @ingroup util
 */

#include "genesis/util/container/bitpacked_pair_vector.hpp"

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace util {
namespace container {

// =================================================================================================
//     BitpackedPairVector I/O
// =================================================================================================

/**
 * @brief Write a BitpackedPairVector to a binary FILE stream.
 *
 * Writes two uint64 header words (width_a, width_b), then the raw storage words.
 * The element count is not written; store it externally (e.g. in a file footer).
 * Throws std::runtime_error on any I/O error.
 */
template<typename U, typename A, typename B>
std::size_t write( BitpackedPairVector<U, A, B> const& bpv, FILE* fp )
{
    auto checked_fwrite = [&]( void const* ptr, std::size_t bytes )
    {
        if( std::fwrite( ptr, 1, bytes, fp ) != bytes ) {
            throw std::runtime_error(
                "BitpackedPairVector write error: " + std::string( std::strerror( errno ))
            );
        }
    };

    // Write field-width header words
    std::uint64_t const wa = static_cast<std::uint64_t>( bpv.first_bit_width() );
    std::uint64_t const wb = static_cast<std::uint64_t>( bpv.second_bit_width() );
    checked_fwrite( &wa, sizeof( wa ));
    checked_fwrite( &wb, sizeof( wb ));

    // Write the raw packed storage words
    auto const& data = bpv.storage().data();
    if( !data.empty() ) {
        checked_fwrite( data.data(), data.size() * sizeof( U ));
    }

    return 2 * sizeof( std::uint64_t ) + data.size() * sizeof( U );
}

/**
 * @brief Read a BitpackedPairVector from a binary FILE stream.
 *
 * Reads two uint64 header words (width_a, width_b), then the raw storage words.
 * The caller must supply @p num_elements (the number of logical entries to reconstruct).
 * Throws std::runtime_error on any I/O error or if the data is inconsistent.
 */
template<typename U, typename A, typename B>
BitpackedPairVector<U, A, B> read( FILE* fp, std::size_t num_elements )
{
    auto checked_fread = [&]( void* ptr, std::size_t bytes )
    {
        if( std::fread( ptr, 1, bytes, fp ) != bytes ) {
            throw std::runtime_error(
                "BitpackedPairVector read error: " + std::string( std::strerror( errno ))
            );
        }
    };

    // Read field-width header words
    std::uint64_t wa = 0, wb = 0;
    checked_fread( &wa, sizeof( wa ));
    checked_fread( &wb, sizeof( wb ));
    std::size_t const width_a = static_cast<std::size_t>( wa );
    std::size_t const width_b = static_cast<std::size_t>( wb );

    // Compute and read the expected number of raw storage words
    static constexpr std::size_t STORAGE_BITS = std::numeric_limits<U>::digits;
    std::size_t const total_width = width_a + width_b;
    std::size_t const num_words = ( total_width * num_elements + STORAGE_BITS - 1 ) / STORAGE_BITS;
    std::vector<U> raw( num_words );
    if( num_words > 0 ) {
        checked_fread( raw.data(), num_words * sizeof( U ));
    }

    // Construct via the raw-storage constructor (validates all sizes and widths)
    return BitpackedPairVector<U, A, B>( num_elements, width_a, width_b, std::move( raw ));
}

} // namespace container
} // namespace util
} // namespace genesis

#endif // GENESIS_UTIL_CONTAINER_BITPACKED_PAIR_VECTOR_IO_H_
