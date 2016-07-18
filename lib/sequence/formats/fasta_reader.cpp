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

#include "utils/io/counting_istream.hpp"

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
void FastaReader::from_stream ( std::istream& input_stream, SequenceSet& sset ) const
{
    utils::InputStream it( utils::make_unique< utils::StreamInputSource >( input_stream ));
    parse_document( it, sset );
}

/**
 * @brief Read all Sequences from a file in Fasta format into a SequenceSet.
 */
void FastaReader::from_file ( std::string const& file_name, SequenceSet& sset ) const
{
    utils::InputStream it( utils::make_unique< utils::FileInputSource >( file_name ));
    parse_document( it, sset );
}

/**
 * @brief Read all Sequences from a std::string in Fasta format into a SequenceSet.
 */
void FastaReader::from_string ( std::string const& input_string, SequenceSet& sset ) const
{
    utils::InputStream it( utils::make_unique< utils::StringInputSource >( input_string ));
    parse_document( it, sset );
}

// =================================================================================================
//     Parsing
// =================================================================================================

void FastaReader::parse_document(
    utils::InputStream& input_stream,
    SequenceSet& sset
) const {
    Sequence seq;
    while( parse_sequence( input_stream, seq ) ) {
        sset.push_back( seq );
    }
}

/**
 * @brief Parse a char sequence in Fasta format.
 *
 * This function takes an input stream and interprets as as a Fasta formatted sequence. It extracts
 * the data and writes it into the given Sequence object. See the class description of FastaReader
 * for the expected data format.
 *
 * The function stops after parsing one such sequence. It returns true if a sequence was extracted
 * and false if the stream is empty. If the input is not in the correct format, an
 * `std::runtime_error` exception is thrown indicating the malicious position in the input stream.
 *
 * More information on the format can be found at:
 *
 *    * http://en.wikipedia.org/wiki/FASTA_format
 *    * http://blast.ncbi.nlm.nih.gov/blastcgihelp.shtml
 *    * http://zhanglab.ccmb.med.umich.edu/FASTA/
 *
 * See parse_sequence_fast() for a faster (~ double the speed), but non-error-checking
 * version of this function.
 */
bool FastaReader::parse_sequence(
    utils::CountingIstream& input_stream,
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
            "Malformed Fasta file: Expecting '>' at beginning of sequence at " + it.at() + "."
        );
    }
    assert( it && *it == '>' );
    ++it;

    // Parse label.
    std::string label = utils::read_while( it, isgraph );
    if( label == "" ) {
        throw std::runtime_error(
            "Malformed Fasta file: Expecting label after '>' at " + it.at() + "."
        );
    }
    sequence.label( label );

    // Check for unexpected end of stream.
    if( !it || ( *it != '\n' && *it != ' ' )) {
        throw std::runtime_error(
            "Malformed Fasta file: Expecting a sequence after the label line at " + it.at() + "."
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
            "Malformed Fasta file: Expecting a sequence after the label line at " + it.at() + "."
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
            "Malformed Fasta file: Expecting a sequence after the label line at " + it.at() + "."
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
            if( !lookup_[c] ) {
                throw std::runtime_error(
                    "Malformed Fasta file: Invalid sequence symbols at " + it.at() + "."
                );
            }

            sites += c;
            ++it;
            ++count;
        }

        if( !it ) {
            throw std::runtime_error(
                "Malformed Fasta file: Sequence line does not end with '\\n' " + it.at() + "."
            );
        }
        assert( it && *it == '\n' );
        ++it;

        if( count == 0 ) {
            throw std::runtime_error(
                "Malformed Fasta file: Empty sequence line at " + it.at() + "."
            );
        }
    }
    assert( !it || *it == '>' );

    if( sites.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Fasta file: Empty sequence at " + it.at() + "."
        );
    }
    sequence.sites() = sites;

    return true;
}
// */

//*
bool FastaReader::parse_sequence(
    utils::InputStream& input_stream,
    Sequence&            sequence
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

    // char* line = it.get_line();
    auto line_p = it.get_line();
    char* line = line_p.first;

    if( line == nullptr || *line == '\0' || *line != '>' ) {
        throw std::runtime_error(
            "Malformed Fasta file: Expecting '>' at beginning of sequence at " + it.at() + "."
        );
    }
    assert( it && *line == '>' );
    ++line;

    sequence.label( std::string( line ) );

    // Skip comments.
    // while( it && *it == ';' ) {
    //     utils::skip_while( it, isprint );
    // }

    // Check for unexpected end of file.
    // if( it.eos() || *it != '\n' ) {
    //     throw std::runtime_error(
    //         "Malformed Fasta file: Expecting a sequence after the label line at " + it.at() + "."
    //     );
    // }
    // assert( it && *it == '\n' );
    // ++it;

    // Parse sequence. At every beginning of the outer loop, we are at a line start.
    std::string sites;
    sites.reserve( 50000 );
    // sequence.sites().reserve (50000);

    // char* buffer[50001];
    // char* buffer_pos;

    while( it && *it != '>' ) {
        assert( it.column() == 1 );

        line_p = it.get_line();

        // std::memcpy( buffer_pos, line,  );

        // char* sv = line;
        // while(*sv != '\0')
        // {
        //     // if( *sv >= 'a' && *sv <= 'z')
        //     //     *sv = *sv - ('a' - 'A');
        //     *sv = toupper(*sv);
        //     sv++;
        // }


        // // size_t count = 0;
        // char* run = line;
        // while( *run != '\0' ) {
        //     // *line = ( to_upper_ ? toupper(*line) : *line );
        //     // *run = toupper(*run);
        //
        //     char c = *run;
        //
        //     // char c = *line;
        //     // char c = ( to_upper_ ? toupper(*line) : *line );
        //     if( !lookup_[c] ) {
        //         throw std::runtime_error(
        //             "Malformed Fasta file: Invalid sequence symbols at " + it.at() + "."
        //         );
        //     }
        //
        //     // sites += toupper(*run);
        //     // sites += *run;
        //     // sites += c;
        //     ++run;
        //     // ++count;
        // }

        // sites += std::string( line_p.first );
        sites += std::string( line_p.first, line_p.second );
        // sites += std::string( line );
        // sequence.sites() += std::string( line );

        // while( it && *it != '\n' ) {
        //     char c = ( to_upper_ ? toupper(*it) : *it );
        //     if( !lookup_[c] ) {
        //         throw std::runtime_error(
        //             "Malformed Fasta file: Invalid sequence symbols at " + it.at() + "."
        //         );
        //     }
        //
        //     sites += c;
        //     ++it;
        //     ++count;
        // }
        //
        // if( !it ) {
        //     throw std::runtime_error(
        //         "Malformed Fasta file: Sequence line does not end with '\\n' " + it.at() + "."
        //     );
        // }
        // assert( it && *it == '\n' );
        // ++it;

        // if( count == 0 ) {
        //     throw std::runtime_error(
        //         "Malformed Fasta file: Empty sequence line at " + it.at() + "."
        //     );
        // }
    }
    assert( !it || *it == '>' );

    // if( sites.length() == 0 ) {
    //     throw std::runtime_error(
    //         "Malformed Fasta file: Empty sequence at " + it.at() + "."
    //     );
    // }

    if( to_upper_ ) {
        sequence.sites() = utils::to_upper_ascii( sites );
    } else {
        sequence.sites() = std::move( sites );
    }

    if( use_validation_ ) {
        for( auto const& c : sequence.sites() ) {
            if( !lookup_[c] ) {
                throw std::runtime_error(
                    "Malformed Fasta file: Invalid sequence symbols at " + it.at() + "."
                );
            }
        }
    }

    return true;
}
// */

/**
 * @brief Parse a Fasta sequence without checking for errors.
 *
 * This is a very fast implementation that neglects input error checking. Thus, the Fasta sequence
 * has to be well-formed in order for this function to work properly. See the class description of
 * FastaReader for the expected data format.
 *
 * If the expected conditions are not met, instead of exceptions, undefined behaviour results.
 * Most probably, it will either write rubbish into the sequence or produce a segfault or an
 * infinite loop. So be warned and check your data first. If they are good, enjoy the speed!
 */
/*
bool FastaReader::parse_sequence_fast(
    utils::InputStream& input_stream,
    Sequence&           sequence
) const {
    // Readability.
    auto& it = input_stream;

    // Check for data.
    if( it.eos() ) {
        return false;
    }

    // Check beginning of sequence.
    assert( *it == '>' );
    ++it;
    assert( it );

    // Parse label.
    std::string label;
    while( *it != '\n' && *it != ' ' ) {
        label += *it;
        ++it;
        assert( it );
    }
    sequence.label( label );
    assert( *it == '\n' || *it == ' ' );

    // Parse metadata.
    std::string metadata;
    if( *it == ' ' ) {
        ++it;
        assert( it );
        while( *it != '\n' ) {
            metadata += *it;
            ++it;
            assert( it );
        }
    }
    sequence.metadata( metadata );
    assert( *it == '\n' );
    ++it;
    assert( it );

    // Skip comments.
    while( *it == ';' ) {
        while( *it != '\n' ) {
            ++it;
        }
        assert( *it == '\n' );
        ++it;
        assert( it );
    }

    // Parse sequence. At every beginning of the outer loop, we are at a line start.
    sequence.sites().clear();
    while( it && *it != '>' ) {
        while( *it != '\n' ) {
            sequence.sites() += *it;
            ++it;
            assert( it );
        }
        assert( *it == '\n' );
        ++it;
    }

    return true;
}
*/

// =================================================================================================
//     Properties
// =================================================================================================

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
        lookup_.set_selection( chars );
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
    if( ! use_validation_ || lookup_.all_set() ) {
        return "";
    } else {
        return lookup_.get_selection();
    }
}

/**
 * @brief Return the internal CharLookup that is used for validating the Sequence sites.
 *
 * This function is provided in case direct access to the lookup is needed. Usually, the
 * valid_chars() function should suffice. See there for details.
 */
utils::CharLookup& FastaReader::valid_char_lookup()
{
    return lookup_;
}

} // namespace sequence
} // namespace genesis
