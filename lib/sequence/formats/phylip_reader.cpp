/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "sequence/formats/phylip_reader.hpp"

#include "sequence/sequence_set.hpp"
#include "sequence/sequence.hpp"
#include "utils/core/fs.hpp"
#include "utils/core/std.hpp"
#include "utils/io/input_stream.hpp"
#include "utils/io/scanner.hpp"
#include "utils/text/string.hpp"

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

/**
 * @brief Create a default PhylipReader. Per default, chars are turned upper case, but not validated.
 *
 * See to_upper() and valid_chars() to change this behaviour.
 */
PhylipReader::PhylipReader()
{
    lookup_.set_all( true );
}

// =================================================================================================
//     Reading
// =================================================================================================

/**
 * @brief Read all Sequences from a std::istream in Phylip format into a SequenceSet.
 *
 * This function is only allowed for Mode::kSequential and Mode::kInterleaved. Automatic mode does
 * not work, as the stream might need to be reset, which is not possible. See mode(Mode).
 */
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
        default:
            throw std::runtime_error(
                "Automatic mode of PhylipReader not possible when using from_stream()."
            );
    }
}

/**
 * @brief Read all Sequences from a file in Phylip format into a SequenceSet.
 */
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
    } else {

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
            sequence_set.push_back( std::move(s) );
        }
    }
}

/**
 * @brief Read all Sequences from a std::string in Phylip format into a SequenceSet.
 */
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
    } else {

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
            sequence_set.push_back( std::move(s) );
        }
    }
}

// =================================================================================================
//     Parsing
// =================================================================================================

/**
 * @brief Parse a Phylip header and return the contained sequence count and length.
 *
 * This helper function expects to find a Phylip header in the form `x y`, which describes the
 * number of sequences `x` in the Phylip data and their length `y`. It leaves the stream at the
 * beginning of the next line.
 *
 * Currently, the function does not support Phylip options. According to the standard, those might
 * follow after the two integers, but will lead to exceptions here.
 */
std::pair<size_t, size_t> PhylipReader::parse_phylip_header( utils::InputStream& it ) const
{
    // Read number of sequences.
    utils::skip_while( it, isblank );
    std::string num_seq_str = utils::read_while( it, isdigit );
    if( num_seq_str.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Expecting sequence number at "
            + it.at() + "."
        );
    }
    size_t num_seq = std::stoi( num_seq_str );

    // Read length of sequences.
    utils::skip_while( it, isblank );
    std::string len_seq_str = utils::read_while( it, isdigit );
    if( len_seq_str.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Expecting sequence length at "
            + it.at() + "."
        );
    }
    size_t len_seq = std::stoi( len_seq_str );

    // Sanity check.
    if( num_seq == 0 || len_seq == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Sequences are empty."
        );
    }

    // Process end of header line.
    utils::skip_while( it, isblank );
    if( !it || *it != '\n' ) {
        throw std::runtime_error(
            "Malformed Phylip " + it.source_name() + ": Expecting end of line at " + it.at() + "."
        );
    }
    ++it;

    return { num_seq, len_seq };
}

/**
 * @brief Parse and return a Phylip label.
 *
 * This helper functions either takes the first `label_length` chars as a label or, if
 * `label_length == 0` takes all chars until the first blank as label. It returns the trimmed
 * label and leaves the stream at the next char after the label (and after subsequent blanks).
 */
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

/**
 * @brief Parse one sequence line.
 *
 * The line (which can also start after a label) is parsed until the first '\\n' char.
 * While parsing, the options to_upper() and valid_chars() are applied according to their
 * settings. The stream is left at the beginning of the next line.
 */
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

/**
 * @brief Parse a whole Phylip file using the sequential variant (Mode::kSequential).
 */
void PhylipReader::parse_phylip_sequential(  utils::InputStream& it, SequenceSet& sset ) const
{
    auto sizes = parse_phylip_header( it );
    size_t num_seq = sizes.first;
    size_t len_seq = sizes.second;

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
        sset.push_back( seq );
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

/**
 * @brief Parse a whole Phylip file using the interleaved variant (Mode::kInterleaved).
 */
void PhylipReader::parse_phylip_interleaved( utils::InputStream& it, SequenceSet& sset ) const
{
    // Parse header line.
    auto sizes = parse_phylip_header( it );
    size_t num_seq = sizes.first;
    size_t len_seq = sizes.second;

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
        sset.push_back( seq );
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

/**
 * @brief Set the mode for reading sequences.
 *
 * Phylip offers two variants for storing the sequences: sequential and interleaved. As there is no
 * option or flag in the file itself, there is no chance of knowing the variant without trying to
 * parse it. If one fails but not the other, it is proabably the latter variant. However, there are
 * instances where both variants are valid at the same time, but yield different sequences.
 * So, in general detecting the correct variant is undecidable, making Phylip a non-well-defined
 * format.
 *
 * In order to avoid those problems, this function explicitly sets the variant being used for
 * parsing. By default, it is set to Mode::kSequential. Use Mode::kInterleaved for the other
 * variant.
 *
 * We also offer a Mode::kAutomatic. It first tries to parse in sequential mode, and, if this fails,
 * in interleaved mode. However, as this might involve starting from the beginning of the data, this
 * is only possible with the from_file() and from_string() readers and does not work when using the
 * from_stream() reader. Also, be aware that using automatic mode is slower because of
 * implementation details induced by those limitations.
 * Try to avoid automatic mode. If possible, try to avoid Phylip at all.
 */
PhylipReader& PhylipReader::mode( Mode value )
{
    mode_ = value;
    return *this;
}

/**
 * Return the currently set mode for parsing Phylip.
 *
 * See the setter mode( Mode ) for details.
 */
PhylipReader::Mode PhylipReader::mode() const
{
    return mode_;
}

/**
 * @brief Set the length of the label in front of the sequences.
 *
 * Phylip has the weird property that labels are written in front of sequences and do not need
 * to have a delimiter, but instead are simply the first `n` characters of the string. This value
 * determines after how many chars the label ends and the actual sequence begins.
 *
 * If set to a value greater than 0, exaclty this many characters are read as label. Thus, they
 * can also contain spaces. Spaces at the beginning or end of a label are stripped. The length
 * that is dictated by the Phylip standard is 10, but any other length can also be used.
 *
 * If set to 0 (default), a relaxed version of Phylip is used instead, where the sequence begin is
 * automatically detected. Labels can then be of arbitrary lengths, as long as they do not contain
 * white spaces. However, in this case, there has to be at least one space or tab character between
 * the label and the sequence. After this first whitespace, the rest of the line is then treated
 * as sequence data.
 *
 * The function returns the PhylipReader object to allow for fluent interfaces.
 */
PhylipReader& PhylipReader::label_length( size_t value )
{
    label_length_ = value;
    return *this;
}

/**
 * @brief Return the currently set label length.
 *
 * See the setter label_length( size_t ) for details.
 */
size_t PhylipReader::label_length() const
{
    return label_length_;
}

/**
 * @brief Set whether Sequence sites are automatically turned into upper case.
 *
 * If set to `true` (default), all sites of the read Sequences are turned into upper case letters
 * automatically. This is demanded by the Phylip standard.
 *
 * The function returns the PhylipReader object to allow for fluent interfaces.
 */
PhylipReader& PhylipReader::to_upper( bool value )
{
    to_upper_ = value;
    return *this;
}

/**
 * @brief Return whether Sequence sites are automatically turned into upper case.
 */
bool PhylipReader::to_upper() const
{
    return to_upper_;
}

/**
 * @brief Set the chars that are used for validating Sequence sites when reading them.
 *
 * When this function is called with a string of chars, those chars are used to validate the sites
 * when reading them. That is, only sequences consisting of the given chars are valid.
 *
 * If set to an empty string, this check is deactivated. This is also the default, meaning that no
 * checking is done.
 *
 * In case that to_upper() is set to `true`: The validation is done after making the char upper
 * case, so that only capital letters have to be provided for validation.
 * In case that to_upper() is set to `false`: All chars that are to be considered valid have to be
 * provided for validation.
 *
 * See `nucleic_acid_codes...()` and `amino_acid_codes...()` functions for presettings of chars
 * that can be used for validation here.
 */
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

/**
 * @brief Return the currently set chars used for validating Sequence sites.
 *
 * An empty string means that no validation is done.
 */
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

/**
 * @brief Return the internal CharLookup that is used for validating the Sequence sites.
 *
 * This function is provided in case direct access to the lookup is needed. Usually, the
 * valid_chars() function should suffice. See there for details.
 */
utils::CharLookup<bool>& PhylipReader::valid_char_lookup()
{
    return lookup_;
}

} // namespace sequence
} // namespace genesis
