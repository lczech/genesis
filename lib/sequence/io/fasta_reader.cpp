/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/io/fasta_reader.hpp"

#include "sequence/io/fasta_parser.hpp"
#include "sequence/sequence_set.hpp"
#include "sequence/sequence.hpp"
#include "utils/core/fs.hpp"
#include "utils/io/counting_istream.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Reading
// =================================================================================================

/**
 * @brief
 */
void FastaReader::from_stream ( std::istream& is, SequenceSet& sset )
{
    auto it = utils::CountingIstream( is );
    Sequence seq;

    while( parse_fasta_sequence( it, seq ) ) {
        sset.push_back( seq );
    }
}

/**
 * @brief
 */
void FastaReader::from_file ( std::string const& fn, SequenceSet& sset )
{
    if( !file_exists( fn ) ) {
        throw std::runtime_error( "File '" + fn + "' not found." );
    }

    std::ifstream ifs( fn );
    from_stream( ifs, sset );
}

/**
 * @brief
 */
void FastaReader::from_string ( std::string const& fs, SequenceSet& sset )
{
    std::istringstream iss( fs );
    from_stream( iss, sset );
}

} // namespace sequence
} // namespace genesis
