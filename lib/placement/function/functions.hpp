#ifndef GENESIS_PLACEMENT_FUNCTION_FUNCTIONS_H_
#define GENESIS_PLACEMENT_FUNCTION_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @brief Provides functions for working with Placements and Pqueries.
 *
 * @file
 * @ingroup placement
 */

#include "placement/sample.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace genesis {
namespace placement {

// =================================================================================================
//     Pquery Names
// =================================================================================================

bool has_name( Pquery const& pquery, std::string const& name );
bool has_name( Sample const& smp, std::string const& name );

Pquery const* find_pquery( Sample const& smp, std::string const& name );
Pquery*       find_pquery( Sample&       smp, std::string const& name );

std::unordered_set<std::string> all_pquery_names( Sample const& sample );

// =================================================================================================
//     Normalization and Sorting
// =================================================================================================

void normalize_weight_ratios( Pquery& pquery );
void normalize_weight_ratios( Sample& smp );

// void sort_placements_by_proximal_length( PlacementTreeEdge& edge );
// void sort_placements_by_proximal_length( Sample& smp );

void sort_placements_by_weight( Pquery& pquery );
void sort_placements_by_weight( Sample& smp );

// =================================================================================================
//     Filtering
// =================================================================================================

void filter_min_accumulated_weight( Pquery& pquery, double threshold = 0.99 );
void filter_min_accumulated_weight( Sample& smp,    double threshold = 0.99 );

void filter_n_max_weight_placements( Pquery& pquery, size_t n = 1 );
void filter_n_max_weight_placements( Sample& smp,    size_t n = 1 );

void filter_min_weight_threshold( Pquery& pquery, double threshold = 0.01 );
void filter_min_weight_threshold( Sample& smp,    double threshold = 0.01 );

void filter_pqueries_keeping_names(  Sample& smp, std::unordered_set<std::string> keep_list );
void filter_pqueries_removing_names( Sample& smp, std::unordered_set<std::string> remove_list );

void filter_pqueries_intersecting_names( Sample& sample_1, Sample& sample_2 );
void filter_pqueries_differing_names(    Sample& sample_1, Sample& sample_2 );

// =================================================================================================
//     Joining and Merging
// =================================================================================================

void copy_pqueries( Sample const& source, Sample& target );

void merge_duplicates( Sample& smp );

void collect_duplicate_pqueries( Sample& smp );

void merge_duplicate_placements( Pquery& pquery );
void merge_duplicate_placements( Sample& smp );

void merge_duplicate_names( Pquery& pquery );
void merge_duplicate_names( Sample& smp );

// =================================================================================================
//     Placement Mass
// =================================================================================================

double total_multiplicity( Pquery const& pqry );

size_t total_placement_count( Sample const& smp );
double total_placement_mass(  Sample const& smp );
double total_placement_mass_with_multiplicities(  Sample const& smp );

std::pair<PlacementTreeEdge const*, size_t> placement_count_max_edge( Sample const& smp );
std::pair<PlacementTreeEdge const*, double> placement_mass_max_edge( Sample const& smp );

// =================================================================================================
//     Histograms
// =================================================================================================

std::vector<double> closest_leaf_weight_distribution( Sample const& sample );

std::vector<int> closest_leaf_depth_histogram( Sample const& smp );

std::vector<int> closest_leaf_distance_histogram (
    Sample const& smp, const double min, const double max, const int bins = 10
);

std::vector<int> closest_leaf_distance_histogram_auto (
    Sample const& smp, double& min, double& max, const int bins = 10
);

} // namespace placement
} // namespace genesis

#endif // include guard
