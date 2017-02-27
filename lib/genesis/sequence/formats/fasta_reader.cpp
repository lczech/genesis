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

#include "genesis/sequence/formats/fasta_reader.hpp"

#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/string.hpp"

#include <assert.h>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Constructor and Rule of Five
// =================================================================================================

FastaReader::FastaReader()
{
    lookup_.set_all( true );
}

// =================================================================================================
//     Reading
// =================================================================================================

void FastaReader::from_stream ( std::istream& input_stream, SequenceSet& sequence_set ) const
{
    // Create an input stream and process it.
    utils::InputStream it( utils::make_unique< utils::StreamInputSource >( input_stream ));
    parse_document( it, sequence_set );
}

SequenceSet FastaReader::from_stream( std::istream& input_stream ) const
{
    // Create a new set and fill it.
    SequenceSet result;
    from_stream( input_stream, result );
    return result;
}

void FastaReader::from_file ( std::string const& file_name, SequenceSet& sequence_set ) const
{
    // Create an input stream and process it.
    utils::InputStream it( utils::make_unique< utils::FileInputSource >( file_name ));
    parse_document( it, sequence_set );
}

SequenceSet FastaReader::from_file( std::string const& file_name ) const
{
    // Create a new set and fill it.
    SequenceSet result;
    from_file( file_name, result );
    return result;
}

void FastaReader::from_string ( std::string const& input_string, SequenceSet& sequence_set ) const
{
    // Create an input stream and process it.
    utils::InputStream it( utils::make_unique< utils::StringInputSource >( input_string ));
    parse_document( it, sequence_set );
}

SequenceSet FastaReader::from_string( std::string const& input_string ) const
{
    // Create a new set and fill it.
    SequenceSet result;
    from_string( input_string, result );
    return result;
}

// =================================================================================================
//     Parsing
// =================================================================================================

void FastaReader::parse_document(
    utils::InputStream& input_stream,
    SequenceSet&        sequence_set
) const {
    Sequence seq;

    if( parsing_method_ == ParsingMethod::kDefault ) {
        while( parse_sequence( input_stream, seq ) ) {
            sequence_set.add( seq );
        }

    } else if( parsing_method_ == ParsingMethod::kPedantic ) {
        while( parse_sequence_pedantic( input_stream, seq ) ) {
            sequence_set.add( seq );
        }

    } else {
        // There are no other methods currently implemented.
        assert( false );
    }
}

bool FastaReader::parse_sequence(
    utils::InputStream& input_stream,
    Sequence&           sequence
) const {
    // Init. Call clear in order to avoid not setting properties that might be added to
    // Sequence in the future. Should not noticeable affect speed, as the sequence string capacities
    // should not change when setting the strings to empty strings.
    auto& it = input_stream;
    sequence.clear();

    // Check for data.
    if( !it ) {
        return false;
    }

    // We will do whole line reading in this parser. Thus, when reporting an error within a line,
    // the input stream will already be at a different position. We can thus only report the
    // beginning of the sequence where the error occured, but not the actual position.
    // If we wanted to do this, we'd need more bookkeeping and awkward helper functions.
    // Use parse_sequence_pedantic() instead in cases where you need exact error reporting.
    auto seq_at = it.at();

    // Scope to ensure that the label and metadata line is only used
    // while we actually are in that line.
    {

    // Get the label and metadata line.
    auto  line_pair = it.get_line();
    char* line = line_pair.first;

    // Check beginning of sequence.
    if( line == nullptr || *line != '>' ) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name()
            + ": Expecting '>' at beginning of sequence at " + seq_at + "."
        );
    }
    assert( line && *line == '>' );
    ++line;

    // Parse label.
    std::string label = utils::read_while( line, isgraph );
    if( label == "" ) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name()
            + ": Expecting label after '>' in sequence at " + seq_at + "."
        );
    }
    sequence.label( label );

    // Check for unexpected end of stream.
    if( *line != '\0' && *line != ' ' ) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name()
            + ": Expecting a sequence after the label line in sequence at " + seq_at + "."
        );
    }
    assert( *line == '\0' || *line == ' ' );

    // Parse metadata.
    std::string metadata;
    if( *line == ' ' ) {
        ++line;
        metadata = utils::read_while( line, isprint );
    }
    sequence.metadata( metadata );

    // Check for unexpected end of file.
    if( *line != '\0' ) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name()
            + ": Unexpected characters at the end of the label line in sequence at " + seq_at + "."
        );
    }
    assert( *line == '\0' );

    } // End of line scope. We are done with the label and metadata line.

    // Skip comments.
    while( it && *it == ';' ) {
        utils::skip_until( it, '\n' );
        assert( it && *it == '\n' );
        ++it;
    }

    // Check for unexpected end of file.
    if( !it ) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name()
            + ": Expecting a sequence after the label line in sequence at " + seq_at + "."
        );
    }
    assert( it );

    // Reserve some tmp memory. We will later copy the content, so that superfluous capacity
    // is stripped.
    // We could do a sites.reserve( ... ) here, but this yields only minor speedups.
    std::string sites;
    // sites.reserve( n );

    // Parse sequence. At every beginning of the loop, we are at a line start.
    while( it && *it != '>' ) {
        assert( it.column() == 1 );

        auto line_pair = it.get_line();
        if( line_pair.second == 0 ) {
            throw std::runtime_error(
                "Malformed Fasta " + it.source_name()
                + ": Empty sequence line in sequence at " + seq_at + "."
            );
        }

        sites += std::string( line_pair.first, line_pair.second );
    }
    assert( !it || *it == '>' );

    if( sites.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name() + ": Empty sequence at " + seq_at + "."
        );
    }

    if( to_upper_ ) {
        sequence.sites() = utils::to_upper_ascii( sites );
    } else {
        // We could do a move here instead, but this way, we save some memory, which might be
        // more reasonable for big sequences files than the small gain in speed.
        // sequence.sites() = std::move(sites);
        sequence.sites() = sites;
    }

    if( use_validation_ ) {
        for( auto const& c : sequence.sites() ) {
            if( !lookup_[c] ) {
                throw std::runtime_error(
                    "Malformed Fasta " + it.source_name()
                    + ": Invalid sequence symbols in sequence at " + seq_at + "."
                );
            }
        }
    }

    return true;
}

bool FastaReader::parse_sequence_pedantic(
    utils::InputStream& input_stream,
    Sequence&           sequence
) const {
    // Init. Call clear in order to avoid not setting properties that might be added to
    // Sequence in the future. Should not noticeable affect speed, as the sequence string capacities
    // should not change when setting the strings to empty strings.
    auto& it = input_stream;
    sequence.clear();

    // Check for data.
    if( !it ) {
        return false;
    }

    // Check beginning of sequence.
    if( it.current() != '>' ) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name()
            + ": Expecting '>' at beginning of sequence at " + it.at() + "."
        );
    }
    assert( it && *it == '>' );
    ++it;

    // Parse label.
    std::string label = utils::read_while( it, isgraph );
    if( label == "" ) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name()
            + ": Expecting label after '>' at " + it.at() + "."
        );
    }
    sequence.label( label );

    // Check for unexpected end of stream.
    if( !it || ( *it != '\n' && *it != ' ' )) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name()
            + ": Expecting a sequence after the label line at " + it.at() + "."
        );
    }
    assert( it && (*it == '\n' || *it == ' ') );

    // Parse metadata.
    std::string metadata;
    if( *it == ' ' ) {
        ++it;
        metadata = utils::read_while( it, isprint );
    }
    sequence.metadata( metadata );

    // Check for unexpected end of file.
    if( !it || *it != '\n' ) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name()
            + ": Expecting a sequence after the label line at " + it.at() + "."
        );
    }
    assert( it && *it == '\n' );

    // Skip comments.
    while( it && *it == ';' ) {
        utils::skip_while( it, isprint );
    }

    // Check for unexpected end of file.
    if( !it || *it != '\n' ) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name()
            + ": Expecting a sequence after the label line at " + it.at() + "."
        );
    }
    assert( it && *it == '\n' );
    ++it;

    // Parse sequence. At every beginning of the outer loop, we are at a line start.
    std::string sites;
    while( it && *it != '>' ) {
        assert( it.column() == 1 );

        size_t count = 0;
        while( it && *it != '\n' ) {
            char c = ( to_upper_ ? toupper(*it) : *it );
            if( use_validation_ && ! lookup_[c] ) {
                throw std::runtime_error(
                    "Malformed Fasta " + it.source_name()
                    + ": Invalid sequence symbols at " + it.at() + "."
                );
            }

            sites += c;
            ++it;
            ++count;
        }

        if( count == 0 ) {
            throw std::runtime_error(
                "Malformed Fasta " + it.source_name()
                + ": Empty sequence line at " + it.at() + "."
            );
        }

        if( !it ) {
            throw std::runtime_error(
                "Malformed Fasta " + it.source_name()
                + ": Sequence line does not end with '\\n' at " + it.at() + "."
            );
        }
        assert( it && *it == '\n' );
        ++it;
    }
    assert( !it || *it == '>' );

    if( sites.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Fasta " + it.source_name()
            + ": Empty sequence at " + it.at() + "."
        );
    }

    // Copy the sequence. We do not use move here, as we can save some memory this way.
    sequence.sites() = sites;

    return true;
}

// =================================================================================================
//     Properties
// =================================================================================================

FastaReader& FastaReader::parsing_method( FastaReader::ParsingMethod value )
{
    parsing_method_ = value;
    return *this;
}

FastaReader::ParsingMethod FastaReader::parsing_method() const
{
    return parsing_method_;
}

FastaReader& FastaReader::to_upper( bool value )
{
    to_upper_ = value;
    return *this;
}

bool FastaReader::to_upper() const
{
    return to_upper_;
}

FastaReader& FastaReader::valid_chars( std::string const& chars )
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

std::string FastaReader::valid_chars() const
{
    // We need to check the valid chars lookup here, because we don't want to return a string
    // of _all_ chars.
    if( ! use_validation_ || lookup_.all_equal_to( true ) ) {
        return "";
    } else {
        return lookup_.get_chars_equal_to( true );
    }
}

utils::CharLookup<bool>& FastaReader::valid_char_lookup()
{
    return lookup_;
}

} // namespace sequence
} // namespace genesis
