#ifndef GENESIS_PLACEMENT_FUNCTION_FUNCTIONS_H_
#define GENESIS_PLACEMENT_FUNCTION_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/placement/sample.hpp"

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

/**
 * @brief Return true iff the given Pquery contains a particular name.
 */
bool has_name( Pquery const& pquery, std::string const& name );

/**
 * @brief Return true iff the given Sample contains a Pquery with a particular name, i.e.,
 * a PqueryName whose name member equals the given name.
 */
bool has_name( Sample const& smp, std::string const& name );

/**
 * @brief Return the first Pquery that has a particular name, or nullptr of none has.
 */
Pquery const* find_pquery( Sample const& smp, std::string const& name );

/**
 * @brief Return the first Pquery that has a particular name, or nullptr of none has.
 */
Pquery*       find_pquery( Sample&       smp, std::string const& name );

/**
 * @brief Return a set of all unique PqueryName%s of the @link Pquery Pqueries @endlink
 * of the given sample.
 *
 * If a Pquery contains multiple names, all of them are added to the set.
 */
std::unordered_set<std::string> all_pquery_names( Sample const& sample );

// =================================================================================================
//     Normalization and Sorting
// =================================================================================================

/**
 * @brief Recalculate the `like_weight_ratio` of the PqueryPlacement&s of a Pquery,
 * so that their sum is 1.0, while maintaining their ratio to each other.
 */
void normalize_weight_ratios( Pquery& pquery );

/**
 * @brief Recalculate the `like_weight_ratio` of the PqueryPlacement&s of each Pquery in the Sample,
 * so that their sum is 1.0, while maintaining their ratio to each other.
 *
 * This function simply calls normalize_weight_ratios( Pquery& pquery ) for all Pqueries of
 * the Sample.
 */
void normalize_weight_ratios( Sample& smp );

// void sort_placements_by_proximal_length( PlacementTreeEdge& edge );
// void sort_placements_by_proximal_length( Sample& smp );

/**
 * @brief Sort the PqueryPlacement%s of a Pquery by their `like_weight_ratio`, in descending order
 * (most likely first).
 */
void sort_placements_by_weight( Pquery& pquery );

/**
 * @brief Sort the PqueryPlacement%s of all @link Pquery Pqueries @endlink by their
 * `like_weight_ratio`, in descending order (most likely first).
 */
void sort_placements_by_weight( Sample& smp );

/**
 * @brief Scale all branch lengths of the Tree and the position of the PqueryPlacement%s by
 * a given factor.
 *
 * This function calls tree::scale_all_branch_lengths() for scaling the tree, and also applies
 * the same scaling to the PqueryPlacement::proximal_length.
 */
void scale_all_branch_lengths( Sample& smp, double factor = 1.0 );

// =================================================================================================
//     Filtering
// =================================================================================================

/**
 * @brief Remove the PqueryPlacement%s with the lowest `like_weight_ratio`, while keeping the
 * accumulated weight (sum of all remaining `like_weight_ratio`s) above a given threshold.
 *
 * This is a cleaning function to get rid of unlikely placement positions, withouth sacrificing
 * too much detail of the overall distribution of weights. The EPA support a similar option, which
 * only writes enough of the most likely placement positions to the output to fullfil a threshold.
 */
void filter_min_accumulated_weight( Pquery& pquery, double threshold = 0.99 );

/**
 * @brief Remove the PqueryPlacement%s with the lowest `like_weight_ratio`, while keeping the
 * accumulated weight (sum of all remaining `like_weight_ratio`s) above a given threshold.
 *
 * This function calls filter_min_accumulated_weight( Pquery& pquery, double threshold ) for all
 * Pqueries of the Sample. See this version of the function for more information.
 */
void filter_min_accumulated_weight( Sample& smp,    double threshold = 0.99 );

/**
 * @brief Remove all PqueryPlacement%s but the `n` most likely ones from the Pquery.
 *
 * Pqueries can contain multiple placements on different branches. For example, the EPA algorithm
 * of RAxML outputs up to the 7 most likely positions for placements to the output Jplace file by
 * default. The property `like_weight_ratio` weights those placement positions so that the sum over
 * all positions (all branches of the tree) per pquery is 1.0.
 *
 * This function removes all but the `n` most likely placements (the ones which have the highest
 * `like_weight_ratio`) from the Pquery. The `like_weight_ratio` of the remaining placements is not
 * changed.
 */
void filter_n_max_weight_placements( Pquery& pquery, size_t n = 1 );

/**
 * @brief Remove all PqueryPlacement%s but the `n` most likely ones from all
 * @link Pquery Pqueries @endlink in the Sample.
 *
 * This function calls filter_n_max_weight_placements( Pquery& pquery, size_t n ) for all Pqueries
 * of the Sample. See this version of the function for more information.
 */
void filter_n_max_weight_placements( Sample& smp,    size_t n = 1 );

/**
 * @brief Remove all PqueryPlacement%s that have a `like_weight_ratio` below the given threshold.
 */
void filter_min_weight_threshold( Pquery& pquery, double threshold = 0.01 );

/**
 * @brief Remove all PqueryPlacement%s that have a `like_weight_ratio` below the given threshold
 * from all @link Pquery Pqueries @endlink of the Sample.
 */
void filter_min_weight_threshold( Sample& smp,    double threshold = 0.01 );

/**
 * @brief Remove all @link Pquery Pqueries@endlink which do not have at least one name that matches
 * the given regex.
 *
 * If the Pquery has a PqueryName whose PqueryName::name value matches the regex, the Pquery is
 * kept. If none of its names matches, the Pquery is removed.
 */
void filter_pqueries_keeping_names( Sample& smp, std::string const& regex );

/**
 * @brief Remove all @link Pquery Pqueries@endlink which do not have at least one name that is in
 * the given keep list.
 *
 * If the Pquery has a PqueryName whose PqueryName::name value is in the `keep_list`, the Pquery is
 * kept. If none of its names is in the `keep_list`, the Pquery is removed.
 *
 * This is similar to filter_pqueries_removing_names(), but not quite the opposite, as Pqueries can
 * have multiple names.
 */
void filter_pqueries_keeping_names(  Sample& smp, std::unordered_set<std::string> keep_list );

/**
 * @brief Remove all @link Pquery Pqueries@endlink which have at least one name that matches the given
 * regex.
 *
 * If the Pquery has a PqueryName whose PqueryName::name value matches the reges,
 * the Pquery is removed. If none of its names matches, the Pquery is kept.
 */
void filter_pqueries_removing_names( Sample& smp, std::string const& regex );

/**
 * @brief Remove all @link Pquery Pqueries@endlink which have at least one name that is in
 * the given remove list.
 *
 * If the Pquery has a PqueryName whose PqueryName::name value is in the `remove_list`,
 * the Pquery is removed. If none of its names is in the `remove_list`, the Pquery is kept.
 *
 * This is similar to filter_pqueries_keeping_names(), but not quite the opposite, as Pqueries can
 * have multiple names.
 */
void filter_pqueries_removing_names( Sample& smp, std::unordered_set<std::string> remove_list );

/**
 * @brief Remove all @link Pquery Pqueries@endlink from the two Sample%s except the ones that
 * have names in common.
 *
 * This function builds the intersection of the set of names of both Samples and only keeps those
 * Pqueries that have a PqueryName with one of those names.
 */
void filter_pqueries_intersecting_names( Sample& sample_1, Sample& sample_2 );

/**
 * @brief Remove all @link Pquery Pqueries@endlink from the two Sample%s that have a name in common.
 *
 * This function builds the intersection of the set of names of both Samples and removes all those
 * Pqueries that have a PqueryName with one of those names.
 *
 * This is not quite the same as building the symmetric difference and keeping those elements, and,
 * although similar, it not the opposite of filter_pqueries_intersecting_names(), because Pqueries
 * can have multiple names.
 */
void filter_pqueries_differing_names(    Sample& sample_1, Sample& sample_2 );

// =================================================================================================
//     Joining and Merging
// =================================================================================================

/**
 * @brief Copy all @link Pquery Pqueries @endlink from the source Sample (left parameter) to the
 * target Sample (right parameter).
 *
 * For this method to succeed, the PlacementTree%s of the Sample%s need to have the same topology,
 * including identical edge_nums and node names.
 * Otherwise, this function throws an `std::runtime_error`.
 *
 * The PlacementTree of the target Sample is not modified. If the average branch length tree is
 * needed instead, see SampleSet::merge_all().
 */
void copy_pqueries( Sample const& source, Sample& target );

/**
 * @brief Look for @link Pquery Pqueries @endlink with the same name and merge them.
 *
 * This function is a wrapper that simply calls three other functions on the provided Sample:
 *
 *     * collect_duplicate_pqueries()
 *     * merge_duplicate_names()
 *     * merge_duplicate_placements()
 *
 * See there for more information on what they do.
 */
void merge_duplicates( Sample& smp );

/**
 * @brief Find all @link Pquery Pqueries @endlink that share a common name and combine them into a
 * single Pquery containing all their collective PqueryPlacement%s and PqueryName%s.
 *
 * The function collects all Pqueries that share at least one name. This is transitive, so that for
 * example three Pqueries with two names each like `(a,b) (b,c) (c,d)` will be combined into one
 * Pquery. Thus, the transitive closure of shared names is collected.
 *
 * All those Pqueries with shared names are combined by simply moving all their Placements and
 * Names into one Pquery and deleting the others. This means that at least the shared names will
 * be doubled after this function. Also, Placements on the same edge can occur.
 * Thus, usually `merge_duplicate_names()` and `merge_duplicate_placements()` are called after
 * this function. The function merge_duplicates() does exaclty this, for convenience.
 */
void collect_duplicate_pqueries( Sample& smp );

/**
 * @brief Merge all PqueryPlacement%s of a Pquery that are on the same TreeEdge into one averaged
 * PqueryPlacement.
 *
 * The merging is done via averaging all values of the PqueryPlacement: `likelihood`,
 * `like_weight_ratio`, `proximal_length`, `pendant_length` and `parsimony`.
 */
void merge_duplicate_placements( Pquery& pquery );

/**
 * @brief Call merge_duplicate_placements( Pquery& ) for each Pquery of a Sample.
 */
void merge_duplicate_placements( Sample& smp );

/**
 * @brief Merge all PqueryName%s that have the same `name` property into one, while adding up their
 * `multiplicity`.
 */
void merge_duplicate_names( Pquery& pquery );

/**
 * @brief Call `merge_duplicate_names()` for each Pquery of the Sample.
 */
void merge_duplicate_names( Sample& smp );

// =================================================================================================
//     Placement Mass
// =================================================================================================

/**
 * @brief Return the sum of all @link PqueryName::multiplicity multiplicities @endlink of the
 * Pquery.
 */
double total_multiplicity( Pquery const& pqry );

/**
 * @brief Return the sum of all @link PqueryName::multiplicity multiplicities @endlink of all the
 * @link Pquery Pqueries@endlink of the Sample.
 */
double total_multiplicity( Sample const& sample );

/**
 * @brief Get the total number of PqueryPlacement%s in all @link Pquery Pqueries @endlink of the
 * given Sample.
 */
size_t total_placement_count( Sample const& smp );

/**
 * @brief Get the summed mass of all PqueryPlacement%s in all @link Pquery Pqueries @endlink of the
 * given Sample, where mass is measured by the
 * @link PqueryPlacement::like_weight_ratio like_weight_ratios@endlink of the PqueryPlacement%s.
 *
 * Be aware that this function only gives the pure sum of the `like_weight_ratio`s. See
 * total_placement_mass_with_multiplicities() for a version of this function, which also takes
 * the @link PqueryName::multiplicity multiplicities @endlink of the Pqueries into account.
 */
double total_placement_mass(  Sample const& smp );

/**
 * @brief Get the mass of all PqueryPlacement%s of the Sample, using the
 * @link PqueryName::multiplicity multiplicities @endlink as factors.
 *
 * This function returns the summed mass of all PqueryPlacement%s in all
 * @link Pquery Pqueries @endlink of the
 * given Sample, where mass is measured by `like_weight_ratio`, and the mass of each Pquery is
 * multiplied by the sum of the @link PqueryName::multiplicity multiplicities @endlink of this
 * Pquery.
 *
 * This method returns the same value as total_placement_mass() in case that the `multiplicity` is
 * left at its default value of 1.0.
 */
double total_placement_mass_with_multiplicities(  Sample const& smp );

/**
 * @brief Get the number of placements on the edge with the most placements, and a pointer to this
 * edge.
 */
std::pair<PlacementTreeEdge const*, size_t> placement_count_max_edge( Sample const& smp );

/**
 * @brief Get the summed mass of the placements on the heaviest edge, measured by their
 * `like_weight_ratio`, and a pointer to this edge.
 */
std::pair<PlacementTreeEdge const*, double> placement_mass_max_edge( Sample const& smp );

// =================================================================================================
//     Histograms
// =================================================================================================

std::vector<double> closest_leaf_weight_distribution( Sample const& sample );

/**
 * @brief Return a histogram representing how many placements have which depth with respect to
 * their closest leaf node.
 *
 * The depth between two nodes on a tree is the number of edges between them. Thus, the depth of a
 * placement (which sits on an edge of the tree) to a specific node is the number of edges between
 * this node and the closer one of the two nodes at the end of the edge where the placement sits.
 *
 * The closest leaf to a placement is thus the leaf node which has the smallest depth to that
 * placement. This function then returns a histogram of how many placements (values of the vector)
 * are there that have a specific depth (indices of the vector) to their closest leaf.
 *
 * Example: A return vector of
 *
 *     histogram[0] = 2334
 *     histogram[1] = 349
 *     histogram[2] = 65
 *     histogram[3] = 17
 *
 * means that there are 2334 placements that sit on an edge which leads to a leaf node (thus, the
 * depth of one of the nodes of the edge is 0). It has 349 placements that sit on an edge where
 * one of its nodes has one neighbour that is a leaf; and so on.
 *
 * The vector is automatically resized to the needed number of elements.
 */
std::vector<int> closest_leaf_depth_histogram( Sample const& smp );

/**
 * @brief Returns a histogram counting the number of placements that have a certain distance to
 * their closest leaf node, divided into equally large intervals between a min and a max distance.
 *
 * The distance range between min and max is divided into `bins` many intervals of equal size.
 * Then, the distance from each placement to its closest leaf node is calculated and the counter for
 * this particular distance inverval in the histogram is incremented.
 *
 * The distance is measured along the `branch_length` values of the edges, taking the
 * `pendant_length` and `proximal_length` of the placements into account. If the distances is
 * outside of the interval [min,max], the counter of the first/last bin is incremented respectively.
 *
 * Example:
 *
 *     double min      =  0.0;
 *     double max      = 20.0;
 *     int    bins     = 25;
 *     double bin_size = (max - min) / bins;
 *     std::vector<int> hist = closest_leaf_distance_histogram (min, max, bins);
 *     for (unsigned int bin = 0; bin < hist.size(); ++bin) {
 *         LOG_INFO << "Bin " << bin << " [" << bin * bin_size << "; "
 *                  << (bin+1) * bin_size << ") has " << hist[bin] << " placements.";
 *     }
 * %
 */
std::vector<int> closest_leaf_distance_histogram (
    Sample const& smp, const double min, const double max, const int bins = 10
);

/**
 * @brief Returns the same type of histogram as closest_leaf_distance_histogram(), but automatically
 * determines the needed boundaries.
 *
 * See closest_leaf_distance_histogram() for general information about what this function does. The
 * difference between both functions is that this one first procresses all distances from
 * placements to their closest leaf nodes to find out what the shortest and longest are, then sets
 * the boundaries of the histogram accordingly. The number of bins is then used to divide this
 * range into intervals of equal size.
 *
 * The boundaries are returned by passing two doubles `min` and `max` to the function by reference.
 * The value of `max` will actually contain the result of std::nextafter() called on the longest
 * distance; this makes sure that the value itself will be placed in the interval.
 *
 * Example:
 *
 *     double min, max;
 *     int    bins = 25;
 *     std::vector<int> hist = closest_leaf_distance_histogram (min, max, bins);
 *     double bin_size = (max - min) / bins;
 *     LOG_INFO << "Histogram boundaries: [" << min << "," << max << ").";
 *     for (unsigned int bin = 0; bin < hist.size(); ++bin) {
 *         LOG_INFO << "Bin " << bin << " [" << bin * bin_size << "; "
 *                  << (bin+1) * bin_size << ") has " << hist[bin] << " placements.";
 *     }
 *
 * It has a slightly higher time and memory consumption than the non-automatic version
 * closest_leaf_distance_histogram(), as it needs to process the values twice in order to find their
 * min and max.
 */
std::vector<int> closest_leaf_distance_histogram_auto (
    Sample const& smp, double& min, double& max, const int bins = 10
);

} // namespace placement
} // namespace genesis

#endif // include guard
