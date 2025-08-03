#pragma once

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2023-2025 Giannis Reppas and Lucas Czech

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

#include <genesis/util/container/bitpacked_vector.hpp>

#include <cstdint>
#include <type_traits>

namespace genesis {
namespace util {
namespace container {

// Explicit instantiations for all possible int sizes.

template class BitpackedVector<uint8_t>;
template class BitpackedVector<uint16_t>;
template class BitpackedVector<uint32_t>;
template class BitpackedVector<uint64_t>;

// During the Binder generation, LLVM unfortunately replaces the uint types with some
// simplified forms. To not run into any surporses, we assert that those are what we expect.
// The last type (uint64_t) is the default for the template parameter, and just completely
// left out by LLVM for some reason, so we cannot check it here.

static_assert(
    std::is_same_v<unsigned char, uint8_t>,
    "Mismatching int types in BitpackedVector instantiation"
);
static_assert(
    std::is_same_v<unsigned short, uint16_t>,
    "Mismatching int types in BitpackedVector instantiation"
);
static_assert(
    std::is_same_v<unsigned int, uint32_t>,
    "Mismatching int types in BitpackedVector instantiation"
);
static_assert(
    std::is_same_v<unsigned long, uint64_t>,
    "Mismatching int types in BitpackedVector instantiation"
);

} // namespace container
} // namespace util
} // namespace genesis
