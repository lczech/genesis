/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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
 * @ingroup tree
 */

#include "genesis/tree/mass_tree/phylo_factor_colors.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Helper Functions
// =================================================================================================

std::vector<size_t> phylo_factor_edge_indices(
    std::vector<PhyloFactor> const& factors,
    size_t max_factor
) {
    // Find as many as we need.
    max_factor = std::min( max_factor, factors.size() );
    std::vector<size_t> result;
    for( size_t i = 0; i < max_factor; ++i ) {
        result.push_back( factors[i].edge_index );
    }
    return result;
}

// =================================================================================================
//     Single Factor Coloring
// =================================================================================================

std::vector<utils::Color> phylo_factor_single_factor_colors(
    Tree const& tree,
    std::vector<PhyloFactor> const& factors,
    size_t factor_index,
    PhyloFactorSingleColors colors
) {
    if( factor_index >= factors.size() ) {
        throw std::invalid_argument( "Invalid index for a phylo factor." );
    }
    auto const& factor = factors[ factor_index ];

    // Prepare all edges in neutral color.
    auto edge_cols = std::vector<utils::Color>( tree.edge_count(), colors.neutral_edges );

    // Helper to set the color of one edge to a value, unless it already has a value,
    // in which case we have an error.
    auto set_color_ = [&]( size_t index, utils::Color color ){
        if( index >= edge_cols.size() ) {
            throw std::runtime_error( "Invalid edge index in a phylo factor." );
        }
        if( edge_cols[ index ] == colors.neutral_edges ) {
            edge_cols[ index ] = color;
        } else {
            throw std::runtime_error(
                "Edge at index " + std::to_string( index ) + " is in multiple edge sets of the "
                "phylo factor."
            );
        }
    };

    // Set the edges of the factor and its subtrees.
    set_color_( factor.edge_index, colors.factor_edge );
    for( auto const e : factor.edge_indices_primary ) {
        set_color_( e, colors.primary_edges );
    }
    for( auto const e : factor.edge_indices_secondary ) {
        set_color_( e, colors.secondary_edges );
    }

    // Get all previous factor edges and colorize them.
    auto const prev_factors = phylo_factor_edge_indices( factors, factor_index );
    for( auto const e : prev_factors ) {
        set_color_( e, colors.previous_factors );
    }

    return edge_cols;
}

// =================================================================================================
//     Factor Clade Coloring
// =================================================================================================

std::vector<utils::Color> phylo_factor_clade_colors(
    Tree const& tree,
    std::vector<PhyloFactor> const& factors,
    size_t num_factors,
    PhyloFactorCladeColors colors
) {
    // Input checks.
    if( num_factors > factors.size() ) {
        throw std::invalid_argument( "Invalid number of factors requested." );
    }
    if( num_factors == 0 ) {
        num_factors = factors.size();
    }
    if( num_factors > colors.clade_colors.size() ) {
        throw std::invalid_argument(
            "Not enough clade colors provided for the requested number of factors."
        );
    }

    // Prepare all edges in base color.
    auto edge_cols = std::vector<utils::Color>( tree.edge_count(), colors.base_edges );

    // Helper to set the color of one edge to a value.
    auto set_color_ = [&]( size_t index, utils::Color color ){
        if( index >= edge_cols.size() ) {
            throw std::runtime_error( "Invalid edge index in a phylo factor." );
        }
        edge_cols[ index ] = color;
    };

    // Color each factor in a color of the clade color set.
    for( size_t i = 0; i < num_factors; ++i ) {
        assert( i < factors.size() );
        assert( i < colors.clade_colors.size() );

        auto const& factor = factors[i];
        auto const& clade_col = colors.clade_colors[i];

        set_color_( factor.edge_index, colors.factor_edges );
        for( auto const e : factor.edge_indices_secondary ) {
            set_color_( e, clade_col );
        }
    }

    return edge_cols;
}

} // namespace tree
} // namespace genesis
