#ifndef GENESIS_SEQUENCE_IO_PHYLIP_PROCESSOR_H_
#define GENESIS_SEQUENCE_IO_PHYLIP_PROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include <assert.h>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class SequenceSet;

// =================================================================================================
//     Phylip Processor
// =================================================================================================

/**
 * @brief Processes a Phylip file.
 */
class PhylipProcessor
{
public:

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    bool from_file   (const std::string  fn, SequenceSet& sset);
    bool from_string (const std::string& fs, SequenceSet& sset);

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    /**
     * @brief Determines the length of the label in front of the sequences.
     *
     * Phylip has the weird property that labels are written in front of sequences and do not need
     * to have a delimiter, but instead are simply the first n characters of the string. This value
     * determines after how many chars the label ends and the actual sequence begins.
     *
     * If set to 0 (default), a relaxed version of Phylip is used, where the sequence begin is
     * automatically detected. However, in this case, there has to be at least one whitespace
     * character (except new lines like CR or LF) between the label and the sequence.
     */
    size_t label_length = 0;

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    bool        to_file   (const SequenceSet& sset, const std::string  fn);
    void        to_string (const SequenceSet& sset,       std::string& fs);
    std::string to_string (const SequenceSet& sset);
};

} // namespace sequence
} // namespace genesis

#endif // include guard
