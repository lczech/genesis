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

// =================================================================================================
//     Reading
// =================================================================================================

/**
 * @brief Read all Sequences from a std::istream in Phylip format into a SequenceSet.
 */
void PhylipReader::from_stream( std::istream& is, SequenceSet& sset ) const
{
    auto it = utils::CountingIstream( is );

    // Read number of sequences.
    lexer::skip_while( it, isblank );
    std::string num_seq_str;
    lexer::copy_while( it, num_seq_str, isdigit );
    if( num_seq_str.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip file: Expecting sequence number at " + it.at() + "."
        );
    }
    long num_seq = std::stoi( num_seq_str );

    // Read length of sequences.
    lexer::skip_while( it, isblank );
    std::string len_seq_str;
    lexer::copy_while( it, len_seq_str, isdigit );
    if( len_seq_str.length() == 0 ) {
        throw std::runtime_error(
            "Malformed Phylip file: Expecting sequence length at " + it.at() + "."
        );
    }
    long len_seq = std::stoi( len_seq_str );

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
            "Malformed Phylip file: Expecting start of sequences at " + it.at() + "."
        );
    }
    ++it;

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
} // namespace sequence
} // namespace genesis
