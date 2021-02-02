/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

#include "genesis/sequence/formats/phylip_writer.hpp"

#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/io/output_stream.hpp"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Writing
// =================================================================================================

// void PhylipWriter::to_stream_interleaved( SequenceSet const& sequence_set, std::ostream& os ) const
// {
//
// }

// void PhylipWriter::to_stream_sequential( SequenceSet const& sequence_set, std::ostream& os ) const
// {
//
// }

void PhylipWriter::write( SequenceSet const& sequence_set, std::shared_ptr<utils::BaseOutputTarget> target ) const
{
    // Shorthand
    auto& os = target->ostream();

    // Check if this is an alignment and get its sequence length.
    size_t length = 0;
    for( Sequence const& s : sequence_set ) {
        if (length == 0) {
            length = s.length();
            continue;
        }
        if (s.length() != length) {
            throw std::runtime_error(
                "Cannot write SequenceSet to Phylip format: Sequences do not have the same length."
            );
        }
    }
    if (length == 0) {
        throw std::runtime_error( "Cannot write SequenceSet to Phylip format: Sequences are empty." );
    }

    // Write sequences to stream.
    os << sequence_set.size() << " " << length << "\n";
    for (Sequence const& s : sequence_set) {
        // Write label.
        if( label_length_ == 0 ) {
            auto const f = std::find_if( std::begin(s.label()), std::end(s.label()), [](char c){
                return ! ::isgraph(c);
            });
            if( std::end(s.label()) != f ) {
                throw std::runtime_error(
                    "Cannot write Sequence to Philip: Sequence label \"" + s.label() +
                    "\" contains non-printable or whitespace characters, "
                    "which cannot be used in the relaxed Phylip format."
                );
            }
            os << s.label() << " ";
        } else {
            os << s.label().substr( 0, label_length_ );
            if( label_length_ > s.label().length() ) {
                os << std::string( label_length_ - s.label().length(), ' ' );
            }
        }

        // Write sequence. If needed, add new line at every line_length_ position.
        if( line_length_ > 0 ) {
            for( size_t i = 0; i < s.length(); i += line_length_ ) {
                // Write line_length_ many characters.
                // (If the string is shorter, as many characters as possible are used.)
                os << s.sites().substr( i, line_length_ ) << "\n";
            }
        } else {
            os << s.sites() << "\n";
        }
    }
}

// =================================================================================================
//     Properties
// =================================================================================================

PhylipWriter& PhylipWriter::label_length( size_t value )
{
    label_length_ = value;
    return *this;
}

size_t PhylipWriter::label_length() const
{
    return label_length_;
}

PhylipWriter& PhylipWriter::line_length( size_t value )
{
    line_length_ = value;
    return *this;
}

size_t PhylipWriter::line_length() const
{
    return line_length_;
}

} // namespace sequence
} // namespace genesis
