#ifndef GENESIS_ALIGNMENT_FASTA_PROCESSOR_H_
#define GENESIS_ALIGNMENT_FASTA_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup alignment
 */

#include <string>

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

class SequenceSet;

// =============================================================================
//     Fasta Processor
// =============================================================================

/**
 * @brief Processes a FASTA file into an alignment object.
 *
 * Specification:
 *
 *  *  http://en.wikipedia.org/wiki/FASTA_format
 *  *  http://blast.ncbi.nlm.nih.gov/blastcgihelp.shtml
 *  *  http://zhanglab.ccmb.med.umich.edu/FASTA/
 * %
 */
class FastaProcessor
{
public:

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    static bool from_file   (const std::string  fn, SequenceSet& aln);
    static bool from_string (const std::string& fs, SequenceSet& aln);

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    static size_t line_length;

    static bool        to_file   (const SequenceSet& sset, const std::string  fn);
    static void        to_string (const SequenceSet& sset,       std::string& fs);
    static std::string to_string (const SequenceSet& sset);
};

} // namespace genesis

#endif // include guard
