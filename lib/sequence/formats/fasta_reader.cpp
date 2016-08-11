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

#include "sequence/formats/fasta_reader.hpp"

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
 * @brief Create a default FastaReader. Per default, chars are turned upper case, but not validated.
 *
 * See to_upper() and valid_chars() to change this behaviour.
 */
FastaReader::FastaReader()
{
    lookup_.set_all( true );
}

// =================================================================================================
//     Reading
// =================================================================================================

/**
 * @brief Read all Sequences from a std::istream in Fasta format into a SequenceSet.
 */
void FastaReader::from_stream ( std::istream& input_stream, SequenceSet& sequence_set ) const
{
    utils::InputStream it( utils::make_unique< utils::StreamInputSource >( input_stream ));
    parse_document( it, sequence_set );
}

/**
 * @brief Read all Sequences from a file in Fasta format into a SequenceSet.
 */
void FastaReader::from_file ( std::string const& file_name, SequenceSet& sequence_set ) const
{
    utils::InputStream it( utils::make_unique< utils::FileInputSource >( file_name ));
    parse_document( it, sequence_set );
}

/**
 * @brief Read all Sequences from a std::string in Fasta format into a SequenceSet.
 */
void FastaReader::from_string ( std::string const& input_string, SequenceSet& sequence_set ) const
{
    utils::InputStream it( utils::make_unique< utils::StringInputSource >( input_string ));
    parse_document( it, sequence_set );
}

// =================================================================================================
//     Parsing
// =================================================================================================

/**
 * @brief Parse a whole fasta document into a SequenceSet.
 *
 * This function is mainly used internally by the reading functions from_...().
 * It uses the currently set parsing_method() for parsing the data.
 */
void FastaReader::parse_document(
    utils::InputStream& input_stream,
    SequenceSet&        sequence_set
) const {
    Sequence seq;

    if( parsing_method_ == ParsingMethod::kDefault ) {
        while( parse_sequence( input_stream, seq ) ) {
            sequence_set.push_back( seq );
        }

    } else if( parsing_method_ == ParsingMethod::kPedantic ) {
        while( parse_sequence_pedantic( input_stream, seq ) ) {
            sequence_set.push_back( seq );
        }

    } else {
        // There are no other methods currently implemented.
        assert( false );
    }
}

/**
 * @brief Parse a Sequence in Fasta format.
 *
 * This function takes an InputStream and interprets it as a Fasta formatted sequence. It extracts
 * the data and writes it into the given Sequence object. See the class description of FastaReader
 * for the expected data format.
 *
 * The function stops after parsing one such sequence. It returns true if a sequence was extracted
 * and false if the stream is empty. If the input is not in the correct format, an
 * `std::runtime_error` exception is thrown indicating the malicious position in the input stream.
 *
 * This method has a maximum line length of utils::InputStream::BlockLength and reports errors
 * only on the line where the sequence starts.  If you have files with longer lines or want error
 * reporting at the exact line and column where the error occurs, use ParsingMethod::kPedantic
 * instead. See FastaReader::ParsingMethod for details.
 */
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

/**
 * @brief Parse a Sequence in Fasta format.
 *
 * This function takes an InputStream and interprets it as a Fasta formatted sequence. It extracts
 * the data and writes it into the given Sequence object. See the class description of FastaReader
 * for the expected data format.
 *
 * The function stops after parsing one such sequence. It returns true if a sequence was extracted
 * and false if the stream is empty. If the input is not in the correct format, an
 * `std::runtime_error` exception is thrown indicating the malicious position in the input stream.
 *
 * Compared to parse_sequence(), this function allows for arbitrarily long lines and
 * reports errors at the exact line and column where they occur. It is however
 * slower. Apart from that, there are no differences. See FastaReader::ParsingMethod for details.
 */
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

/**
 * @brief Set the parsing method.
 *
 * The parsing method is used for all the reader functions and parse_document().
 * See the @link FastaReader::ParsingMethod ParsingMethod enum@endlink for details.
 */
FastaReader& FastaReader::parsing_method( FastaReader::ParsingMethod value )
{
    parsing_method_ = value;
    return *this;
}

/**
 * @brief Return the currently set parsing method.
 *
 * See the @link FastaReader::ParsingMethod ParsingMethod enum@endlink for details.
 */
FastaReader::ParsingMethod FastaReader::parsing_method() const
{
    return parsing_method_;
}

/**
 * @brief Set whether Sequence sites are automatically turned into upper case.
 *
 * If set to `true` (default), all sites of the read Sequences are turned into upper case letters
 * automatically. This is demanded by the Fasta standard.
 * The function returns the FastaReader object to allow for fluent interfaces.
 */
FastaReader& FastaReader::to_upper( bool value )
{
    to_upper_ = value;
    return *this;
}

/**
 * @brief Return whether Sequence sites are automatically turned into upper case.
 */
bool FastaReader::to_upper() const
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

/**
 * @brief Return the currently set chars used for validating Sequence sites.
 *
 * An empty string means that no validation is done.
 */
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

/**
 * @brief Return the internal CharLookup that is used for validating the Sequence sites.
 *
 * This function is provided in case direct access to the lookup is needed. Usually, the
 * valid_chars() function should suffice. See there for details.
 */
utils::CharLookup<bool>& FastaReader::valid_char_lookup()
{
    return lookup_;
}

} // namespace sequence
} // namespace genesis
