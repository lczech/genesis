#ifndef GENESIS_SEQUENCE_IO_FASTA_PARSER_H_
#define GENESIS_SEQUENCE_IO_FASTA_PARSER_H_

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {

class CountingIstream;

}

namespace sequence {

class Sequence;

// =================================================================================================
//     Fasta Parser
// =================================================================================================

/*
 *  *  http://en.wikipedia.org/wiki/FASTA_format
 *  *  http://blast.ncbi.nlm.nih.gov/blastcgihelp.shtml
 *  *  http://zhanglab.ccmb.med.umich.edu/FASTA/
 */

bool parse_fasta_sequence(      utils::CountingIstream& input_stream, Sequence& sequence );
bool parse_fasta_sequence_fast( utils::CountingIstream& input_stream, Sequence& sequence );

} // namespace sequence
} // namespace genesis

#endif // include guard
