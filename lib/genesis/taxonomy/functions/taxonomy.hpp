#ifndef GENESIS_TAXONOMY_FUNCTIONS_TAXONOMY_H_
#define GENESIS_TAXONOMY_FUNCTIONS_TAXONOMY_H_

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
 * @brief
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"

#include <functional>
#include <iosfwd>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Tags for Tag Dispatch
// =================================================================================================

/**
 * @brief Tag used for find_taxon().
 */
struct DepthFirstSearch{};

/**
 * @brief Tag used for find_taxon().
 */
struct BreadthFirstSearch{};

/**
 * @brief Alias for DepthFirstSearch
 */
using DFS = DepthFirstSearch;

/**
 * @brief Alias for BreadthFirstSearch
 */
using BFS = BreadthFirstSearch;

// =================================================================================================
//     Find Functions
// =================================================================================================

/**
 * @brief Alias for find_taxon(..., DepthFirstSearch{})
 */
template< class UnaryPredicate >
Taxon const* find_taxon( Taxonomy const& tax, UnaryPredicate p )
{
    return find_taxon( tax, p, DepthFirstSearch{} );
}

/**
 * @brief Alias for find_taxon(..., DepthFirstSearch{})
 */
template< class UnaryPredicate >
Taxon* find_taxon( Taxonomy& tax, UnaryPredicate p )
{
    return find_taxon(tax, p, DepthFirstSearch{} );
}

/**
 * @brief Find a Taxon based on a given predicate by recursively searching the Taxonomy in a depth first manner.
 */
template< class UnaryPredicate >
Taxon const* find_taxon( Taxonomy const& tax, UnaryPredicate p, DepthFirstSearch )
{
    for( auto const& c : tax ) {
        if( p( c ) ) {
            return &c;
        }
        auto rec = find_taxon( c, p, DepthFirstSearch{} );
        if( rec != nullptr ) {
            return rec;
        }
    }
    return nullptr;
}

/**
 * @brief Find a Taxon based on a given predicate by recursively searching the Taxonomy in a breadth first manner.
 */
template< class UnaryPredicate >
Taxon const* find_taxon( Taxonomy const& tax, UnaryPredicate p, BreadthFirstSearch )
{
    std::queue< Taxon const* > taxa_queue;
    for( auto& t : tax ) {
        taxa_queue.push( &t );
    }

    while( not taxa_queue.empty() ) {
        auto const& cur = *taxa_queue.front();
        taxa_queue.pop();

        if( p( cur ) ) {
            return &cur;
        }

        for( auto const& t : cur ) {
            taxa_queue.push( &t );
        }
    }
    return nullptr;
}

/**
 * @brief Find a Taxon based on a given predicate by recursively searching the Taxonomy according to a search strategy.
 */
template< class SearchStrategy, class UnaryPredicate >
Taxon* find_taxon( Taxonomy& tax, UnaryPredicate p, SearchStrategy strat )
{
    // Avoid code duplication according to Scott Meyers.
    auto const& ctax = static_cast< Taxonomy const& >( tax );
    return const_cast< Taxon* >( find_taxon( ctax, p, strat ));
}

/**
 * @brief Alias for find_taxon_by_name(..., DepthFirstSearch{}).
 */
Taxon const* find_taxon_by_name( Taxonomy const& tax, std::string const& name );

/**
* @brief Alias for find_taxon_by_name(..., DepthFirstSearch{}).
 */
Taxon*       find_taxon_by_name( Taxonomy&       tax, std::string const& name );

/**
* @brief Alias for find_taxon_by_id(..., DepthFirstSearch{}).
 */
Taxon const* find_taxon_by_id( Taxonomy const& tax, std::string const& id );

/**
* @brief Alias for find_taxon_by_id(..., DepthFirstSearch{}).
 */
Taxon*       find_taxon_by_id( Taxonomy&       tax, std::string const& id );

/**
 * @brief Find a Taxon with a given name by recursively searching the Taxonomy according to a search strategy.
 */
template< class SearchStrategy >
Taxon const* find_taxon_by_name( Taxonomy const& tax, std::string const& name, SearchStrategy strat )
{
    return find_taxon( tax, [&name]( Taxon const& t ){
        return t.name() == name;
    }, strat );
}

/**
 * @brief Find a Taxon with a given name by recursively searching the Taxonomy according to a search strategy.
 */
template< class SearchStrategy >
Taxon*       find_taxon_by_name( Taxonomy&       tax, std::string const& name, SearchStrategy strat )
{
    // Avoid code duplication according to Scott Meyers.
    auto const& ctax = static_cast< Taxonomy const& >( tax );
    return const_cast< Taxon* >( find_taxon_by_name( ctax, name, strat ));
}

/**
 * @brief Find a Taxon with a given ID by recursively searching the Taxonomy according to a search strategy.
 */
template< class SearchStrategy >
Taxon const* find_taxon_by_id( Taxonomy const& tax, std::string const& id, SearchStrategy strat )
{
    return find_taxon( tax, [&id]( Taxon const& t ){
        return t.id() == id;
    }, strat );
}

/**
 * @brief Find a Taxon with a given ID by recursively searching the Taxonomy according to a search strategy.
 */
template< class SearchStrategy >
Taxon*       find_taxon_by_id( Taxonomy&       tax, std::string const& id, SearchStrategy strat )
{
    // Avoid code duplication according to Scott Meyers.
    auto const& ctax = static_cast< Taxonomy const& >( tax );
    return const_cast< Taxon* >( find_taxon_by_id<SearchStrategy>( ctax, id, strat ));
}

// =================================================================================================
//     Accessors
// =================================================================================================

/**
 * @brief Return the level of depth of a given Taxon.
 *
 * This level is the number of parents the Taxon has, excluding the Taxonomy which contains them.
 * That means, the immediate children of a Taxonomy all have level 0, their children level 1,
 * and so on.
 */
size_t taxon_level( Taxon const& taxon );

/**
 * @brief Return the total number of taxa contained in the Taxomony, i.e., the number of
 * (non-unique) names of all children (recursively).
 *
 * Example: The Taxonomy
 *
 *     Tax_1
 *         Tax_2
 *             Tax_3
 *         Tax_4
 *             Tax_3
 *     Tax_5
 *
 * contains a total of 6 taxa. The name `Tax_3` appears twice and is counted twice.
 */
size_t total_taxa_count( Taxonomy const& tax );

/**
 * @brief Return the number of lowest level @link Taxon Taxa@endlink (i.e., taxa without sub-taxa)
 * in the Taxonomy.
 *
 * The function counts the number of taxa without any sub-taxa, that is, the "leaves" of the
 * Taxonomy.
 *
 * Example: The Taxonomy
 *
 *     Tax_1
 *         Tax_2
 *             Tax_3
 *         Tax_4
 *             Tax_5
 *     Tax_6
 *         Tax_7
 *
 * contains 3 such taxa, i.e., `Tax_3`, `Tax_5` and `Tax_7`.
 */
size_t taxa_count_lowest_levels( Taxonomy const& tax );

/**
 * @brief Count the number of @link Taxon Taxa@endlink at a certain level of depth in the
 * Taxonomy.
 *
 * The function returns how many @link Taxon Taxa@endlink there are in the Taxonomy that are
 * at a certain level - that is excluding the number of their respective sub-taxa.
 * The first/top level has depth 0.
 *
 * See @link taxa_count_levels( Taxonomy const& tax ) here@endlink for a version of this function
 * that returns those values for all levels of depth.
 */
size_t taxa_count_at_level( Taxonomy const& tax, size_t level );

/**
 * @brief Count the number of @link Taxon Taxa@endlink at each level of depth in the Taxonomy.
 *
 * The function returns how many @link Taxon Taxa@endlink there are in the Taxonomy that are
 * at each level - that is excluding the number of their respective sub-taxa.
 * The first/top level has depth 0; it's count is the first element in the returned vector,
 * and so on.
 *
 * This function returns the values of taxa_count_at_level( Taxonomy const& tax, size_t level )
 * for all levels of depth.
 */
std::vector< size_t > taxa_count_levels( Taxonomy const& tax );

/**
 * @brief Count the number of @link Taxon Taxa@endlink in a Taxonomy that have a certain rank
 * assigned to them.
 *
 * The function recursively iterates all sub-taxa of the Taxonomy and counts how many of the
 * @link Taxon Taxa@endlink have the given rank assigned (case sensitive or not).
 *
 * See @link taxa_count_ranks( Taxonomy const&, bool ) here@endlink for a version of this function
 * that returns this number for all ranks in the Taxonomy.
 */
size_t taxa_count_with_rank(
    Taxonomy const&    tax,
    std::string const& rank,
    bool               case_sensitive = false
);

/**
 * @brief Count the number of @link Taxon Taxa@endlink in a Taxonomy per rank.
 *
 * The function gives a list of all ranks found in the Taxonomy, with a count of how many Taxa
 * there are that have this rank.
 *
 * It is similar to
 * @link taxa_count_with_rank( Taxonomy const&, std::string const&, bool ) this function@endlink,
 * but gives the result for all ranks.
 *
 * If the optional parameter `case_sensitive` is set to `true`, all ranks are treated case
 * sensitive, that is, ranks with different case produce different entries.
 * If left at the default `false`, they are converted to lower case first, so that they are all
 * treated case insensitivly.
 */
std::unordered_map< std::string, size_t> taxa_count_ranks(
    Taxonomy const& tax,
    bool            case_sensitive = false
);

/**
 * @brief Return `true` iff all IDs of the @link Taxon Taxa@endlink in the Taxonomy are unique.
 */
bool has_unique_ids( Taxonomy const& tax );

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
 * @brief Sort the @link Taxon Taxa@endlink of a Taxonomy by their name.
 *
 * After calling this function, the @link Taxon Taxa@endlink are stored in the order given by their
 * names. This is useful for e.g., output.
 *
 * @param tax       Taxonomy to be sorted.
 * @param recursive Optional, default is `true`. If set to `true`, the sub-taxa are also sorted.
 *                  If set to `false`, only the immediate children of the given Taxonomy are sorted.
 * @param case_sensitive Optional, default is `false`. Determines whether the name string comparison
 *                  is done in a case sensitive manner or not.
 */
void sort_by_name( Taxonomy& tax, bool recursive = true, bool case_sensitive = false );

/**
 * @brief Remove all @link Taxon Taxa @endlink at a given level of depth in the Taxonomy hierarchy,
 * and all their children.
 *
 * That is, providing `level = 0` has the same effect as calling
 * @link Taxonomy::clear_children() clear_children() @endlink on the given Taxonomy;
 * `level = 1` has this effect for the children of the given Taxonomy; and so on.
 *
 * See taxon_level() for more information on the level.
 */
void remove_taxa_at_level( Taxonomy& tax, size_t level );

// =================================================================================================
//     Print and Output
// =================================================================================================

/**
 * @brief Print the contents of a Taxonomy, i.e., all nested taxa, up to a limit of 10.
 *
 * This simple output function prints the first 10 nested @link Taxon Taxa@endlink of a Taxonomy.
 * If you need all Taxa and more control over what you want to print, see PrinterNested class.
 */
std::ostream& operator << ( std::ostream& out, Taxonomy const& tax );

/**
 * @brief Validate the internal data structures of a Taxonomy and its child
 * @link Taxon Taxa Taxa@endlink.
 *
 * The function validates the correctness of internal pointers, particularly, the
 * @link Taxon::parent() parent pointers@endlink of Taxon.
 * If the structure is broken, a @link utils::Logging log message@endlink is logged to `LOG_INFO`
 * and the functions returns `false`.
 *
 * @param taxonomy The Taxonomy object to validate.
 * @param stop_at_first_error Optional, defaults to `false`. By default, all errors are reported.
 *     If set to `true`, only the first one is logged and the function immediately returns `false`
 *     (or runs through and returns `true` if no errors are found).
 */
bool validate( Taxonomy const& taxonomy, bool stop_at_first_error = false );

} // namespace taxonomy
} // namespace genesis

#endif // include guard
