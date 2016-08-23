#ifndef GENESIS_SEQUENCE_FUNCTIONS_CONSENSUS_H_
#define GENESIS_SEQUENCE_FUNCTIONS_CONSENSUS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Sequence;
class SequenceCounts;
class SequenceSet;

// =================================================================================================
//     Consensus
// =================================================================================================

// ------------------------------------------------------
//     Majorities
// ------------------------------------------------------

std::string consensus_sequence_with_majorities(
    SequenceCounts const& counts,
    bool                  allow_gaps = true,
    char                  gap_char   = '-'
);

std::string consensus_sequence_with_majorities(
    SequenceSet const&    sequences,
    std::string const&    characters,
    bool                  allow_gaps = true,
    char                  gap_char   = '-'
);

std::string consensus_sequence_with_majorities(
    SequenceSet const&    sequences,
    bool                  allow_gaps = true
);

// ------------------------------------------------------
//     Ambiguities
// ------------------------------------------------------

std::string consensus_sequence_with_ambiguities(
    SequenceCounts const& counts,
    double                similarity_factor = 0.9,
    bool                  allow_gaps = true
);

std::string consensus_sequence_with_ambiguities(
    SequenceSet const&    sequences,
    double                similarity_factor = 0.9,
    bool                  allow_gaps = true
);

// ------------------------------------------------------
//     Threshold
// ------------------------------------------------------

std::string consensus_sequence_with_threshold(
    SequenceCounts const& counts,
    double                frequency_threshold = 0.6,
    bool                  allow_gaps = true
);

std::string consensus_sequence_with_threshold(
    SequenceSet const&    sequences,
    double                frequency_threshold = 0.6,
    bool                  allow_gaps = true
);

} // namespace sequence
} // namespace genesis

#endif // include guard
