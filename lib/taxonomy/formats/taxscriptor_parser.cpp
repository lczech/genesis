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
 * @brief Implementation of TaxscriptorParser class.
 *
 * @file
 * @ingroup taxonomy
 */

#include "taxonomy/formats/taxscriptor_parser.hpp"

#include "taxonomy/taxon.hpp"
#include "taxonomy/taxscriptor.hpp"
#include "utils/text/string.hpp"

#include <algorithm>
#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Parsing
// =================================================================================================

/**
 * @brief Parse a taxonomic description string into a Taxscriptor object and return it.
 *
 * See @link TaxscriptorParser the class description@endlink for details on what this parser does.
 */
Taxscriptor TaxscriptorParser::from_string( std::string const& taxscriptor ) const
{
    std::vector< std::string > elements;

    // Split the given string, while keeping empty parts.
    elements = utils::split( taxscriptor, delimiters_, false );

    // If there are no elements, the string was empty. Nothing to do.
    if( elements.size() == 0 ) {
        assert( taxscriptor == "" );
        return Taxscriptor();
    }

    // Remove white spaces.
    if( trim_whitespaces_ ) {
        for( auto& r : elements ) {
            r = utils::trim( r );
        }
    }

    // The first name in the list of sub-taxa must not be empty.
    if( elements.front() == "" ) {
        throw std::runtime_error( "Cannot use Taxscriptor if first Taxon is empty." );
    }

    // The last name is ommited if empty.
    if( remove_trailing_delimiter_ && elements.back() == "" ) {
        elements.pop_back();
    }

    // Resolve empty elements.
    std::string prev_name;
    for( size_t i = 0; i < elements.size(); ++i ) {
        auto& name = elements[i];

        // If a sub-taxon is empty, use the super-taxon.
        // As we previously checked that the first taxon is not empty, this is well-formed.
        if( name == "" ) {
            assert( i > 0 && prev_name != "" );
            name = prev_name;
        }

        prev_name = name;
    }

    return Taxscriptor( elements );
}

/**
 * @brief Shortcut function alias for from_string().
 *
 * This shortcut enables to use a TaxscriptorParser object as functor.
 */
Taxscriptor TaxscriptorParser::operator() ( std::string const& taxscriptor ) const
{
    return from_string( taxscriptor );
}

/**
 * @brief Helper function to turn a Taxon into a Taxscriptor.
 *
 * This function is probably not need often, as the Taxscriptor is a helper object from
 * a taxonomic description string towards a Taxon object, but not the other way round.
 * In order to get the string from a Taxon, see the TaxscriptorGenerator class instead.
 *
 * However, this function might still be useful in some cases. You never know.
 */
Taxscriptor TaxscriptorParser::from_taxon(  Taxon const& taxon ) const
{
    // Start with an empty vector that will store the super-taxa of the given taxon.
    std::vector<std::string> elements;

    // Add taxa in reverse order: the deepest taxon will be stored first.
    // This is fast with a vector.
    Taxon const* r = &taxon;
    while( r != nullptr ) {
        elements.push_back( r->name() );
        r = r->parent();
    }

    // Now reverse and return the result.
    std::reverse( elements.begin(), elements.end() );
    return Taxscriptor( elements );
}

/**
 * @brief Shortcut function alias for from_taxon().
 *
 * This shortcut enables to use a TaxscriptorParser object as functor.
 */
Taxscriptor TaxscriptorParser::operator() ( Taxon const& taxon ) const
{
    return from_taxon( taxon );
}

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Set the chars used to split the taxonomic description string.
 *
 * Those chars are used to split the taxon name into its hierarchical parts.
 * Default is ';', as this is the usual value in many databases. See Taxscriptor for details.
 *
 * If this value is set to multiple chars (string longer than 1), any of them is used for splitting.
 *
 * Example: The taxonomic description
 *
 *     Archaea;Euryarchaeota;Halobacteria;
 *
 * is split into "Archaea", "Euryarchaeota" and "Halobacteria".
 */
TaxscriptorParser& TaxscriptorParser::delimiters( std::string const& value )
{
    delimiters_ = value;
    return *this;
}

/**
 * @brief Return the currelty set delimiter chars used to split the taxonomic description string.
 *
 * See @link delimiters( std::string const& value ) the setter@endlink for details.
 */
std::string TaxscriptorParser::delimiters() const
{
    return delimiters_;
}

/**
 * @brief Set whether to trim whitespaces around the taxonomic elements after splitting them.
 *
 * Default is `true`. If set to true, the taxa given are trimmed off white spaces after splitting
 * them. This is helpful if the input string is copied from some spreadsheet application or
 * CSV file, where spaces between cells might be added.
 *
 * If set to `false`, all elements are left as they are.
 *
 * Example: The line
 *
 *     Archaea; Aigarchaeota; Aigarchaeota Incertae Sedis;	11091	class	123
 *
 * contains spaces both between the taxa names (separated by `;`), as well as within the names.
 * Only the former ones will be trimmed, while latter ones are left as they are.
 */
TaxscriptorParser& TaxscriptorParser::trim_whitespaces( bool value )
{
    trim_whitespaces_ = value;
    return *this;
}

/**
 * @brief Return the currently set value whether whitespaces are trimmed off the taxonomic elements.
 *
 * See @link trim_whitespaces( bool value ) the setter@endlink for details.
 */
bool TaxscriptorParser::trim_whitespaces() const
{
    return trim_whitespaces_;
}

/**
 * @brief Set whether to remove an empty taxonomic element at the end, if it occurs.
 *
 * In many taxonomic databases, the taxonomic string representation end with a ';' by default.
 * When splitting such a string, this results in an empty last element. If this option is set to
 * `true` (default), this element is removed from the Taxscriptor.
 *
 * If set to `false`, the element is not removed, but instead treated as a normal "empty" element,
 * which means, it is replaced by the value of the preceeding element. See
 * @link TaxscriptorParser the class description@endlink for details on that.
 */
TaxscriptorParser& TaxscriptorParser::remove_trailing_delimiter( bool value )
{
    remove_trailing_delimiter_ = value;
    return *this;
}

/**
 * @brief Return whether currently trailing delimiters are removed from the taxonomic description
 * string.
 *
 * See @link remove_trailing_delimiter( bool value ) the setter@endlink for details.
 */
bool TaxscriptorParser::remove_trailing_delimiter() const
{
    return remove_trailing_delimiter_;
}

} // namespace taxonomy
} // namespace genesis
