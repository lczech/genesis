/**
 * @brief Implementation of Sequence class.
 *
 * @file
 * @ingroup alignment
 */

#include "alignment/sequence.hpp"

#include <algorithm>

#include "utils/logging.hpp"
#include "utils/utils.hpp"

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

/**
 * @brief Replaces all occurences of `search` by `replace`.
 */
void Sequence::Replace(char search, char replace)
{
    sites_ = StringReplaceAll (sites_, std::string(1, search), std::string(1, replace));
}

// =============================================================================
//     Dump and Debug
// =============================================================================

/**
 * @brief Prints the label and the whole sequence (possibly very long!).
 */
std::string Sequence::Dump() const
{
    return Label() + ": " + Sites();
}

} // namespace genesis
