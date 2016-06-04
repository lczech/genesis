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

#include "sequence/formats/fasta_writer.hpp"

#include "sequence/sequence_set.hpp"
#include "sequence/sequence.hpp"
#include "utils/core/fs.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Writing
// =================================================================================================

/**
 * @brief Write a single Sequence to an output stream in Fasta format.
 */
void FastaWriter::write_sequence( Sequence const& seq, std::ostream& os ) const
{
    // Write label.
    os << ">" << seq.label();

    // Write metadata if available.
    if( seq.metadata().size() > 0 ) {
        os << " " << seq.metadata();
    }
    os << "\n";

    // Write sequence. If needed, add new line at every line_length_ position.
    if (line_length_ > 0) {
        for (size_t i = 0; i < seq.length(); i += line_length_) {
            // Write line_length_ many characters.
            // (If the string is shorter, as many characters as possible are used.)
            os << seq.sites().substr(i, line_length_) << "\n";
        }
    } else {
        os << seq.sites() << "\n";
    }
}

/**
 * @brief Write Sequences of a SequenceSet to a stream in Fasta format.
 */
void FastaWriter::to_stream( SequenceSet const& sset, std::ostream& os ) const
{
    for( Sequence const& seq : sset ) {
        write_sequence( seq, os );
    }
}

/**
 * @brief Write Sequences of a SequenceSet to a file in Fasta format.
 */
void FastaWriter::to_file( SequenceSet const& sset, std::string const& fn ) const
{
    if( utils::file_exists( fn ) ) {
        throw std::runtime_error( "File '" + fn + "' already exists." );
    }

    std::ofstream ofs( fn );
    if( ofs.fail() ) {
        throw std::runtime_error( "Cannot write to file '" + fn + "'." );
    }

    to_stream( sset, ofs );
}

/**
 * @brief Return Sequences of a SequenceSet in form of a Fasta formatted string.
 *
 * Caveat: This might be a long string! If you simply want to examine a Sequence or SequenceSet,
 * have a look at the print() and print_color() functions.
 */
std::string FastaWriter::to_string ( SequenceSet const& sset ) const
{
    std::ostringstream oss;
    to_stream( sset, oss );
    return oss.str();
}

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Set the line length, which determines after how many chars (Sequence sites) lines breaks
 * are inserted when writing the Fasta file.
 *
 * Default is `80`. If set to `0`, no breaks are inserted.
 * The functions returns the FastaWriter object to allow fluent interfaces.
 */
FastaWriter& FastaWriter::line_length( size_t value )
{
    line_length_ = value;
    return *this;
}

/**
 * @brief Get the current line length.
 *
 * See the setter line_length() for details.
 */
size_t FastaWriter::line_length() const
{
    return line_length_;
}

} // namespace sequence
} // namespace genesis
