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

#include "taxonomy/functions.hpp"

#include "taxonomy/taxon.hpp"
#include "taxonomy/taxonomy.hpp"

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
 * @brief Find a Taxon with a given name by recursively searching the taxonomy.
 */
Taxon const* find_taxon( Taxonomy const& tax, std::string const& name )
{
    for( auto const& c : tax ) {
        if( c.name() == name ) {
            return &c;
        }
        auto rec = find_taxon( c, name );
        if( rec != nullptr ) {
            return rec;
        }
    }
    return nullptr;
}

/**
 * @brief Find a Taxon with a given name by recursively searching the taxonomy.
 */
Taxon* find_taxon( Taxonomy& tax, std::string const& name )
{
    // Avoid code duplication according to Scott Meyers.
    auto const& ctax = static_cast< Taxonomy const >( tax );
    return const_cast< Taxon* >( find_taxon( ctax, name ));
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
 *         Tax_3
 *             Tax_4
 *     Tax_5
 *
 * contains a total of 5 taxa.
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
 * @brief Add nested child taxa to a Taxonomy by splitting a string with their names.
 *
 * This function takes a string `children` containing the names of taxa, separated from each other
 * by any of the chars given in the parameter `delimiters`. Those taxa are added to the Taxonomy by
 * nesting them.
 *
 * Example: The input string
 *
 *     Tax_1;Tax_2;;Tax_4
 *
 * results in the Taxonomy
 *
 *     Tax_1
 *         Tax_2
 *             Tax_2
 *                 Tax_4
 *
 * The first taxon in the string cannot be empty. Otherwise an `std::runtime_error` is thrown.
 * If any of the later taxa are empty, the taxon name of the previous level taxon is used instead.
 * This is useful for unspecified taxa in deeper taxonomies. The only exception to this is the last
 * taxon. If it is empty, it is simply omitted. This is because many taxonomy databases end the
 * taxonomic string representation with a ';' by default.
 *
 * This function is the reverse of taxonomic_string().
 *
 * @param taxonomy   Taxonomy to add the taxa to.
 *
 * @param children   String containing a list of taxa, separated by any of the chars in
 *                   `delimiters`. The first element must not be empty, thus ";Tax_2" is not
 *                   allowed.
 *
 * @param delimiters Optional, defaults to ';'. Determines the characters used to split the
 *                   `children` string.
 *
 * @param trim_whitespaces Optional, defaults to `true`. If set to true, the taxa given by `children`
 *                   are trimmed off white spaces after splitting them. This is helpful if the input
 *                   string is copied from some spreadsheet application or CSV file, where spaces
 *                   between cells might be added. Default is to trim.
 *
 * @return           Return value of the function is the deepest Taxon that was given in the
 *                   `children` string, i.e., the last element after splitting the string.
 */
Taxon& add_children_from_string(
    Taxonomy&          taxonomy,
    std::string const& children,
    std::string const& delimiters,
    bool               trim_whitespaces
){
    // Split the given string, while keeping empty parts.
    auto taxa = utils::split( children, delimiters, false );

    // If there are no elements, the string was empty. This is illegal.
    if( taxa.size() == 0 ) {
        assert( children == "" );
        throw std::runtime_error( "Cannot add empty child to taxonomy." );
    }

    // Remove white spaces.
    if( trim_whitespaces ) {
        for( auto& r : taxa ) {
            r = utils::trim( r );
        }
    }

    // The first name in the list of sub-taxa must not be empty.
    if( taxa.front() == "" ) {
        throw std::runtime_error( "Cannot add children to taxomonmy if first child is empty." );
    }

    // The last name is ommited if empty.
    if( taxa.back() == "" ) {
        taxa.pop_back();
    }

    // Prepare: we need a Taxonomy to add children to. This pointer is updated in the loop so that
    // we go deeper and deeper into the taxonomy. Also, we keep track of the previously assigned
    // name, for cases where a taxon is empty.
    Taxonomy*   cur_taxon = &taxonomy;
    std::string prev_name;

    // Add the names to the Taxonomy.
    for( auto name : taxa ) {
        // If a sub-taxon is empty, use the super-taxon.
        // As we previously checked that the first taxon is not empty, this is well-formed.
        if( name == "" ) {
            name = prev_name;
        }

        cur_taxon  = &cur_taxon->add_child( name );
        prev_name = name;
    }

    // Convert to Taxon. This is always legal because we have ensured that we are adding at least
    // one sub-taxon to the taxonomy, and in the loop set cur_taxon to this taxon, so that it is
    // actually of dynamic type Taxon.
    return static_cast< Taxon& >( *cur_taxon );
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
 * @brief Return the taxonomic string representation of a given Taxon.
 *
 * This function is the reverse of add_children_from_string(). It returns a string with all
 * names of the super-taxa of the given taxon, concatenated using `delimiter`.
 * If `trim_nested_duplicates` is set to true (default), lower level names are set to empty if they
 * are the same as higher level names.
 *
 * Example: For a taxon with this list of parents
 *
 *     Tax_1
 *         Tax_1
 *             Tax_2
 *
 * the functions returns "Tax_1;;Tax_2", and respectively "Tax_1;Tax_1;Tax_2" without trimming
 * nested duplicates.
 */
std::string taxonomic_string( Taxon const& taxon, std::string delimiter, bool trim_nested_duplicates )
{
    // This implementation is probably not the fastest, but it is simple and kind of elegant.
    // Start with an empty vector that will store the super-taxa of the given taxon.
    std::vector<std::string> taxa;

    // Add taxa in reverse order: the deepest taxon will be stored first.
    // This is fast with a vector.
    Taxon const* r = &taxon;
    while( r != nullptr ) {
        taxa.push_back( r->name() );
        r = r->parent();
    }

    // If wanted, set all taxa to an empty string for which the super-taxon has the same name.
    // As we stored them in reverse order, we can simply go from start to one-but-the-end and check
    // for equality.
    if( trim_nested_duplicates ) {
        for( size_t i = 0; i < taxa.size() - 1; ++i ) {
            if( taxa[i] == taxa[i+1] ) {
                taxa[i] = "";
            }
        }
    }

    // Now reverse and return the joined result.
    std::reverse( taxa.begin(), taxa.end() );
    return utils::join( taxa, delimiter );
}

} // namespace taxonomy
} // namespace genesis
