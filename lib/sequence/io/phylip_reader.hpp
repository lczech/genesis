#ifndef GENESIS_SEQUENCE_IO_PHYLIP_READER_H_
#define GENESIS_SEQUENCE_IO_PHYLIP_READER_H_

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "utils/tools/char_lookup.hpp"

#include <iosfwd>
#include <string>
#include <utility>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {

class CountingIstream;

} // namespace utils

namespace sequence {

class SequenceSet;
class Sequence;

// =================================================================================================
//     Phylip Reader
// =================================================================================================

/**
 * @brief Read Phylip data.
 *
 * This class provides simple facilities for reading Phylip data. It supports to read
 *
 *   * from_stream()
 *   * from_file()
 *   * from_string()
 *
 * Exemplary usage:
 *
 *     std::string infile = "path/to/file.phylip";
 *     SequenceSet sset;
 *
 *     PhylipReader()
 *         .to_upper()
 *         .validate_chars( nucleic_acid_codes_all() )
 *         .from_file( infile, sset );
 *
 * The expected data format roughly follows
 * [the original definition](http://evolution.genetics.washington.edu/phylip/doc/sequence.html).
 * See mode( Mode ) to selected between sequential, interleaved and automatic mode.
 * We furthermore support a relaxed version, where the label can be of any length.
 * See label_length( size_t ) for more information.
 *
 * Using to_upper( bool ), the sequences can automatically be turned into upper case letter.
 * Also, see validate_chars( std::string const& ) for a way of checking correct input sequences.
 */
class PhylipReader
{
public:

    // ---------------------------------------------------------------------
    //     Types and Enums
    // ---------------------------------------------------------------------

    /**
     * @brief Enum to distinguish between the different file variants of Phylip.
     * See mode( Mode value ) for more details.
     */
    enum class Mode
    {
        /**
         * @brief Read the data in Phylip sequential mode.
         */
        kSequential,

        /**
        * @brief Read the data in Phylip interleaved mode.
        */
        kInterleaved,

        /**
        * @brief Infer the Phylip mode via trial and error.
        */
        kAutomatic
    };

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    PhylipReader();
    ~PhylipReader() = default;

    PhylipReader( PhylipReader const& ) = default;
    PhylipReader( PhylipReader&& )      = default;

    PhylipReader& operator= ( PhylipReader const& ) = default;
    PhylipReader& operator= ( PhylipReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    std::pair<size_t, size_t> parse_phylip_header(
        utils::CountingIstream& it
    ) const;

    std::string parse_phylip_label(
        utils::CountingIstream& it
    ) const;

    std::string parse_phylip_sequence_line(
        utils::CountingIstream& it
    ) const;

    void parse_phylip_interleaved(
        utils::CountingIstream& it,
        SequenceSet& sset
    ) const;

    void parse_phylip_sequential(
        utils::CountingIstream& it,
        SequenceSet& sset
    ) const;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    void from_stream ( std::istream&      is, SequenceSet& sset ) const;
    void from_file   ( std::string const& fn, SequenceSet& sset ) const;
    void from_string ( std::string const& fs, SequenceSet& sset ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    PhylipReader& mode( Mode value );
    Mode          mode() const;

    PhylipReader& label_length( size_t value );
    size_t        label_length() const;

    PhylipReader& to_upper( bool value );
    bool          to_upper() const;

    PhylipReader& validate_chars( std::string const& chars );
    std::string   validate_chars() const;

    bool is_validating() const;
    utils::CharLookup& valid_char_lookup();

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    Mode              mode_         = Mode::kSequential;
    size_t            label_length_ = 0;
    bool              to_upper_     = true;
    utils::CharLookup lookup_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
