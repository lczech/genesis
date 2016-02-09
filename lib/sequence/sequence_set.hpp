#ifndef GENESIS_SEQUENCE_SEQUENCE_SET_H_
#define GENESIS_SEQUENCE_SEQUENCE_SET_H_

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include <string>
#include <vector>

namespace genesis {
namespace sequence {

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

} // namespace sequence
} // namespace genesis

#endif // include guard
