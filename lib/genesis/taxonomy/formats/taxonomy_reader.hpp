#ifndef GENESIS_TAXONOMY_FORMATS_TAXONOMY_READER_H_
#define GENESIS_TAXONOMY_FORMATS_TAXONOMY_READER_H_

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

#include "genesis/taxonomy/formats/taxopath_parser.hpp"
#include "genesis/utils/formats/csv/reader.hpp"

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
 * @link name_field_position( int value ) name_field_position()@endlink,
 * @link rank_field_position( int value ) rank_field_position()@endlink and
 * @link id_field_position( int value ) id_field_position()@endlink to determine which of the
 * fields represent the taxon name, its rank, and its ID, respectively. For example, given the line
 * from above, those would have to be set to `0` and `2`, and `1`, respectively. That is, the first
 * field of the line is the name of the Taxon, the third ("order") the rank, and the second ("14")
 * its ID. All other fields of the line are ignored, which in the example is the field "119".
 *
 * The taxon name is expected to be a taxonomic path string. This is what we call a string
 * consisting of the different parts of the taxonomic hierarchy, usually separated by semicola.
 * See Taxopath for a description of the expected format.
 *
 * This string is split into its @link Taxon Taxa@endlink using a TaxopathParser.
 * In order to change the behaviour of this splitting, access the parser via taxopath_parser().
 *
 * In summary, by default, this reader reads tab-separated lines and expects the taxonomy entry
 * to be the first (or only) field in the line and to be a taxonomic path in the format
 * described at Taxopath.
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
        std::string id;
    };

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    /**
     * @brief Default constructor.
     *
     * Initializes the CsvReader so that tabs are used as field separators instead of commata.
     */
    TaxonomyReader();
    ~TaxonomyReader() = default;

    TaxonomyReader( TaxonomyReader const& ) = default;
    TaxonomyReader( TaxonomyReader&& )      = default;

    TaxonomyReader& operator= ( TaxonomyReader const& ) = default;
    TaxonomyReader& operator= ( TaxonomyReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    /**
     * @brief Read taxonomy data until the end of the stream is reached,
     * and add the contents to a Taxonomy.
     */
    void from_stream( std::istream&      is, Taxonomy& tax ) const;

    /**
     * @brief Read a taxonomy file and add its contents to a Taxonomy.
     */
    void from_file  ( std::string const& fn, Taxonomy& tax ) const;

    /**
     * @brief Read a string with taxonomy data and add its contents to a Taxonomy.
     */
    void from_string( std::string const& is, Taxonomy& tax ) const;

    /**
     * @brief Read taxonomy data until the end of the stream is reached,
     * and return the Taxonomy.
     */
    Taxonomy from_stream( std::istream&      is ) const;

    /**
     * @brief Read a taxonomy file and and return the Taxonomy.
     */
    Taxonomy from_file  ( std::string const& fn ) const;

    /**
     * @brief Read a string with taxonomy data and and return the Taxonomy.
     */
    Taxonomy from_string( std::string const& is ) const;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    /**
     * @brief Parse all data from an InputStream into a Taxonomy object.
     */
    void parse_document(
        utils::InputStream& it,
        Taxonomy&           tax
    ) const;

    /**
     * @brief Read a single line of a taxonomy file and return the contained name and rank.
     *
     * The name is expected to be a taxonomic path string. See Taxopath for details on that format.
     */
    Line parse_line(
        utils::InputStream& it
    ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

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
    utils::CsvReader&  csv_reader();

    /**
     * @brief Get the TaxopathParser used for parsing taxonomic path strings.
     *
     * The name field is expected to be a taxonomic path string. It is turned into a Taxon
     * using the settings of the TaxopathParser. See there for details. See Taxopath for
     * a path of the expected string format.
     */
    TaxopathParser& taxopath_parser();

    /**
     * @brief Set the position of the field in each line where the taxon name (Taxopath) is located.
     *
     * This value determines at with position (zero based) the field for the taxon name is located.
     *
     * For example, in a taxonomy file with entries like
     *
     *     Archaea;Crenarchaeota;Thermoprotei;	7	class	119
     *
     * this value would have to be set to `0`, as this is where the taxon name is found. This reader
     * expects the taxon name to be a Taxopath. This is what we call a string of taxonomic hierarchy
     * elements, usually separated by semicola. See Taxopath for details.
     *
     * By default, this value is set to `0`, that is, the first field. As it does not make sense to
     * skip this value, it cannot be set to values below zero - which is different from
     * @link rank_field_position( int value ) rank_field_position@endlink. An exception is thrown
     * should this be attempted.
     */
    TaxonomyReader& name_field_position( int value );

    /**
     * @brief Get the currently set position of the field in each line where the taxon name is located.
     *
     * See @link name_field_position( int value ) the setter@endlink of this function for details.
     */
    int             name_field_position() const;

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
    TaxonomyReader& rank_field_position( int value );

    /**
     * @brief Get the currently set position of the field in each line where the rank name is located.
     *
     * See @link rank_field_position( int value ) the setter@endlink of this function for details.
     */
    int             rank_field_position() const;

    /**
     * @brief Set the position of the field in each line where the ID is located.
     *
     * This value determines at with position (zero based) the field for the ID is located.
     *
     * For example, in a taxonomy file with entries like
     *
     *     Archaea;Crenarchaeota;Thermoprotei;	7	class	119
     *
     * this value could have to be set to `1`, as this is where the ID "7" is found.
     *
     * If the file does not contain any IDs, or if this field should be skipped, set it to
     * a value of `-1`. This is also the default.
     */
    TaxonomyReader& id_field_position( int value );

    /**
     * @brief Get the currently set position of the field in each line where the ID is located.
     *
     * See @link id_field_position( int value ) the setter@endlink of this function for details.
     */
    int             id_field_position() const;

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
    TaxonomyReader& expect_strict_order( bool value );

    /**
     * @brief Return whether currently the reader expects a strict order of taxa.
     *
     * See @link expect_strict_order( bool value ) the setter@endlink for more information.
     */
    bool            expect_strict_order() const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    utils::CsvReader  csv_reader_;
    TaxopathParser taxopath_parser_;

    int  name_field_position_ =  0;
    int  rank_field_position_ = -1;
    int  id_field_position_   = -1;
    bool expect_strict_order_ = false;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
