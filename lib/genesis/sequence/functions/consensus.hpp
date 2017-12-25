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
class SiteCounts;
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
 * the one that appears first in SiteCounts::characters().
 *
 * For an alternative version of this function that takes those ambiguities into account, see
 * consensus_sequence_with_ambiguities(). Also, for a version of this function that takes a
 * threshold for the character frequencies into account, see consensus_sequence_with_threshold().
 * However, both of them currently only work for nucleic acid codes (`ACGT`).
 */
std::string consensus_sequence_with_majorities(
    SiteCounts const&     counts,
    bool                  allow_gaps = true,
    char                  gap_char   = '-'
);

/**
 * @brief Calculate the majority rule consensus sequence by using the most frequent character at
 * each site.
 *
 * See @link consensus_sequence_with_majorities( SiteCounts const&, bool, char )
 * consensus_sequence_with_majorities(...)@endlink for details.
 *
 * This function is merely a wrapper that instead of a SiteCounts objects, takes a SequenceSet
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
 * See @link consensus_sequence_with_majorities( SiteCounts const&, bool, char )
 * consensus_sequence_with_majorities(...)@endlink for details.
 *
 * This function is merely a wrapper that instead of a SiteCounts objects, takes a SequenceSet
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
 * and both other counts (`C` and `G`) be `0`, this results in the code `W` at that site. If however
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
 * considered. As the ambiguity for a gap combined with any other character is still a gap,
 * sites where gap is the most frequent charater or is within the deviation get a gap as result.
 *
 * If `allow_gaps` is set to `false` instead, gaps are not considered. That means, the ambiguities
 * are calculated as if there were no gaps. So even if a site contains mostly gaps, but only a few
 * other characters, those will be used. Solely all-gap sites result in a gap at that site.
 *
 * Remark: As this function expects nucleic acid codes, the gap character is fixed to '-' here.
 * The ambiguity codes are converted using nucleic_acid_ambiguity_code(). See there for more
 * information on the used codes.
 *
 * If the provided SiteCounts object does not use nucleic acid codes, or if the
 * `similarity_factor` is not within the range `[ 0.0, 1.0 ]`, an exception is thrown.
 */
std::string consensus_sequence_with_ambiguities(
    SiteCounts const&     counts,
    double                similarity_factor = 0.9,
    bool                  allow_gaps        = true
);

/**
 * @brief Calculate a consensus sequence by using the most frequent characters at each site,
 * for nucleic acid codes `ACGT` and their ambiguities.
 *
 * See @link consensus_sequence_with_ambiguities( SiteCounts const&, double, bool )
 * consensus_sequence_with_ambiguities(...)@endlink for details.
 * This is merely a wrapper function that takes a SequenceSet instead of a SiteCounts object.
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
 * <code>count('T') == 42</code>, and both other counts (`C` and `G`) be 0.
 * Then, neither `A` nor `T` have counts above the threshold, but combined they do, so the result is
 * code `W == AT` at that site. If however `use_ambiguities` is `false`, the mask character `X` is
 * used for sites that are below the threshold.
 *
 * Furthermore, if `allow_gaps` is set to `true` (default), gaps are counted when determining the
 * threshold and checking whether the frequency is above it. That is, gaps are then treated as just
 * another character at the site. For sites where the gap frequency is needed to reach the
 * threshold, a gap is used as consensus.
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
 * If the provided SiteCounts object does not use nucleic acid codes, or if the
 * `frequency_threshold` is not within the range `[ 0.0, 1.0 ]`, an exception is thrown.
 */
std::string consensus_sequence_with_threshold(
    SiteCounts const&     counts,
    double                frequency_threshold = 0.6,
    bool                  allow_gaps          = true,
    bool                  use_ambiguities     = true
);

/**
 * @brief Calculate a consensus sequence where the character frequency needs to be above a given
 * threshold, for nucleic acid codes `ACGT`.
 *
 * See @link consensus_sequence_with_threshold( SiteCounts const&, double, bool, bool )
 * consensus_sequence_with_ambiguities(...)@endlink for details.
 * This is merely a wrapper function that takes a SequenceSet instead of a SiteCounts object.
 */
std::string consensus_sequence_with_threshold(
    SequenceSet const&    sequences,
    double                frequency_threshold = 0.6,
    bool                  allow_gaps          = true,
    bool                  use_ambiguities     = true
);

// ------------------------------------------------------
//     Cavener
// ------------------------------------------------------

/**
 * @brief Calculate a consensus sequence using the method by Cavener
 * for nucleic acid codes `ACGT` and their ambiguities.
 *
 * For every site, the nucleotides are sorted by frequency. Then, the following checks are performed:
 *
 *  1. A single nucleotide is used if its frequency is at least 50% and more than twice as high
 *     as the second most frequent nucleotide.
 *  2. If two nucleotides occur in at leat 75% of the sequences, and rule 1 does not apply,
 *     their ambiguity code is used (double-degenerate code).
 *  3. If none of the above applies, but one of the nucleotides has a count of zero,
 *     the (triple-degenerate) code of the other three nucleotides is used.
 *  4. In all other cases, the code 'N' is used.
 *
 * The method is meant for finding a consensus sequence in sets of sequences without gaps.
 * This implementation however also allows gaps, which are just treated as a normal character
 * if @p allow_gaps is `true` (default). In this case, if any of rules 1-3 applies to a gap
 * character, the result is simply reduced to a gap. That is, whenever a gap is used to form
 * a degenerate code according to the rules, the whole code is reduced to a gap.
 *
 * If however @p allow_gaps is `false`, gap characters are completely ignored from the counting.
 *
 * The method was described in:
 *
 * > D. R. Cavener, "Comparison of the consensus sequence flanking translational start sites in
 * > Drosophila and vertebrates", Nucleic Acids Res., vol. 15, no. 4, 1987.
 *
 * The method is also used by the Transfac Project:
 *
 * > V. Matys et al., "TRANSFAC: Transcriptional regulation, from patterns to profiles",
 * > Nucleic Acids Res., vol. 31, no. 1, pp. 374–378, 2003.
 */
std::string consensus_sequence_cavener(
    SiteCounts const&     counts,
    bool                  allow_gaps        = true
);

/**
 * @brief Calculate a consensus sequence using the method by Cavener
 * for nucleic acid codes `ACGT` and their ambiguities.
 *
 * See @link consensus_sequence_cavener( SiteCounts const&, bool )
 * consensus_sequence_cavener(...)@endlink for details.
 * This is merely a wrapper function that takes a SequenceSet instead of a SiteCounts object.
 */
std::string consensus_sequence_cavener(
    SequenceSet const&    sequences,
    bool                  allow_gaps        = true
);


} // namespace sequence
} // namespace genesis

#endif // include guard
