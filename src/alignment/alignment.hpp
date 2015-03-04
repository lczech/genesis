#ifndef GNS_ALIGNMENT_ALIGNMENT_H_
#define GNS_ALIGNMENT_ALIGNMENT_H_

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

class Alignment
{
public:

    // -----------------------------------------------------
    //     Constructor and Typedefs
    // -----------------------------------------------------

    ~Alignment();
    void clear();

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    Sequence* FindSequence (std::string label) const;

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
