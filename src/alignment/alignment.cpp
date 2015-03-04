/**
 * @brief Implementation of Alignment class.
 *
 * @file
 * @ingroup alignment
 */

#include "alignment/alignment.hpp"

#include <sstream>

namespace genesis {

// =============================================================================
//     Alignment
// =============================================================================

/**
 * @brief
 */
void Alignment::clear()
{
    sequences.clear();
}

std::string Alignment::Dump()
{
    std::ostringstream out;
    for (Sequence s : sequences) {
        out << s.Label() << " [" << s.Length() << "]\n";
    }
    return out.str();
}

} // namespace genesis
