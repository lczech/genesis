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

#include "genesis/taxonomy/functions/taxonomy.hpp"

#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/printers/nested.hpp"

#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <assert.h>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Find Functions
// =================================================================================================

Taxon const* find_taxon_by_name( Taxonomy const& tax, std::string const& name )
{
    return find_taxon_by_name( tax, name, DepthFirstSearch{} );
}

Taxon* find_taxon_by_name( Taxonomy& tax, std::string const& name )
{
    return find_taxon_by_name( tax, name, DepthFirstSearch{} );
}

Taxon const* find_taxon_by_id( Taxonomy const& tax, std::string const& id )
{
    return find_taxon_by_id( tax, id, DepthFirstSearch{} );
}

Taxon*       find_taxon_by_id( Taxonomy&       tax, std::string const& id )
{
    return find_taxon_by_id( tax, id, DepthFirstSearch{} );
}

// =================================================================================================
//     Accessors
// =================================================================================================

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

size_t total_taxa_count( Taxonomy const& tax )
{
    size_t count = tax.size();
    for( auto const& t : tax ) {
        count += total_taxa_count( t );
    }
    return count;
}

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

bool has_unique_ids( Taxonomy const& tax )
{
    std::unordered_set<std::string> ids;
    bool has_duplicates = false;

    auto collect_and_check = [&]( Taxon const& tax ){
        if( ids.count( tax.id() ) > 0 ) {
            has_duplicates = true;
            return;
        }
        ids.insert( tax.id() );
    };
    preorder_for_each( tax, collect_and_check );

    return ! has_duplicates;
}

// =================================================================================================
//     Modifiers
// =================================================================================================

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
        // std::sort( tax.begin(), tax.end(), comp_by_name_cs );
        tax.sort( comp_by_name_cs );
    } else {
        // std::sort( tax.begin(), tax.end(), comp_by_name_ci );
        tax.sort( comp_by_name_ci );
    }

    // Run recursion if necessary.
    if( recursive ) {
        for( auto& child : tax ) {
            sort_by_name( child, true, case_sensitive );
        }
    }
}

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

std::ostream& operator << ( std::ostream& out, Taxonomy const& tax )
{
    // We use a nested printer with max 10 lines per default.
    auto printer = PrinterNested();
    printer.line_limit( 10 );
    printer.print( out, tax );
    return out;
}

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
