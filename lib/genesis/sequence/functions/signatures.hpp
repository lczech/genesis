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

#include<string>
#include<vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class SignatureSpecifications;
class Sequence;
class SequenceSet;

// =================================================================================================
//     K-mers
// =================================================================================================

/**
 * @brief Count the occurences of k-mers in the @p sequence according to the lookup @p settings.
 *
 * The function returns a vector that contains the count for each k-mer that can be build from
 * the characters in the alphabet.
 *
 * The resulting vector is indexed using the same order of k-mers as produced by kmer_list().
 */
std::vector<size_t> kmer_counts( Sequence const& sequence, SignatureSpecifications const& settings );

/**
 * @brief Return the list of all possible k-mers for the `k` and `alphabet` of a SignatureSpecifications.
 *
 * The order in this vector is the same as used in kmer_counts().
 */
std::vector<std::string> kmer_list( SignatureSpecifications const& settings );

// =================================================================================================
//     Helpers
// =================================================================================================

/**
 * @brief Get the size needed to store reverse complement kmers for nucleic acid sequences.
 */
size_t kmer_reverse_complement_size( SignatureSpecifications const& settings );

/**
 * @brief Get a map from indices of kmer_list() and kmer_counts() vectors to a smaller list
 * of size kmer_reverse_complement_size() which combines reverse complementary kmers
 * for nucleic acid sequences.
 */
std::vector<size_t> kmer_reverse_complement_indices( SignatureSpecifications const& settings );

// =================================================================================================
//     Signatures
// =================================================================================================

std::vector<double> signature_frequencies(
    Sequence const& seq,
    SignatureSpecifications const& settings
);

} // namespace sequence
} // namespace genesis

#endif // include guard
