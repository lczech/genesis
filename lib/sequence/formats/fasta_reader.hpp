#ifndef GENESIS_SEQUENCE_FORMATS_FASTA_READER_H_
#define GENESIS_SEQUENCE_FORMATS_FASTA_READER_H_

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
    class InputStream;
}

namespace sequence {
    class SequenceSet;
    class Sequence;
}

// =================================================================================================
//     Fasta Reader
// =================================================================================================

namespace sequence {

/**
 * @brief Read Fasta data.
 *
 * This class provides simple facilities for reading Fasta data. It supports to read
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
 *         .valid_chars( nucleic_acid_codes_all() )
 *         .from_file( infile, sset );
 *
 * The expected data format:
 *
 *   1. Has to start with a '>' character, followed by a label and possibly metadata, ended by a
 *      '\\n'. All text after the first space is considered to be metadata.
 *   2. An arbitrary number of comment lines, starting with ';', can follow, but are ignored.
 *   3. After that, a sequence has to follow, over one or more lines and ending in a '\\n' character.
 *
 * Using to_upper(bool), the sequences can automatically be turned into upper case letter.
 * Also, see valid_chars( std::string const& chars ) for a way of checking correct input sequences.
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
    //     Reading
    // ---------------------------------------------------------------------

    void from_stream ( std::istream&      input_stream, SequenceSet& sequence_set ) const;
    void from_file   ( std::string const& file_name,    SequenceSet& sequence_set ) const;
    void from_string ( std::string const& input_string, SequenceSet& sequence_set ) const;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    void parse_document(
        utils::InputStream& input_stream,
        SequenceSet& sset
    ) const;

    bool parse_sequence(
        utils::CountingIstream& input_stream,
        Sequence&           sequence
    ) const;

    bool parse_sequence(
        utils::InputStream& input_stream,
        Sequence&           sequence
    ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    FastaReader& to_upper( bool value );
    bool         to_upper() const;

    FastaReader& valid_chars( std::string const& chars );
    std::string  valid_chars() const;

    utils::CharLookup& valid_char_lookup();

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    bool              to_upper_       = true;
    bool              use_validation_ = false;
    utils::CharLookup lookup_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
