/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @brief Implementation of the Placement Functions.
 *
 * @file
 * @ingroup placement
 */

#include "genesis/placement/function/masses.hpp"

#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/function/operators.hpp"
#include "genesis/tree/default/distances.hpp"
#include "genesis/tree/default/functions.hpp"
#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/core/std.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <exception>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


#ifdef GENESIS_OPENMP
#   include <omp.h>
#endif

namespace genesis {
namespace placement {

// =================================================================================================
//     Mulitplicities
// =================================================================================================

double total_multiplicity( Pquery const& pqry )
{
    double mult = 0.0;
    for( auto const& name : pqry.names() ) {
        mult += name.multiplicity;
    }
    return mult;
}

double total_multiplicity( Sample const& sample )
{
    double mult = 0.0;
    for( auto const& pqry : sample.pqueries() ) {
        mult += total_multiplicity( pqry );
    }
    return mult;
}

// =================================================================================================
//     Masses
// =================================================================================================

std::vector<double> placement_mass_per_edges_with_multiplicities( Sample const& sample )
{
    auto result = std::vector<double>( sample.tree().edge_count(), 0.0 );

    for( auto const& pqry : sample.pqueries() ) {
        auto const mult = total_multiplicity( pqry );
        for( auto const& place : pqry.placements() ) {
            result[ place.edge().index() ] += place.like_weight_ratio * mult;
        }
    }

    return result;
}

utils::Matrix<double> placement_mass_per_edges_with_multiplicities( SampleSet const& sample_set )
{
    auto result = utils::Matrix<double>();

    // Edge case.
    if( sample_set.size() == 0 ) {
        return result;
    }

    // Init matrix.
    auto const set_size = sample_set.size();
    result = utils::Matrix<double>( set_size, sample_set[ 0 ].sample.tree().edge_count(), 0.0 );

    // Return completely empty matrix in edge cases.
    if( result.rows() == 0 || result.cols() == 0 ) {
        return result;
    }

    // Fill matrix.
    #pragma omp parallel for schedule(dynamic)
    for( size_t i = 0; i < set_size; ++i ) {
        auto const& smp = sample_set[ i ].sample;

        if( smp.tree().edge_count() != result.cols() ) {
            throw std::runtime_error(
                "Cannot calculate placement weights per edge matrix "
                "for Samples with Trees of different size."
            );
        }

        for( auto const& pqry : smp.pqueries() ) {
            auto const mult = total_multiplicity( pqry );
            for( auto const& place : pqry.placements() ) {
                result( i, place.edge().index() ) += place.like_weight_ratio * mult;
            }
        }
    }

    return result;
}

double total_placement_mass_with_multiplicities(  Sample const& smp )
{
    double sum = 0.0;
    for( const auto& pqry : smp.pqueries() ) {
        double mult = 0.0;
        for( auto const& name : pqry.names() ) {
            mult += name.multiplicity;
        }

        double lwr_sum = 0.0;
        for( auto const& place : pqry.placements() ) {
            lwr_sum += place.like_weight_ratio;
        }
        sum += lwr_sum * mult;
    }
    return sum;
}

// =================================================================================================
//     Masses without Multiplicities
// =================================================================================================

std::vector<double> placement_mass_per_edge_without_multiplicities( Sample const& sample )
{
    auto result = std::vector<double>( sample.tree().edge_count(), 0.0 );

    for( auto const& pqry : sample.pqueries() ) {
        for( auto const& place : pqry.placements() ) {
            result[ place.edge().index() ] += place.like_weight_ratio;
        }
    }

    return result;
}

utils::Matrix<double> placement_mass_per_edge_without_multiplicities( SampleSet const& sample_set )
{
    auto result = utils::Matrix<double>();

    // Edge case.
    if( sample_set.size() == 0 ) {
        return result;
    }

    // Init matrix.
    auto const set_size = sample_set.size();
    result = utils::Matrix<double>( set_size, sample_set[ 0 ].sample.tree().edge_count(), 0.0 );

    // Return completely empty matrix in edge cases.
    if( result.rows() == 0 || result.cols() == 0 ) {
        return result;
    }

    // Fill matrix.
    #pragma omp parallel for schedule(dynamic)
    for( size_t i = 0; i < set_size; ++i ) {
        auto const& smp = sample_set[ i ].sample;

        if( smp.tree().edge_count() != result.cols() ) {
            throw std::runtime_error(
                "Cannot calculate placement weights per edge matrix "
                "for Samples with Trees of different size."
            );
        }

        for( auto const& pqry : smp.pqueries() ) {
            for( auto const& place : pqry.placements() ) {
                result( i, place.edge().index() ) += place.like_weight_ratio;
            }
        }
    }

    return result;
}

double total_placement_mass_without_multiplicities(  Sample const& smp )
{
    double sum = 0.0;
    for( const auto& pqry : smp.pqueries() ) {
        for( auto& place : pqry.placements() ) {
            sum += place.like_weight_ratio;
        }
    }
    return sum;
}

} // namespace placement
} // namespace genesis
