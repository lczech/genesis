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

#include "taxonomy/functions/taxscriptor.hpp"
#include "taxonomy/taxon.hpp"
#include "taxonomy/taxonomy.hpp"
#include "taxonomy/taxscriptor.hpp"

#include "utils/core/fs.hpp"
#include "utils/core/std.hpp"
#include "utils/io/input_stream.hpp"
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

/**
 * @brief Default constructor.
 *
 * Initializes the CsvReader so that tabs are used as field separators instead of commata.
 */
TaxonomyReader::TaxonomyReader()
{
    csv_reader_.separator_chars( "\t" );
}

// =================================================================================================
//     Reading
// =================================================================================================

/**
 * @brief Read taxonomy data until the end of the stream is reached,
 * and add the contents to a Taxonomy.
 */
void TaxonomyReader::from_stream( std::istream& is, Taxonomy& tax ) const
{
    utils::InputStream it( utils::make_unique< utils::StreamInputSource >( is ));
    parse_document( it, tax );
}

/**
 * @brief Read a taxonomy file and add its contents to a Taxonomy.
 */
void TaxonomyReader::from_file( std::string const& fn, Taxonomy& tax ) const
{
    utils::InputStream it( utils::make_unique< utils::FileInputSource >( fn ));
    parse_document( it, tax );
}

/**
 * @brief Read a string with taxonomy data and add its contents to a Taxonomy.
 */
void TaxonomyReader::from_string( std::string const& is, Taxonomy& tax ) const
{
    utils::InputStream it( utils::make_unique< utils::StringInputSource >( is ));
    parse_document( it, tax );
}

// =================================================================================================
//     Parsing
// =================================================================================================

/**
 * @brief Parse all data from an InputStream into a Taxonomy object.
 */
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

        // Parse the taxscriptor and add it to the taxonomy.
        auto& taxon = add_from_taxscriptor(
            tax,
            taxscriptor_parser_.from_string( line.name ),
            expect_strict_order_
        );

        // Set the rank.
        taxon.rank( line.rank );
    }
}

/**
 * @brief Read a single line of a taxonomy file and return the contained name and rank.
 *
 * The name is expected to be a taxonomic description string. See Taxscriptor for details on that
 * format.
 */
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

    return result;
}

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Get the @link utils::CsvReader CsvReader@endlink used for reading a taxonomy file.
 *
 * This can be used to modify the reading behaviour, particularly values like the separator chars
 * within the lines of the file. By default, the TaxonomyReader uses a tab `\t` char to separate
 * fields, which is different from the comma ',' that is used as default by the CsvReader.
 *
 * It is also possible to change other properties of the CsvReader, for example escaping behaviour,
 * if the input data needs special treatment in those regards.
 *
 * See @link utils::CsvReader CsvReader@endlink for details about those properties.
 */
utils::CsvReader& TaxonomyReader::csv_reader()
{
    return csv_reader_;
}

/**
 * @brief Get the TaxscriptorParser used for parsing taxonomic description strings.
 *
 * The name field is expected to be a taxonomic description string. It is turned into a Taxon
 * using the settings of the TaxscriptorParser. See there for details. See Taxscriptor for
 * a description of the expected string format.
 */
TaxscriptorParser& TaxonomyReader::taxscriptor_parser()
{
    return taxscriptor_parser_;
}

/**
 * @brief Set the position of the field in each line where the taxon name (Taxscriptor) is located.
 *
 * This value determines at with position (zero based) the field for the taxon name is located.
 *
 * For example, in a taxonomy file with entries like
 *
 *     Archaea;Crenarchaeota;Thermoprotei;	7	class	119
 *
 * this value would have to be set to `0`, as this is where the taxon name is found. This reader
 * expects the taxon name to be a Taxscriptor. This is what we call a string of taxonomic hierarchy
 * elements, usually separated by semicola. See Taxscriptor for details.
 *
 * By default, this value is set to `0`, that is, the first field. As it does not make sense to
 * skip this value, it cannot be set to values below zero - which is different from
 * @link rank_field_position( int value ) rank_field_position@endlink. An exception is thrown
 * should this be attempted.
 */
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
