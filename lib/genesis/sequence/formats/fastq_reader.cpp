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

#include "genesis/sequence/formats/fastq_reader.hpp"

#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/char.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <thread>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Constructor and Rule of Five
// =================================================================================================

FastqReader::FastqReader()
{
    lookup_.set_all( true );
}

// =================================================================================================
//     Reading
// =================================================================================================

SequenceSet FastqReader::read( std::shared_ptr< utils::BaseInputSource > source ) const
{
    SequenceSet result;
    utils::InputStream is( source );
    parse_document( is, result );
    return result;
}

void FastqReader::read(
    std::shared_ptr< utils::BaseInputSource > source,
    SequenceSet& sequence_set
) const {
    utils::InputStream is( source );
    parse_document( is, sequence_set );
}

// =================================================================================================
//     Parsing
// =================================================================================================

void FastqReader::parse_document(
    utils::InputStream& input_stream,
    SequenceSet&        sequence_set
) const {
    Sequence tmp_seq;
    while( parse_sequence_( input_stream, tmp_seq )) {
        sequence_set.add( tmp_seq );
    }
}

bool FastqReader::parse_sequence(
    utils::InputStream& input_stream,
    Sequence&           sequence
) const {
    return parse_sequence_( input_stream, sequence );
}

// =================================================================================================
//     Parsing Internals
// =================================================================================================

bool FastqReader::parse_sequence_( utils::InputStream& input_stream, Sequence& sequence ) const
{
    // Init. Call clear() in order to avoid not setting properties that might be added to
    // Sequence in the future. Should not noticeable affect speed, as the sequence string capacities
    // should not change when setting the strings to empty strings.
    sequence.clear();

    // Check for data. If there is nothing, stop here. If there is data, then from here on,
    // we expect a full fastq sequence to be present - otherwise, one of the below functions throws.
    if( !input_stream ) {
        return false;
    }

    // Parse all elements of a Fastq sequence.
    // We use a shared buffer for all of these functions that is filled with data from the input
    // stream. This has the following reasoning: Sequence files can be quite big, and appending to
    // a string can cause its size to double whenver the capacity is reached (depending on the STL
    // implementation). We want to avoid that, which can usually be done by making a fresh copy of
    // the string. However, reading into a string first, and then making a copy of it, necessitates
    // two memory allocations. We can circumvent the first by using this buffer, which is re-used.
    // Hence, we here rely on the fact that string::clear() does not change the capacity of the buffer.
    parse_label1_(  input_stream, sequence );
    parse_sites_(   input_stream, sequence );
    parse_label2_(  input_stream, sequence );
    parse_quality_( input_stream, sequence );

    return true;
}

void FastqReader::parse_label1_( utils::InputStream& input_stream, Sequence& sequence ) const
{
    auto& it = input_stream;
    buffer_.clear();

    // Check beginning of sequence.
    if( !it || *it != '@' ) {
        throw std::runtime_error(
            "Malformed Fastq " + it.source_name()
            + ": Expecting '@' at beginning of sequence at line "
            + std::to_string( it.line() ) + "."
        );
    }
    assert( it && *it == '@' );
    ++it;

    // Parse label.
    it.get_line( buffer_ );
    auto const buffer_is_print = std::all_of(
        buffer_.cbegin(),
        buffer_.cend(),
        []( char c ){
            return utils::is_print( c );
        }
    );
    if( buffer_.empty() || !buffer_is_print ) {
        throw std::runtime_error(
            "Malformed Fastq " + it.source_name() + ": Expecting valid label after '@' "
            "in sequence at line " + std::to_string( it.line() ) + ", but instead the label "
            "is empty or contains non-printable characters."
        );
    }

    // Copy the label into the sequence, which also makes sure that we do not store extra capacity.
    sequence.label( buffer_ );
}

void FastqReader::parse_sites_( utils::InputStream& input_stream, Sequence& sequence ) const
{
    // Some prep shorthand.
    auto& it = input_stream;
    buffer_.clear();

    // Check for unexpected end of file.
    if( !it ) {
        throw std::runtime_error(
            "Malformed Fastq " + it.source_name()
            + ": Expecting a sequence sites line after the first label line at line "
            + std::to_string( it.line() - 1 ) + "."
        );
    }
    assert( it );

    // Parse sequence. At every beginning of the loop, we are at a line start.
    // Continue until we find the '+' char, which marks the beginning of the second label
    // for the quality line(s). This is the ill-defined part of the format that we have to live with.
    while( it && *it != '+' ) {

        // The function is only called internally, and only ever when we are at the beginning
        // of a new line. Assert this.
        assert( it.column() == 1 );

        // The get_line function appends to the buffer.
        it.get_line( buffer_ );
    }
    assert( !it || *it == '+' );

    if( buffer_.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Fastq " + it.source_name() + ": Empty sequence at line "
            + std::to_string( it.line() - 1 ) + "."
        );
    }

    // Apply site casing, if needed.
    if( site_casing_ == SiteCasing::kToUpper ) {
        utils::to_upper_ascii_inplace( buffer_ );
    } else if( site_casing_ == SiteCasing::kToLower ) {
        utils::to_lower_ascii_inplace( buffer_ );
    }

    // Validate, if needed.
    if( use_validation_ ) {
        for( auto const& c : buffer_ ) {
            if( !lookup_[c] ) {
                throw std::runtime_error(
                    "Malformed Fastq " + it.source_name() + ": Invalid sequence symbol "
                    + utils::char_to_hex( c )
                    + " in sequence near line " + std::to_string( it.line() - 1 ) + "."
                );
            }
        }
    }

    // Copy the buffer to the sequence sites, which removes surplus capacity.
    sequence.sites( buffer_ );
}

void FastqReader::parse_label2_( utils::InputStream& input_stream, Sequence& sequence ) const
{
    auto& it = input_stream;
    buffer_.clear();

    // Check beginning of sequence.
    if( !it || *it != '+' ) {
        throw std::runtime_error(
            "Malformed Fastq " + it.source_name()
            + ": Expecting '+' at beginning of sequence at line "
            + std::to_string( it.line() ) + "."
        );
    }
    assert( it && *it == '+' );
    ++it;

    // Parse label. No need to run the vailidty check here again, as we can simply compare
    // against line1 that was read before. So, we can use the buffer.
    // The get_line function appends to the buffer.
    it.get_line( buffer_ );

    if( ! buffer_.empty() && buffer_ != sequence.label() ) {
        throw std::runtime_error(
            "Malformed Fastq " + it.source_name() + ": Expecting the second label line to either " +
            "be empty or equal to the first label line at line " + std::to_string( it.line() ) + "."
        );
    }
}

void FastqReader::parse_quality_( utils::InputStream& input_stream, Sequence& sequence ) const
{
    auto& it = input_stream;
    buffer_.clear();

    // Check for unexpected end of file.
    if( !it ) {
        throw std::runtime_error(
            "Malformed Fastq " + it.source_name()
            + ": Expecting quality scores after the second label line at line "
            + std::to_string( it.line() - 1 ) + "."
        );
    }
    assert( it );

    // Parse qualities. At every beginning of the loop, we are at a line start.
    // Continue until we have read as much characters as the sequence is long.
    while( it && buffer_.size() < sequence.sites().size() ) {

        // Again, this function is only called internally, and only ever when we are at the
        // beginning of a new line. Assert this.
        assert( it.column() == 1 );

        // The get_line function appends to the buffer.
        it.get_line( buffer_ );
    }
    assert( !it || buffer_.size() >= sequence.sites().size() );

    if( buffer_.size() != sequence.sites().size() ) {
        throw std::runtime_error(
            "Malformed Fastq " + it.source_name()
            + ": Expecting the quality scores to be of the same length as the sequence at line "
            + std::to_string( it.line() - 1 ) + "."
        );
    }

    // Run the plugin, if availabe.
    if( quality_string_plugin_ ) {
        quality_string_plugin_( buffer_, sequence );
    }
}

// =================================================================================================
//     Properties
// =================================================================================================

FastqReader& FastqReader::site_casing( SiteCasing value )
{
    site_casing_ = value;
    return *this;
}

FastqReader::SiteCasing FastqReader::site_casing() const
{
    return site_casing_;
}

FastqReader& FastqReader::valid_chars( std::string const& chars )
{
    if( chars.size() == 0 ) {
        lookup_.set_all( true );
        use_validation_ = false;
    } else {
        lookup_.set_all( false );
        lookup_.set_selection( chars, true );
        use_validation_ = true;
    }

    return *this;
}

std::string FastqReader::valid_chars() const
{
    // We need to check the valid chars lookup here, because we don't want to return a string
    // of _all_ chars.
    if( ! use_validation_ || lookup_.all_equal_to( true ) ) {
        return "";
    } else {
        return lookup_.get_chars_equal_to( true );
    }
}

utils::CharLookup<bool>& FastqReader::valid_char_lookup()
{
    return lookup_;
}

FastqReader& FastqReader::quality_encoding( QualityEncoding encoding )
{
    quality_encoding_ = encoding;
    return *this;
}

QualityEncoding FastqReader::quality_encoding()
{
    return quality_encoding_;
}

FastqReader& FastqReader::quality_string_plugin( quality_string_function const& plugin )
{
    quality_string_plugin_ = plugin;
    return *this;
}


} // namespace sequence
} // namespace genesis
