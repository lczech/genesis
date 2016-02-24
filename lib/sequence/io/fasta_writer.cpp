/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/io/fasta_writer.hpp"

#include "sequence/sequence_set.hpp"
#include "sequence/sequence.hpp"
#include "utils/core/fs.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Writing
// =================================================================================================

/**
 * @brief Write Sequences of a SequenceSet to a stream in Fasta format.
 */
void FastaWriter::to_stream( SequenceSet const& sset, std::ostream& os ) const
{
    for( Sequence const& s : sset ) {
        // Write label.
        os << ">" << s.label();

        // Write metadata if available.
        if( s.metadata().size() > 0 ) {
            os << " " << s.metadata();
        }
        os << "\n";

        // Write sequence. If needed, add new line at every line_length_ position.
        if (line_length_ > 0) {
            for (size_t i = 0; i < s.length(); i += line_length_) {
                // Write line_length_ many characters.
                // (If the string is shorter, as many characters as possible are used.)
                os << s.sites().substr(i, line_length_) << "\n";
            }
        } else {
            os << s.sites() << "\n";
        }
    }
}

/**
 * @brief Write Sequences of a SequenceSet to a file in Fasta format.
 */
void FastaWriter::to_file( SequenceSet const& sset, std::string const& fn ) const
{
    if( utils::file_exists( fn ) ) {
        throw std::runtime_error( "File '" + fn + "' already exists." );
    }

    std::ofstream ofs( fn );
    if( ofs.fail() ) {
        throw std::runtime_error( "Cannot write to file '" + fn + "'." );
    }

    to_stream( sset, ofs );
}

/**
 * @brief Return Sequences of a SequenceSet in form of a Fasta formatted string.
 *
 * Caveat: This might be a long string! If you simply want to examine a Sequence or SequenceSet,
 * have a look at the print() and print_color() functions.
 */
std::string FastaWriter::to_string ( SequenceSet const& sset ) const
{
    std::ostringstream oss;
    to_stream( sset, oss );
    return oss.str();
}

// =================================================================================================
//     Properties
// =================================================================================================

/**
 * @brief Set the line length, which determines after how many chars (Sequence sites) lines breaks
 * are inserted when writing the Fasta file.
 *
 * Default is `80`. If set to `0`, no breaks are inserted.
 * The functions returns the FastaWriter object to allow fluent interfaces.
 */
FastaWriter& FastaWriter::line_length( size_t value )
{
    line_length_ = value;
    return *this;
}

/**
 * @brief Get the current line length.
 *
 * See the setter line_length() for details.
 */
size_t FastaWriter::line_length() const
{
    return line_length_;
}

} // namespace sequence
} // namespace genesis
