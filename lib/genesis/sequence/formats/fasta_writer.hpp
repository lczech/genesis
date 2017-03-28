#ifndef GENESIS_SEQUENCE_FORMATS_FASTA_WRITER_H_
#define GENESIS_SEQUENCE_FORMATS_FASTA_WRITER_H_

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
 * @ingroup sequence
 */

#include <iosfwd>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Sequence;
class SequenceSet;

// =================================================================================================
//     Fasta Writer
// =================================================================================================

/**
 * @brief Write Fasta data.
 *
 * This class provides simple facilities for writing Fasta data. It supports to write
 *
 *   * to_stream()
 *   * to_file()
 *   * to_string()
 *
 * Exemplary usage:
 *
 *     std::string outfile = "path/to/file.fasta";
 *     SequenceSet sset;
 *
 *     FastaWriter()
 *         .line_length( 100 )
 *         .to_file( sset, outfile );
 *
 * See FastaReader for a description of the Fasta format.
 */
class FastaWriter
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    FastaWriter()  = default;
    ~FastaWriter() = default;

    FastaWriter( FastaWriter const& ) = default;
    FastaWriter( FastaWriter&& )      = default;

    FastaWriter& operator= ( FastaWriter const& ) = default;
    FastaWriter& operator= ( FastaWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    /**
     * @brief Write a single Sequence to an output stream in Fasta format.
     */
    void write_sequence( Sequence const& seq, std::ostream& os ) const;

    /**
     * @brief Write Sequences of a SequenceSet to a stream, using the Fasta format.
     */
    void        to_stream ( SequenceSet const& sset, std::ostream&      os ) const;

    /**
     * @brief Write Sequences of a SequenceSet to a file, using the Fasta format.
     *
     * If the file cannot be written to, the function throws an exception. Also, by default, if the file
     * already exists, an exception is thrown.
     * See @link utils::Options::allow_file_overwriting( bool ) Options::allow_file_overwriting()@endlink to
     * change this behaviour.
     */
    void        to_file   ( SequenceSet const& sset, std::string const& fn ) const;

    /**
     * @brief Return Sequences of a SequenceSet in form of a Fasta formatted string.
     *
     * Caveat: This might be a long string! If you simply want to examine a Sequence or SequenceSet,
     * have a look at the print() and print_color() functions.
     */
    std::string to_string ( SequenceSet const& sset ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    /**
     * @brief Set the line length, which determines after how many chars (Sequence sites) lines breaks
     * are inserted when writing the Fasta file.
     *
     * Default is `80`. If set to `0`, no breaks are inserted.
     * The functions returns the FastaWriter object to allow fluent interfaces.
     */
    FastaWriter& line_length( size_t value );

    /**
     * @brief Get the current line length.
     *
     * See the setter line_length( size_t ) for details.
     */
    size_t       line_length() const;

    /**
     * @brief Set whether metadata is written.
     *
     * If set to `true` (default), the metadata of the Sequence%s is written after the label,
     * separated by a space. This is also the format that FastaReader can read.
     *
     * The functions returns the FastaWriter object to allow fluent interfaces.
     */
    FastaWriter& enable_metadata( bool value );

    /**
     * @brief Return whether currently the FastaWriter is set to write metadata.
     *
     * See the setter enable_metadata( bool ) for details.
     */
    bool         enable_metadata();

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    size_t line_length_     = 80;
    bool   enable_metadata_ = true;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
