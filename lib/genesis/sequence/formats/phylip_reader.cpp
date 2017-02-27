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

#include "genesis/sequence/formats/phylip_reader.hpp"

#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Constructor and Rule of Five
// =================================================================================================

PhylipReader::PhylipReader()
{
    lookup_.set_all( true );
}

// =================================================================================================
//     Reading
// =================================================================================================

void PhylipReader::from_stream( std::istream& input_stream, SequenceSet& sequence_set ) const
{
    utils::InputStream it( utils::make_unique< utils::StreamInputSource >( input_stream ));

    switch( mode_ ) {
        case Mode::kSequential:
            parse_phylip_sequential( it, sequence_set );
            break;

        case Mode::kInterleaved:
            parse_phylip_interleaved( it, sequence_set );
            break;

        case Mode::kAutomatic:
            throw std::runtime_error(
                "Automatic mode of PhylipReader not possible when using from_stream()."
            );

        default:
            // We already covered all cases above. This cannot happen.
            assert( false );
    }
}

SequenceSet PhylipReader::from_stream( std::istream& input_stream ) const
{
    // Create a new set and fill it.
    SequenceSet result;
    from_stream( input_stream, result );
    return result;
}

void PhylipReader::from_file( std::string const& file_name, SequenceSet& sequence_set ) const
{
    // This function is very similar to from_string, but has some differences in treating the input
    // when needing to restart in automatic mode. Unfortunately, we have to live with this code
    // duplication for now. Everything else would end up in a mess of small helper functions, which
    // is also not nice.

    // Get stream.
    utils::InputStream it( utils::make_unique< utils::FileInputSource >( file_name ));

    // If the mode is specified, use it.
    if( mode_ == Mode::kSequential ) {
        parse_phylip_sequential( it, sequence_set );
    } else if( mode_ == Mode::kInterleaved ) {
        parse_phylip_interleaved( it, sequence_set );

    // If the mode is automatic, we need to do some magic.
    } else if( mode_ == Mode::kAutomatic ) {

        // We need a temporary set, because in case of failure, we need to start from the beginning,
        // but we do not want to clear all other sequences in the set (that might be there from
        // other stuff done before calling this function).
        SequenceSet tmp;

        // Try sequential first.
        try {
            parse_phylip_sequential( it, tmp );

        // If it fails, restart the stream and try interleaved.
        // If this also throws, the file is ill formatted and the exception will be forwared to
        // the caller of this function.
        } catch ( ... ) {
            tmp.clear();

            // Prepare stream. Again. Then parse.
            utils::InputStream it( utils::make_unique< utils::FileInputSource >( file_name ));
            parse_phylip_interleaved( it, tmp );
        }

        // If we reach this point, one of the parses succeeded.
        // Move all sequences to the actual target.
        for( auto s : tmp ) {
            sequence_set.add( std::move(s) );
        }

    } else {
        // We already covered all cases above. This cannot happen.
        assert( false );
    }
}

SequenceSet PhylipReader::from_file( std::string const& file_name ) const
{
    // Create a new set and fill it.
    SequenceSet result;
    from_file( file_name, result );
    return result;
}

void PhylipReader::from_string( std::string const& input_string, SequenceSet& sequence_set ) const
{
    // This function is very similar to from_file. See there for some more code explanations and for
    // the reason why we currently do not re-engineer this to avoid this code duplication.

    // Prepare stream.
    utils::InputStream it( utils::make_unique< utils::StringInputSource >( input_string ));

    // If the mode is specified, use it.
    if( mode_ == Mode::kSequential ) {
        parse_phylip_sequential( it, sequence_set );
    } else if( mode_ == Mode::kInterleaved ) {
        parse_phylip_interleaved( it, sequence_set );

    // If the mode is automatic, we need to do some magic.
    } else if( mode_ == Mode::kAutomatic ) {

        // Temporary set.
        SequenceSet tmp;

        // Try sequential first.
        try {
            parse_phylip_sequential( it, tmp );

        // If it fails, restart the stream and try interleaved.
        } catch ( ... ) {
            tmp.clear();

            // Prepare stream. Again. Then parse.
            utils::InputStream it( utils::make_unique< utils::StringInputSource >( input_string ));
            parse_phylip_interleaved( it, tmp );
        }

        // If we reach this point, one of the parses succeeded.
        // Move all sequences to the actual target.
        for( auto s : tmp ) {
            sequence_set.add( std::move(s) );
        }

    } else {
        // We already covered all cases above. This cannot happen.
        assert( false );
    }
}

SequenceSet PhylipReader::from_string( std::string const& input_string ) const
{
    // Create a new set and fill it.
    SequenceSet result;
    from_string( input_string, result );
    return result;
}

// =================================================================================================
//     Parsing
// =================================================================================================

PhylipReader::Header PhylipReader::parse_phylip_header( utils::InputStream& it ) const
{
    Header result;

    // Read number of sequences.
    utils::skip_while( it, isblank );
    std::string num_seq_str = utils::read_while( it, isdigit );
    if( num_seq_str.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Expecting sequence number at "
            + it.at() + "."
        );
    }
    result.num_sequences = std::stoi( num_seq_str );

    // Read length of sequences.
    utils::skip_while( it, isblank );
    std::string len_seq_str = utils::read_while( it, isdigit );
    if( len_seq_str.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Expecting sequence length at "
            + it.at() + "."
        );
    }
    result.len_sequences = std::stoi( len_seq_str );

    // Sanity check.
    if( result.num_sequences == 0 || result.len_sequences == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Sequences are empty."
        );
    }

    // Process end of header line and proceed to first non-empty line.
    utils::skip_while( it, isblank );
    result.options = utils::trim_right( utils::read_to_end_of_line( it ));
    if( !it || *it != '\n' ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Expecting end of line at " + it.at() + "."
        );
    }
    utils::skip_while( it, '\n' );

    return result;
}

std::string PhylipReader::parse_phylip_label( utils::InputStream& it ) const
{
    std::string label;

    // Labels need to start with some graphical char.
    if( !it || !isgraph( *it ) ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Expecting label at " + it.at() + "."
        );
    }

    // Scan label until first blank/tab.
    if( label_length_ == 0 ) {
        label = utils::read_while( it, isgraph );
        if( !it || !isblank( *it ) ) {
            throw std::runtime_error(
                "Malformed Phylip " + it.source_name() + ": Expecting delimiting white space at "
                + it.at() + "."
            );
        }
        utils::skip_while( it, isblank );

    // Scan label for `label_length` many chars.
    } else {
        for( size_t i = 0; i < label_length_; ++i ) {
            if( !it || !isprint( *it ) ) {
                throw std::runtime_error(
                    "Malformed Phylip " + it.source_name() + ": Invalid label at " + it.at() + "."
                );
            }

            label += *it;
            ++it;
        }
    }

    label = utils::trim( label );
    assert( label.size() > 0 );
    return label;
}

std::string PhylipReader::parse_phylip_sequence_line( utils::InputStream& it ) const
{
    std::string seq;

    // Process the whole line.
    while( it && *it != '\n' ) {
        // Skip all blanks and digits.
        if( isblank( *it ) || isdigit( *it ) ) {
            ++it;
            continue;
        }

        // Check and process current char.
        char c = ( to_upper_ ? toupper(*it) : *it );
        if( use_validation_ && !lookup_[c] ) {
            throw std::runtime_error(
                "Malformed Phylip " + it.source_name() + ": Invalid sequence symbols at "
                + it.at() + "."
            );
        }
        seq += c;
        ++it;
    }

    // All lines need to end with \n
    if( !it ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Sequence line does not end with '\\n' "
            + it.at() + "."
        );
    }

    assert( it && *it == '\n' );
    ++it;

    return seq;
}

void PhylipReader::parse_phylip_sequential(  utils::InputStream& it, SequenceSet& sset ) const
{
    // Parse header line.
    auto header = parse_phylip_header( it );
    size_t num_seq = header.num_sequences;
    size_t len_seq = header.len_sequences;

    // Process the given number of sequences. If there are not enough, the inner functions will
    // throw. If there are too many, the check at the end will throw.
    for( size_t seq_n = 0; seq_n < num_seq; ++seq_n ) {
        assert( it.column() == 1 );
        Sequence seq;

        // Parse label.
        seq.label( parse_phylip_label( it ) );

        // Parse sequence. As long as we did not read as many sites as the header claimed, we read
        // more lines from the input stream. If we then read too many chars (checked in the next
        // step), the file is ill formatted. This is because a sequence always has to end with \n,
        // and the label of the next sequence always has to start at the beginning of the line.
        seq.sites().reserve( len_seq );
        while( seq.sites().length() < len_seq ) {
            seq.sites() += parse_phylip_sequence_line( it );
            assert( it.column() == 1 );
        }

        // Check sequence length.
        if( seq.sites().length() > len_seq ) {
            throw std::runtime_error(
                "Malformed Phylip " + it.source_name() + ": Sequence with length "
                + std::to_string( seq.sites().length() ) + " instead of "
                + std::to_string( len_seq ) + " at " + it.at() + "."
            );
        }
        assert( seq.sites().length() == len_seq );

        // Add to set.
        sset.add( seq );
    }

    // Final checks.
    utils::skip_while( it, isspace );
    if( it ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Expected end of file at " + it.at() + "."
        );
    }
    assert( sset.size() == num_seq );
}

void PhylipReader::parse_phylip_interleaved( utils::InputStream& it, SequenceSet& sset ) const
{
    // Parse header line.
    auto header = parse_phylip_header( it );
    size_t num_seq = header.num_sequences;
    size_t len_seq = header.len_sequences;

    // Helper function that checks the sequence length and throws if it is too long.
    auto check_seq_len = [ &it, &len_seq ] ( Sequence const& seq ) {
        if( seq.length() > len_seq ) {
            throw std::runtime_error(
                "Malformed Phylip " + it.source_name() + ": Sequence with length "
                + std::to_string( seq.sites().length() ) + " instead of "
                + std::to_string( len_seq ) + " at " + it.at() + "."
            );
        }
    };

    // Process the first block, which contains the labels.
    for( size_t seq_n = 0; seq_n < num_seq; ++seq_n ) {
        assert( it.column() == 1 );
        Sequence seq;

        // Parse label.
        seq.label( parse_phylip_label( it ) );

        // Reserve mem and parse first part of sequence.
        seq.sites().reserve( len_seq );
        seq.sites() += parse_phylip_sequence_line( it );
        check_seq_len( seq );

        // Add to set.
        sset.add( seq );
    }

    // Helper function that checks whether there are still sequences in the set that are not yet
    // done (i.e., don't have `len_seq` length).
    auto unfinished_sequences = [ & ] () {
        for( auto const& seq : sset ) {
            assert( seq.length() <= len_seq );
            if( seq.length() < len_seq ) {
                return true;
            }
        }
        return false;
    };

    while( unfinished_sequences() ) {
        // Each block might start with an empty line. Skip.
        if( !it ) {
            throw std::runtime_error(
                "Malformed Phylip " + it.source_name() + ": Unexpected end of file at "
                + it.at() + "."
            );
        }
        if( *it == '\n' ) {
            ++it;
        }

        // Parse the next block.
        for( size_t seq_n = 0; seq_n < num_seq; ++seq_n ) {
            assert( it.column() == 1 );
            sset[seq_n].sites() += parse_phylip_sequence_line( it );
            check_seq_len( sset[seq_n] );
        }
    }

    assert( sset.size() == num_seq );
}

// =================================================================================================
//     Properties
// =================================================================================================

PhylipReader& PhylipReader::mode( Mode value )
{
    mode_ = value;
    return *this;
}

PhylipReader::Mode PhylipReader::mode() const
{
    return mode_;
}

PhylipReader& PhylipReader::label_length( size_t value )
{
    label_length_ = value;
    return *this;
}

size_t PhylipReader::label_length() const
{
    return label_length_;
}

PhylipReader& PhylipReader::to_upper( bool value )
{
    to_upper_ = value;
    return *this;
}

bool PhylipReader::to_upper() const
{
    return to_upper_;
}

PhylipReader& PhylipReader::valid_chars( std::string const& chars )
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

std::string PhylipReader::valid_chars() const
{
    // We need to check the valid chars lookup here, because we don't want to return a string
    // of _all_ chars.
    if( ! use_validation_ || lookup_.all_equal_to( true ) ) {
        return "";
    } else {
        return lookup_.get_chars_equal_to( true );
    }
}

utils::CharLookup<bool>& PhylipReader::valid_char_lookup()
{
    return lookup_;
}

} // namespace sequence
} // namespace genesis
