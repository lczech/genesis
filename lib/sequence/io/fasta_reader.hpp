#ifndef GENESIS_SEQUENCE_IO_FASTA_READER_H_
#define GENESIS_SEQUENCE_IO_FASTA_READER_H_

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "utils/tools/char_lookup.hpp"

#include <iosfwd>
#include <string>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {

class CountingIstream;

} // namespace utils

namespace sequence {

class FastaInputIterator;
class SequenceSet;
class Sequence;

// =================================================================================================
//     Fasta Reader
// =================================================================================================

/**
 * @brief Read FASTA data.
 *
 * This class provides simple facilities for reading FASTA data. Currently, it supports to read
 *
 *   * from_stream()
 *   * from_file()
 *   * from_string()
 *
 * Exemplary usage:
 *
 *     std::string infile = "path/to/file.fasta";
 *     SequenceSet sset;
 *
 *     FastaReader()
 *         .to_upper()
 *         .validate_chars( nucleic_acid_codes_all() )
 *         .from_file( infile, sset );
 *
 * The expected data format:
 *
 *   1. Has to start with a '>' character, followed by a label and possibly metadata, ended by a
 *      '\\n'. All text after the first space is considered to be metadata.
 *   2. An arbitrary number of comment lines, starting with ';', can follow, but are ignored.
 *   3. After that, a sequence has to follow, over one or more lines and ending in a '\\n' character.
 *
 * See validate_chars() for a way of checking correct input sequences.
 */
class FastaReader
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    FastaReader();
    ~FastaReader() = default;

    FastaReader( FastaReader const& ) = default;
    FastaReader( FastaReader&& )      = default;

    FastaReader& operator= ( FastaReader const& ) = default;
    FastaReader& operator= ( FastaReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    bool parse_fasta_sequence(
        utils::CountingIstream& input_stream,
        Sequence&               sequence
    ) const;

    bool parse_fasta_sequence_fast(
        utils::CountingIstream& input_stream,
        Sequence&               sequence
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

    FastaReader& to_upper( bool value );
    bool         to_upper() const;

    FastaReader& validate_chars( std::string const& chars );
    std::string  validate_chars() const;

    bool is_validating() const;
    utils::CharLookup& valid_char_lookup();

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    bool              to_upper_ = true;
    utils::CharLookup lookup_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
