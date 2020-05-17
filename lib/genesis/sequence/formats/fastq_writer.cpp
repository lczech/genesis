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

#include "genesis/sequence/formats/fastq_writer.hpp"

#include "genesis/sequence/functions/quality.hpp"
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

void FastqWriter::write( Sequence const& sequence, std::shared_ptr<utils::BaseOutputTarget> target ) const
{
    write_sequence( sequence, target->ostream() );
}

void FastqWriter::write( SequenceSet const& sequence_set, std::shared_ptr<utils::BaseOutputTarget> target ) const
{
    auto& os = target->ostream();
    for( Sequence const& sequence : sequence_set ) {
        write_sequence( sequence, os );
    }
}

void FastqWriter::write_sequence( Sequence const& seq, std::ostream& os ) const
{
    // Write label.
    os << "@" << seq.label() << "\n";

    // Write sequence. If needed, add new line at every line_length_ position.
    if( line_length_ > 0 ) {
        for( size_t i = 0; i < seq.length(); i += line_length_ ) {
            // Write line_length_ many characters.
            // (If the string is shorter, as many characters as possible are used.)
            os << seq.sites().substr( i, line_length_ ) << "\n";
        }
    } else {
        os << seq.sites() << "\n";
    }

    // Write third line, repeat label if necessary.
    if( repeat_label_ ) {
        os << "+" << seq.label() << "\n";
    } else {
        os << "+\n";
    }

    // Write phred quality score.
    if( seq.phred_scores().size() == seq.sites().size() ) {

        // Default case: proper phred quality scores. We do a lot of string copies here (first,
        // to get the scores in string form, then possibly for wrapping the lines), which is slow.
        // For now, we do not need to write Fastq that often, so we can live with that.
        // Can be optimized if needed. Same for the "const dummy scores" case below.
        auto const scores = quality_encode_from_phred_score( seq.phred_scores() );
        if( line_length_ > 0 ) {
            for( size_t i = 0; i < scores.size(); i += line_length_ ) {
                // Write line_length_ many characters.
                // (If the string is shorter, as many characters as possible are used.)
                os << scores.substr( i, line_length_ ) << "\n";
            }
        } else {
            os << scores << "\n";
        }

    } else if( seq.phred_scores().size() == 0 ) {

        // Special case: Sequence does not have phred quality scores.
        // Either throw, or use const dummy scores.
        if( fill_missing_quality_ == 255 ) {
            throw std::runtime_error(
                "Sequence without phred scores found. "
                "Use FastqWriter::fill_missing_quality() to use dummy score values instead."
            );
        } else {

            // Slightly slow: Make a string, then put it to the stream.
            // Too lazy for a faster implementation, as this will rarely be needed anyway.
            auto const dummy_chr = quality_encode_from_phred_score( fill_missing_quality_ );
            auto const dummy_str = std::string( seq.sites().size(), dummy_chr );
            assert( dummy_str.size() == seq.sites().size() );

            if( line_length_ > 0 ) {
                for( size_t i = 0; i < dummy_str.size(); i += line_length_ ) {
                    // Write line_length_ many characters.
                    // (If the string is shorter, as many characters as possible are used.)
                    os << dummy_str.substr( i, line_length_ ) << "\n";
                }
            } else {
                os << dummy_str << "\n";
            }
        }

    } else {
        // Error case.
        throw std::runtime_error(
            "Invalid Sequence with phred scores of different length than the sequence has sites."
        );
    }
}

// =================================================================================================
//     Properties
// =================================================================================================

FastqWriter& FastqWriter::line_length( size_t value )
{
    line_length_ = value;
    return *this;
}

size_t FastqWriter::line_length() const
{
    return line_length_;
}

FastqWriter& FastqWriter::fill_missing_quality( unsigned char value )
{
    fill_missing_quality_ = value;
    return *this;
}

unsigned char FastqWriter::fill_missing_quality() const
{
    return fill_missing_quality_;
}

FastqWriter& FastqWriter::repeat_label( bool value )
{
    repeat_label_ = value;
    return *this;
}

bool FastqWriter::repeat_label() const
{
    return repeat_label_;
}

} // namespace sequence
} // namespace genesis
