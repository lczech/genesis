/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/io/fasta_parser.hpp"

#include "sequence/sequence.hpp"
#include "utils/io/counting_istream.hpp"

#include <assert.h>
#include <cctype>
#include <stdexcept>
#include <string>

namespace genesis {
namespace sequence {

/**
 * @brief Parse a fasta sequence.
 *
 * This function takes an input stream and interprets as as a fasta sequence. It extracts the data
 * and writes it into the given Sequence object. The expected format:
 *
 *   * Has to start with a `>` character, followed by a label and possibly metadata and a '\n'.
 *   * An arbitrary number of comment lines, starting with ';', can follow, but are skipped.
 *   * After that, a sequence has to follow, over one or more lines and ending in a `\n` character.
 *
 * If the input is not in the correct format, exceptions are thrown indicating the malicious
 * position in the input stream. The function then stops after parsing one such sequence.
 * It returns true if a sequence was extracted and false if the stream is empty.
 *
 * See parse_fasta_sequence_fast() for a faster (~ double the speed), but not error checked
 * version of this function.
 */
bool parse_fasta_sequence( utils::CountingIstream& input_stream, Sequence& sequence )
{
    // Readability.
    auto& it = input_stream;

    // Check for data.
    if( it.eof() ) {
        return false;
    }

    // Check beginning of sequence.
    if( it.current() != '>' ) {
        throw std::runtime_error(
            "Malformed fasta file: Expecting '>' at beginning of sequence at " + it.at() + "."
        );
    }
    assert( *it == '>' );
    ++it;

    // Parse label.
    sequence.label().clear();
    while( it && isgraph(*it) ) {
        sequence.label() += *it;
        ++it;
    }

    // Check for unexpected end of file.
    if( it.eof() || (*it != '\n' && *it != ' ')) {
        throw std::runtime_error(
            "Malformed fasta file: Expecting a sequence after the label line at "
            + it.at() + "."
        );
    }
    assert( *it == '\n' || *it == ' ' );

    // Parse metadata.
    sequence.metadata().clear();
    if( *it == ' ' ) {
        ++it;
        while( it && isprint(*it) ) {
            sequence.metadata() += *it;
            ++it;
        }
    }

    // Check for unexpected end of file.
    if( it.eof() || *it != '\n' ) {
        throw std::runtime_error(
            "Malformed fasta file: Expecting a sequence after the label line at "
            + it.at() + "."
        );
    }
    assert( *it == '\n' );

    // Skip comments.
    while( it && *it == ';' ) {
        while( it && isprint(*it) ) {
            ++it;
        }
    }

    // Check for unexpected end of file.
    if( it.eof() || *it != '\n' ) {
        throw std::runtime_error(
            "Malformed fasta file: Expecting a sequence after the label line at "
            + it.at() + "."
        );
    }
    assert( *it == '\n' );
    ++it;

    // Parse sequence. At every beginning of the outer loop, we are at a line start.
    sequence.sites().clear();
    while( it ) {
        assert( it.column() == 1 );

        if( *it == '>' ) {
            break;
        }
        while( it && isgraph(*it) ) {
            sequence.sites() += *it;
            ++it;
        }
        if( it && *it != '\n' ) {
            throw std::runtime_error(
                "Malformed fasta file: Illegal sequence symbols at " + it.at() + "."
            );
        }
        assert( *it == '\n' );
        ++it;
    }

    return true;
}

/**
 * @brief Parse a fasta sequence without checking for errors.
 *
 * This is a very fast implementation that neglects input error checking. Thus, the fasta sequence
 * has to be well-formed in order for this function to work properly. See parse_fasta_sequence()
 * for a description of the expected format. No fasta comments are however allowed in this
 * function.
 *
 * If those conditions are not met, instead of warnings, undefined behaviour may result. Most
 * probably, it will just write rubbish into the sequence, but potentially, endless loops may
 * result. So be warned and check your data first. If they are good, enjoy the speed!
 */
bool parse_fasta_sequence_fast( utils::CountingIstream& input_stream, Sequence& sequence )
{
    // Readability.
    auto& it = input_stream;

    // Check for data.
    if( it.eof() ) {
        return false;
    }

    // Check beginning of sequence.
    assert( *it == '>' );
    it.advance_non_counting();
    assert( it );

    // Parse label.
    sequence.label().clear();
    while( *it != '\n' && *it != ' ' ) {
        sequence.label() += *it;
        it.advance_non_counting();
        assert( it );
    }
    assert( *it == '\n' || *it == ' ' );

    // Parse metadata.
    sequence.metadata().clear();
    if( *it == ' ' ) {
        it.advance_non_counting();
        assert( it );
        while( *it != '\n' ) {
            sequence.metadata() += *it;
            it.advance_non_counting();
            assert( it );
        }
    }
    assert( *it == '\n' );
    it.advance_non_counting();
    assert( it );

    // Parse sequence. At every beginning of the outer loop, we are at a line start.
    sequence.sites().clear();
    while( it ) {
        if( *it == '>' ) {
            break;
        }
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

} // namespace sequence
} // namespace genesis
