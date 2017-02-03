#ifndef GENESIS_PLACEMENT_FORMATS_JPLACE_READER_H_
#define GENESIS_PLACEMENT_FORMATS_JPLACE_READER_H_

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
 * @ingroup placement
 */

#include <string>
#include <unordered_map>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {
    class InputStream;
    class JsonDocument;
}

namespace placement {
    class Sample;
    class SampleSet;
}

// =================================================================================================
//     Jplace Reader
// =================================================================================================

namespace placement {

/**
 * @brief Read Jplace data.
 *
 * This class provides facilities for reading Jplace files. It supports to read
 *
 *   * from_file()
 *   * from_string()
 *   * from_stream()
 *   * from_document()
 *
 * Exemplary usage:
 *
 *     std::string infile = "path/to/file.jplace";
 *     Sample smp = JplaceReader()
 *         .invalid_number_behaviour( InvalidNumberBehaviour::kCorrect )
 *         .from_file( infile );
 *
 * Using @link invalid_number_behaviour( InvalidNumberBehaviour ) invalid_number_behaviour()@endlink,
 * it is possible to change how the reader reacts to malformed jplace files.
 * See InvalidNumberBehaviour for the valid options.
 *
 * The Jplace format is described in the following publication:
 *
 *     Matsen FA, Hoffman NG, Gallagher A, Stamatakis A. 2012.
 *     A Format for Phylogenetic Placements.
 *     PLoS ONE 7(2): e31009. doi:10.1371/journal.pone.0031009
 *     http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0031009
 *
 * See Sample for the data structure used to store the Pqueries and the reference Tree.
 */
class JplaceReader
{
    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

public:

    JplaceReader()  = default;
    ~JplaceReader() = default;

    JplaceReader( JplaceReader const& ) = default;
    JplaceReader( JplaceReader&& )      = default;

    JplaceReader& operator= ( JplaceReader const& ) = default;
    JplaceReader& operator= ( JplaceReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

public:

    /**
     * @brief Read `jplace` data from a stream into a Sample.
     */
    Sample from_stream( std::istream& is ) const;

    /**
     * @brief Read a file and parse it as a Jplace document into a Sample.
     */
    Sample from_file( std::string const& fn ) const;

    /**
     * @brief Parse a string as a Jplace document into a Sample.
     */
    Sample from_string( std::string const& jplace ) const;

    /**
     * @brief Take a JsonDocument and parse it as a Jplace document into a Sample.
     */
    Sample from_document( utils::JsonDocument& doc ) const;

    /**
     * @brief Read a list of files and parse them as a Jplace document into a SampleSet.
     */
    SampleSet from_files( std::vector<std::string> const& fns ) const;

    /**
     * @brief Parse a list of strings as a Jplace document into a SampleSet.
     */
    SampleSet from_strings( std::vector<std::string> const& jps ) const;

    /**
     * @brief Read a list of files and parse them as a Jplace document into a SampleSet.
     *
     * The Sample%s are added to the SampleSet, so that existing Samples in the SampleSet are kept.
     */
    void from_files    ( std::vector<std::string> const& fns, SampleSet& set ) const;

    /**
     * @brief Parse a list of strings as a Jplace document into a SampleSet.
     *
     * The Sample%s are added to the SampleSet, so that existing Samples in the SampleSet are kept.
     */
    void from_strings  ( std::vector<std::string> const& jps, SampleSet& set ) const;

    // ---------------------------------------------------------------------
    //     Processing
    // ---------------------------------------------------------------------

private:

    /**
     * @brief Internal helper function that checks whether the `version` key in a JsonDocument
     * corresponds to a valid version number for the JplaceReader.
     */
    void process_json_version(    utils::JsonDocument const& doc ) const;

    /**
     * @brief Internal helper function that processes the `metadata` key of a JsonDocument and stores
     * its value in the Sample metadata member.
     */
    void process_json_metadata( utils::JsonDocument const& doc, Sample& smp ) const;

    /**
     * @brief Internal helper function that processes the `tree` key of a JsonDocument and stores it as
     * the Tree of a Sample.
     */
    void process_json_tree(       utils::JsonDocument const& doc, Sample& smp ) const;

    /**
     * @brief Internal helper function that processes the `fields` key of a JsonDocument and returns
     * its values.
     */
    std::vector<std::string> process_json_fields( utils::JsonDocument const& doc ) const;

    /**
     * @brief Internal helper function that processes the `placements` key of a JsonDocument and stores
     * the contained pqueries in the Sample.
     */
    void process_json_placements(
        utils::JsonDocument&       doc,
        Sample&                    smp,
        std::vector<std::string>   fields
    ) const;

    // ---------------------------------------------------------------------
    //     Jplace Version
    // ---------------------------------------------------------------------

public:

    /**
     * @brief Returns the version number that this class is written for. Currently, this is "3".
     */
    static std::string version   ();

    /**
     * @brief Checks whether the version of the jplace format works with this parser.
     *
     * This parser is intended for `jplace` versions 2 and 3. If while reading a different version
     * tag is found, the reader will trigger a warning and try to continue anyway.
     */
    static bool        check_version ( size_t             version );

    /**
     * @copydoc check_version( size_t version ).
     */
    static bool        check_version ( std::string const& version );

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

public:

    /**
     * @brief Enum to determine the behaviour of the reader in case of invalid numbers.
     *
     * When reading from `jplace` files, the numerical fields of each placement can contain small
     * deviations or inaccuracies. Those are due to numerical issues or floating point number to
     * text conversions, which occur when storing the exact values in a textual representation.
     *
     * Currently, the following checks are offered:
     *
     *   * `like_weight_ratio < 0.0`
     *   * `like_weight_ratio > 1.0`
     *   * `pendant_length    < 0.0`
     *   * `proximal_length   < 0.0`
     *   * `proximal_length   > branch_length`
     *
     * This enum offers choices to decide how the JplaceReader reacts when facing those errors.
     * See invalid_number_behaviour( InvalidNumberBehaviour ) to set the behaviour.
     */
    enum class InvalidNumberBehaviour {
        /**
         * @brief Ignore invalid numbers.
         *
         * Erroneous numbers are simply ignored, meaning, they are stored as-is in the data
         * structure. This is the default.
         */
        kIgnore,

         /**
          * @brief Log invalid numbers to LOG_WARN.
          *
          * When encountering an erroneous number, a warning stating the error is logged. The
          * number is stored as-is in the data structure.
          */
        kLog,

        /**
         * @brief Correct invalid numbers to the next best correct number.
         *
         * Erroneous numbers are silently corrected. This means, they are set to the nearest correct
         * value. For example, a value that cannot be smaller than 0.0 will be set to 0.0.
         */
        kCorrect,

        /**
         * @brief Combination of kLog and kCorrect.
         */
        kLogAndCorrect,

        /**
         * @brief Throw an `std::runtime_error` when encountering an invalid number.
         */
        kThrow
    };

    /**
     * @brief Return the currenlty set InvalidNumberBehaviour.
     */
    InvalidNumberBehaviour invalid_number_behaviour() const;

    /**
     * @brief Set the InvalidNumberBehaviour.
     *
     * This setter controls the InvalidNumberBehaviour of the JplaceReader. The default value is
     * InvalidNumberBehaviour::kIgnore. See InvalidNumberBehaviour for the valid options.
     *
     * The function returns the JplaceReader object to allow for a fluent interface.
     */
    JplaceReader&          invalid_number_behaviour( InvalidNumberBehaviour val );

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    InvalidNumberBehaviour invalid_number_behaviour_ = InvalidNumberBehaviour::kIgnore;

};

} // namespace placement
} // namespace genesis

#endif // include guard
