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
 * @brief Implementation of TaxscriptorGenerator class.
 *
 * @file
 * @ingroup taxonomy
 */

#include "taxonomy/formats/taxscriptor_generator.hpp"

#include "taxonomy/taxon.hpp"
#include "taxonomy/taxonomy.hpp"
#include "taxonomy/taxscriptor.hpp"
#include "utils/text/string.hpp"

#include <algorithm>
#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Generating
// =================================================================================================

/**
 * @brief Return a string representation of a Taxscriptor.
 *
 * This generator function uses the settings of this class to generate the string.
 */
std::string TaxscriptorGenerator::to_string( Taxscriptor const& taxscriptor ) const
{
    std::string res;

    if( trim_nested_duplicates_ ) {
        // We are going to delete elements, so first make a copy.
        auto cpy = taxscriptor.elements();

        // Delete element if the one before it is the same.
        // Skip the first and the last one.
        for( size_t i = cpy.size() - 1; i >= 1; --i ) {
            if( cpy[i] == cpy[i-1] ) {
                cpy[i] = "";
            }
        }
        res = utils::join( cpy, delimiter_ );

    } else {
        // Without trimming, the result is easy to create.
        res = utils::join( taxscriptor.elements(), delimiter_ );
    }

    if( append_delimiter_ ) {
        return res + delimiter_;
    } else {
        return res;
    }
}

/**
 * @brief Shortcut function alias for
 * @link to_string( Taxscriptor const& taxscriptor ) const to_string( Taxscriptor )@endlink.
 *
 * This shortcut enables to use a TaxscriptorGenerator object as functor.
 */
std::string TaxscriptorGenerator::operator() ( Taxscriptor const& taxscriptor ) const
{
    return to_string( taxscriptor );
}

/**
 * @brief Return a string representation of a Taxon.
 *
 * This generator function uses the settings of this class to generate the string.
 */
std::string TaxscriptorGenerator::to_string( Taxon const& taxon ) const
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
    if( trim_nested_duplicates_ ) {
        for( size_t i = 0; i < taxa.size() - 1; ++i ) {
            if( taxa[i] == taxa[i+1] ) {
                taxa[i] = "";
            }
        }
    }

    // Now reverse and return the joined result.
    std::reverse( taxa.begin(), taxa.end() );
    auto res = utils::join( taxa, delimiter_ );

    if( append_delimiter_ ) {
        return res + delimiter_;
    } else {
        return res;
    }
}

/**
 * @brief Shortcut function alias for
 * @link to_string( Taxon const& taxon ) const to_string( Taxon )@endlink.
 *
 * This shortcut enables to use a TaxscriptorGenerator object as functor.
 */
std::string TaxscriptorGenerator::operator() ( Taxon const& taxon ) const
{
    return to_string( taxon );
}

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Set the string used to join the taxonomic description string elements.
 *
 * This value is used in between the elements of the taxonomic description string.
 * Default is ';', as this is the usual value in many databases. See Taxscriptor for details.
 */
TaxscriptorGenerator& TaxscriptorGenerator::delimiter( std::string const& value )
{
    delimiter_ = value;
    return *this;
}

/**
 * @brief Return the currelty set value used to join the taxonomic description string elements.
 *
 * See @link delimiter( std::string const& value ) the setter@endlink for details.
 */
std::string TaxscriptorGenerator::delimiter() const
{
    return delimiter_;
}

/**
 * @brief Set whether lower level taxa are omitted if they are the same as the next higher level
 * one.
 *
 * If set to `true`, lower level names are set to empty if they are the same as higher level names.
 * Default is `false`, that is, nothing is trimmed.
 *
 * Example: For a Taxscriptor
 *
 *     [ "Tax_1", "Tax_1", "Tax_2" ]
 *
 * @link to_string( Taxscriptor const& taxscriptor ) const the generator function@endlink returns
 * `Tax_1;Tax_1;Tax_2`, and respectively `Tax_1;;Tax_2` with trimming nested duplicates.
 */
TaxscriptorGenerator& TaxscriptorGenerator::trim_nested_duplicates( bool value )
{
    trim_nested_duplicates_ = value;
    return *this;
}

/**
 * @brief Return the currently set value whether to trim nested duplicates of taxa names.
 *
 * See @link trim_nested_duplicates( bool value ) the setter@endlink for details.
 */
bool TaxscriptorGenerator::trim_nested_duplicates() const
{
    return trim_nested_duplicates_;
}

/**
 * @brief Set whether to append the delimiter string to the generated string.
 *
 * In many taxonomic databases, the string representation ends with the delimiter char, for example
 *
 *     Animalia;Vertebrata;Mammalia;Carnivora;
 *
 * This setting determines whether this last delimiter is appended or not.
 */
TaxscriptorGenerator& TaxscriptorGenerator::append_delimiter( bool value )
{
    append_delimiter_ = value;
    return *this;
}

/**
 * @brief Return whether currently a delimiter is appended to the taxonomic description string.
 *
 * See @link append_delimiter( bool value ) the setter@endlink for details.
 */
bool TaxscriptorGenerator::append_delimiter() const
{
    return append_delimiter_;
}

} // namespace taxonomy
} // namespace genesis
