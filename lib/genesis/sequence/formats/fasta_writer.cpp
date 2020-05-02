/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include <cassert>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Writing
// =================================================================================================

void FastaWriter::write( Sequence const& sequence, std::shared_ptr<utils::BaseOutputTarget> target ) const
{
    write_sequence( sequence, target->ostream() );
}

void FastaWriter::write( SequenceSet const& sequence_set, std::shared_ptr<utils::BaseOutputTarget> target ) const
{
    auto& os = target->ostream();
    for( Sequence const& sequence : sequence_set ) {
        write_sequence( sequence, os );
    }
}

void FastaWriter::write_sequence( Sequence const& seq, std::ostream& os ) const
{
    // Write label.
    os << ">";
    switch( abundance_notation_ ) {
        case AbundanceNotation::kNone: {
            os << seq.label();
            break;
        }
        case AbundanceNotation::kUnderscore: {
            os << seq.label() << "_" << seq.abundance();
            break;
        }
        case AbundanceNotation::kSize: {
            os << seq.label() << ";size=" << seq.abundance();
            break;
        }
        default: {
            assert( false );
        }
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

FastaWriter& FastaWriter::abundance_notation( FastaWriter::AbundanceNotation value )
{
    abundance_notation_ = value;
    return *this;
}

FastaWriter::AbundanceNotation FastaWriter::abundance_notation() const
{
    return abundance_notation_;
}

} // namespace sequence
} // namespace genesis
