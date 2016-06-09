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

#include "taxonomy/formats/taxonomy_reader.hpp"

#include "taxonomy/functions.hpp"
#include "taxonomy/taxon.hpp"
#include "taxonomy/taxonomy.hpp"

#include "utils/core/fs.hpp"
#include "utils/io/counting_istream.hpp"
#include "utils/text/string.hpp"

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
    auto it = utils::CountingIstream( is );

    while( it ) {
        auto line = parse_line( it );

        if( line.name == "" ) {
            // Maybe here should be a warning instead of silent skipping...
            continue;
        }

        add_children_from_string( tax, line.name );
    }
}

void TaxonomyReader::from_file( std::string const& fn, Taxonomy& tax ) const
{
    if( ! utils::file_exists( fn ) ) {
        throw std::runtime_error( "File '" + fn + "' not found." );
    }

    std::ifstream ifs( fn );
    if( ifs.fail() ) {
        throw std::runtime_error( "Cannot read from file '" + fn + "'." );
    }

    from_stream( ifs, tax );
}

void TaxonomyReader::from_string( std::string const& fs, Taxonomy& tax ) const
{
    std::istringstream iss( fs );
    from_stream( iss, tax );
}

// =================================================================================================
//     Parsing
// =================================================================================================

TaxonomyReader::Line TaxonomyReader::parse_line(
    utils::CountingIstream& it
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

    return result;
}

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Get the CsvReader used for reading a taxonomy file.
 *
 * This can be used to modify the reading behaviour, particularly values like the separator chars
 * within the lines of the file. See CsvReader for details about those properties.
 */
utils::CsvReader& TaxonomyReader::csv_reader()
{
    return csv_reader_;
}

/**
 * @brief Set the position of the field in each line where the taxon name is located.
 *
 * This value determines at with position (zero based) the field for the taxon name is located.
 *
 * For example, in a taxonomy file with entries like
 *
 *     Archaea;Crenarchaeota;Thermoprotei;	7	class	119
 *
 * this value would have to be set to `0`, as this is where the taxon name is found.
 *
 * By default, this value is set to `0`, that is, the first field. As it does not make sense to
 * skip this value, it cannot be set to values below zero - which is different from
 * @link rank_field_position( int value ) rank_field_position@endlink. An exception is thrown
 * should this be attempted.
 */
TaxonomyReader& TaxonomyReader::name_field_position( int value )
{
    if( value < 0 ) {
        throw std::out_of_range(
            "Cannot set TaxonomyReader::name_field_position to a value below zero."
        );
    }
    name_field_position_ = value;
    return *this;
}

/**
 * @brief Get the currently set position of the field in each line where the taxon name is located.
 *
 * See @link name_field_position( int value ) the setter@endlink of this function for details.
 */
int TaxonomyReader::name_field_position() const
{
    return name_field_position_;
}

/**
 * @brief Set the position of the field in each line where the rank name is located.
 *
 * This value determines at with position (zero based) the field for the rank name is located.
 *
 * For example, in a taxonomy file with entries like
 *
 *     Archaea;Crenarchaeota;Thermoprotei;	7	class	119
 *
 * this value would have to be set to `2`, as this is where the rank name "class" is found.
 *
 * If the file does not contain any rank names, or if this field should be skipped, set it to
 * a value of `-1`. This is also the default.
 */
TaxonomyReader& TaxonomyReader::rank_field_position( int value )
{
    rank_field_position_ = value;
    return *this;
}

/**
 * @ briefGet the currently set position of the field in each line where the rank name is located.
 *
 * See @link rank_field_position( int value ) the setter@endlink of this function for details.
 */
int TaxonomyReader::rank_field_position() const
{
    return rank_field_position_;
}

/**
 * @brief Set whether the reader expects a strict order of taxa.
 *
 * In a strictly ordered taxonomy file, the super-groups have to be listed before any sub-groups.
 *
 * For example, the list
 *
 *     Archaea;
 *     Archaea;Aenigmarchaeota;
 *     Archaea;Crenarchaeota;
 *     Archaea;Crenarchaeota;Thermoprotei;
 *
 * is in strict order.
 *
 * If this property is set to `true`, the reader expects this ordering and throws an exception
 * if there is a violation, that is, if there is a sub-group in the list without a previous
 * entry of its super-group (recursively). This is useful to check a file for consistency, e.g.,
 * it might happen that some super-group is misspelled by accident.
 *
 * If set to `false` (default), the order is ignored and all super-groups are created if necessary.
 */
TaxonomyReader& TaxonomyReader::expect_strict_order( bool value )
{
    expect_strict_order_ = value;
    return *this;
}

/**
 * @brief Return whether currently the reader expects a strict order of taxa.
 *
 * See @link expect_strict_order( bool value ) the setter@endlink for more information.
 */
bool TaxonomyReader::expect_strict_order() const
{
    return expect_strict_order_;
}

} // namespace taxonomy
} // namespace genesis
