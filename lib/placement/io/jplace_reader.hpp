#ifndef GENESIS_PLACEMENT_IO_JPLACE_READER_H_
#define GENESIS_PLACEMENT_IO_JPLACE_READER_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {
    class CountingIstream;
    class JsonDocument;
}

namespace placement {
    class PlacementMap;
    class PlacementMapSet;
}

// =================================================================================================
//     Jplace Reader
// =================================================================================================

namespace placement {

/**
 * @brief Read a Jplace document and create a PlacementMap object from it.
 *
 * The Jplace format is described in the following publication:
 *
 *     Matsen FA, Hoffman NG, Gallagher A, Stamatakis A. 2012.
 *     A Format for Phylogenetic PlacementMap.
 *     PLoS ONE 7(2): e31009. doi:10.1371/journal.pone.0031009
 *     http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0031009
 *
 * See PlacementMap for the data structure used to store the Pqueries.
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

    void from_files    ( std::vector<std::string> const& fns, PlacementMapSet& set ) const;
    void from_strings  ( std::vector<std::string> const& jps, PlacementMapSet& set ) const;

    void from_stream   ( std::istream&       is,         PlacementMap& placements ) const;
    void from_file     ( std::string const&  fn,         PlacementMap& placements ) const;
    void from_string   ( std::string const&  jplace,     PlacementMap& placements ) const;
    void from_document ( utils::JsonDocument const& doc, PlacementMap& placements ) const;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

protected:

    bool parse_version(
        utils::CountingIstream& input_stream
    ) const;

    // ---------------------------------------------------------------------
    //     Processing
    // ---------------------------------------------------------------------

protected:

    void process_json_version(    utils::JsonDocument const& doc ) const;

    void process_json_tree(       utils::JsonDocument const& doc, PlacementMap& placements ) const;

    std::vector<std::string> process_json_fields( utils::JsonDocument const& doc ) const;

    void process_json_placements(
        utils::JsonDocument const& doc,
        PlacementMap&              placements,
        std::vector<std::string>   fields
    ) const;

    void process_json_metadata( utils::JsonDocument const& doc, PlacementMap& placements ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

public:

    /**
     * @brief Enum to determine the behaviour of the reader in case of invalid numbers.
     */
    enum class InvalidNumberBehaviour {
        /** @brief Ignore invalid numbers. */
        kIgnore,

         /** @brief Log invalid numbers to LOG_WARN. */
        kLog,

        /** @brief Correct invalid numbers to the next best correct number. */
        kCorrect,

        /** @brief Combination of kLog and kCorrect. */
        kLogAndCorrect,

        /** @brief Throw an `std::runtime_error` when encountering an invalid number. */
        kThrow
    };

    InvalidNumberBehaviour invalid_number_behaviour() const;
    JplaceReader&          invalid_number_behaviour( InvalidNumberBehaviour val );

    static std::string version   ();
    static bool        check_version ( std::string const& version );

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    InvalidNumberBehaviour invalid_number_behaviour_ = InvalidNumberBehaviour::kIgnore;

};

} // namespace placement
} // namespace genesis

#endif // include guard
