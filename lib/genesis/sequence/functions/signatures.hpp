#ifndef GENESIS_SEQUENCE_FUNCTIONS_SIGNATURES_H_
#define GENESIS_SEQUENCE_FUNCTIONS_SIGNATURES_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
#include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class SignatureSpecifications;
class Sequence;
class SequenceSet;

// =================================================================================================
//     Signatures
// =================================================================================================

/**
 * @brief Count the occurences of k-mers in the @p sequence according to the @p settings.
 *
 * The function returns a vector that contains the count for each k-mer that can be build from
 * the characters in the alphabet, in the order given by SignatureSpecifications::kmer_list().
 */
std::vector<size_t> signature_counts(
    Sequence const& sequence,
    SignatureSpecifications const& settings
);

std::vector<double> signature_frequencies(
    Sequence const& seq,
    SignatureSpecifications const& settings
);

} // namespace sequence
} // namespace genesis

#endif // include guard
