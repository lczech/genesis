#ifndef GENESIS_SEQUENCE_COUNTS_H_
#define GENESIS_SEQUENCE_COUNTS_H_

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

#include "genesis/utils/math/matrix.hpp"
#include "genesis/utils/tools/char_lookup.hpp"

#include <cstdint>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forwardd Declarations
// =================================================================================================

class Sequence;
class SequenceSet;

// =================================================================================================
//     Sequence Counts
// =================================================================================================

/**
 * @brief Store counts of the occurence for certain characters of Sequence%s.
 *
 * This class is a helper for statistical analyses of Sequences, and for calculating consensus
 * sequences and  the like. It stores a @link utils::Matrix Matrix@endlink of counts, for a
 * set of characters and a given sequence length.
 *
 * For example, after adding several Sequences, the matrix might look like this (site indices in
 * columns, characters in rows):
 *
 * site |  0 |  1 |  2 |  3 |  4
 * -    | -: | -: | -: | -: | -:
 * A    |  3 |  0 |  1 |  3 |  0
 * C    |  1 |  2 |  1 |  1 |  4
 * G    |  0 |  1 |  1 |  0 |  1
 * T    |  2 |  1 |  3 |  3 |  1
 * -    |  2 |  4 |  2 |  1 |  2
 *
 * The class has to be constructed with the desired set of characters and sequences length.
 * All not-included characters are simply ignored when adding Sequences.
 *
 * Use add_sequence() and add_sequences() to add counts. Use count_of() and count_at() to get the
 * counter values for specific positions in the matrix.
 */
class SequenceCounts
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Type of uint used for internally counting the freuqencies of Sequence sites.
     *
     * We use this alias here, because in the future, we might need to adjust this value:
     * Either to save memory if many different objects of type SequenceCounts are needed, so that
     * they need to be small; or on the contrary, to allow for more Sequence&s being counted by
     * using a broader type here.
     */
    using CountsIntType = uint32_t;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Default constructor.
     *
     * This instanciates an object with no characters to count and no sequence length. It is thus
     * empty and cannot be used for any further analyses. It is provided here becase offering
     * a default constructor might be good in some cases.
     */
    SequenceCounts() = default;

    /**
     * @brief Construct an object that counts the occurences of the given @p characters for
     * Sequence%s of lenght @p length.
     */
    SequenceCounts( std::string const& characters, size_t length );

    ~SequenceCounts() = default;

    SequenceCounts( SequenceCounts const& ) = default;
    SequenceCounts( SequenceCounts&& )      = default;

    SequenceCounts& operator= ( SequenceCounts const& ) = default;
    SequenceCounts& operator= ( SequenceCounts&& )      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Return the number of sites used for counting.
     *
     * This has to match the Sequence::length() property of the Sequence%s to be added for counting.
     */
    size_t        length() const;

    /**
     * @brief Return the character set that is used for counting.
     */
    std::string   characters() const;

    /**
     * @brief Return the number of processed Sequence%s.
     */
    CountsIntType added_sequences_count() const;

    /**
     * @brief Return the count of a specific character at a given site.
     *
     * If the charater is not part of the set of used characters, the function throws an exception.
     * See characters() to retrieve that set.
     */
    CountsIntType count_of( size_t site_index, char   character ) const;

    /**
     * @brief Return the count for a character and a site, given their indices.
     */
    CountsIntType count_at( size_t site_index, size_t character_index ) const;

    // std::vector< CountsIntType> counts_at( size_t site_index ) const;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    /**
     * @brief Process a single Sequence and add its counts to the existing ones.
     */
    void add_sequence( Sequence const& sequence );

    /**
    * @brief Process a single sequence in string form and add its counts to the existing ones.
    */
    void add_sequence( std::string const& sites );

    /**
    * @brief Process a SequenceSet and add its counts to the existing ones for all contained
    * Sequence%s.
    */
    void add_sequences( SequenceSet const& sequences );

    /**
     * @brief Clear the object, that is, delete everything.
     *
     * This function sets the object status to the same that the
     * @link SequenceCounts() default constructor@endlink gives. Thus, it is not usable any more.
     * It is mainly intended to save memory when many objects are used and then no longer needed.
     *
     * For an alternative function that simply resets the counts to zero but keeps the dimentions
     * of the count matrix, see clear_counts().
     */
    void clear();

    /**
     * @brief Reset all counts to 0.
     *
     * This clears the counts so that the object is as if newly created, while keeping the
     * counted characters and length of the count matrix.
     */
    void clear_counts();

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string                        characters_;
    utils::CharLookup< unsigned char > lookup_;
    utils::Matrix< CountsIntType >     counts_;
    CountsIntType                      num_seqs_ = 0;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
