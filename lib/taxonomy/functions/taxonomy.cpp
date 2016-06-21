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
 * @brief
 *
 * @file
 * @ingroup taxonomy
 */

#include "taxonomy/functions/taxonomy.hpp"

#include "taxonomy/taxon.hpp"
#include "taxonomy/taxonomy.hpp"
#include "taxonomy/taxscriptor.hpp"

#include "utils/core/logging.hpp"
#include "utils/text/string.hpp"

#include <algorithm>
#include <assert.h>
#include <ostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Accessors
// =================================================================================================

/**
 * @brief Find a Taxon with a given name by recursively searching the Taxonomy.
 */
Taxon const* find_taxon_by_name( Taxonomy const& tax, std::string const& name )
{
    for( auto const& c : tax ) {
        if( c.name() == name ) {
            return &c;
        }
        auto rec = find_taxon_by_name( c, name );
        if( rec != nullptr ) {
            return rec;
        }
    }
    return nullptr;
}

/**
 * @brief Find a Taxon with a given name by recursively searching the Taxonomy.
 */
Taxon* find_taxon_by_name( Taxonomy& tax, std::string const& name )
{
    // Avoid code duplication according to Scott Meyers.
    auto const& ctax = static_cast< Taxonomy const& >( tax );
    return const_cast< Taxon* >( find_taxon_by_name( ctax, name ));
}

/**
 * @brief Return the level of depth of a given Taxon.
 *
 * This level is the number of parents the Taxon has, excluding the Taxonomy which contains them.
 * That means, the immediate children of a Taxonomy all have level 0, their children level 1,
 * and so on.
 */
size_t taxon_level( Taxon const& taxon )
{
    size_t res = 0;
    Taxon const* parent = taxon.parent();
    while( parent != nullptr ) {
        parent = parent->parent();
        ++res;
    }
    return res;
}

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
size_t total_taxa_count( Taxonomy const& tax )
{
    size_t count = tax.size();
    for( auto const& t : tax ) {
        count += total_taxa_count( t );
    }
    return count;
}

// =================================================================================================
//     Iterators
// =================================================================================================

/**
 * @brief Apply a function to all taxa of the Taxonomy, traversing it in levelorder.
 *
 * The given Taxonomy is traversed in levelorder (i.e., breadth-first search). If
 * `include_inner_taxa` is set to true (default), the provided functional is called for all
 * @link Taxon Taxa @endlink. Otherwise, the functional is only called for the taxa of lowest rank,
 * that is, for each Taxon that does not have sub-taxa.
 */
void levelorder_for_each(
    Taxonomy& tax,
    std::function< void( Taxon& )> fn,
    bool include_inner_taxa
) {
    std::queue< Taxon* > taxa_queue;
    for( auto& t : tax ) {
        taxa_queue.push( &t );
    }

    while( ! taxa_queue.empty() ) {
        auto& cur = *taxa_queue.front();

        if( include_inner_taxa || cur.size() == 0 ) {
            fn( cur );
        }

        for( auto& t : cur ) {
            taxa_queue.push( &t );
        }
        taxa_queue.pop();
    }
}

/**
 * @brief Apply a function to all taxa of the Taxonomy, traversing it in preorder.
 *
 * The given Taxonomy is traversed in preorder (i.e., a variant of depth-first search). If
 * `include_inner_taxa` is set to true (default), the provided functional is called for all
 * @link Taxon Taxa @endlink. Otherwise, the functional is only called for the taxa of lowest rank,
 * that is, for each Taxon that does not have sub-taxa.
 */
void preorder_for_each(
    Taxonomy& tax,
    std::function< void( Taxon& )> fn,
    bool include_inner_taxa
) {
    for( auto& t : tax ) {
        if( include_inner_taxa || t.size() == 0 ) {
            fn( t );
        }

        preorder_for_each( t, fn, include_inner_taxa );
    }
}

/**
 * @brief Apply a function to all taxa of the Taxonomy, traversing it in postorder.
 *
 * The given Taxonomy is traversed in postorder (i.e., a variant of depth-first search). If
 * `include_inner_taxa` is set to true (default), the provided functional is called for all
 * @link Taxon Taxa @endlink. Otherwise, the functional is only called for the taxa of lowest rank,
 * that is, for each Taxon that does not have sub-taxa.
 */
void postorder_for_each(
    Taxonomy& tax,
    std::function< void( Taxon& )> fn,
    bool include_inner_taxa
) {
    for( auto& t : tax ) {
        postorder_for_each( t, fn, include_inner_taxa );

        if( include_inner_taxa || t.size() == 0 ) {
            fn( t );
        }
    }
}

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
void sort_by_name( Taxonomy& tax, bool recursive, bool case_sensitive )
{
    // Make two functions for case sensitive and insensitive comparison.
    auto comp_by_name_cs = []( Taxon const& lhs, Taxon const& rhs ) {
        return lhs.name() < rhs.name();
    };
    auto comp_by_name_ci = []( Taxon const& lhs, Taxon const& rhs ) {
        return utils::to_lower( lhs.name() ) < utils::to_lower( rhs.name() );
    };

    // Sort.
    if( case_sensitive ) {
        std::sort( tax.begin(), tax.end(), comp_by_name_cs );
    } else {
        std::sort( tax.begin(), tax.end(), comp_by_name_ci );
    }

    // Run recursion if necessary.
    if( recursive ) {
        for( auto& child : tax ) {
            sort_by_name( child, true, case_sensitive );
        }
    }
}

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
void remove_taxa_at_level( Taxonomy& tax, size_t level )
{
    // Recursive implementation, because we are lazy.
    if( level == 0 ) {
        tax.clear_children();
    } else {
        for( auto& c : tax ) {
            remove_taxa_at_level( c, level - 1 );
        }
    }
}

// =================================================================================================
//     Print and Output
// =================================================================================================

/**
 * @brief Local helper function to print the contents of a Taxonomy or Taxon to an ostream.
 */
void print_to_ostream(
    std::ostream&   out,
    Taxonomy const& tax,
    size_t          indent
) {
    auto in = std::string( indent, '\t' );
    for( auto const& t : tax ) {
        out << in << t.name() << "\n";
        print_to_ostream( out, t, indent + 1 );
    }
}

/**
 * @brief Print the contents of a Taxonomy, i.e., all nested taxa.
 */
std::ostream& operator << ( std::ostream& out, Taxonomy const& tax )
{
    print_to_ostream( out, tax, 0 );
    return out;
}

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
bool validate( Taxonomy const& taxonomy, bool stop_at_first_error )
{
    bool res = true;
    for( auto const& c : taxonomy ) {

        // Check current parent.
        if( c.parent() != nullptr && c.parent() != &taxonomy ) {
            LOG_INFO << "Taxon child with invalid parent pointer: " << c.name();
            if( stop_at_first_error ) {
                return false;
            } else {
                res = false;
            }
        }

        // Recurse.
        if( ! validate( c )) {
            if( stop_at_first_error ) {
                return false;
            } else {
                res = false;
            }
        }
    }
    return res;
}

} // namespace taxonomy
} // namespace genesis
