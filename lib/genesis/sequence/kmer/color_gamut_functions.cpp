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

#include "genesis/sequence/kmer/color_gamut_functions.hpp"
#include "genesis/taxonomy/functions/kmer.hpp"
#include "genesis/taxonomy/functions/taxonomy.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/containers/matrix/operators.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/io/deserializer.hpp"
#include "genesis/utils/io/serializer.hpp"
#include "genesis/utils/math/bitvector.hpp"

// The KmerColorGamut class is only available from C++17 onwards.
#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <ios>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Color Gamut Initialization
// =================================================================================================

// -------------------------------------------------------------------------
//     add_secondary_colors_with_binary_reduction
// -------------------------------------------------------------------------

void add_secondary_colors_with_binary_reduction(
    KmerColorGamut& gamut
) {
    // In the absence of a phylogeny or taxonomy to group our primary colors by,
    // we use a binary reduction, essentially creating a complete binary tree
    // through recursive pairwise grouping of the elements. We currently only allow
    // to call it if no other secondary colors have been added to the set yet.
    if( gamut.get_color_list().size() != 1 + gamut.get_element_count() ) {
        throw std::invalid_argument(
            "Cannot initialize Kmer Color Gamut with binary reduction "
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
        assert( last == gamut.get_color_list().size() );
        if( last - first == 0 ) {
            return;
        }

        // Record the starting index of the new block that will be appended,
        // and process elements pairwise in the current block [first, last).
        size_t new_start = gamut.get_color_list().size();
        for( size_t i = first; i < last; i += 2 ) {

            // Default case: two groups to merge into a new color.
            if( i + 1 < last ) {
                gamut.add_merged_color( i, i+1 );
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
                gamut.add_merged_color( i, leftover_index );
                leftover_index = 0;
            }
        }

        // Recursively merge the newly created block.
        recursive_merge_( new_start, gamut.get_color_list().size() );
    };

    // Start the recursive merging using the entire initial range of primary colors.
    // That is, we start after the empty color, and stop at the color index that corresponds
    // to  the last primary (single bit set) color. This is an ugly hard-coded dependency
    // on the inner workings of the class... but well, good enough for now.
    recursive_merge_( 1, gamut.get_color_list().size() );

    // At the end, there should be no leftover color; all should be in some secondary color.
    // Due to the exit condition of the recursive function, the leftover is set to the last
    // element that has all colors set (which is hence not really a leftover).
    assert( leftover_index == gamut.get_color_list().size() - 1 );

    // A binary tree has one fewer inner nodes than tips. We have added those as colors,
    // and have the empty color, so our total is now double the number of primary colors.
    // Also, the last secondary color should contain _all_ primary colors.
    assert( gamut.get_color_list().size() == 2 * gamut.get_element_count() );
    assert( all_set( gamut.get_color_list().back().elements ));
}

// -------------------------------------------------------------------------
//     add_secondary_colors_from_bitvectors
// -------------------------------------------------------------------------

void add_secondary_colors_from_bitvectors(
    KmerColorGamut& gamut,
    std::vector<utils::Bitvector> const& bitvecs,
    bool test_for_all_set_color
) {
    // We need at least one fully set bitvector, to guarantee that our imaginary colors
    // work as expected, i.e, such that they can always find at least that one as a match.
    bool found_all_set = false;

    // Add all bitvectors as secondary colors
    for( auto const& bv : bitvecs ) {
        // Validity checks
        if( bv.size() != gamut.get_element_count() ) {
            throw std::invalid_argument(
                "Cannot initialize Kmer Color Gamut with Bitvectors of size "
                "that does not match the element count"
            );
        }
        if( utils::pop_count( bv ) < 2 ) {
            throw std::invalid_argument(
                "Cannot initialize Kmer Color Gamut with Bitvectors representing "
                "the empty color or primary colors (i.e., zero or single bit set)"
            );
        }
        if( test_for_all_set_color && utils::all_set( bv )) {
            found_all_set = true;
        }

        // Add a new color for the bitvector, which also checks that it is not a duplicate.
        auto cp = bv;
        gamut.add_color( std::move( cp ));
    }

    // Final check for the all-set color, such that our minimal superset always succeeds.
    if( test_for_all_set_color && ! found_all_set ) {
        throw std::invalid_argument(
            "Cannot initialize Kmer Color Gamut with Bitvectors "
            "that do not contain an all-set bitvector"
        );
    }
}

// -------------------------------------------------------------------------
//     add_secondary_colors_from_groups
// -------------------------------------------------------------------------

void add_secondary_colors_from_groups(
    KmerColorGamut& gamut,
    std::vector<std::vector<size_t>> const& groups,
    bool test_for_all_set_color
) {
    // Quick and dirty: create bitvectors from the indices, then forward to the bitvector function.
    // We are currently not expecting more than a few hundred or thousand initial colors, so this
    // should be fine for initialization. If extended to more colors, might need optimization.
    std::vector<utils::Bitvector> bitvecs;
    for( auto const& group_indices : groups ) {
        bitvecs.emplace_back( utils::Bitvector( gamut.get_element_count(), group_indices ));
    }
    add_secondary_colors_from_bitvectors( gamut, bitvecs, test_for_all_set_color );
}

// -------------------------------------------------------------------------
//     make_secondary_colors_from_taxonomy
// -------------------------------------------------------------------------

std::vector<utils::Bitvector> make_secondary_colors_from_taxonomy(
    taxonomy::Taxonomy const& tax,
    size_t power_set_limit,
    bool omit_primary_colors
) {
    using namespace utils;
    using namespace taxonomy;

    // Cautionary check of computational limits within this function. If this function ever
    // gets called with a power set limit of even close to 64, it would explode the memory
    // anyway... but we leave that to the user, and only check the technical limits here.
    if( power_set_limit == 0 || power_set_limit > 63 ) {
        throw std::invalid_argument( "Invalid power_set_limit" );
    }

    // Prepare the result and get the number of bits per bitvector, i.e., the number of groups.
    // We also need to keep track of the group indices below each taxon, for later accumulation.
    // We do that in a map from the taxon to the list of _all_ group indices below it.
    std::vector<utils::Bitvector> colors;
    auto const num_groups = count_taxon_groups( tax );
    if( num_groups == 0 ) {
        return colors;
    }

    // Recurse the taxonomy. At each stage, we collect all group indices, and build bitvectors.
    std::function<void(Taxonomy const&, std::vector<size_t>&)> recursion_ = [&](
        Taxonomy const& taxon,
        std::vector<size_t>& group_indices
    ) {
        // Collect all grouped children of the current taxon. For extended taxa, we collect all
        // group indices below them, and treat them as a single group for the power set.
        std::vector<std::vector<size_t>> child_indices;
        for( auto const& child : taxon ) {
            auto const& data = child.data<KmerTaxonData>();
            switch( data.group_status ) {
                case KmerTaxonData::GroupStatus::kAssigned: {
                    if( data.group_index > num_groups ) {
                        throw std::invalid_argument(
                            "Invalid KmerTaxonData::GroupStatus, invalid group index > num groups"
                        );
                    }

                    // For taxa that have been assigned to a group, collect their indices.
                    // As multiple taxa can be assigned to the same group (if the combined
                    // sizes are still within the limits of TaxonGroupingLimits), we need
                    // to count unique group indices here. Could be made more efficient,
                    // but good enough for now.
                    bool found_group_index = false;
                    for( auto const& ci : child_indices ) {
                        assert( !ci.empty() );
                        if( ci.size() == 1 && ci[0] == data.group_index ) {
                            found_group_index = true;
                        }
                    }
                    if( ! found_group_index ) {
                        child_indices.emplace_back( std::vector<size_t>( 1, data.group_index ));

                        // This is the first time that we are processing this particular group,
                        // and it is a singleton, i.e., a taxon without further children.
                        // We hence might want to add it as a primary color.
                        if( ! omit_primary_colors ) {
                            auto group_elements = Bitvector( num_groups );
                            group_elements.set( data.group_index );
                            colors.push_back( std::move( group_elements ));
                        }
                    }
                    break;
                }
                case KmerTaxonData::GroupStatus::kExpanded: {
                    // For taxa that have been expanded (because they are too big), we recurse,
                    // which processes all groups below them, and then collect these groups.
                    child_indices.emplace_back();
                    recursion_( child, child_indices.back() );
                    break;
                }
                case KmerTaxonData::GroupStatus::kUnprocessed:
                default: {
                    throw std::invalid_argument(
                        "Invalid KmerTaxonData::GroupStatus, Taxonomy not properly processed"
                    );
                }
            }
        }

        // Sort by the first element. Probably not really needed, but might
        // improve visual order of the taxonomy is not sorted by groups already.
        std::sort(
            child_indices.begin(), child_indices.end(),
            []( std::vector<size_t> const& lhs, std::vector<size_t> const& rhs ){
                if( lhs.empty() || rhs.empty() ) {
                    throw std::runtime_error( "Empty taxon child group" );
                }
                return lhs[0] < rhs[0];
            }
        );

        // Now we have a list of all child group indices of the current taxon.
        // We can build their power set (or just flag all-set bitvector, if too big).
        if( child_indices.empty() ) {
            auto const* taxon_ptr = dynamic_cast<Taxon const*>( &taxon );
            if( taxon_ptr ) {
                throw std::runtime_error( "Empty group at taxon " + taxon_ptr->name() );
            } else {
                throw std::runtime_error( "Empty group at root" );
            }
        } else if( child_indices.size() <= power_set_limit ) {
            // We add the power set of all immediate child groups of this taxon as colors.
            // For this, we set up a counter (mask) that we go through, and add a color for each
            // value, activating the elements in the color corresponding to all group children.
            auto const max_mask = static_cast<size_t>( 1 ) << child_indices.size();
            for( size_t mask = 0; mask < max_mask; ++mask ) {
                auto group_elements = Bitvector( num_groups );

                // Check each bit of the mask, and if it is set, that means that in this iteration,
                // we set all elements in the bitvector corresponding to all indices of this group.
                size_t activated_groups = 0;
                for( size_t a = 0; a < child_indices.size(); ++a ) {
                    assert( ! child_indices[a].empty() );
                    if(( mask & (static_cast<size_t>( 1 ) << a )) == 0 ) {
                        continue;
                    }
                    ++activated_groups;

                    // We have a set bit in the mask, meaning that this group of inidices needs
                    // to be added for this color in the power set.
                    for( auto cigi : child_indices[a] ) {
                        assert( cigi < group_elements.size() );
                        assert( group_elements.get( cigi ) == false );
                        group_elements.set( cigi );
                    }
                }

                // Only add the color if it only contains more than one group.
                if( activated_groups > 1 ) {
                    colors.push_back( std::move( group_elements ));
                }
            }
        } else {
            // Here we have a taxon which has more than the limit many immediate child groups.
            // We do not want to build a power set for all of them (that would give too many colors),
            // and instead just include it as a single all-set bitvector of the groups.
            auto group_elements = Bitvector( num_groups );
            for( auto const& ci : child_indices ) {
                assert( ! ci.empty() );
                for( auto cigi : ci ) {
                    assert( cigi < group_elements.size() );
                    assert( group_elements.get( cigi ) == false );
                    group_elements.set( cigi );
                }
            }
            colors.push_back( std::move( group_elements ));
        }

        // Lastly, we also need to add all child indices that we found to our returned
        // group index list, so that the higher-up call of the recursion can use those.
        for( auto const& ci : child_indices ) {
            assert( ! ci.empty() );
            for( auto cigi : ci ) {
                assert( cigi < num_groups );
                group_indices.push_back( cigi );
            }
        }

        // We now sort the group indices, mainly just so that our downstream bitvectors
        // are also sorted. Also, we check that all indices are unique, which they need
        // to be, as we excluded duplicate sibling indices. If we find duplicates here still,
        // that means that duplicate group indices are distributed across non-silbing taxa.
        std::sort( group_indices.begin(), group_indices.end() );
        if( contains_duplicates( group_indices )) {
            throw std::runtime_error( "Duplicate group indices that are not siblings" );
        }
    };
    std::vector<size_t> group_indices;
    recursion_( tax, group_indices );
    assert( group_indices.size()  == num_groups );
    assert( group_indices.front() == 0 );
    assert( group_indices.back()  == num_groups - 1 );
    assert( std::is_sorted( group_indices.begin(), group_indices.end() ));
    assert( ! contains_duplicates( group_indices ));

    // Lastly, we need to check that the all-set color is part of the color set, in order for
    // the kmer color gamut to work properly. This is the case if there is one single highest
    // taxon such as "root" that contains all others. But if the taxonomy starts at, say, the
    // domain level, then we'd have three top level taxa, but none that contained all others.
    if( ! colors.empty() && ! all_set( colors.back() )) {
        colors.push_back( Bitvector( num_groups, true ));
    }

    return colors;
}

// =================================================================================================
//     Color Gamut Functions
// =================================================================================================

size_t count_unique_lookup_keys( KmerColorGamut const& gamut )
{
    // Loop over the entire multimap, but "jump" over groups of the same key.
    auto const& lookup = gamut.get_color_lookup();
    size_t cnt = 0;
    for (auto it = lookup.begin(); it != lookup.end(); ) {
        ++cnt;
        it = lookup.equal_range( it->first ).second;
    }
    return cnt;
}

void verify_unique_colors( KmerColorGamut const& gamut )
{
    // We copy to a set for this. Using a pairwise comparison between all colors of the same hash
    // would be far more efficient, but this function is mostly for debugging only anyway.
    assert( gamut.get_color_list().size() == gamut.get_color_lookup().size() );
    std::unordered_set<utils::Bitvector> elements_bvs;
    for( auto const& color : gamut.get_color_list() ) {
        elements_bvs.insert( color.elements );
    }
    if( gamut.get_color_list().size() != elements_bvs.size() ) {
        throw std::runtime_error( "Kmer Color Gamut contains duplicate colors" );
    }
}

// =================================================================================================
//     Printing
// =================================================================================================

std::string print_kmer_color_list( KmerColorGamut const& gamut )
{
    assert( gamut.get_color_list().size() == gamut.get_color_lookup().size() );

    size_t const int_width = std::ceil( std::log10( gamut.get_color_list().size() ));
    std::stringstream ss;
    ss << "Colors: " << gamut.get_color_list().size() << "\n";
    for( size_t i = 0; i < gamut.get_color_list().size(); ++i ) {
        auto const& color = gamut.get_color_at(i);
        ss << std::setw( int_width ) << i;
        ss << " " << utils::to_bit_string( color.elements );
        ss << "\n";
    }
    return ss.str();
}

std::string print_kmer_color_lookup( KmerColorGamut const& gamut )
{
    assert( gamut.get_color_list().size() == gamut.get_color_lookup().size() );

    std::stringstream ss;
    ss << "Colors: " << gamut.get_color_list().size();
    ss << ", unique keys: " << count_unique_lookup_keys( gamut );
    ss << "\n";

    // Outer loop: iterate over the hashes while "jumping" over groups of duplicate keys.
    auto const& lookup = gamut.get_color_lookup();
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

std::string print_kmer_color_gamut( KmerColorGamut const& gamut )
{
    auto const gamut_matrix = gamut.get_gamut_matrix();
    if( gamut_matrix.empty() ) {
        return "";
    }

    // Proper alignment for nicer output
    auto const max_gamut = *std::max_element( gamut_matrix.begin(), gamut_matrix.end() );
    size_t const first_width = std::ceil( std::log10( gamut_matrix.rows() ));
    size_t const col_width = std::ceil( std::log10( std::max( max_gamut, gamut_matrix.cols() )));

    // Write header line with the element indices
    std::stringstream ss;
    ss << std::string( first_width, ' ' );
    for( size_t c = 0; c < gamut_matrix.cols(); ++c ) {
        ss << " " << std::setw( col_width ) << c;
    }
    ss << "\n";

    // Write the content of the gamut, with an extra first column for the color indices.
    for( size_t r = 0; r < gamut_matrix.rows(); ++r ) {
        ss << std::setw( first_width ) << r;
        for( size_t c = 0; c < gamut_matrix.cols(); ++c ) {
            ss << " " << std::setw( col_width ) << gamut_matrix( r, c );
        }
        ss << "\n";
    }

    return ss.str();
}

std::string print_kmer_color_gamut_summary( KmerColorGamut const& gamut )
{
    assert( gamut.get_color_list().size() == gamut.get_color_lookup().size() );

    // Report how many of the gamut are real and how many are imaginar numbers
    auto const& gamut_matrix = gamut.get_gamut_matrix();
    auto const& stats = gamut.get_gamut_statistics();
    auto const gamut_size = gamut_matrix.rows() * gamut_matrix.cols();
    auto const gamut_empty = gamut_size - ( stats.real_color_count + stats.imag_color_count);
    auto const real_per = gamut_size == 0 ? 0.0 : 100.0 * stats.real_color_count / gamut_size;
    auto const imag_per = gamut_size == 0 ? 0.0 : 100.0 * stats.imag_color_count / gamut_size;
    auto const empt_per = gamut_size == 0 ? 0.0 : 100.0 * gamut_empty / gamut_size;

    // Proper alignment for nicer output
    size_t const gamut_width = (
        gamut_matrix.empty()
        ? 1
        : std::ceil( std::log10( std::max({
            stats.real_color_count, stats.imag_color_count, gamut_empty
        })))
    );
    size_t const percent_width = (
        gamut_matrix.empty()
        ? 3
        : std::ceil( std::log10( std::max({
            std::ceil( real_per ), std::ceil( imag_per ), std::ceil( empt_per  )
        }))) + 2
    );

    // Count all colors
    std::stringstream ss;
    ss << "Elements:    " << gamut.get_element_count() << "\n";
    ss << "Colors:      " << gamut.get_color_list().size() << "\n";
    ss << "Max colors:  " << gamut.get_max_color_count() << "\n";
    ss << "Unique keys: " << count_unique_lookup_keys( gamut ) << "\n";
    ss << "Gamut size:  " << gamut_matrix.rows() << " x " << gamut_matrix.cols() << "\n";
    ss << std::fixed << std::setprecision(1);
    ss << "Gamut real:  " << std::setw(gamut_width) << stats.real_color_count;
    ss << " (" << std::setw(percent_width) << real_per << "%)\n";
    ss << "Gamut imag:  " << std::setw(gamut_width) << stats.imag_color_count;
    ss << " (" << std::setw(percent_width) << imag_per << "%)\n";
    ss << "Gamut empty: " << std::setw(gamut_width) << gamut_empty;
    ss << " (" << std::setw(percent_width) << empt_per << "%)\n";
    return ss.str();
}

// =================================================================================================
//     Serialization
// =================================================================================================

void serialize_kmer_color_gamut_colors(
    KmerColorGamut const& gamut,
    std::shared_ptr<utils::BaseOutputTarget> output_target
) {
    // We need to iterate through the color vector manually here,
    // as the Bitvectors are wrapped in a struct for future compatibility.
    auto ser = utils::Serializer( output_target );
    auto const& color_list = gamut.get_color_list();
    ser << color_list.size();
    for( auto const& color : color_list ) {
        ser << color.elements;
    }
}

void serialize_kmer_color_gamut_matrix(
    KmerColorGamut const& gamut,
    std::shared_ptr<utils::BaseOutputTarget> output_target
) {
    // Functionality for std::vector and Matrix is already
    // implemented in the respective classes and functions.
    auto ser = utils::Serializer( output_target );
    ser << gamut.get_gamut_matrix();
}

std::vector<utils::Bitvector> deserialize_kmer_color_gamut_colors(
    std::shared_ptr<utils::BaseInputSource> input_source
) {
    // Functionality for std::vector and Bitvector is already
    // implemented in the respective classes and functions.
    std::vector<utils::Bitvector> result;
    auto deser = utils::Deserializer( input_source );
    deser >> result;
    return result;
}

utils::Matrix<size_t> deserialize_kmer_color_gamut_matrix(
    std::shared_ptr<utils::BaseInputSource> input_source
) {
    // Functionality for std::vector and Matrix is already
    // implemented in the respective classes and functions.
    utils::Matrix<size_t> result;
    auto deser = utils::Deserializer( input_source );
    deser >> result;
    return result;
}

} // namespace sequence
} // namespace genesis

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17
