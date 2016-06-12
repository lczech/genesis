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
#include <unordered_map>
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
//     Taxpressions
// =================================================================================================

/**
 * @brief Add nested child taxa to a Taxonomy by splitting a Taxpression string with their names.
 *
 * This function takes a string `taxpression` containing the names of taxa, separated from each other
 * by any of the chars given in the parameter `delimiters`. Those taxa are added to the Taxonomy by
 * nesting them. See Taxonomy for details on Taxpressions.
 *
 * Example: The input taxpression string
 *
 *     Tax_1;Tax_2;;Tax_4;
 *
 * results in the Taxonomy
 *
 *     Tax_1
 *         Tax_2
 *             Tax_2
 *                 Tax_4
 *
 * The first taxon in the string cannot be empty. Otherwise an `std::runtime_error` is thrown.
 * If any of the later taxa are empty (as in the example above), the taxon name of the previous
 * level taxon is used instead. This is useful for unspecified taxa in deeper taxonomies.
 *
 * The only exception to this is the last taxon. If it is empty, that is, if the string ends with
 * the delimiter char, this  is simply omitted (see example above). This is because many taxonomy
 * databases end the taxonomic string representation with a ';' by default.
 *
 * This function is the reverse of taxpression().
 *
 * @param taxonomy   Taxonomy to add the taxa to.
 *
 * @param taxpression Taxpression string containing a list of taxa, separated by any of the chars
 *                   in `delimiters`. The first element must not be empty, thus ";Tax_2" is not
 *                   allowed.
 *
 * @param delimiters Optional, defaults to ';'. Determines the characters used to split the
 *                   `taxpression` string.
 *
 * @param trim_whitespaces Optional, defaults to `true`. If set to true, the taxa given by
 *                   `taxpression` are trimmed off white spaces after splitting them. This is
 *                   helpful if the input string is copied from some spreadsheet application or
 *                   CSV file, where spaces between cells might be added. Default is to trim.
 *
 * @param expect_parents Optional, defaults to `false`. If set to true, the function expects all
 *                   super-taxa to exists, that is, all taxa except for the last one. In the example
 *                   above, the last taxon is "Tax_4". If then this expectation is not met, that is,
 *                   if not all super-taxa exist, an `std::runtime_error` exception is thrown.
 *
 *                   If left at the default (`false`), all necessary super-taxa are created if they
 *                   do not exists yet.
 *
 * @return           Return value of the function is the deepest Taxon that was given in the
 *                   `taxpression` string, i.e., the last element after splitting the string.
 */
Taxon& add_from_taxpression(
    Taxonomy&          taxonomy,
    std::string const& taxpression,
    std::string const& delimiters,
    bool               trim_whitespaces,
    bool               expect_parents
){
    // Split the given string, while keeping empty parts.
    auto taxa = utils::split( taxpression, delimiters, false );

    // If there are no elements, the string was empty. This is illegal.
    if( taxa.size() == 0 ) {
        assert( taxpression == "" );
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
        throw std::runtime_error( "Cannot add taxpression to taxomonmy if first taxon is empty." );
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
    for( size_t i = 0; i < taxa.size(); ++i ) {
        auto name = taxa[i];

        // If a sub-taxon is empty, use the super-taxon.
        // As we previously checked that the first taxon is not empty, this is well-formed.
        if( name == "" ) {
            assert( prev_name != "" );
            name = prev_name;
        }

        // If we expect parents to exists, we need to check if the child exists
        // (but only when we are still considering parents, and not the last taxon itself,
        // that is, if we are not at the last element of the vector).
        if( expect_parents && ( i < taxa.size() - 1 ) && ( ! cur_taxon->has_child( name )) ) {
            throw std::runtime_error(
                "Not all super-taxa of the given taxon are present in the given Taxonomy."
            );
        }

        cur_taxon = &cur_taxon->add_child( name );
        prev_name = name;
    }

    // Convert to Taxon. This is always legal because we have ensured that we are adding at least
    // one sub-taxon to the taxonomy, and in the loop set cur_taxon to this taxon, so that it is
    // actually of dynamic type Taxon.
    return dynamic_cast< Taxon& >( *cur_taxon );
}

/**
 * @brief Return the Taxpression string representation of a given Taxon.
 *
 * This function is the reverse of add_from_taxpression(). It returns a string with all
 * names of the super-taxa of the given taxon (and the taxon itself), concatenated using `delimiter`.
 * If `trim_nested_duplicates` is set to `true`, lower level names are set to empty if
 * they are the same as higher level names. Default is `false`, that is, nothing is trimmed.
 *
 * Example: For a taxon with this list of parents
 *
 *     Tax_1
 *         Tax_1
 *             Tax_2
 *
 * the functions returns "Tax_1;Tax_1;Tax_2", and respectively "Tax_1;;Tax_2" with trimming
 * nested duplicates.
 *
 * See Taxonomy for details on Taxpressions.
 */
std::string taxpression( Taxon const& taxon, std::string delimiter, bool trim_nested_duplicates )
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

// /**
//  * @brief Find a Taxon in a Taxonomy, given its Taxpression.
//  *
//  * See Taxonomy for details on Taxpressions.
//  */
// Taxon const* find_taxon_by_taxpression( Taxonomy const& tax, std::string const& taxpression )
// {
//
// }
//
// /**
//  * @brief Find a Taxon in a Taxonomy, given its Taxpression.
//  *
//  * See Taxonomy for details on Taxpressions.
//  */
// Taxon* find_taxon_by_taxpression( Taxonomy& tax, std::string const& taxpression )
// {
//     // Avoid code duplication according to Scott Meyers.
//     auto const& ctax = static_cast< Taxonomy const& >( tax );
//     return const_cast< Taxon* >( find_taxon_by_taxpression( ctax, taxpression ));
// }

// =================================================================================================
//     Ranks
// =================================================================================================


/**
 * @brief Local helper data that stores the abbreviations and names of common taxonomic ranks.
 */
static const std::unordered_map<char, std::string> rank_abbreviations = {
    { 'd', "Domain" },
    { 'k', "Kingdom" },
    { 'p', "Phylum" },
    { 'c', "Class" },
    { 'o', "Order" },
    { 'f', "Family" },
    { 'g', "Genus" },
    { 's', "Species" }
};

/**
 * @brief Get the taxonomic rank name given its abbreviation.
 *
 * The common taxonomic ranks are used:
 *
 *     D Domain
 *     K Kingdom
 *     P Phylum
 *     C Class
 *     O Order
 *     F Family
 *     G Genus
 *     S Species
 *
 * If any of those abbreviations (case-independend) is given, the full rank name is returned.
 * For all other input, an empty string is returned.
 */
std::string rank_from_abbreviation( char r )
{
    char c = tolower( r );
    if( rank_abbreviations.count( c ) > 0 ) {
        return rank_abbreviations.at( c );
    } else {
        return "";
    }
}

/**
 * @brief Get the abbreviation of a taxonomic rank name.
 *
 * This function returns the abbreviation for a given common taxonomic rank name,
 * case-independently. See rank_from_abbreviation() for a list of valid rank names.
 * If the given rank name is invalid, an empty string is returned.
 */
std::string rank_to_abbreviation( std::string const& rank )
{
    auto r = utils::to_lower( rank );
    for( auto const& p : rank_abbreviations ) {
        if( utils::to_lower( p.second ) == r ) {
            return std::string( 1, p.first );
        }
    }
    return "";
}

/**
 * @brief Resolve a combined rank and name entry of the form "k_Bacteria" into the full rank
 * and the name, i.e. "Kingdom" and "Bacteria".
 *
 * The function returns a pair of `{ "rank", "name" }`.
 *
 * The expected format of the input string is "x_abc", where "x" is a rank name abbreviation
 * and "abc" is a taxon name. If the string is in this format, it is split and the rank name
 * abbreviation is resolved.
 * If this abbreviation is valid, the rank (first) and the name (second) are returned.
 * See rank_from_abbreviation() for the list of valid rank name abbreviations.
 * The number of underscores is irrelevant, that is, `C___Mammalia` also works and will return
 * `{ "Class", "Mammalia" }`.
 *
 * If any of the conditions is not met (either, the string does not start with "x_", or the rank
 * name abbreviation is invalid), the rank is left empty, and the whole given string is used as
 * name. Thus, this function also works on normal taxon names.
 */
std::pair< std::string, std::string > resolve_rank_abbreviation( std::string const& entry )
{
    std::string rank = "";
    std::string name = entry;

    // Check whether the name is of the form "X_something".
    // If so, use it to split off the rank name and resolve the abbreviation.
    if( entry.size() >= 2 && entry[1] == '_' ) {
        rank = rank_from_abbreviation( entry[0] );
    }

    // If the previous step was successful and yielded a valid rank name,
    // shorten the actual name accordingly.
    if( rank != "" ) {
        size_t pos = entry.find_first_not_of( "_", 1 );
        name = entry.substr( pos );
    }

    return { rank, name };
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
 * @brief Return a string vector representation of a given Taxon and its super-taxa.
 *
 * The function returns a vector with the names of all super-taxa of the given Taxon (and the taxon
 * itself).
 *
 * For example, when given the last Taxon in this Taxonomy:
 *
 *     Tax_1
 *         Tax_1
 *             Tax_2
 *
 * the function gives a vector with strings `{ "Tax_1", "Tax_1", "Tax_2" }`.
 */
std::vector<std::string> taxonomic_vector( Taxon const& taxon )
{
    // Start with an empty vector that will store the super-taxa of the given taxon.
    std::vector<std::string> taxa;

    // Add taxa in reverse order: the deepest taxon will be stored first.
    // This is fast with a vector.
    Taxon const* r = &taxon;
    while( r != nullptr ) {
        taxa.push_back( r->name() );
        r = r->parent();
    }

    // Now reverse and return the result.
    std::reverse( taxa.begin(), taxa.end() );
    return taxa;
}

} // namespace taxonomy
} // namespace genesis
