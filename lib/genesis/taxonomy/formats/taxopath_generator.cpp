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
 * @brief Implementation of TaxopathGenerator class.
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/formats/taxopath_generator.hpp"

#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxopath.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Generating
// =================================================================================================

std::string TaxopathGenerator::to_string( Taxopath const& taxopath ) const
{
    std::string res;

    if( trim_nested_duplicates_ ) {
        // We are going to delete elements, so first make a copy.
        auto cpy = taxopath.elements();

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
        res = utils::join( taxopath.elements(), delimiter_ );
    }

    if( append_delimiter_ ) {
        return res + delimiter_;
    } else {
        return res;
    }
}

std::string TaxopathGenerator::operator() ( Taxopath const& taxopath ) const
{
    return to_string( taxopath );
}

std::string TaxopathGenerator::to_string( Taxon const& taxon ) const
{
    // This implementation is probably not the fastest, but it is simple and kind of elegant.
    // Start with an empty vector that will store the super-taxa of the given taxon.
    std::vector<std::string> taxa;

    // Add taxa in reverse order: the deepest taxon will be stored first.
    // This is fast with a vector.
    Taxon const* r = &taxon;
    while( r != nullptr ) {
        switch( field_ ) {
            case TaxonField::kName: {
                taxa.push_back( r->name() );
                break;
            }
            case TaxonField::kRank: {
                taxa.push_back( r->rank() );
                break;
            }
            case TaxonField::kId: {
                taxa.push_back( r->id() );
                break;
            }
            default: {
                throw std::invalid_argument( "Invalid TaxonField in TaxopathGenerator." );
            }
        }
        r = r->parent();
    }

    // If wanted, set all taxa to an empty string for which the super-taxon has the same name/field.
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

std::string TaxopathGenerator::operator() ( Taxon const& taxon ) const
{
    return to_string( taxon );
}

// =================================================================================================
//     Properties
// =================================================================================================

TaxopathGenerator& TaxopathGenerator::field( TaxonField value )
{
    field_ = value;
    return *this;
}

TaxopathGenerator::TaxonField TaxopathGenerator::field() const
{
    return field_;
}

TaxopathGenerator& TaxopathGenerator::delimiter( std::string const& value )
{
    delimiter_ = value;
    return *this;
}

std::string TaxopathGenerator::delimiter() const
{
    return delimiter_;
}

TaxopathGenerator& TaxopathGenerator::trim_nested_duplicates( bool value )
{
    trim_nested_duplicates_ = value;
    return *this;
}

bool TaxopathGenerator::trim_nested_duplicates() const
{
    return trim_nested_duplicates_;
}

TaxopathGenerator& TaxopathGenerator::append_delimiter( bool value )
{
    append_delimiter_ = value;
    return *this;
}

bool TaxopathGenerator::append_delimiter() const
{
    return append_delimiter_;
}

} // namespace taxonomy
} // namespace genesis
