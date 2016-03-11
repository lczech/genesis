/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/io/fasta_reader.hpp"

#include "sequence/sequence_set.hpp"
#include "sequence/sequence.hpp"
#include "utils/core/fs.hpp"
#include "utils/io/counting_istream.hpp"
#include "utils/io/lexer/scanner.hpp"

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
 * See to_upper() and validate_chars() to change this behaviour.
 */
FastaReader::FastaReader()
{
    lookup_.set_all( true );
}

// =================================================================================================
//     Parsing
// =================================================================================================

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
 * See parse_fasta_sequence_fast() for a faster (~ double the speed), but non-error-checking
 * version of this function.
 */
bool FastaReader::parse_fasta_sequence(
    utils::CountingIstream& input_stream,
    Sequence&               sequence
) const {
    // Readability.
    auto& it = input_stream;

    // Check for data.
    if( it.eos() ) {
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
    if( it.eos() || (*it != '\n' && *it != ' ')) {
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
    if( it.eos() || *it != '\n' ) {
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
    if( it.eos() || *it != '\n' ) {
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
bool FastaReader::parse_fasta_sequence_fast(
    utils::CountingIstream& input_stream,
    Sequence&               sequence
) const {
    // Readability.
    auto& it = input_stream;

    // Check for data.
    if( it.eos() ) {
        return false;
    }

    // Check beginning of sequence.
    assert( *it == '>' );
    it.advance_non_counting();
    assert( it );

    // Parse label.
    std::string label;
    while( *it != '\n' && *it != ' ' ) {
        label += *it;
        it.advance_non_counting();
        assert( it );
    }
    sequence.label( label );
    assert( *it == '\n' || *it == ' ' );

    // Parse metadata.
    std::string metadata;
    if( *it == ' ' ) {
        it.advance_non_counting();
        assert( it );
        while( *it != '\n' ) {
            metadata += *it;
            it.advance_non_counting();
            assert( it );
        }
    }
    sequence.metadata( metadata );
    assert( *it == '\n' );
    it.advance_non_counting();
    assert( it );

    // Skip comments.
    while( *it == ';' ) {
        while( *it != '\n' ) {
            it.advance_non_counting();
        }
        assert( *it == '\n' );
        it.advance_non_counting();
        assert( it );
    }

    // Parse sequence. At every beginning of the outer loop, we are at a line start.
    sequence.sites().clear();
    while( it && *it != '>' ) {
        while( *it != '\n' ) {
            sequence.sites() += *it;
            it.advance_non_counting();
            assert( it );
        }
        assert( *it == '\n' );
        it.advance_non_counting();
    }

    return true;
}

// =================================================================================================
//     Reading
// =================================================================================================

/**
 * @brief Read all Sequences from a std::istream in Fasta format into a SequenceSet.
 */
void FastaReader::from_stream ( std::istream& is, SequenceSet& sset ) const
{
    auto it = utils::CountingIstream( is );
    Sequence seq;

    while( parse_fasta_sequence( it, seq ) ) {
        sset.push_back( seq );
    }
}

/**
 * @brief Read all Sequences from a file in Fasta format into a SequenceSet.
 */
void FastaReader::from_file ( std::string const& fn, SequenceSet& sset ) const
{
    if( ! utils::file_exists( fn ) ) {
        throw std::runtime_error( "File '" + fn + "' not found." );
    }

    std::ifstream ifs( fn );
    if( ifs.fail() ) {
        throw std::runtime_error( "Cannot read from file '" + fn + "'." );
    }

    from_stream( ifs, sset );
}

/**
 * @brief Read all Sequences from a std::string in Fasta format into a SequenceSet.
 */
void FastaReader::from_string ( std::string const& fs, SequenceSet& sset ) const
{
    std::istringstream iss( fs );
    from_stream( iss, sset );
}

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
FastaReader& FastaReader::validate_chars( std::string const& chars )
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
std::string FastaReader::validate_chars() const
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
bool FastaReader::is_validating() const
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
utils::CharLookup& FastaReader::valid_char_lookup()
{
    return lookup_;
}

} // namespace sequence
} // namespace genesis
