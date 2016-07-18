#ifndef GENESIS_PLACEMENT_FORMATS_JPLACE_READER_H_
#define GENESIS_PLACEMENT_FORMATS_JPLACE_READER_H_

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
 *   * from_document()
 *
 * Exemplary usage:
 *
 *     std::string infile = "path/to/file.jplace";
 *     Sample map;
 *
 *     JplaceReader()
 *         .invalid_number_behaviour( InvalidNumberBehaviour::kCorrect )
 *         .from_file( infile, map );
 *
 * The Jplace format is described in the following publication:
 *
 *     Matsen FA, Hoffman NG, Gallagher A, Stamatakis A. 2012.
 *     A Format for Phylogenetic Placements.
 *     PLoS ONE 7(2): e31009. doi:10.1371/journal.pone.0031009
 *     http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0031009
 *
 * See Sample for the data structure used to store the Pqueries.
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

    void from_files    ( std::vector<std::string> const& fns, SampleSet& set ) const;
    void from_strings  ( std::vector<std::string> const& jps, SampleSet& set ) const;

    void from_stream   ( std::istream&       is,         Sample& smp ) const;
    void from_file     ( std::string const&  fn,         Sample& smp ) const;
    void from_string   ( std::string const&  jplace,     Sample& smp ) const;
    void from_document ( utils::JsonDocument const& doc, Sample& smp ) const;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

protected:

    void parse_version( utils::InputStream& input_stream ) const;

    std::unordered_map<std::string, std::string> parse_metadata(
        utils::InputStream& input_stream
    ) const;

    std::vector<std::string> parse_fields( utils::InputStream& input_stream ) const;

    void parse_tree( utils::InputStream& input_stream ) const;

    // ---------------------------------------------------------------------
    //     Processing
    // ---------------------------------------------------------------------

protected:

    void process_json_version(    utils::JsonDocument const& doc ) const;

    void process_json_tree(       utils::JsonDocument const& doc, Sample& smp ) const;

    std::vector<std::string> process_json_fields( utils::JsonDocument const& doc ) const;

    void process_json_placements(
        utils::JsonDocument const& doc,
        Sample&                    smp,
        std::vector<std::string>   fields
    ) const;

    void process_json_metadata( utils::JsonDocument const& doc, Sample& smp ) const;

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

    InvalidNumberBehaviour invalid_number_behaviour() const;
    JplaceReader&          invalid_number_behaviour( InvalidNumberBehaviour val );

    static std::string version   ();
    static bool        check_version ( std::string const& version );
    static bool        check_version ( size_t             version );

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    InvalidNumberBehaviour invalid_number_behaviour_ = InvalidNumberBehaviour::kIgnore;

};

} // namespace placement
} // namespace genesis

#endif // include guard
