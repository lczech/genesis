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
 * @brief
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/functions/taxonomy.hpp"

#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/printers/nested.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <assert.h>
#include <ostream>
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
size_t taxa_count_lowest_levels( Taxonomy const& tax )
{
    size_t count = 0;
    for( auto const& t : tax ) {
        if( t.size() == 0 ) {
            ++count;
        } else {
            count += taxa_count_lowest_levels( t );
        }
    }
    return count;
}

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
size_t taxa_count_at_level( Taxonomy const& tax, size_t level )
{
    // Recursive implementation, because we are lazy.
    size_t count = 0;
    if( level == 0 ) {
        count += tax.size();
    } else {
        for( auto& c : tax ) {
            count += taxa_count_at_level( c, level - 1 );
        }
    }
    return count;
}

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
std::vector< size_t > taxa_count_levels( Taxonomy const& tax )
{
    if( tax.size() == 0 ) {
        return std::vector< size_t >();
    }

    std::vector< size_t > result( 1, 0 );
    result[ 0 ] = tax.size();

    for( auto const& child : tax ) {
        auto cres = taxa_count_levels( child );

        if( result.size() < cres.size() + 1 ) {
            result.resize( cres.size() + 1, 0 );
        }

        for( size_t i = 0; i < cres.size(); ++i ) {
            result[ i+1 ] += cres[ i ];
        }
    }
    return result;
}

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
    bool               case_sensitive
) {
    size_t count = 0;
    for( auto const& t : tax ) {

        // Count.
        if( case_sensitive ) {
            if( t.rank() == rank ) {
                ++count;
            }
        } else {
            if( utils::equals_ci( t.rank(), rank )) {
                ++count;
            }
        }

        // Recurse.
        count += taxa_count_with_rank( t, rank, case_sensitive );
    }
    return count;
}

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
    bool            case_sensitive
) {
    std::unordered_map< std::string, size_t> result;

    for( auto const& taxon : tax ) {
        if( case_sensitive ) {
            ++result[ taxon.rank() ];
        } else {
            ++result[ utils::to_lower( taxon.rank() ) ];
        }

        // Recurse.
        auto children = taxa_count_ranks( taxon, case_sensitive );
        for( auto const& ctaxon : children ) {
            result[ ctaxon.first ] += ctaxon.second;
        }
    }

    return result;
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
 * @brief Print the contents of a Taxonomy, i.e., all nested taxa, up to a limit of 10.
 *
 * This simple output function prints the first 10 nested @link Taxon Taxa@endlink of a Taxonomy.
 * If you need all Taxa and more control over what you want to print, see PrinterNested class.
 */
std::ostream& operator << ( std::ostream& out, Taxonomy const& tax )
{
    // We use a nested printer with max 10 lines per default.
    auto printer = PrinterNested();
    printer.line_limit( 10 );
    printer.print( out, tax );
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
    // Local recursive implementation of the function.
    std::function< bool ( Taxonomy const&, bool, size_t ) > validate_rec = [&] (
        Taxonomy const& tax, bool stop_at_first_error, size_t level
    ) {
        bool res = true;
        for( auto const& c : tax ) {

            // Check current parent.
            if( (level == 0 && c.parent() != nullptr) || (level > 0 && c.parent() != &tax) ) {
                LOG_INFO << "Taxon child with invalid parent pointer: " << c.name();
                if( stop_at_first_error ) {
                    return false;
                } else {
                    res = false;
                }
            }

            // Recurse.
            if( ! validate_rec( c, stop_at_first_error, level + 1 )) {
                if( stop_at_first_error ) {
                    return false;
                } else {
                    res = false;
                }
            }
        }
        return res;
    };

    return validate_rec( taxonomy, stop_at_first_error, 0 );
}

} // namespace taxonomy
} // namespace genesis
