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

#include "genesis/sequence/formats/phylip_reader.hpp"

#include "genesis/sequence/sequence_set.hpp"
#include "genesis/sequence/sequence.hpp"
#include "genesis/utils/core/algorithm.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
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

SequenceSet PhylipReader::read( std::shared_ptr<utils::BaseInputSource> source ) const
{
    // Create a new set and fill it.
    SequenceSet result;
    read( source, result );
    return result;
}

void PhylipReader::read( std::shared_ptr<utils::BaseInputSource> source, SequenceSet& target ) const
{
    utils::InputStream it( source );
    switch( mode_ ) {
        case Mode::kSequential: {
            parse_phylip_sequential( it, target );
            break;
        }
        case Mode::kInterleaved: {
            parse_phylip_interleaved( it, target );
            break;
        }
        default: {
            // We already covered all cases above. This cannot happen.
            assert( false );
        }
    }
}

// =================================================================================================
//     Parsing
// =================================================================================================

PhylipReader::Header PhylipReader::parse_phylip_header( utils::InputStream& it ) const
{
    Header result;

    // Read number of sequences.
    utils::skip_while( it, ::isblank );
    std::string num_seq_str = utils::read_while( it, ::isdigit );
    if( num_seq_str.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Expecting sequence number at "
            + it.at() + "."
        );
    }
    result.num_sequences = std::stoi( num_seq_str );

    // Read length of sequences.
    utils::skip_while( it, ::isblank );
    std::string len_seq_str = utils::read_while( it, ::isdigit );
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
    utils::skip_while( it, ::isblank );
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
    if( !it || ! ::isgraph( *it ) ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Expecting label at " + it.at() + "."
        );
    }

    // Scan label until first blank/tab.
    if( label_length_ == 0 ) {
        label = utils::read_while( it, ::isgraph );
        if( !it || ! ::isblank( *it ) ) {
            throw std::runtime_error(
                "Malformed Phylip " + it.source_name() + ": Expecting delimiting white space at "
                + it.at() + "."
            );
        }
        utils::skip_while( it, ::isblank );

    // Scan label for `label_length` many chars.
    } else {
        for( size_t i = 0; i < label_length_; ++i ) {
            if( !it || ! ::isprint( *it ) ) {
                throw std::runtime_error(
                    "Malformed Phylip " + it.source_name() + ": Invalid label at " + it.at() + "."
                );
            }

            label += *it;
            ++it;
        }
        label = utils::trim( label );
    }

    label = utils::trim( label );
    assert( label.size() > 0 );
    return label;
}

std::string PhylipReader::parse_phylip_sequence_line( utils::InputStream& it ) const
{
    // Read the (rest of) the current line from the input.
    auto seq = it.get_line();

    // Clean up as needed. Blanks always, digits only on demand.
    utils::erase_if( seq, []( char c ){
        return c == ' ' || c == '\t';
    });
    if( remove_digits_ ) {
        utils::erase_if( seq, []( char c ){
            return  ::isdigit( c );
        });
    }

    // Change case as needed.
    if( site_casing_ == SiteCasing::kToUpper ) {
        seq = utils::to_upper_ascii( seq );
    } else if( site_casing_ == SiteCasing::kToLower ) {
        seq = utils::to_lower_ascii( seq );
    }

    // Validate as needed.
    if( use_validation_ ) {
        for( auto const& c : seq ) {
            if( !lookup_[c] ) {
                throw std::runtime_error(
                    "Malformed Phylip " + it.source_name() + ": Invalid sequence symbol "
                    + utils::char_to_hex( c )
                    + " in sequence near line " + std::to_string( it.line() - 1 ) + "."
                );
            }
        }
    }

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

PhylipReader& PhylipReader::site_casing( SiteCasing value )
{
    site_casing_ = value;
    return *this;
}

PhylipReader::SiteCasing PhylipReader::site_casing() const
{
    return site_casing_;
}

PhylipReader& PhylipReader::remove_digits( bool value )
{
    remove_digits_ = value;
    return *this;
}

bool PhylipReader::remove_digits() const
{
    return remove_digits_;
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
