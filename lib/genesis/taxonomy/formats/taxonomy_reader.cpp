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

#include "genesis/taxonomy/formats/taxonomy_reader.hpp"

#include "genesis/taxonomy/functions/taxopath.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxopath.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/text/string.hpp"

#include <assert.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Constructor and Rule of Five
// =================================================================================================

TaxonomyReader::TaxonomyReader()
{
    csv_reader_.separator_chars( "\t" );
}

// =================================================================================================
//     Reading
// =================================================================================================

void TaxonomyReader::from_stream( std::istream& is, Taxonomy& tax ) const
{
    utils::InputStream it( utils::make_unique< utils::StreamInputSource >( is ));
    parse_document( it, tax );
}

void TaxonomyReader::from_file( std::string const& fn, Taxonomy& tax ) const
{
    utils::InputStream it( utils::make_unique< utils::FileInputSource >( fn ));
    parse_document( it, tax );
}

void TaxonomyReader::from_string( std::string const& is, Taxonomy& tax ) const
{
    utils::InputStream it( utils::make_unique< utils::StringInputSource >( is ));
    parse_document( it, tax );
}

Taxonomy TaxonomyReader::from_stream( std::istream& is ) const
{
    Taxonomy res;
    from_stream( is, res );
    return res;
}

Taxonomy TaxonomyReader::from_file( std::string const& fn ) const
{
    Taxonomy res;
    from_file( fn, res );
    return res;
}

Taxonomy TaxonomyReader::from_string( std::string const& is ) const
{
    Taxonomy res;
    from_string( is, res );
    return res;
}

// =================================================================================================
//     Parsing
// =================================================================================================

void TaxonomyReader::parse_document(
    utils::InputStream& it,
    Taxonomy&           tax
) const {
    while( it ) {
        // Get line as name rank pair.
        auto line = parse_line( it );

        if( line.name == "" ) {
            // Maybe here should be a warning instead of silent skipping...
            continue;
        }

        // Parse the taxopath and add it to the taxonomy.
        auto& taxon = add_from_taxopath(
            tax,
            taxopath_parser_.from_string( line.name ),
            expect_strict_order_
        );

        // Set the rank and ID.
        taxon.rank( line.rank );
        taxon.id( line.id );
    }
}

TaxonomyReader::Line TaxonomyReader::parse_line(
    utils::InputStream& it
) const {
    // Get the fields of the current line.
    auto fields = csv_reader_.parse_line( it );

    // Helper function to find the correct field for a property, or throw if invalid.
    auto get_field = [&] ( int field_pos, std::string field_name ) {
        // Check if field is actually "active".
        if( field_pos < 0 ) {
            return std::string();
        }

        // Cast so that array lookup does not complain.
        assert( field_pos >= 0 );
        auto pos = static_cast< size_t >( field_pos );

        // Check for invalid position.
        if( pos >= fields.size() ) {
            throw std::out_of_range(
                "Invalid position for taxonomy " + field_name + " field while reading. Expect "
                + field_name + " at position " + utils::to_string( pos ) + " (zero-based), "
                + "but the line only contains " + utils::to_string( fields.size() )
                + " fields at line " + utils::to_string( it.line() - 1 ) + "."
            );
        }

        // Return result.
        assert( pos < fields.size() );
        return fields[ pos ];
    };

    // Read fields from line.
    Line result;
    result.name = get_field( name_field_position_, "name" );
    result.rank = get_field( rank_field_position_, "rank" );
    result.id   = get_field( id_field_position_,   "ID" );

    return result;
}

// =================================================================================================
//     Properties
// =================================================================================================

utils::CsvReader& TaxonomyReader::csv_reader()
{
    return csv_reader_;
}

TaxopathParser& TaxonomyReader::taxopath_parser()
{
    return taxopath_parser_;
}

TaxonomyReader& TaxonomyReader::name_field_position( int value )
{
    // We could also use size_t instead of int here to avoid setting the value to sub-zero.
    // However, now we have consistency with the rank field position, which is nicer.
    if( value < 0 ) {
        throw std::out_of_range(
            "Cannot set TaxonomyReader::name_field_position to a value below zero."
        );
    }
    name_field_position_ = value;
    return *this;
}

int TaxonomyReader::name_field_position() const
{
    return name_field_position_;
}

TaxonomyReader& TaxonomyReader::rank_field_position( int value )
{
    rank_field_position_ = value;
    return *this;
}

int TaxonomyReader::rank_field_position() const
{
    return rank_field_position_;
}

TaxonomyReader& TaxonomyReader::id_field_position( int value )
{
    id_field_position_ = value;
    return *this;
}

int TaxonomyReader::id_field_position() const
{
    return id_field_position_;
}

TaxonomyReader& TaxonomyReader::expect_strict_order( bool value )
{
    expect_strict_order_ = value;
    return *this;
}

bool TaxonomyReader::expect_strict_order() const
{
    return expect_strict_order_;
}

} // namespace taxonomy
} // namespace genesis
