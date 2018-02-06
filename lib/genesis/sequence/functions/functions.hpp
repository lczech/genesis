#ifndef GENESIS_SEQUENCE_FUNCTIONS_FUNCTIONS_H_
#define GENESIS_SEQUENCE_FUNCTIONS_FUNCTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
#include "genesis/utils/tools/char_lookup.hpp"

#include <iosfwd>
#include <map>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Sequence;
class SequenceSet;

// =================================================================================================
//     Characteristics
// =================================================================================================

/**
 * @brief Find sites by character and mark them in a @link utils::Bitvector Bitvector@endlink.
 *
 * The function iterates the sites of a Sequence and checkes whether the char at a site is in the
 * provided set of @p chars (case insensitive).
 * If so, the resulting Bitvector is set to `true` at that position.
 */
utils::Bitvector find_sites(
    Sequence const& seq,
    std::string const& chars
);

/**
 * @brief Find sites by character and mark them in a @link utils::Bitvector Bitvector@endlink.
 *
 * The function iterates the sites of a Sequence and checkes whether the char at a site is set to
 * true in the provided lookup of @p chars. If so, the resulting Bitvector is set to `true` at
 * that position.
 */
utils::Bitvector find_sites(
    Sequence const& seq,
    utils::CharLookup<bool> const& chars
);

/**
 * @brief Return a @link utils::Bitvector Bitvector@endlink that is `true` where the Sequence has
 * a gap and `false` where not.
 *
 * The `gap_chars` are used case-insensitively to determine what is considerted to be a gap.
 * By default, nucleic_acid_codes_undetermined() are used, but any other set of characters
 * is allowed.
 */
utils::Bitvector gap_sites(
    Sequence const& seq,
    std::string const& gap_chars = nucleic_acid_codes_undetermined()
);

/**
 * @brief Return a @link utils::Bitvector Bitvector@endlink that is `true` where all Sequence%s in
 * the SequenceSet have a gap and `false` where not, that is, where at least on Sequence is not a
 * gap.
 *
 * The `gap_chars` are used case-insensitively to determine what is considerted to be a gap.
 * By default, nucleic_acid_codes_undetermined() are used, but any other set of characters
 * is allowed.
 */
utils::Bitvector gap_sites(
    SequenceSet const& set,
    std::string const& gap_chars = nucleic_acid_codes_undetermined()
);

/**
 * @brief Returns true iff all Sequence%s only consist of the given `chars`.
 *
 * For presettings of usable chars, see the functions `nucleic_acid_codes_...` and
 * `amino_acid_codes_...`. For example, to check whether the sequences are nucleic acids,
 * use `nucleic_acid_codes_all()`. The chars are treated case-insensitive.
 *
 * If `chars` contains invalid (non-standard ASCII) characters, an `std::invalid_argument`
 * exception is thrown.
 */
bool validate_chars( SequenceSet const& set, std::string const& chars );

/**
 * @brief Return the length of the longest Sequence in the SequenceSet.
 */
size_t longest_sequence_length( SequenceSet const& set );

/**
 * @brief Return the total length (sum) of all Sequence%s in the SequenceSet.
 */
size_t total_length( SequenceSet const& set );

/**
 * @brief Return true iff all Sequence%s in the SequenceSet have the same length.
 */
bool is_alignment( SequenceSet const& set );

// =================================================================================================
//     Modifiers
// =================================================================================================

/**
 * @brief Remove all sites from a Sequence where the given @link utils::Bitvector Bitvector@endlink
 * is `true`, and keep all others.
 *
 * The Bitvector needs to have the same size as the Sequence, otherwise an expection is thrown.
 *
 * This function is for example useful in combination with gap_sites().
 */
void remove_sites( Sequence&    seq, utils::Bitvector sites );

/**
 * @brief Remove all sites from all Sequence%s in a SequenceSet where the given
 * @link utils::Bitvector Bitvector@endlink is `true`, and keep all others.
 *
 * The Bitvector and all Sequences need to have the same size, otherwise an expection is thrown.
 * This check is done before any Sequence is changed. Thus, if the function throws for this reason,
 * the Sequences are left unchanged.
 *
 * This function is for example useful in combination with gap_sites().
 */
void remove_sites( SequenceSet& set, utils::Bitvector sites );

/**
 * @brief Remove all sites that only contain gap characters from the SequenceSet.
 */
void remove_gap_sites( SequenceSet& set, std::string const& gap_chars = nucleic_acid_codes_undetermined() );

/**
 * @brief Remove all of the characters in `search` from the sites of the Sequence.
 */
void remove_characters( Sequence&    seq, std::string const& search );

/**
 * @brief Remove all of the characters in `search` from the sites of the Sequence%s in the
 * SequenceSet.
 */
void remove_characters( SequenceSet& set, std::string const& search );

/**
 * @brief Remove all gap characters from the sites of the Sequence.
 *
 * This function is an alias for remove_characters(), which by default uses the gap sites of
 * nucleic_acid_codes_undetermined().
 */
void remove_all_gaps( Sequence&    seq, std::string const& gap_chars = nucleic_acid_codes_undetermined() );

/**
 * @brief Remove all gap characters from the sites of the Sequence%s in the SequenceSet.
 *
 * This function is an alias for remove_characters(), which by default uses the gap sites of
 * nucleic_acid_codes_undetermined().
 */
void remove_all_gaps( SequenceSet& set, std::string const& gap_chars = nucleic_acid_codes_undetermined() );

/**
 * @brief Replace all occurences of the chars in `search` by the `replace` char, for all sites in
 * the given Sequence.
 *
 * The function is case sensitive. Thus, you need to use both cases for the search string if you
 * are unsure. The replace char is always used as is, independent of the case of the matching
 * search char.
 */
void replace_characters( Sequence&    seq, std::string const& search, char replacement );

/**
 * @brief Replace all occurences of the chars in `search` by the `replace` char, for all sites in
 * the Sequence%s in the given SequenceSet.
 *
 * The function is case sensitive. Thus, you need to use both cases for the search string if you
 * are unsure. The replace char is always used as is, independent of the case of the matching
 * search char.
 */
void replace_characters( SequenceSet& set, std::string const& search, char replacement );

/**
 * @brief Replace all occurrences of `U` by `T` in the sites of the Sequence.
 *
 * This is a small helper function for sequences with nucleic acid codes. It is case sensitive,
 * that is, lower case `u` is replaced by lower case `t`, and upper case `U` by upper case `T`.
 */
void replace_u_with_t( Sequence&    seq );

/**
 * @brief Replace all occurrences of `U` by `T` in the sites of all Sequence%s in the SequenceSet.
 *
 * This is a small helper function for sequences with nucleic acid codes. It is case sensitive,
 * that is, lower case `u` is replaced by lower case `t`, and upper case `U` by upper case `T`.
 */
void replace_u_with_t( SequenceSet& set );

/**
 * @brief Replace all occurrences of `T` by `U` in the sites of the Sequence.
 *
 * This is a small helper function for sequences with nucleic acid codes. It is case sensitive,
 * that is, lower case `t` is replaced by lower case `u`, and upper case `T` by upper case `U`.
 */
void replace_t_with_u( Sequence&    seq );

/**
 * @brief Replace all occurrences of `T` by `U` in the sites of all Sequence%s in the SequenceSet.
 *
 * This is a small helper function for sequences with nucleic acid codes. It is case sensitive,
 * that is, lower case `t` is replaced by lower case `u`, and upper case `T` by upper case `U`.
 */
void replace_t_with_u( SequenceSet& set );

/**
 * @brief Provide options for changing how merge_duplicate_sequences() handles the counts
 * of merged Sequence%s.
 *
 * This allows to remove duplicate sequences while still keeping track of how many there were
 * before merging them.
 */
enum class MergeDuplicateSequencesCountPolicy
{
    /**
     * @brief The counts are discarded.
     */
    kDiscard,

    /**
     * @brief The counts are appended to the sequence label, separated by the counter_prefix.
     */
    kAppendToLabel,

};

/**
 * @brief Merge all Sequence%s in a SequenceSet that have identical sites.
 *
 * The merging is done by removing all but the first Sequence with identical sites. That means,
 * the resulting "representative" of a set of merged Sequences has the label of the
 * original Sequence that was first in the SequenceSet.
 *
 * Using the MergeDuplicateSequencesCountPolicy, it is possible to store the counts of the
 * Sequences (i.e., how often they appeard before merging) within the label of the Sequence,
 * separated by `counter_prefix`. With the default settings, the count is
 * appended to the label, separated by an underscore.
 */
void merge_duplicate_sequences(
    SequenceSet& set,
    MergeDuplicateSequencesCountPolicy count_policy = MergeDuplicateSequencesCountPolicy::kDiscard,
    std::string const& counter_prefix = "_"
);

// =================================================================================================
//     Normalization
// =================================================================================================

/**
 * @brief Call normalize_nucleic_acid_code() for each site of the Sequence.
 *
 * See there for details.
 */
void normalize_nucleic_acid_codes( Sequence& sequence, bool accept_degenerated = true );

/**
 * @brief Call normalize_nucleic_acid_code() for each site of all Sequence%s in the SequenceSet.
 *
 * See there for details.
 */
void normalize_nucleic_acid_codes( SequenceSet& sequence_set, bool accept_degenerated = true );

/**
 * @brief Call normalize_amino_acid_code() for each site of the Sequence.
 *
 * See there for details.
 */
void normalize_amino_acid_codes( Sequence& sequence, bool accept_degenerated = true );

/**
 * @brief Call normalize_amino_acid_code() for each site of all Sequence%s in the SequenceSet.
 *
 * See there for details.
 */
void normalize_amino_acid_codes( SequenceSet& sequence_set, bool accept_degenerated = true );

// =================================================================================================
//     Filters
// =================================================================================================

/**
 * @brief Remove all Sequence%s from the SequenceSet whose @link Sequence::length() length@endlink
 * is below the given `min_length`.
 *
 * See also filter_max_sequence_length() and filter_min_max_sequence_length().
 */
void filter_min_sequence_length( SequenceSet& set, size_t min_length );

/**
 * @brief Remove all Sequence%s from the SequenceSet whose @link Sequence::length() length@endlink
 * is above the given `max_length`.
 *
 * See also filter_min_sequence_length() and filter_min_max_sequence_length().
 */
void filter_max_sequence_length( SequenceSet& set, size_t max_length );

/**
 * @brief Remove all Sequence%s from the SequenceSet whose @link Sequence::length() length@endlink
 * is not inbetween the `min_length` and `max_length`.
 *
 * This function has the same effect as calling filter_min_sequence_length() and
 * filter_max_sequence_length(), but does it in one iteration over the SequenceSet.
 */
void filter_min_max_sequence_length( SequenceSet& set, size_t min_length, size_t max_length );

// =================================================================================================
//     Print and Output
// =================================================================================================

/**
 * @brief Print a Sequence to an ostream in the form "label: sites".
 *
 * As this is meant for quickly having a look at the Sequence, only the first 100 sites are printed.
 * If you need all sites, or more settings like color, use SimplePrinter.
 */
std::ostream& operator << ( std::ostream& out, Sequence    const& seq );

/**
 * @brief Print a SequenceSet to an ostream in the form "label: sites".
 *
 * As this is meant for quickly having a look at the SequenceSet, only the first 10 Sequences and
 * the first 100 sites of each are printed. If you need all sequences and sites,
 * or more settings like color, use SimplePrinter.
 */
std::ostream& operator << ( std::ostream& out, SequenceSet const& set );

} // namespace sequence
} // namespace genesis

#endif // include guard
