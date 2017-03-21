#ifndef GENESIS_SEQUENCE_FUNCTIONS_CONSENSUS_H_
#define GENESIS_SEQUENCE_FUNCTIONS_CONSENSUS_H_

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

/**
 * @brief Calculate the majority rule consensus sequence by using the most frequent character at
 * each site.
 *
 * The function creates a consensus sequence by using the character at each position that has the
 * highest count (or frequency). It does not assume any character codes. Thus, it works for all
 * kinds of sequence codes, e.g., nucleic acid or amino acid codes.
 *
 * The optional parameter `allow_gaps` (default is `true`) determines whether gaps in the consensus
 * sequence are allowed. By default, if a site consists mostly of gaps, the consensus sequence also
 * contains a gap at that site. If however this option is set to `false`, the consensus sequence
 * will contain the most frequent non-gap character, even if there are more gaps at this site than
 * the character itself. In other words, if the parameter is set to `false`, gaps are treated as
 * missing characters instead of another type of character for computing the consensus.
 * The only exception are gaps-only sites; in this case, the resulting sites contain a gap
 * characters even if the parameter is set to `false`.
 *
 * The optional parameter `gap_char` (default value '-') is used for sites where no counts are
 * available (i.e., are all zero), or, if `allow_gaps` is set to `true`, for sites that contain
 * mostly gaps.
 *
 * Furthermore, if two or more characters have the same frequency, the first one is used. That is,
 * the one that appears first in SequenceCounts::characters().
 *
 * For an alternative version of this function that takes those ambiguities into account, see
 * consensus_sequence_with_ambiguities(). Also, for a version of this function that takes a
 * threshold for the character frequencies into account, see consensus_sequence_with_threshold().
 * However, both of them currently only work for nucleic acid codes (`ACGT`).
 */
std::string consensus_sequence_with_majorities(
    SequenceCounts const& counts,
    bool                  allow_gaps = true,
    char                  gap_char   = '-'
);

/**
 * @brief Calculate the majority rule consensus sequence by using the most frequent character at
 * each site.
 *
 * See @link consensus_sequence_with_majorities( SequenceCounts const&, bool, char )
 * consensus_sequence_with_majorities(...)@endlink for details.
 *
 * This function is merely a wrapper that instead of a SequenceCount objects, takes a SequenceSet
 * object and the set of characters to be used for counting character frequencies in the Sequence%s.
 * That means, only the provided characters are counted and used for the consensus sequence.
 * This is useful in order to get rid of errors and noise in the Sequence%s. For example, if you
 * want to build a consensus sequence for a set of sequences with amino acid codes, use
 * amino_acid_codes_plain() for set `characters` parameter.
 */
std::string consensus_sequence_with_majorities(
    SequenceSet const&    sequences,
    std::string const&    characters,
    bool                  allow_gaps = true,
    char                  gap_char   = '-'
);

/**
 * @brief Calculate the majority rule consensus sequence by using the most frequent character at
 * each site for nucleic acid codes `ACGT`.
 *
 * See @link consensus_sequence_with_majorities( SequenceCounts const&, bool, char )
 * consensus_sequence_with_majorities(...)@endlink for details.
 *
 * This function is merely a wrapper that instead of a SequenceCount objects, takes a SequenceSet
 * object consisting of Sequence%s with nucleic acid codes `ACGT` and uses '-' for gaps.
 */
std::string consensus_sequence_with_majorities(
    SequenceSet const&    sequences,
    bool                  allow_gaps = true
);

// ------------------------------------------------------
//     Ambiguities
// ------------------------------------------------------

/**
 * @brief Calculate a consensus sequence by using the most frequent characters at each site,
 * for nucleic acid codes `ACGT` and their ambiguities.
 *
 * The function calculates a consensus sequence for nucleic acid codes (`ACGT`), using their
 * ambiguity codes (e.g., `W` for "weak" == `AT`) if the counts (i.e., character frequencies) are
 * similar at a site. It uses `similarity_factor` to decide which counts are close enough to each
 * other in order to be considered ambiguous.
 *
 * For example, with `similarity_factor == 1.0`, only exact matches are used, that is, if two
 * counts are exactly the same. Let <code>count('A') == 42</code> and <code>count('T') == 42</code>,
 * and both other counts be `0`, this results in the code `W` at that site. If however
 * <code>count('T') == 41</code>, only `A` is used for the site.
 * Thus, with `similarity_factor == 1.0`, this function behaves very similar to
 * consensus_sequence_with_majorities(), except in cases were two counts are exaclty the same.
 *
 * On the other hand, with `similarity_factor == 0.0`, all codes that are present at a site are
 * considered to be ambiguous. That is, if a site contains `counts > 0` for `A`, `G` and `T`, the
 * resulting site gets the code `D` ("not C").
 *
 * For intermediate values, e.g., the default `0.9`, the value is used as a threshold to decide
 * the ambiguities. For example, let <code>count('A') == 42</code> and <code>count('T') == 38</code>,
 * and both other counts be `0`. Then, the allowed deviation from the maximum `42` is
 * `0.9 * 42 = 37.8`. Thus, as the count for `T` is above this value, those two codes are
 * considered ambiguous, resulting in a `W` at that site.
 *
 * The optional parameter `allow_gaps` (default is `true`) behaves similar to its counterpart in
 * consensus_sequence_with_majorities(). If set to `true`, the count of the gap character is also
 * considered. If then the count of no character is within the similarity range of the gap count,
 * the result contains a gap at that site. If however there are codes within the range (i.e., above
 * `similarity_factor * max_count`), those are used instead, even if gaps are more frequent.
 *
 * If `allow_gaps` is set to `false` instead, gaps are not considered. That means, the ambiguities
 * are calculated as if there were no gaps. So even if a site contains mostly gaps, but only a few
 * other characters, those will be used. Solely all-gap sites result in a gap at that site.
 *
 * Remark: As this function expects nucleic acid codes, the gap character is fixed to '-' here.
 * The ambiguity codes are converted using nucleic_acid_ambiguity_code(). See there for more
 * information on the used codes.
 *
 * If the provided SequenceCounts object does not use nucleic acid codes, or if the
 * `similarity_factor` is not within the range `[ 0.0, 1.0 ]`, an exception is thrown.
 */
std::string consensus_sequence_with_ambiguities(
    SequenceCounts const& counts,
    double                similarity_factor = 0.9,
    bool                  allow_gaps        = true
);

/**
 * @brief Calculate a consensus sequence by using the most frequent characters at each site,
 * for nucleic acid codes `ACGT` and their ambiguities.
 *
 * See @link consensus_sequence_with_ambiguities( SequenceCounts const&, double, bool )
 * consensus_sequence_with_ambiguities(...)@endlink for details.
 * This is merely a wrapper function that takes a SequenceSet instead of a SequenceCounts object.
 */
std::string consensus_sequence_with_ambiguities(
    SequenceSet const&    sequences,
    double                similarity_factor = 0.9,
    bool                  allow_gaps        = true
);

// ------------------------------------------------------
//     Threshold
// ------------------------------------------------------

/**
 * @brief Calculate a consensus sequence where the character frequency needs to be above a given
 * threshold, for nucleic acid codes `ACGT`.
 *
 * The function calculates a consensus sequence for nucleic acid codes (`ACGT`). It uses the
 * frequency of characters at each site to determine the consensus. The frequency is relative to the
 * total number of counts at that site, thus, it is a value in the range `[ 0.0, 1.0 ]`.
 *
 * If the frequency of a character at a site is above the given `frequency_threshold`, it is used
 * for the consensus. If not, the resulting character depends on `use_ambiguities`.
 * If `use_ambiguities` is set to `true` (default), the sorted frequencies of the characters are
 * added until the threshold is reached, and the ambiguity code for those characters is used.
 * For example, let `frequency_threshold == 0.9`, <code>count('A') == 42</code> and
 * <code>count('T') == 42</code>, and both other counts be 0. Then, neither `A` nor `T` have counts
 * above the threshold, but combined they do, so the result is code `W == AT` at that site.
 * If however `use_ambiguities` is `false`, the mask character `X` is used for sites that are below
 * the threshold.
 *
 * Furthermore, if `allow_gaps` is set to `true` (default), gaps are counted when determining the
 * threshold and checking whether the frequency is above it. That is, gaps are then treated as just
 * another character at the site. For sites that mostly contain gaps and the gap frequency reaches
 * the threshold, a gap is used as consensus.
 * If `allow_gaps` is `false`, however, gaps are not counted for determining the frequency of the
 * other characters. This is similar to the counterpart in consensus_sequence_with_majorities().
 * Solely sites that are gaps-only result in a gap char for the consensus then.
 *
 * For `frequency_threshold < 0.5`, it may happen that more than one character has a frequency
 * above the threshold. In such cases, the most frequent character is used (or, if they have exaclty
 * the same counts, they are used in the order `ACGT`).
 * This is in line with the behaviour of consensus_sequence_with_majorities(). Usually, however,
 * the threshold is above 0.5 anyway, as this gives more meaningful results. If you want to use
 * ambiguity characters for low frequency characters, you can use
 * consensus_sequence_with_ambiguities() instead.
 *
 * An extreme case is a `frequency_threshold` of 1.0. In this case, for sites which only have one
 * character, this one is directly used in the consensus. Sites with multiple different characters
 * result in the ambiguity code of all occuring characters at that site. Thus, the function then
 * behaves similar to consensus_sequence_with_ambiguities() with a `similarity_factor` of 0.0.
 *
 * The other extrem case is a `frequency_threshold` of 0.0. In this case, the function simply uses
 * the most frequent character per site, as it always fulfills the threshold. As said above, if then
 * more than one character has exactly the same frequency, they are used in the order `ACGT`, thus
 * the function then behaves similar to consensus_sequence_with_majorities().
 *
 * Remark: As this function expects nucleic acid codes, the gap character is fixed to '-' here,
 * and the mask character to 'X'.
 * The ambiguity codes are converted using nucleic_acid_ambiguity_code(). See there for more
 * information on the used codes.
 *
 * If the provided SequenceCounts object does not use nucleic acid codes, or if the
 * `frequency_threshold` is not within the range `[ 0.0, 1.0 ]`, an exception is thrown.
 */
std::string consensus_sequence_with_threshold(
    SequenceCounts const& counts,
    double                frequency_threshold = 0.6,
    bool                  allow_gaps          = true,
    bool                  use_ambiguities     = true
);

/**
 * @brief Calculate a consensus sequence where the character frequency needs to be above a given
 * threshold, for nucleic acid codes `ACGT`.
 *
 * See @link consensus_sequence_with_threshold( SequenceCounts const&, double, bool, bool )
 * consensus_sequence_with_ambiguities(...)@endlink for details.
 * This is merely a wrapper function that takes a SequenceSet instead of a SequenceCounts object.
 */
std::string consensus_sequence_with_threshold(
    SequenceSet const&    sequences,
    double                frequency_threshold = 0.6,
    bool                  allow_gaps          = true,
    bool                  use_ambiguities     = true
);

} // namespace sequence
} // namespace genesis

#endif // include guard
