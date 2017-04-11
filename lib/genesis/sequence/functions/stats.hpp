#ifndef GENESIS_SEQUENCE_FUNCTIONS_STATS_H_
#define GENESIS_SEQUENCE_FUNCTIONS_STATS_H_

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

#include "genesis/sequence/functions/codes.hpp"
#include "genesis/utils/math/bitvector.hpp"

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Sequence;
class SequenceCounts;
class SequenceSet;

// =================================================================================================
//     Characteristics
// =================================================================================================

/**
 * @brief Get a histogram of the occurrences of particular sites, given a Sequence.
 *
 * This gives the raw counts of how often each site (character) appears in the Sequence.
 * See base_frequencies() for the relative version of this function.
 */
std::map<char, size_t> site_histogram( Sequence    const& seq );

/**
 * @brief Get a histogram of the occurrences of particular sites, given a SequenceSet.
 *
 * This gives the raw counts of how often each site (character) appears in the whole set.
 * See base_frequencies() for the relative version of this function.
 */
std::map<char, size_t> site_histogram( SequenceSet const& set );

/**
 * @brief Get the base frequencies of the sites in a Sequence given the base chars.
 *
 * This returns the relative proportions of the given `plain_chars` to each other. Typically,
 * the given chars come from either nucleic_acid_codes_plain() or amino_acid_codes_plain(),
 * depending on the dataset.
 *
 * It is necessary to select those chars on a per-dataset basis, as it is up to the user to define
 * the meaning of those chars.
 */
std::map<char, double> base_frequencies( Sequence    const& seq, std::string const& plain_chars );

/**
 * @brief Get the base frequencies of the sites in a SequenceSet given the base chars.
 *
 * See the Sequence implementation of this function for details.
 */
std::map<char, double> base_frequencies( SequenceSet const& set, std::string const& plain_chars );

/**
 * @brief Count the number of occurrences of the given `chars` within the sites of the SequenceSet.
 *
 * This function can be used to count e.g. gaps or ambiguous characters in sequences.
 * For presettings of usable chars, see the functions `nucleic_acid_codes_...` and
 * `amino_acid_codes_...`. The chars are treated case-insensitive.
 *
 * If `chars` contains invalid (non-standard ASCII) characters, an `std::invalid_argument`
 * exception is thrown.
 */
size_t count_chars( SequenceSet const& set, std::string const& chars );

/**
 * @brief Return the "gapyness" of the Sequence%s, i.e., the proportion of gap chars
 * and other completely undetermined chars to the total length of all sequences.
 *
 * This function returns a value in the interval 0.0 (no gaps and undetermined chars at all)
 * and 1.0 (all chars are undetermined).
 * See `nucleic_acid_codes_undetermined()` and `amino_acid_codes_undetermined()` for presettings
 * of gap character that can be used here depending on the data set type.
 * The chars are treated case-insensitive.
 * In the special case that there are no sequences or sites, 0.0 is returned.
 */
double gapyness( SequenceSet    const& set, std::string const& gap_chars );

size_t gap_site_count( SequenceCounts const& counts );

// =================================================================================================
//     K-mers
// =================================================================================================

/**
 * @brief Count the occurences of k-mers of size @p k, for nucleic acids "ACGT".
 *
 * The function is similar to kmer_counts( Sequence const&, size_t k, std::string const& alphabet ),
 * but slighly faster, and only accpets Sequence%s that solely consists of "ACGT" characters.
 * Otherwise it throws.
 */
std::vector<size_t> kmer_counts( Sequence const& seq, size_t k );

/**
 * @brief Count the occurences of k-mers of size @p k, for a given @p alphabet.
 *
 * The function returns a vector that contains the count for each k-mer that can be build from
 * the characters in the given @p alphabet. The alphabet is normalized prior to processing, using
 * normalize_codes(). Characters in the Sequence that are not in the alphabet are ignored.
 *
 * The resulting vector is indexed using the same order of k-mers as produced by kmer_list().
 */
std::vector<size_t> kmer_counts( Sequence const& seq, size_t k, std::string const& alphabet );

/**
 * @brief Return the list of all possible k-mers for a given @p k and @p alphabet.
 *
 * The order in this vector is the same as used in
 * @link kmer_counts( Sequence const&, size_t k, std::string const& alphabet ) kmer_counts()@endlink.
 */
std::vector<std::string> kmer_list( size_t k, std::string const& alphabet );

} // namespace sequence
} // namespace genesis

#endif // include guard
