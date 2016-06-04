#ifndef GENESIS_SEQUENCE_FORMATS_FASTA_WRITER_H_
#define GENESIS_SEQUENCE_FORMATS_FASTA_WRITER_H_

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

    void write_sequence( Sequence const& seq, std::ostream& os ) const;

    void        to_stream ( SequenceSet const& sset, std::ostream&      os ) const;
    void        to_file   ( SequenceSet const& sset, std::string const& fn ) const;
    std::string to_string ( SequenceSet const& sset ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    FastaWriter& line_length( size_t value );
    size_t       line_length() const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    size_t line_length_ = 80;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
