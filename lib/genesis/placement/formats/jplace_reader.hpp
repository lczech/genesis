#ifndef GENESIS_PLACEMENT_FORMATS_JPLACE_READER_H_
#define GENESIS_PLACEMENT_FORMATS_JPLACE_READER_H_

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
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "genesis/utils/io/input_source.hpp"

#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {
    class JsonDocument;
}

namespace tree {
    class TreeEdge;
}

namespace placement {
    using PlacementTreeEdge = tree::TreeEdge;

    class Pquery;
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
 * This class provides facilities for reading `jplace` files into a Sample or SampleSet.
 * Exemplary usage:
 *
 *     std::string infile = "path/to/file.jplace";
 *     Sample smp = JplaceReader()
 *         .invalid_number_behaviour( InvalidNumberBehaviour::kFix )
 *         .read( from_file( infile ));
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
 * This parser is written according to the `jplace` standard version 3, as described in the above
 * document, but also incorporates parsing of the previous versions 1 and 2.
 * See http://matsen.github.io/pplacer/generated_rst/pplacer.html#json-format-specification
 * for differences between the versions of the jplace standard.
 *
 * See Sample and SampleSet for the data structures used to store the Pqueries and the reference Tree.
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
     * @brief Read from an input source.
     *
     * Use functions such as utils::from_file() and utils::from_string() to conveniently
     * get an input source that can be used here.
     */
    Sample read(
        std::shared_ptr<utils::BaseInputSource> source
    ) const;

    /**
     * @brief Take a JsonDocument and parse it as a Jplace document into a Sample.
     *
     * Note that the JsonDocument is modified, in order to save memory.
     */
    Sample read(
        utils::JsonDocument& doc
    ) const;

    /**
     * @brief Read from multiple sources and parse them as a Jplace documents into a SampleSet.
     */
    SampleSet read(
        std::vector<std::shared_ptr<utils::BaseInputSource>> sources
    ) const;

    /**
     * @brief Read from multiple sources and parse them as a Jplace documents into an existing
     * SampleSet.
     *
     * The Sample%s are added to the SampleSet, so that existing Samples in the SampleSet are kept.
     */
    void read(
        std::vector<std::shared_ptr<utils::BaseInputSource>> sources,
        SampleSet& target
    ) const;

    // ---------------------------------------------------------------------
    //     Processing
    // ---------------------------------------------------------------------

private:

    /**
     * @brief Get the version of the jplace document.
     *
     * The function simply returns the value of the `version` field of the top level `json` object.
     * If that field is not found, or does not contain an integer number, `-1` is returned.
     *
     * According to the standard, this is an integer number. If this ever changes to a different
     * format, this function has to be changed accordingly.
     */
    int get_jplace_version_( utils::JsonDocument const& doc ) const;

    /**
     * @brief Internal helper function that checks whether the `version` key in a JsonDocument
     * corresponds to a valid version number for the JplaceReader.
     */
    void process_jplace_version_( utils::JsonDocument const& doc ) const;

    /**
     * @brief Internal helper function that processes the `metadata` key of a JsonDocument and stores
     * its value in the Sample metadata member.
     */
    void process_jplace_metadata_( utils::JsonDocument const& doc, Sample& smp ) const;

    /**
     * @brief Internal helper function that processes the `tree` key of a JsonDocument and stores it as
     * the Tree of a Sample.
     */
    void process_jplace_tree_( utils::JsonDocument const& doc, Sample& smp ) const;

    /**
     * @brief Internal helper function that processes the `fields` key of a JsonDocument and returns
     * its values.
     */
    std::vector<std::string> process_jplace_fields_( utils::JsonDocument const& doc ) const;

    /**
     * @brief Internal helper function that processes the `placements` key of a JsonDocument and stores
     * the contained pqueries in the Sample.
     */
    void process_jplace_placements_(
        utils::JsonDocument&            doc,
        Sample&                         smp,
        std::vector<std::string> const& fields
    ) const;

    /**
     * @brief Internal helper function that processes the `p` part of a placement.
     */
    void process_jplace_placements_p_(
        utils::JsonDocument const&      pqry_obj,
        Pquery&                         pquery,
        std::vector<std::string> const& fields,
        std::unordered_map<size_t, PlacementTreeEdge*> const& edge_num_map
    ) const;

    /**
     * @brief Internal helper function that processes the `n`, `m`, and `nm` part of a placement.
     */
    void process_jplace_placements_nm_(
        utils::JsonDocument const&      pqry_obj,
        Pquery&                         pquery
    ) const;

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
         * @brief Fix invalid numbers to the next best correct number.
         *
         * Erroneous numbers are silently corrected. This means, they are set to the nearest correct
         * value. For example, a value that cannot be smaller than 0.0 will be set to 0.0.
         */
        kFix,

        /**
         * @brief Combination of kLog and kFix.
         */
        kLogAndFix,

        /**
         * @brief Throw an `std::runtime_error` when encountering an invalid number.
         */
        kThrow
    };

    /**
     * @brief Return the currenlty set InvalidNumberBehaviour.
     */
    InvalidNumberBehaviour invalid_number_behaviour() const
    {
        return invalid_number_behaviour_;
    }

    /**
     * @brief Set the InvalidNumberBehaviour.
     *
     * This setter controls the InvalidNumberBehaviour of the JplaceReader. The default value is
     * InvalidNumberBehaviour::kIgnore. See InvalidNumberBehaviour for the valid options.
     *
     * The function returns the JplaceReader object to allow for a fluent interface.
     */
    JplaceReader& invalid_number_behaviour( InvalidNumberBehaviour val )
    {
        invalid_number_behaviour_ = val;
        return *this;
    }

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    InvalidNumberBehaviour invalid_number_behaviour_ = InvalidNumberBehaviour::kIgnore;

};

} // namespace placement
} // namespace genesis

#endif // include guard
