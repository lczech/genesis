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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "genesis/sequence/kmer/alphabet.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace sequence {

// ================================================================================================
//     Nucleotide Alphabet
// ================================================================================================

// Need out-of-line definition in C++11... Just to make the compiler happy.

std::string const KmerAlphabet::ALPHABET = "ACGT";

// std::array<uint8_t, 256> constexpr KmerAlphabet::ascii_to_nt4;

} // namespace sequence
} // namespace genesis
