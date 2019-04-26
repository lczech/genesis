/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
 * @brief Implementation of TaxopathParser class.
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/formats/taxopath_parser.hpp"

#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxopath.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Parsing
// =================================================================================================

Taxopath TaxopathParser::parse( std::string const& taxopath ) const
{
    std::vector< std::string > elements;

    // Split the given string, while keeping empty parts.
    elements = utils::split( taxopath, delimiters_, false );

    // If there are no elements, the string was empty. Nothing to do.
    if( elements.size() == 0 ) {
        assert( taxopath == "" );
        return Taxopath();
    }

    // Remove white spaces.
    if( trim_whitespaces_ ) {
        for( auto& r : elements ) {
            r = utils::trim( r );
        }
    }

    // The first name in the list of sub-taxa must not be empty.
    if( elements.front() == "" ) {
        throw std::runtime_error( "Cannot use Taxopath if first Taxon is empty." );
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

    return Taxopath( elements );
}

Taxopath TaxopathParser::parse(  Taxon const& taxon ) const
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
    return Taxopath( elements );
}

} // namespace taxonomy
} // namespace genesis
