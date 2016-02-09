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
//     Characteristics
// ================================================================================================

size_t count_chars( SequenceSet const& set, std::string const& chars );

size_t total_length( SequenceSet const& set );

bool validate_chars( SequenceSet const& set, std::string const& chars );

bool is_alignment( SequenceSet const& set );

double gapyness( SequenceSet const& set, std::string const& undetermined_chars );

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
