#ifndef GENESIS_SEQUENCE_KMER_COLOR_SET_FUNCTIONS_H_
#define GENESIS_SEQUENCE_KMER_COLOR_SET_FUNCTIONS_H_

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

#include "genesis/sequence/kmer/color_set.hpp"

#include <array>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Color Set Functions
// =================================================================================================

std::string kmer_color_type_to_string( KmerColorSet::Color const& color );
std::array<size_t, 4> count_kmer_color_set_color_types( KmerColorSet const& cset );

size_t count_unique_lookup_keys( KmerColorSet const& cset );
void verify_unique_colors( KmerColorSet const& cset );

std::string print_kmer_color_list( KmerColorSet const& cset, size_t int_width = 6 );
std::string print_kmer_lookup_map( KmerColorSet const& cset );
std::string print_kmer_color_set_summary( KmerColorSet const& cset );

} // namespace sequence
} // namespace genesis

#endif // include guard
