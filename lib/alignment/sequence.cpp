/**
 * @brief Implementation of Sequence class.
 *
 * @file
 * @ingroup alignment
 */

#include "alignment/sequence.hpp"

#include <algorithm>

#include "utils/core/logging.hpp"
#include "utils/string.hpp"

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
void Sequence::remove_gaps()
{
    sites_.erase(std::remove(sites_.begin(), sites_.end(), gap_char), sites_.end());
}

/**
 * @brief Replaces all occurences of `search` by `replace`.
 */
void Sequence::replace(char search, char replace)
{
    sites_ = string_replace_all (sites_, std::string(1, search), std::string(1, replace));
}

// =============================================================================
//     Dump and Debug
// =============================================================================

/**
 * @brief Prints the label and the whole sequence (possibly very long!).
 */
std::string Sequence::dump() const
{
    return label() + ": " + sites();
}

} // namespace genesis
