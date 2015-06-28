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

    bool from_file   (const std::string  fn, SequenceSet& aln);
    bool from_string (const std::string& fs, SequenceSet& aln);

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    bool        to_file   (const SequenceSet& sset, const std::string  fn);
    void        to_string (const SequenceSet& sset,       std::string& fs);
    std::string to_string (const SequenceSet& sset);

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    /**
     * @brief Determines after how many chars to do a line break when printing a FASTA file.
     *
     * Default is `80`. If set to `0`, no breaks are inserted.
     */
    size_t line_length = 80;
};

} // namespace genesis

#endif // include guard
