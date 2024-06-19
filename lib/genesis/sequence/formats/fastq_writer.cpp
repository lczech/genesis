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
//     Internal helper
// =================================================================================================

/**
 * @brief Local function template that does the actual work.
 *
 * We introduce a template param for std::string and std::string_view, if C++17 is used.
 */
template<class StringType>
static void fastq_writer_write_sequence_helper_(
    StringType const& label,
    StringType const& sites,
    StringType const& quality,
    size_t line_length,
    bool repeat_label,
    std::ostream& os
) {
    // This function is only called internally, with the correct sizes.
    assert( sites.size() == quality.size() );

    // Helper function to write lines with potential wrapping.
    auto write_wrapped_ = [&]( StringType const& str ){
        if( line_length > 0 ) {
            for( size_t i = 0; i < str.size(); i += line_length ) {
                // Write line_length many characters.
                // (If the string is shorter, as many characters as possible are used.)
                // os << str.substr( i, line_length ) << "\n";
                auto const sub = str.substr( i, line_length );
                os.write( sub.data(), sub.size() );
                os.write( "\n", 1 );
            }
        } else {
            // os << str << "\n";
            os.write( str.data(), str.size() );
            os.write( "\n", 1 );
        }
    };

    // Write label.
    // os << "@" << label << "\n";
    os.write( "@", 1 );
    os.write( label.data(), label.size() );
    os.write( "\n", 1 );

    // Write sequence.
    write_wrapped_( sites );

    // Write third line, repeat label if necessary.
    if( repeat_label ) {
        // os << "+" << label << "\n";
        os.write( "+", 1 );
        os.write( label.data(), label.size() );
        os.write( "\n", 1 );
    } else {
        // os << "+\n";
        os.write( "+\n", 2 );
    }

    // Write the phred quality score.
    write_wrapped_( quality );
}

// =================================================================================================
//     Writing
// =================================================================================================

void FastqWriter::write(
    Sequence const& sequence,
    std::shared_ptr<utils::BaseOutputTarget> target
) const {
    // Produce the phred quality score.
    std::string quality_string;
    if( sequence.phred_scores().size() == sequence.sites().size() ) {

        // Default case: proper phred quality scores. We do a lot of string copies here (first,
        // to get the scores in string form, then possibly for wrapping the lines), which is slow.
        // For now, we do not need to write Fastq that often, so we can live with that.
        // Can be optimized if needed. Same for the "const dummy scores" case below.
        quality_string = quality_encode_from_phred_score( sequence.phred_scores() );

    } else if( sequence.phred_scores().size() == 0 ) {

        // Make a string filled with the filler quality char.
        quality_string = make_filled_quality_string_( sequence.sites().size() );

    } else {
        // Error case.
        throw std::runtime_error(
            "Invalid Sequence with phred scores of different length than the sequence has sites."
        );
    }

    // Now write all of this to the stream
    write_sequence_( sequence.label(), sequence.sites(), quality_string, target->ostream() );
}

void FastqWriter::write(
    Sequence const& sequence,
    std::string const& quality_string,
    std::shared_ptr<utils::BaseOutputTarget> target
) const {
    // We want to avoid mistakes here of calling this function with a provided qualith string,
    // in situations where the sequence itself already contains one.
    if( ! sequence.phred_scores().empty() ) {
        throw std::runtime_error(
            "Cannot write Fastq sequence with provided quality string "
            "if the sequence contains phred scores already."
        );
    }

    // Check that the quality string has the right lenght, or fill in otherwise.
    if( quality_string.size() == sequence.sites().size() ) {
        write_sequence_( sequence.label(), sequence.sites(), quality_string, target->ostream() );
    } else if( quality_string.size() == 0 ) {
        auto const filled_string = make_filled_quality_string_( sequence.sites().size() );
        write_sequence_( sequence.label(), sequence.sites(), filled_string, target->ostream() );
    } else {
        throw std::runtime_error(
            "Invalid given quality string of different length than the sequence has sites."
        );
    }
}

void FastqWriter::write(
    SequenceSet const& sequence_set,
    std::shared_ptr<utils::BaseOutputTarget> target
) const {
    for( Sequence const& sequence : sequence_set ) {
        write( sequence, target );
    }
}

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)

void FastqWriter::write(
    std::string_view const& label,
    std::string_view const& sites,
    std::string_view const& quality,
    std::shared_ptr<utils::BaseOutputTarget> target
) const {
    // We need to make sure that a quality string is given, or filled in.
    // We use an internal buffer to store the filled quality if needed, and redirect to there.
    std::string quality_buffer;
    std::string_view quality_view = quality;
    if( quality.empty() ) {
        quality_buffer = make_filled_quality_string_( sites.size() );
        quality_view = std::string_view( quality_buffer );
    } else if( quality.size() != sites.size() ) {
        throw std::runtime_error(
            "Invalid Sequence with quality string of different length than the sequence has sites."
        );
    }

    // Now write the data, using the view into either the original quality, or the buffer.
    fastq_writer_write_sequence_helper_<std::string_view>(
        label, sites, quality_view, line_length_, repeat_label_, target->ostream()
    );
}

#endif

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
    std::string const& label,
    std::string const& sites,
    std::string const& quality,
    std::ostream& os
) const {
    fastq_writer_write_sequence_helper_<std::string>(
        label, sites, quality, line_length_, repeat_label_, os
    );
}

} // namespace sequence
} // namespace genesis
