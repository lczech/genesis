#ifndef GENESIS_ALIGNMENT_SEQUENCESET_H_
#define GENESIS_ALIGNMENT_SEQUENCESET_H_

/**
 * @brief
 *
 * @file
 * @ingroup alignment
 */

#include <string>
#include <vector>

#include "alignment/sequence.hpp"

namespace genesis {

class SequenceSet
{
public:

    // -----------------------------------------------------
    //     Constructor and Typedefs
    // -----------------------------------------------------

    ~SequenceSet();
    void clear();

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    Sequence* FindSequence (std::string label) const;

    // -----------------------------------------------------
    //     Modifiers
    // -----------------------------------------------------

    void RemoveList (std::vector<std::string> labels, bool invert = false);

    // -----------------------------------------------------
    //     Sequence Modifiers
    // -----------------------------------------------------

    void RemoveGaps();
    void Replace(char search, char replace);

    // -----------------------------------------------------
    //     Dump and Debug
    // -----------------------------------------------------

    std::string Dump() const;

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    std::vector<Sequence*> sequences;
};

} // namespace genesis

#endif // include guard
