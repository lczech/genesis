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
    void clear();
    std::string Dump();

    std::vector<Sequence> sequences;
};

} // namespace genesis

#endif // include guard
