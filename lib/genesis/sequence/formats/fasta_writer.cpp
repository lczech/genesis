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

#include "genesis/sequence/formats/fasta_writer.hpp"

#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/io/output_stream.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Writing
// =================================================================================================

void FastaWriter::write_sequence( Sequence const& seq, std::ostream& os ) const
{
    // Write label.
    os << ">" << seq.label();

    // Write metadata if available.
    if( enable_metadata_ && seq.metadata().size() > 0 ) {
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

void FastaWriter::to_stream( SequenceSet const& sset, std::ostream& os ) const
{
    for( Sequence const& seq : sset ) {
        write_sequence( seq, os );
    }
}

void FastaWriter::to_file( SequenceSet const& sset, std::string const& filename ) const
{
    std::ofstream ofs;
    utils::file_output_stream( filename, ofs );
    to_stream( sset, ofs );
}

std::string FastaWriter::to_string ( SequenceSet const& sset ) const
{
    std::ostringstream oss;
    to_stream( sset, oss );
    return oss.str();
}

// =================================================================================================
//     Properties
// =================================================================================================

FastaWriter& FastaWriter::line_length( size_t value )
{
    line_length_ = value;
    return *this;
}

size_t FastaWriter::line_length() const
{
    return line_length_;
}

FastaWriter& FastaWriter::enable_metadata( bool value )
{
    enable_metadata_ = value;
    return *this;
}

bool FastaWriter::enable_metadata()
{
    return enable_metadata_;
}

} // namespace sequence
} // namespace genesis
