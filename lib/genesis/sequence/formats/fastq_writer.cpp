/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
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

void FastqWriter::write(
    Sequence const& sequence,
    std::shared_ptr<utils::BaseOutputTarget> target
) const {
    write_sequence( sequence, target->ostream() );
}

void FastqWriter::write(
    Sequence const& sequence,
    std::string const& quality_string,
    std::shared_ptr<utils::BaseOutputTarget> target
) const {
    write_sequence( sequence, quality_string, target->ostream() );
}

void FastqWriter::write(
    SequenceSet const& sequence_set,
    std::shared_ptr<utils::BaseOutputTarget> target
) const {
    auto& os = target->ostream();
    for( Sequence const& sequence : sequence_set ) {
        write_sequence( sequence, os );
    }
}

void FastqWriter::write_sequence(
    Sequence const& seq,
    std::ostream& os
) const {
    // Produce the phred quality score.
    std::string quality_string;
    if( seq.phred_scores().size() == seq.sites().size() ) {

        // Default case: proper phred quality scores. We do a lot of string copies here (first,
        // to get the scores in string form, then possibly for wrapping the lines), which is slow.
        // For now, we do not need to write Fastq that often, so we can live with that.
        // Can be optimized if needed. Same for the "const dummy scores" case below.
        quality_string = quality_encode_from_phred_score( seq.phred_scores() );

    } else if( seq.phred_scores().size() == 0 ) {

        // Make a string filled with the filler quality char.
        quality_string = make_filled_quality_string_( seq.sites().size() );

    } else {
        // Error case.
        throw std::runtime_error(
            "Invalid Sequence with phred scores of different length than the sequence has sites."
        );
    }

    // Now write all of this to the stream
    write_sequence_( seq, quality_string, os );
}

void FastqWriter::write_sequence(
    Sequence const& seq,
    std::string const& quality_string,
    std::ostream& os
) const {
    // We want to avoid mistakes here of calling this function with a provided qualith string,
    // in situations where the sequence itself already contains one.
    if( ! seq.phred_scores().empty() ) {
        throw std::runtime_error(
            "Cannot write Fastq sequence with provided quality string "
            "if the sequence contains phred scores already."
        );
    }

    // Check that the quality string has the right lenght, or fill in otherwise.
    if( quality_string.size() == seq.sites().size() ) {
        write_sequence_( seq, quality_string, os );
    } else if( quality_string.size() == 0 ) {
        auto const filled_string = make_filled_quality_string_( seq.sites().size() );
        write_sequence_( seq, filled_string, os );
    } else {
        throw std::runtime_error(
            "Invalid given quality string of different length than the sequence has sites."
        );
    }
}

// =================================================================================================
//     Internal Members
// =================================================================================================

std::string FastqWriter::make_filled_quality_string_(
    size_t length
) const {
    // Special case: Sequence does not have phred quality scores.
    // Either throw, or use const dummy scores.
    if( fill_missing_quality_ == 255 ) {
        throw std::runtime_error(
            "Sequence without phred scores found. "
            "Use FastqWriter::fill_missing_quality() to use dummy score values instead."
        );
    }

    // Slightly slow: Make a string, then put it to the stream.
    // Too lazy for a faster implementation, as this will rarely be needed anyway.
    auto const dummy_chr = quality_encode_from_phred_score( fill_missing_quality_ );
    auto quality_string = std::string( length, dummy_chr );
    assert( quality_string.size() == length );
    return quality_string;
}

void FastqWriter::write_sequence_(
    Sequence const& sequence,
    std::string const& quality_string,
    std::ostream& os
) const {
    // This function is only called internally, with the correct sizes.
    assert( sequence.sites().size() == quality_string.size() );

    // Helper function to write lines with potential wrapping.
    auto write_wrapped_ = [&]( std::string const& str ){
        if( line_length_ > 0 ) {
            for( size_t i = 0; i < str.size(); i += line_length_ ) {
                // Write line_length_ many characters.
                // (If the string is shorter, as many characters as possible are used.)
                os << str.substr( i, line_length_ ) << "\n";
            }
        } else {
            os << str << "\n";
        }
    };

    // Write label.
    os << "@" << sequence.label() << "\n";

    // Write sequence.
    write_wrapped_( sequence.sites() );

    // Write third line, repeat label if necessary.
    if( repeat_label_ ) {
        os << "+" << sequence.label() << "\n";
    } else {
        os << "+\n";
    }

    // Write the phred quality score.
    write_wrapped_( quality_string );
}

} // namespace sequence
} // namespace genesis
