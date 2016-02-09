#ifndef GENESIS_SEQUENCE_FUNCTIONS_H_
#define GENESIS_SEQUENCE_FUNCTIONS_H_

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/sequence_set.hpp"

namespace genesis {
namespace sequence {

// ================================================================================================
//     Accessors
// ================================================================================================

Sequence const* find_sequence( SequenceSet const& set, std::string const& label );

// ================================================================================================
//     Modifiers
// ================================================================================================

/*

void remove_list(SequenceSet& set, std::vector<std::string> const& labels, bool invert);

// ================================================================================================
//     Sequence Modifiers
// ================================================================================================

void remove_gaps();
void replace(char search, char replace);

// ================================================================================================
//     Dump
// ================================================================================================

std::string dump() const;

*/

} // namespace sequence
} // namespace genesis

#endif // include guard
