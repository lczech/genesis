#ifndef GENESIS_TREE_BIPARTITION_RF_H_
#define GENESIS_TREE_BIPARTITION_RF_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech

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

#include "genesis/tree/tree.hpp"
#include "genesis/tree/tree_set.hpp"
#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/math/bitvector.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>

namespace genesis {
namespace tree {

// =================================================================================================
//     Helper Functions
// =================================================================================================

/**
 * @brief Get a mapping from taxon names to unique IDs.
 *
 * The IDs are consecutive, starting at 0. Only tip/leaf names are processed,
 * which need to be unique and non-empty.
 *
 * The mappning can then be used for unique identification of taxa, which is needed for the
 * RF distance calculation functions.
 */
std::unordered_map<std::string, size_t> rf_taxon_name_map( Tree const& tree );

/**
 * @brief Get all split Bitvectors for a given Tree.
 *
 * For each inner edge of the @p tree, a Bitvector is produced that contains `true` bits at all
 * indices of the tips on one side of the split, using @p names for getting indices of leaf nodes.
 *
 * The Bitvector%s are normalized, that is, their first bit is always set to 0. This makes sure
 * that the two ways of representing each split result in the same Bitvector.
 */
std::vector<utils::Bitvector> rf_get_bitvectors(
    Tree const& tree,
    std::unordered_map<std::string, size_t> const& names
);

/**
 * @brief Get an occurrence map for each split found in the given TreeSet.
 *
 * The given @p trees need to contain the same leaf node names. Then, all their splits are
 * computed, represented a Bitvector%s of tips. For each such split, another Bitvector is produced
 * that is `true` at every tree in @p trees that contains that split.
 *
 * In other words, the function yields a map from Bitvector%s (keys) that represent splits
 * to Bitvector%s (mapped values) that represent occurences of these splits in the given @p trees.
 * The size of the map hence is the number of unique splits in all @p trees;
 * the size of the key Bitvectors is the number of taxa in the trees;
 * and the size of the mapped value Bitvectors is the number of trees, that is TreeSet::size().
 */
std::unordered_map<utils::Bitvector, utils::Bitvector> rf_get_occurrences(
    TreeSet const& trees
);

/**
 * @brief Get an occurrence map for each split found in some Tree%s.
 *
 * This is a special case of the more general rf_get_occurrences( TreeSet const& trees ) function,
 * which takes one additional Tree into account. This @p lhs tree gets index 0 in the resulting
 * Bitvectors of the mapped values, while all trees in @p rhs get their index in the TreeSet plus one.
 *
 * The function is meant as an accelaration for computing the distance from one Tree to several
 * other Tree%s, and is used by rf_distance_absolute( Tree const& lhs, TreeSet const& rhs ).
 */
std::unordered_map<utils::Bitvector, utils::Bitvector> rf_get_occurrences(
    Tree const& lhs,
    TreeSet const& rhs
);

/**
 * @brief Get an occurrence map for each split found in two trees.
 *
 * This is a special case of the more general rf_get_occurrences( TreeSet const& trees ) function,
 * which takes two Tree%s and computes their split occurences.
 */
std::unordered_map<utils::Bitvector, utils::Bitvector> rf_get_occurrences(
    Tree const& lhs,
    Tree const& rhs
);

// =================================================================================================
//     Absolute RF Distance Functions
// =================================================================================================

/**
 * @brief Compute the pairwise absolute RF (Robinson-Foulds) distance metric between a set of @p trees.
 *
 * The function computes the unweighted absolute RF distance.
 */
utils::Matrix<size_t> rf_distance_absolute( TreeSet const& trees );

/**
 * @brief Compute the absolute RF (Robinson-Foulds) distance metric between a given @p lhs Tree
 * and all of the trees in the @p rhs TreeSet.
 *
 * The function computes the unweighted absolute RF distance.
 * This is meant as an accelaration if the pairwise distance is not needed.
 */
std::vector<size_t> rf_distance_absolute( Tree const& lhs, TreeSet const& rhs );

/**
 * @brief Compute the absolute RF (Robinson-Foulds) distance metric between two Tree%s.
 *
 * The function computes the unweighted absolute RF distance.
 */
size_t rf_distance_absolute( Tree const& lhs, Tree const& rhs );

// =================================================================================================
//     Relative RF Distance Functions
// =================================================================================================

/**
 * @brief Compute the pairwise relative RF (Robinson-Foulds) distance metric between a set of @p trees.
 *
 * The function computes the unweighted relative RF distance.
 *
 * This internally simply uses rf_distance_absolute(), and divides the result properly;
 * hence, if both variants are needed (absolute and relative), it might be faster to duplicate
 * that normalization code (simply copy from this function), instead of computing the RF distance
 * twice, which is what happens if both rf_distance_relative() and rf_distance_absolute() are called.
 */
utils::Matrix<double> rf_distance_relative( TreeSet const& trees );

/**
 * @brief Compute the relative RF (Robinson-Foulds) distance metric between a given @p lhs Tree
 * and all of the trees in the @p rhs TreeSet.
 *
 * The function computes the unweighted relative RF distance.
 * This is meant as an accelaration if the pairwise distance is not needed.
 *
 * This internally simply uses rf_distance_absolute(), and divides the result properly;
 * hence, if both variants are needed (absolute and relative), it might be faster to duplicate
 * that normalization code (simply copy from this function), instead of computing the RF distance
 * twice, which is what happens if both rf_distance_relative() and rf_distance_absolute() are called.
 */
std::vector<double> rf_distance_relative( Tree const& lhs, TreeSet const& rhs );

/**
 * @brief Compute the relative RF (Robinson-Foulds) distance metric between two Tree%s.
 *
 * The function computes the unweighted relative RF distance.
 *
 * This internally simply uses rf_distance_absolute(), and divides the result properly;
 * hence, if both variants are needed (absolute and relative), it might be faster to duplicate
 * that normalization code (simply copy from this function), instead of computing the RF distance
 * twice, which is what happens if both rf_distance_relative() and rf_distance_absolute() are called.
 */
double rf_distance_relative( Tree const& lhs, Tree const& rhs );

} // namespace tree
} // namespace genesis

#endif // include guard
