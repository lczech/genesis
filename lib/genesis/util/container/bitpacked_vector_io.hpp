#ifndef GENESIS_UTIL_CONTAINER_BITPACKED_VECTOR_IO_H_
#define GENESIS_UTIL_CONTAINER_BITPACKED_VECTOR_IO_H_

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
 * @brief Binary file I/O helpers for BitpackedVector.
 *
 * Provides write_bitpacked_vector() and read_bitpacked_vector() free functions
 * that serialise and deserialise a BitpackedVector to/from a binary FILE stream.
 * The element count (size) is not written; callers must store and supply it separately.
 *
 * On-disk layout (all values native-endian uint64):
 * @code
 *   [bit_width — 1 × uint64]
 *   [raw storage words — ceil(size * bit_width / 64) × uint64]
 * @endcode
 *
 * @file
 * @ingroup util
 */

#include "genesis/util/container/bitpacked_vector.hpp"

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
//     BitpackedVector I/O
// =================================================================================================

/**
 * @brief Write a BitpackedVector to a binary FILE stream.
 *
 * Writes a single uint64 header (bit_width), then the raw storage words.
 * The element count is not written; store it externally (e.g. in a file footer).
 * Throws std::runtime_error on any I/O error.
 */
template<typename U, typename T>
std::size_t write_bitpacked_vector( BitpackedVector<U, T> const& bv, FILE* fp )
{
    auto checked_fwrite = [&]( void const* ptr, std::size_t bytes ) {
        if( std::fwrite( ptr, 1, bytes, fp ) != bytes ) {
            throw std::runtime_error(
                "BitpackedVector write error: " + std::string( std::strerror( errno ))
            );
        }
    };

    // Write element count, then bit width as uint64 header words
    std::uint64_t const sz = static_cast<std::uint64_t>( bv.size() );
    std::uint64_t const bw = static_cast<std::uint64_t>( bv.bit_width() );
    checked_fwrite( &sz, sizeof( sz ));
    checked_fwrite( &bw, sizeof( bw ));

    // Write the raw packed storage words
    auto const& data = bv.data();
    if( !data.empty() ) {
        checked_fwrite( data.data(), data.size() * sizeof( U ));
    }

    return 2 * sizeof( std::uint64_t ) + data.size() * sizeof( U );
}

/**
 * @brief Read a BitpackedVector from a binary FILE stream.
 *
 * Reads the uint64 header (bit_width) then the raw storage words.
 * The element count is read from the stream (written by write_bitpacked_vector()).
 * Throws std::runtime_error on any I/O error or if the data is inconsistent.
 */
template<typename U, typename T>
BitpackedVector<U, T> read_bitpacked_vector( FILE* fp )
{
    auto checked_fread = [&]( void* ptr, std::size_t bytes ) {
        if( std::fread( ptr, 1, bytes, fp ) != bytes ) {
            throw std::runtime_error(
                "BitpackedVector read error: " + std::string( std::strerror( errno ))
            );
        }
    };

    // Read element count
    std::uint64_t sz = 0;
    checked_fread( &sz, sizeof( sz ));
    std::size_t const num_elements = static_cast<std::size_t>( sz );

    // Read the bit width header word
    std::uint64_t bit_width_raw = 0;
    checked_fread( &bit_width_raw, sizeof( bit_width_raw ));
    std::size_t const bit_width = static_cast<std::size_t>( bit_width_raw );

    // Compute and read the expected number of raw storage words
    static constexpr std::size_t STORAGE_BITS = std::numeric_limits<U>::digits;
    std::size_t const num_words = ( bit_width * num_elements + STORAGE_BITS - 1 ) / STORAGE_BITS;
    std::vector<U> raw( num_words );
    if( num_words > 0 ) {
        checked_fread( raw.data(), num_words * sizeof( U ));
    }

    // Construct via the raw-storage constructor (validates size)
    return BitpackedVector<U, T>( num_elements, bit_width, std::move( raw ));
}

} // namespace container
} // namespace util
} // namespace genesis

#endif // GENESIS_UTIL_CONTAINER_BITPACKED_VECTOR_IO_H_
