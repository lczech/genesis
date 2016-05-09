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

#include "taxonomy/rank.hpp"
#include "taxonomy/taxonomy.hpp"

#include "utils/text/string.hpp"

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
 * @brief Find a Rank with a given name by recursively searching the taxonomy.
 */
Rank const* find_taxon( Taxonomy const& tax, std::string const& name )
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
 * @brief Find a Rank with a given name by recursively searching the taxonomy.
 */
Rank* find_taxon( Taxonomy& tax, std::string const& name )
{
    // Avoid code duplication according to Scott Meyers.
    auto const& ctax = static_cast<Taxonomy const>( tax );
    return const_cast<Rank*>( find_taxon( ctax, name ));
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
 * If any of the later taxa are empty, the taxon name of the previous level rank is used instead.
 * This is useful for unspecified ranks in deeper taxonomies.
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
 * @return           Return value of the function is the deepest Rank that was given in the
 *                   `children` string, i.e., the last element after splitting the string.
 */
Rank& add_children_from_string(
    Taxonomy&          taxonomy,
    std::string const& children,
    std::string const& delimiters,
    bool               trim_whitespaces
){
    // Split the given string, while keeping empty parts.
    auto ranks = utils::split( children, delimiters, false );

    // If there are no elements, the string was empty. This is illegal.
    if( ranks.size() == 0 ) {
        assert( children == "" );
        throw std::runtime_error( "Cannot add empty child to taxonomy." );
    }

    // Remove white spaces.
    if( trim_whitespaces ) {
        for( auto& r : ranks ) {
            r = utils::trim( r );
        }
    }

    // The first name in the list of sub-ranks must not be empty.
    if( ranks.front() == "" ) {
        throw std::runtime_error( "Cannot add children to taxomonmy if first child is empty." );
    }

    // Prepare: we need a Taxonomy to add children to. This pointer is updated in the loop so that
    // we go deeper and deeper into the taxonomy. Also, we keep track of the previously assigned
    // name, for cases where a rank is empty.
    Taxonomy*   cur_rank = &taxonomy;
    std::string prev_name;

    // Add the names to the Taxonomy.
    for( auto name : ranks ) {
        // If a sub-rank is empty, use the super-rank.
        // As we previously checked that the first rank is not empty, this is well-formed.
        if( name == "" ) {
            name = prev_name;
        }

        cur_rank  = &cur_rank->add_child( name );
        prev_name = name;
    }

    // Convert to Rank. This is always legal because we have ensured that we are adding at least
    // one sub-rank to the taxonomy, and in the loop set cur_rank to this rank, so that it is
    // actually of dynamic type Rank.
    return static_cast< Rank& >( *cur_rank );
}

/**
 * @brief Remove all sub-ranks that are deeper in the hierarchy than the given level.
 *
 * That is, providing `level = 0` has the same effect as calling Taxonomy::clear_children() on the
 * given Taxonomy; `level = 1` has this effect for the children of the given Taxonomy; and so on.
 */
void remove_ranks_deeper_than( Taxonomy& tax, size_t level )
{
    if( level == 0 ) {
        tax.clear_children();
    } else {
        for( auto& c : tax ) {
            remove_ranks_deeper_than( c, level - 1 );
        }
    }
}

// =================================================================================================
//     Print and Output
// =================================================================================================

/**
 * @brief Local helper function to print the contents of a Taxonomy or Rank to an ostream.
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
 * @brief Print the contents of a Taxonomy, i.e., all nested ranks.
 */
std::ostream& operator << ( std::ostream& out, Taxonomy const& tax )
{
    print_to_ostream( out, tax, 0 );
    return out;
}

/**
 * @brief Return the taxonomic string representation of a given Rank.
 *
 * This function is the reverse of add_children_from_string(). It returns a string with all
 * names of the super-taxa of the given rank, concatenated using `delimiter`.
 * If `trim_nested_duplicates` is set to true (default), lower level names are set to empty if they
 * are the same as higher level names.
 *
 * Example: For a rank with this list of parents
 *
 *     Tax_1
 *         Tax_1
 *             Tax_2
 *
 * the functions returns "Tax_1;;Tax_2", and respectively "Tax_1;Tax_1;Tax_2" without trimming
 * nested duplicates.
 */
std::string taxonomic_string( Rank const& rank, std::string delimiter, bool trim_nested_duplicates )
{
    // This implementation is probably not the fastest, but it is simple and kind of elegant.
    // Start with an empty vector that will store the super-taxa of the given rank.
    std::vector<std::string> taxa;

    // Add taxa in reverse order: the deepest rank will be stored first.
    // This is fast with a vector.
    Rank const* r = &rank;
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
