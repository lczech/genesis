/**
 * @brief Implementation of Sequence class.
 *
 * @file
 * @ingroup alignment
 */

#include "alignment/sequence.hpp"

#include <algorithm>

namespace genesis {

// =============================================================================
//     Constructor and Destructor
// =============================================================================

/**
 * @brief Destructor.
 */
Sequence::~Sequence()
{
    //
}

// =============================================================================
//     Mutators
// =============================================================================

/**
 * @brief Removes all occurences of `gap_char` from the sequence.
 */
void Sequence::RemoveGaps()
{
    sites_.erase(std::remove(sites_.begin(), sites_.end(), gap_char), sites_.end());
}

// =============================================================================
//     Dump and Debug
// =============================================================================

/**
 * @brief Prints the whole sequence (possibly very long!).
 */
std::string Sequence::Dump() const
{
    return Sites();
}

} // namespace genesis
