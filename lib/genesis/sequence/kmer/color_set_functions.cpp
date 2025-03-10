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

#include "genesis/sequence/kmer/color_set_functions.hpp"
#include "genesis/utils/math/bitvector.hpp"

#include <cassert>
#include <iomanip>
#include <ios>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Color Set Functions
// =================================================================================================

size_t count_unique_lookup_keys( KmerColorSet const& cset )
{
    // Loop over the entire multimap, but "jump" over groups of the same key.
    auto const& lookup = cset.get_lookup_map();
    size_t cnt = 0;
    for (auto it = lookup.begin(); it != lookup.end(); ) {
        ++cnt;
        it = lookup.equal_range( it->first ).second;
    }
    return cnt;
}

void verify_unique_colors( KmerColorSet const& cset )
{
    // We copy to a set for this. Using a pairwise comparison between all colors of the same hash
    // would be far more efficient, but this function is mostly for debugging only anyway.
    assert( cset.get_color_list().size() == cset.get_lookup_map().size() );
    std::unordered_set<utils::Bitvector> elements_bvs;
    for( auto const& color : cset.get_color_list() ) {
        elements_bvs.insert( color.elements );
    }
    if( cset.get_color_list().size() != elements_bvs.size() ) {
        throw std::runtime_error( "Kmer Color Set contains duplicate colors" );
    }
}

// =================================================================================================
//     Printing
// =================================================================================================

std::string print_kmer_color_list( KmerColorSet const& cset, size_t int_width )
{
    assert( cset.get_color_list().size() == cset.get_lookup_map().size() );

    std::stringstream ss;
    ss << "Colors: " << cset.get_color_list().size();
    ss << ", primary: " << cset.get_primary_color_count();
    ss << ", max real: " << cset.get_max_real_color_count();
    ss << "\n";
    for( size_t i = 0; i < cset.get_color_list().size(); ++i ) {
        auto const& color = cset.get_color_list()[i];
        ss << std::setw( int_width ) << i;
        ss << " " << utils::to_bit_string( color.elements );
        ss << "\n";
    }
    return ss.str();
}

std::string print_kmer_lookup_map( KmerColorSet const& cset )
{
    assert( cset.get_color_list().size() == cset.get_lookup_map().size() );

    std::stringstream ss;
    ss << "Colors: " << cset.get_color_list().size();
    ss << ", unique keys: " << count_unique_lookup_keys( cset );
    ss << "\n";

    // Outer loop: iterate over the hashes while "jumping" over groups of duplicate keys.
    auto const& lookup = cset.get_lookup_map();
    for( auto it = lookup.begin(); it != lookup.end(); ) {
        auto range = lookup.equal_range(it->first);

        ss << "0x" << std::setw(2 * sizeof(size_t)) << std::setfill('0') << std::hex << it->first;
        ss << " --> { ";
        bool first = true;
        for( auto inner = range.first; inner != range.second; ++inner ) {
            if( ! first ) {
                ss << ", ";
            }
            first = false;
            ss << std::dec << inner->second;
        }
        ss << " }\n";
        it = range.second;
    }
    return ss.str();
}

std::string print_kmer_color_set_summary( KmerColorSet const& cset )
{
    assert( cset.get_color_list().size() == cset.get_lookup_map().size() );

    // Count all colors
    std::stringstream ss;
    ss << "Colors:      " << cset.get_color_list().size() << "\n";
    ss << "Empty:       " << color_counts[0] << "\n";
    ss << "Primary:     " << color_counts[1] << "\n";
    ss << "Secondary:   " << color_counts[2] << "\n";
    ss << "Imaginary:   " << color_counts[3] << "\n";
    ss << "Max real:    " << cset.get_max_real_color_count() << "\n";
    ss << "Unique keys: " << count_unique_lookup_keys( cset ) << "\n";
    return ss.str();
}

std::string print_kmer_color_set_lookup_statistics( KmerColorSet const& cset )
{
    std::stringstream ss;
    auto const& stats = cset.get_lookup_statistics();
    ss << "existing_is_empty        = " << stats.existing_is_empty << "\n";
    ss << "existing_contains_target = " << stats.existing_contains_target << "\n";
    ss << "target_color_exists      = " << stats.target_color_exists << "\n";
    ss << "real_color_added         = " << stats.real_color_added << "\n";
    ss << "imaginary_color_added    = " << stats.imaginary_color_added << "\n";
    return ss.str();
}

} // namespace sequence
} // namespace genesis
