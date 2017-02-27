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

#include "genesis/taxonomy/functions/ranks.hpp"

#include "genesis/utils/text/string.hpp"

#include <unordered_map>

namespace genesis {
namespace taxonomy {

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

} // namespace taxonomy
} // namespace genesis
