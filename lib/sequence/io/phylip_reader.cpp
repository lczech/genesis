/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/io/phylip_reader.hpp"

#include "sequence/sequence_set.hpp"
#include "sequence/sequence.hpp"
#include "utils/core/fs.hpp"
#include "utils/io/counting_istream.hpp"
#include "utils/io/lexer/scanner.hpp"
#include "utils/text/string.hpp"

#include <assert.h>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace sequence {

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
std::pair<size_t, size_t> PhylipReader::parse_phylip_header( utils::CountingIstream& it ) const
{
    // Read number of sequences.
    lexer::skip_while( it, isblank );
    std::string num_seq_str;
    lexer::copy_while( it, num_seq_str, isdigit );
    if( num_seq_str.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip file: Expecting sequence number at " + it.at() + "."
        );
    }
    size_t num_seq = std::stoi( num_seq_str );

    // Read length of sequences.
    lexer::skip_while( it, isblank );
    std::string len_seq_str;
    lexer::copy_while( it, len_seq_str, isdigit );
    if( len_seq_str.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip file: Expecting sequence length at " + it.at() + "."
        );
    }
    size_t len_seq = std::stoi( len_seq_str );

    // Sanity check.
    if( num_seq == 0 || len_seq == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip file: Sequences are empty."
        );
    }

    // Process end of header line.
    lexer::skip_while( it, isblank );
    if( !it || *it != '\n' ) {
        throw std::runtime_error(
            "Malformed Phylip file: Expecting end of line at " + it.at() + "."
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
std::string PhylipReader::parse_phylip_label( utils::CountingIstream& it ) const
{
    std::string label;

    // Scan label until first blank/tab.
    if( label_length_ == 0 ) {
        lexer::copy_while( it, label, isgraph );
        if( !it || !isblank( *it ) ) {
            throw std::runtime_error(
                "Malformed Phylip file: Expecting delimiting white space at " + it.at() + "."
            );
        }
        lexer::skip_while( it, isblank );

    // Scan label for `label_length` many chars.
    } else {
        for( size_t i = 0; i < label_length_; ++i ) {
            if( !it || !isprint( *it ) ) {
                throw std::runtime_error(
                    "Malformed Phylip file: Invalid label at " + it.at() + "."
                );
            }

            label += *it;
            ++it;
        }
    }

    return text::trim( label );
}

/**
 * @brief Parse one sequence line.
 *
 * The line (which can also start after a label) is parsed until the first '\\n' char.
 * While parsing, the options to_upper() and validate_chars() are applied according to their
 * settings. The stream is left at the beginning of the next line.
 */
std::string PhylipReader::parse_phylip_sequence_line( utils::CountingIstream& it ) const
{
    std::string seq;
    size_t count = 0;

    // Process the whole line.
    while( it && *it != '\n' ) {
        ++count;

        // Skip all blanks and digits.
        if( isblank( *it ) || isdigit( *it ) ) {
            ++it;
            continue;
        }

        // Check and process current char.
        char c = ( to_upper_ ? toupper(*it) : *it );
        if( !lookup_[c] ) {
            throw std::runtime_error(
                "Malformed Phylip file: Invalid sequence symbols at " + it.at() + "."
            );
        }
        seq += c;
        ++it;
    }

    // All lines need to end with \n
    if( !it ) {
        throw std::runtime_error(
            "Malformed Phylip file: Sequence line does not end with '\\n' " + it.at() + "."
        );
    }

    // Sequence lines cannot be empty.
    if( count == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip file: Empty sequence line at " + it.at() + "."
        );
    }

    assert( it && *it == '\n' );
    ++it;

    return seq;
}

void PhylipReader::parse_phylip_interleaved( utils::CountingIstream& it, SequenceSet& sset ) const
{
    (void) it;
    (void) sset;

    auto sizes = parse_phylip_header( it );
    (void) sizes;
}

void PhylipReader::parse_phylip_sequential(  utils::CountingIstream& it, SequenceSet& sset ) const
{
    (void) it;
    (void) sset;

    auto sizes = parse_phylip_header( it );
    (void) sizes;
}

// =================================================================================================
//     Reading
// =================================================================================================

/**
 * @brief Read all Sequences from a std::istream in Phylip format into a SequenceSet.
 */
void PhylipReader::from_stream( std::istream& is, SequenceSet& sset ) const
{
    auto it = utils::CountingIstream( is );
    (void) it;

    throw std::runtime_error( "Not yet fully implemented." );

    sset.clear();
    // Sequence seq;
    //
    // while( parse_fasta_sequence( it, seq ) ) {
    //     sset.push_back( seq );
    // }
}

/**
 * @brief Read all Sequences from a file in Phylip format into a SequenceSet.
 */
void PhylipReader::from_file( std::string const& fn, SequenceSet& sset ) const
{
    if( !file_exists( fn ) ) {
        throw std::runtime_error( "File '" + fn + "' not found." );
    }

    std::ifstream ifs( fn );
    if( ifs.fail() ) {
        throw std::runtime_error( "Cannot read from file '" + fn + "'." );
    }

    from_stream( ifs, sset );
}

/**
 * @brief Read all Sequences from a std::string in Phylip format into a SequenceSet.
 */
void PhylipReader::from_string( std::string const& fs, SequenceSet& sset ) const
{
    std::istringstream iss( fs );
    from_stream( iss, sset );
}

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Set the length of the label in front of the sequences.
 *
 * Phylip has the weird property that labels are written in front of sequences and do not need
 * to have a delimiter, but instead are simply the first `n` characters of the string. This value
 * determines after how many chars the label ends and the actual sequence begins.
 *
 * If set to 0 (default), a relaxed version of Phylip is used, where the sequence begin is
 * automatically detected. Labels can then be of arbitrary lengths, as long as they do not contain
 * white spaces. However, in this case, there has to be at least one space or tab character between
 * the label and the sequence. After this first whitespace, the rest of the line is then treated
 * as sequence data.
 *
 * If set to a value greater than 0, exaclty this many characters are read as label. Thus, they
 * can also contain spaces. Spaces at the beginning or end of a label are stripped. The length
 * that is dictated by the Phylip standard is 10, but any other length can also be used.
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
 * when reading them. If set to an empty string, this check is deactivated. This is also the
 * default, meaning that no checking is done.
 *
 * In case that to_upper() is set to `true`: The validation is done after making the char upper
 * case, so that only capital letters have to be provided for validation.
 * In case that to_upper() is set to `false`: All chars that are to be considered valid have to be
 * provided for validation.
 *
 * See `nucleic_acid_codes...()` and `amino_acid_codes...()` functions for presettings of chars
 * that can be used for validation here.
 */
PhylipReader& PhylipReader::validate_chars( std::string const& chars )
{
    // If we do not want to validate, simply set all chars in the lookup to true. This saves us
    // from making that discintion in the actual parsing process. There, we can then always just
    // check the lookup table and don't have to check a flag or so.
    if( chars.size() == 0 ) {
        lookup_.set_all( true );
    } else {
        lookup_.set_all( false );
        lookup_.set_selection( chars );
    }

    return *this;
}

/**
 * @brief Return the currently set chars used for validating Sequence sites.
 *
 * If none are set, an empty string is returned. See is_validating() for checking whether chars are
 * set for validating - this is equal to checking whether this function returns an empty string.
 */
std::string PhylipReader::validate_chars() const
{
    // We need to distinguish the validating status here, because in case that no validating chars
    // are set, the table is all true - which would return a string of _all_ instead of no chars.
    if( is_validating() ) {
        return lookup_.get_selection();
    } else {
        return "";
    }
}

/**
 * @brief Return whether currently chars are set for validating the Sequence sites.
 *
 * This functions returns `true` iff there are chars set for validating Sequence sites.
 * Use validate_chars() for getting those chars.
 */
bool PhylipReader::is_validating() const
{
    // We could use a flag instead of this, but this function is not critical for speed, so this
    // should work as well.
    return lookup_.all_set();
}

/**
 * @brief Return the internal CharLookup that is used for validating the Sequence sites.
 *
 * This function is provided in case direct access to the lookup is needed. Usually, the
 * validate_chars() function should suffice. See there for details.
 */
utils::CharLookup& PhylipReader::valid_char_lookup()
{
    return lookup_;
}

} // namespace sequence
} // namespace genesis
