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
//     Color Set Initialization
// =================================================================================================

// -------------------------------------------------------------------------
//     add_secondary_colors_with_binary_reduction
// -------------------------------------------------------------------------

void add_secondary_colors_with_binary_reduction(
    KmerColorSet& cset
) {
    // In the absence of a phylogeny or taxonomy to group our primary colors by,
    // we use a binary reduction, essentially creating a complete binary tree
    // through recursive pairwise grouping of the elements. We currently only allow
    // to call it if no other secondary colors have been added to the set yet.
    if( cset.get_color_list().size() != 1 + cset.get_element_count() ) {
        throw std::invalid_argument(
            "Cannot initialize Kmer Color Set with binary reduction "
            "if other colors have already been added"
        );
    }

    // Define a recursive lambda that takes the first index and one-past-the-last index
    // for the current block of colors that we want to merge and create secondary colors from.
    size_t leftover_index = 0;
    std::function<void(size_t, size_t)> recursive_merge_ = [&]( size_t first, size_t last )
    {
        // Base case: no merge needed if no more elements are left.
        assert( first <= last );
        assert( last == cset.get_color_list().size() );
        if( last - first == 0 ) {
            return;
        }

        // Record the starting index of the new block that will be appended,
        // and process elements pairwise in the current block [first, last).
        size_t new_start = cset.get_color_list().size();
        for( size_t i = first; i < last; i += 2 ) {

            // Default case: two groups to merge into a new color.
            if( i + 1 < last ) {
                cset.add_merged_color( i, i+1 );
                continue;
            }

            // Edge case: If there is an odd number of elements in the current group,
            // we note down the last element as a leftover, as it cannot be merged,
            // and we do not want to add duplicates. It will be merged later.
            // Alternatively, if there is already a leftover from a previous uneven group,
            // we merge this into our current last (uneven) element, to even the odds.
            // At any time, there can be at most one leftover element, and it will
            // always be merged once another uneven sized group is found.
            // This corresponds to building a binary tree where the last branch is always
            // attached at the earliest possibility.
            assert( i + 1 == last );
            if( leftover_index == 0 ) {
                leftover_index = i;
            } else {
                cset.add_merged_color( i, leftover_index );
                leftover_index = 0;
            }
        }

        // Recursively merge the newly created block.
        recursive_merge_( new_start, cset.get_color_list().size() );
    };

    // Start the recursive merging using the entire initial range of primary colors.
    // That is, we start after the empty color, and stop at the color index that corresponds
    // to  the last primary (single bit set) color. This is an ugly hard-coded dependency
    // on the inner workings of the class... but well, good enough for now.
    recursive_merge_( 1, cset.get_color_list().size() );

    // At the end, there should be no leftover color; all should be in some secondary color.
    // Due to the exit condition of the recursive function, the leftover is set to the last
    // element that has all colors set (which is hence not really a leftover).
    assert( leftover_index == cset.get_color_list().size() - 1 );

    // A binary tree has one fewer inner nodes than tips. We have added those as colors,
    // and have the empty color, so our total is now double the number of primary colors.
    // Also, the last secondary color should contain _all_ primary colors.
    assert( cset.get_color_list().size() == 2 * cset.get_element_count() );
    assert( all_set( cset.get_color_list().back().elements ));
}

// -------------------------------------------------------------------------
//     add_secondary_colors_from_bitvectors
// -------------------------------------------------------------------------

void add_secondary_colors_from_bitvectors(
    KmerColorSet& cset,
    std::vector<utils::Bitvector> const& bitvecs,
    bool test_for_all_set_color
) {
    // We need at least one fully set bitvector, to guarantee that our imaginary colors
    // work as expected, i.e, such that they can always find at least that one as a match.
    bool found_all_set = false;

    // Add all bitvectors as secondary colors
    for( auto const& bv : bitvecs ) {
        // Validity checks
        if( bv.size() != cset.get_element_count() ) {
            throw std::invalid_argument(
                "Cannot initialize Kmer Color Set with Bitvectors of size "
                "that does not match the element count"
            );
        }
        if( utils::pop_count( bv ) < 2 ) {
            throw std::invalid_argument(
                "Cannot initialize Kmer Color Set with Bitvectors representing "
                "the empty color or primary colors (i.e., zero or single bit set)"
            );
        }
        if( test_for_all_set_color && utils::all_set( bv )) {
            found_all_set = true;
        }

        // Add a new color for the bitvector, which also checks that it is not a duplicate.
        auto cp = bv;
        cset.add_color( std::move( cp ));
    }

    // Final check for the all-set color, such that our minimal superset always succeeds.
    if( test_for_all_set_color && ! found_all_set ) {
        throw std::invalid_argument(
            "Cannot initialize Kmer Color Set with Bitvectors "
            "that do not contain an all-set bitvector"
        );
    }
}

// -------------------------------------------------------------------------
//     add_secondary_colors_from_groups
// -------------------------------------------------------------------------

void add_secondary_colors_from_groups(
    KmerColorSet& cset,
    std::vector<std::vector<size_t>> const& groups,
    bool test_for_all_set_color
) {
    // Quick and dirty: create bitvectors from the indices, then forward to the bitvector function.
    // We are currently not expecting more than a few hundred or thousand initial colors, so this
    // should be fine for initialization. If extended to more colors, might need optimization.
    std::vector<utils::Bitvector> bitvecs;
    for( auto const& group_indices : groups ) {
        bitvecs.emplace_back( utils::Bitvector( cset.get_element_count(), group_indices ));
    }
    add_secondary_colors_from_bitvectors( cset, bitvecs, test_for_all_set_color );
}

// =================================================================================================
//     Color Set Functions
// =================================================================================================

size_t count_unique_lookup_keys( KmerColorSet const& cset )
{
    // Loop over the entire multimap, but "jump" over groups of the same key.
    auto const& lookup = cset.get_color_lookup();
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
    assert( cset.get_color_list().size() == cset.get_color_lookup().size() );
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
    assert( cset.get_color_list().size() == cset.get_color_lookup().size() );

    std::stringstream ss;
    ss << "Colors: " << cset.get_color_list().size() << "\n";
    for( size_t i = 0; i < cset.get_color_list().size(); ++i ) {
        auto const& color = cset.get_color_at(i);
        ss << std::setw( int_width ) << i;
        ss << " " << utils::to_bit_string( color.elements );
        ss << "\n";
    }
    return ss.str();
}

std::string print_kmer_color_lookup( KmerColorSet const& cset )
{
    assert( cset.get_color_list().size() == cset.get_color_lookup().size() );

    std::stringstream ss;
    ss << "Colors: " << cset.get_color_list().size();
    ss << ", unique keys: " << count_unique_lookup_keys( cset );
    ss << "\n";

    // Outer loop: iterate over the hashes while "jumping" over groups of duplicate keys.
    auto const& lookup = cset.get_color_lookup();
    for( auto it = lookup.begin(); it != lookup.end(); ) {
        auto range = lookup.equal_range(it->first);

        ss << "0x" << std::setw(2 * sizeof(size_t)) << std::setfill('0') << std::hex << it->first;
        ss << " --> { ";

        // Inner loop: All values that have that hash key.
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

std::string print_kmer_color_gamut( KmerColorSet const& cset, size_t int_width )
{
    auto const gamut = cset.get_gamut();
    std::stringstream ss;

    // Write header line with the element indices
    ss << std::string( int_width, ' ' );
    for( size_t c = 0; c < gamut.cols(); ++c ) {
        ss << " " << std::setw( int_width ) << c;
    }
    ss << "\n";

    // Write the content of the gamut, with an extra first colum for the color indices.
    for( size_t r = 0; r < gamut.rows(); ++r ) {
        ss << std::setw( int_width ) << r;
        for( size_t c = 0; c < gamut.cols(); ++c ) {
            ss << " " << std::setw( int_width ) << gamut( r, c );
        }
        ss << "\n";
    }

    return ss.str();
}

std::string print_kmer_color_set_summary( KmerColorSet const& cset )
{
    assert( cset.get_color_list().size() == cset.get_color_lookup().size() );
    auto const& stats = cset.get_gamut_statistics();

    // Count all colors
    std::stringstream ss;
    ss << "Elements:    " << cset.get_element_count() << "\n";
    ss << "Colors:      " << cset.get_color_list().size() << "\n";
    ss << "Max colors:  " << cset.get_max_color_count() << "\n";
    ss << "Unique keys: " << count_unique_lookup_keys( cset ) << "\n";
    ss << "Gamut size:  " << cset.get_gamut().rows() << "x" << cset.get_gamut().cols() << "\n";
    ss << "Gamut real:  " << stats.real_color_count << "\n";
    ss << "Gamut imag:  " << stats.imag_color_count << "\n";
    return ss.str();
}

} // namespace sequence
} // namespace genesis
