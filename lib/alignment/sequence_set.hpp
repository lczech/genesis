#ifndef GENESIS_ALIGNMENT_SEQUENCE_SET_H_
#define GENESIS_ALIGNMENT_SEQUENCE_SET_H_

/**
 * @brief
 *
 * @file
 * @ingroup alignment
 */

#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Sequence;

// =================================================================================================
//     SequenceSet
// =================================================================================================

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

    Sequence* find_sequence (std::string label) const;

    // -----------------------------------------------------
    //     Modifiers
    // -----------------------------------------------------

    void remove_list (std::vector<std::string> labels, bool invert = false);

    // -----------------------------------------------------
    //     Sequence Modifiers
    // -----------------------------------------------------

    void remove_gaps();
    void replace(char search, char replace);

    // -----------------------------------------------------
    //     Dump and Debug
    // -----------------------------------------------------

    std::string dump() const;

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    std::vector<Sequence*> sequences;
};

} // namespace genesis

#endif // include guard
