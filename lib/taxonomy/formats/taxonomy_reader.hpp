#ifndef GENESIS_TAXONOMY_FORMATS_TAXONOMY_READER_H_
#define GENESIS_TAXONOMY_FORMATS_TAXONOMY_READER_H_

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

#include "taxonomy/formats/taxscriptor_parser.hpp"
#include "utils/formats/csv/reader.hpp"

#include <string>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {

    class InputStream;

} // namespace utils

namespace taxonomy {

    class Taxonomy;

// =================================================================================================
//     Taxonomy Reader
// =================================================================================================

/**
 * @brief Read Taxonomy file formats.
 *
 * This reader populates a Taxonomy. It supports to read
 *
 *   * from_stream()
 *   * from_file()
 *   * from_string()
 *
 * Exemplary usage:
 *
 *     std::string infile = "path/to/taxonomy.txt";
 *     Taxonomy tax;
 *
 *     TaxonomyReader()
 *         .rank_field_position( 2 )
 *         .expect_strict_order( true )
 *         .from_file( infile, tax );
 *
 * It expects one taxon per input line. This line can also contain other information, for example
 *
 *     Archaea;Crenarchaeota;Thermoprotei;Desulfurococcales;	14	order	119
 *
 * In order to separate the fields of the input, a @link utils::CsvReader CsvReader@endlink is used.
 * By default, all its properties except for the separator chars are left at their default values.
 * The separator char is set to a tab `\t` instead of a comma, as this is more common for
 * taxonomy files.
 *
 * Use the getter csv_reader() to access the CsvReader and change its behaviour, for example, to
 * change the field separator char. Also, all other properties of the CsvReader can be adjusted
 * in order to suit any char-separated input format.
 *
 * Once the fields of a line are split, this reader uses its properties
 * @link name_field_position( int value ) name_field_position()@endlink and
 * @link rank_field_position( int value ) rank_field_position()@endlink to determine which of the
 * fields represent the taxon name and its rank, respectively. For example, given the line from
 * above, those would have to be set to `0` and `2`. All other fields of the line are ignored,
 * which in the example are "14" and "119".
 *
 * The taxon name is expected to be a taxonomic description string. This is what we call a string
 * consisting of the different parts of the taxonomic hierarchy, usually separated by semicola.
 * See Taxscriptor for a description of the expected format.
 *
 * This string is split into its @link Taxon Taxa@endlink using a TaxscriptorParser.
 * In order to change the behaviour of this splitting, access the parser via taxscriptor_parser().
 *
 * In summary, by default, this reader reads tab-separated lines and expects the taxonomy entry
 * to be the first (or only) field in the line and to be a taxonomic description in the format
 * descript at Taxscriptor.
 */
class TaxonomyReader
{
public:

    // ---------------------------------------------------------------------
    //     Enums and Structs
    // ---------------------------------------------------------------------

    /**
     * @brief Internal helper structure that stores the relevant data of one line while reading.
     */
    struct Line
    {
        std::string name;
        std::string rank;
    };

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    TaxonomyReader();
    ~TaxonomyReader() = default;

    TaxonomyReader( TaxonomyReader const& ) = default;
    TaxonomyReader( TaxonomyReader&& )      = default;

    TaxonomyReader& operator= ( TaxonomyReader const& ) = default;
    TaxonomyReader& operator= ( TaxonomyReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    void from_stream( std::istream&      is, Taxonomy& tax ) const;
    void from_file  ( std::string const& fn, Taxonomy& tax ) const;
    void from_string( std::string const& is, Taxonomy& tax ) const;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    void parse_document(
        utils::InputStream& it,
        Taxonomy&           tax
    ) const;

    Line parse_line(
        utils::InputStream& it
    ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    utils::CsvReader&  csv_reader();
    TaxscriptorParser& taxscriptor_parser();

    TaxonomyReader& name_field_position( int value );
    int             name_field_position() const;

    TaxonomyReader& rank_field_position( int value );
    int             rank_field_position() const;

    TaxonomyReader& expect_strict_order( bool value );
    bool            expect_strict_order() const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    utils::CsvReader  csv_reader_;
    TaxscriptorParser taxscriptor_parser_;

    int  name_field_position_ =  0;
    int  rank_field_position_ = -1;
    bool expect_strict_order_ = false;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
