#ifndef GENESIS_POPULATION_FUNCTIONS_GENOME_LOCUS_H_
#define GENESIS_POPULATION_FUNCTIONS_GENOME_LOCUS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/genome_locus.hpp"
#include "genesis/sequence/sequence_dict.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Output
// =================================================================================================

inline std::string to_string( GenomeLocus const& locus )
{
    // Error case.
    if( locus.chromosome.empty() ) {
        throw std::invalid_argument( "Invalid GenomeLocus with empty chromosome." );
    }

    // Special case.
    if( locus.position == 0 ) {
        return locus.chromosome;
    }

    // General case.
    return locus.chromosome + ":" + std::to_string( locus.position );
}

inline std::ostream& operator << ( std::ostream& os, GenomeLocus const& locus )
{
    os << to_string( locus );
    return os;
}

// =================================================================================================
//     Comparion Operators
// =================================================================================================

// Really good example of the claim that C++ needs a lot of boilerplate code...
// We here provide all comparison operators for GenomeLocus, but also comparing loci given
// as chromosome (`std::string`) and position (`size_t`) separately,
// and then also overload all of those for using references and shared pointers to a SequenceDict.
//
// Previously (before also offering overloads that take SequenceDict), we just spelled out all
// overloads manually, but that became too cumbersome with the extra overloads. So now we use a
// macro that defines all of them for us. It is not super nice for the doxygen code documentation,
// as the doxygen preprocessor does not fully seem to be able to expand the macro. Hence, we cannot
// use copydoc to refer to specific functions, and have re-written the doc blocks of the main
// functions (the ones that the macro-expanded functions call) so that they work for all overloads.

#define ADD_LOCUS_COMPARISON_OVERLOADS( CMP_FUNC ) \
    /** @copydoc CMP_FUNC */ \
    inline int CMP_FUNC( \
        GenomeLocus const& l, \
        std::string const& r_chromosome, size_t r_position \
    ) { \
        return CMP_FUNC( l.chromosome, l.position, r_chromosome, r_position ); \
    } \
    \
    /** @copydoc CMP_FUNC */ \
    inline int CMP_FUNC( \
        std::string const& l_chromosome, size_t l_position, \
        GenomeLocus const& r \
    ) { \
        return CMP_FUNC( l_chromosome, l_position, r.chromosome, r.position ); \
    } \
    \
    /** @copydoc CMP_FUNC */ \
    inline int CMP_FUNC( \
        GenomeLocus const& l, \
        GenomeLocus const& r \
    ) { \
        return CMP_FUNC( l.chromosome, l.position, r.chromosome, r.position ); \
    } \
    \
    /** @copydoc CMP_FUNC */ \
    inline int CMP_FUNC( \
        GenomeLocus const& l, \
        std::string const& r_chromosome, size_t r_position, \
        genesis::sequence::SequenceDict const& sequence_dict \
    ) { \
        return CMP_FUNC( l.chromosome, l.position, r_chromosome, r_position, sequence_dict ); \
    } \
    \
    /** @copydoc CMP_FUNC */ \
    inline int CMP_FUNC( \
        std::string const& l_chromosome, size_t l_position, \
        GenomeLocus const& r, \
        genesis::sequence::SequenceDict const& sequence_dict \
    ) { \
        return CMP_FUNC( l_chromosome, l_position, r.chromosome, r.position, sequence_dict ); \
    } \
    \
    /** @copydoc CMP_FUNC */ \
    inline int CMP_FUNC( \
        GenomeLocus const& l, \
        GenomeLocus const& r, \
        genesis::sequence::SequenceDict const& sequence_dict \
    ) { \
        return CMP_FUNC( l.chromosome, l.position, r.chromosome, r.position, sequence_dict ); \
    } \
    \
    /** @copydoc CMP_FUNC */ \
    inline int CMP_FUNC( \
        std::string const& l_chromosome, size_t l_position, \
        std::string const& r_chromosome, size_t r_position, \
        std::shared_ptr<genesis::sequence::SequenceDict> const& sequence_dict \
    ) { \
        if( sequence_dict ) { \
            return CMP_FUNC( l_chromosome, l_position, r_chromosome, r_position, *sequence_dict ); \
        } else { \
            return CMP_FUNC( l_chromosome, l_position, r_chromosome, r_position ); \
        } \
    } \
    \
    /** @copydoc CMP_FUNC */ \
    inline int CMP_FUNC( \
        GenomeLocus const& l, \
        std::string const& r_chromosome, size_t r_position, \
        std::shared_ptr<genesis::sequence::SequenceDict> const& sequence_dict \
    ) { \
        if( sequence_dict ) { \
            return CMP_FUNC( l.chromosome, l.position, r_chromosome, r_position, *sequence_dict ); \
        } else { \
            return CMP_FUNC( l.chromosome, l.position, r_chromosome, r_position ); \
        } \
    } \
    \
    /** @copydoc CMP_FUNC */ \
    inline int CMP_FUNC( \
        std::string const& l_chromosome, size_t l_position, \
        GenomeLocus const& r, \
        std::shared_ptr<genesis::sequence::SequenceDict> const& sequence_dict \
    ) { \
        if( sequence_dict ) { \
            return CMP_FUNC( l_chromosome, l_position, r.chromosome, r.position, *sequence_dict ); \
        } else { \
            return CMP_FUNC( l_chromosome, l_position, r.chromosome, r.position ); \
        } \
    } \
    \
    /** @copydoc CMP_FUNC */ \
    inline int CMP_FUNC( \
        GenomeLocus const& l, \
        GenomeLocus const& r, \
        std::shared_ptr<genesis::sequence::SequenceDict> const& sequence_dict \
    ) { \
        if( sequence_dict ) { \
            return CMP_FUNC( l.chromosome, l.position, r.chromosome, r.position, *sequence_dict ); \
        } else { \
            return CMP_FUNC( l.chromosome, l.position, r.chromosome, r.position ); \
        } \
    }

// -------------------------------------------------------------------------
//     Spaceship <=>
// -------------------------------------------------------------------------

/**
 * @brief Three-way comparison (spaceship operator `<=>`) for two loci in a genome.
 *
 * We generally compare loci based on their chromosome first, and then,
 * if both chromosomes are identicaly, based on their position within that chromosome.
 * The comparison returns `-1` if the left locus is before the right locus,
 * `+1` for the opposite, and `0` if the two loci are equal.
 *
 * We offer several overloads of this function:
 *
 *   - The two loci that we want to compare can be provided either as GenomeLocus instances,
 *     or as a `std::string` for the chromosome, and a `size_t` for the position. There are
 *     overloads for every combination of those two ways of specifying loci.
 *   - Furthermore, in overloads that do not provide a
 *     @link ::genesis::sequence::SequenceDict SequenceDict@endlink,
 *     chromosome names are expected to be sorted in lexicographical order, hence, two loci on
 *     different chromosomes will first compare the ordering of their chromosome names
 *     lexicographically, and then (if identical) compare the positions.
 *   - In overloads that take a @link ::genesis::sequence::SequenceDict SequenceDict@endlink,
 *     chromosome names are not compared lexicographically, but instead by the order as given by
 *     the SequenceDict.
 *   - The SequenceDict can either be provided by reference, or as a `std::shared_ptr`. In the
 *     latter case, it is only used when the pointer is valid; otherwise, the overload without
 *     SequenceDict is used instead. This is meant as a simplification for situations where
 *     one might or might not have a SequenceDict to work with.
 *
 * The latter type of overloads allow to be more flexible with the sorting orders of chromosomes.
 *
 * @see There are several related comparison functions for genomic loci. See locus_compare(),
 * locus_equal(), locus_inequal(), locus_less(), locus_greater(), locus_less_or_equal(), and
 * locus_greater_or_equal().
 */
inline int locus_compare(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position
) {
    // This is a lot of branching. Maybe there is a smarter way. Good enough for now though,
    // and let's hope the compiler optimizes this for us a little bit.
    if( l_chromosome < r_chromosome ) {
        return -1;
    } else if( l_chromosome > r_chromosome ) {
        return +1;
    } else {
        assert( l_chromosome == r_chromosome );
        if( l_position < r_position ) {
            return -1;
        } else if( l_position > r_position ) {
            return +1;
        }
    }
    assert( l_position == r_position );
    return 0;
}

/**
 * @copydoc locus_compare()
 */
inline int locus_compare(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position,
    ::genesis::sequence::SequenceDict const& sequence_dict
) {
    // Here, we do not want to compare chromosome names directly, but instead their order
    // in the given dict. So, we get their indices, and compare those.
    auto const l_chr_idx = sequence_dict.index_of( l_chromosome );
    auto const r_chr_idx = sequence_dict.index_of( r_chromosome );
    if( l_chr_idx < r_chr_idx ) {
        return -1;
    } else if( l_chr_idx > r_chr_idx ) {
        return +1;
    } else {
        assert( l_chromosome == r_chromosome );
        assert( l_chr_idx == r_chr_idx );
        if( l_position < r_position ) {
            return -1;
        } else if( l_position > r_position ) {
            return +1;
        }
    }
    assert( l_position == r_position );
    return 0;
}

// Add all other overloads for GenomeLocus and SequenceDict combinations.
ADD_LOCUS_COMPARISON_OVERLOADS( locus_compare )

// Let's be cool and add the actual spaceship, even if genesis is currently using C++11.
// Users might compile with later versions, so this might be useful to have.

#if __cplusplus >= 202002L

/**
 * @copydoc locus_compare( std::string const&, size_t, std::string const&, size_t )
 */
inline int operator <=> ( GenomeLocus const& l, GenomeLocus const& r )
{
    return locus_compare( l.chromosome, l.position, r.chromosome, r.position );
}

#endif // __cplusplus >= 202002L

// -------------------------------------------------------------------------
//     Equality ==
// -------------------------------------------------------------------------

/**
 * @brief Equality comparison (`==`) for two loci in a genome.
 *
 * @see There are several related comparison functions for genomic loci. See locus_compare(),
 * locus_equal(), locus_inequal(), locus_less(), locus_greater(), locus_less_or_equal(), and
 * locus_greater_or_equal().
 */
inline bool locus_equal(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position
) {
    return l_chromosome == r_chromosome && l_position == r_position;
}

/**
 * @copydoc locus_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_equal(
    GenomeLocus const& l,
    std::string const& r_chromosome, size_t r_position
) {
    return locus_equal( l.chromosome, l.position, r_chromosome, r_position );
}

/**
 * @copydoc locus_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_equal(
    std::string const& l_chromosome, size_t l_position,
    GenomeLocus const& r
) {
    return locus_equal( l_chromosome, l_position, r.chromosome, r.position );
}

/**
 * @copydoc locus_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_equal(
    GenomeLocus const& l,
    GenomeLocus const& r
) {
    return locus_equal( l.chromosome, l.position, r.chromosome, r.position );
}

/**
 * @copydoc locus_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool operator == ( GenomeLocus const& l, GenomeLocus const& r )
{
    return locus_equal( l.chromosome, l.position, r.chromosome, r.position );
}

// -------------------------------------------------------------------------
//     Inequality !=
// -------------------------------------------------------------------------

/**
 * @brief Inequality comparison (`!=`) for two loci in a genome.
 *
 * @see There are several related comparison functions for genomic loci. See locus_compare(),
 * locus_equal(), locus_inequal(), locus_less(), locus_greater(), locus_less_or_equal(), and
 * locus_greater_or_equal().
 */
inline bool locus_inequal(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position
) {
    return ! locus_equal( l_chromosome, l_position, r_chromosome, r_position );
}

/**
 * @copydoc locus_inequal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_inequal(
    GenomeLocus const& l,
    std::string const& r_chromosome, size_t r_position
) {
    return locus_inequal( l.chromosome, l.position, r_chromosome, r_position );
}

/**
 * @copydoc locus_inequal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_inequal(
    std::string const& l_chromosome, size_t l_position,
    GenomeLocus const& r
) {
    return locus_inequal( l_chromosome, l_position, r.chromosome, r.position );
}

/**
 * @copydoc locus_inequal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_inequal(
    GenomeLocus const& l,
    GenomeLocus const& r
) {
    return locus_inequal( l.chromosome, l.position, r.chromosome, r.position );
}

/**
 * @copydoc locus_inequal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool operator != ( GenomeLocus const& l, GenomeLocus const& r )
{
    return locus_inequal( l.chromosome, l.position, r.chromosome, r.position );
}

// -------------------------------------------------------------------------
//     Less than <
// -------------------------------------------------------------------------

/**
 * @brief Less than comparison (`<`) for two loci in a genome.
 *
 * See locus_compare() for notes on the chromosome comparison order and the available overloads.
 *
 * @see There are several related comparison functions for genomic loci. See locus_compare(),
 * locus_equal(), locus_inequal(), locus_less(), locus_greater(), locus_less_or_equal(), and
 * locus_greater_or_equal().
 */
inline bool locus_less(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position
) {
    return l_chromosome < r_chromosome || ( l_chromosome == r_chromosome && l_position < r_position );
}

/**
 * @copydoc locus_less()
 */
inline bool locus_less(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position,
    ::genesis::sequence::SequenceDict const& sequence_dict
) {
    // Same logic as above, but using chromosome indices in the dict, instead of their names.
    auto const l_chr_idx = sequence_dict.index_of( l_chromosome );
    auto const r_chr_idx = sequence_dict.index_of( r_chromosome );
    return l_chr_idx < r_chr_idx || ( l_chr_idx == r_chr_idx && l_position < r_position );
}

// Add all other overloads for GenomeLocus and SequenceDict combinations.
ADD_LOCUS_COMPARISON_OVERLOADS( locus_less )

/**
 * @copydoc locus_less( std::string const&, size_t, std::string const&, size_t )
 */
inline bool operator < ( GenomeLocus const& l, GenomeLocus const& r )
{
    return locus_less( l.chromosome, l.position, r.chromosome, r.position );
}

// -------------------------------------------------------------------------
//     Greater than >
// -------------------------------------------------------------------------

/**
 * @brief Greater than comparison (`>`) for two loci in a genome.
 *
 * @copydetails locus_less( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_greater(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position
) {
    // Just use the existing function, but with reverse l and r.
    return locus_less( r_chromosome, r_position, l_chromosome, l_position );
}

/**
 * @copydoc locus_greater( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_greater(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position,
    ::genesis::sequence::SequenceDict const& sequence_dict
) {
    // Just use the existing function, but with reverse l and r.
    return locus_less( r_chromosome, r_position, l_chromosome, l_position, sequence_dict );
}

// Add all other overloads for GenomeLocus and SequenceDict combinations.
ADD_LOCUS_COMPARISON_OVERLOADS( locus_greater )

/**
 * @copydoc locus_greater( std::string const&, size_t, std::string const&, size_t )
 */
inline bool operator > ( GenomeLocus const& l, GenomeLocus const& r )
{
    return locus_greater( l.chromosome, l.position, r.chromosome, r.position );
}

// -------------------------------------------------------------------------
//     Less than or equal <=
// -------------------------------------------------------------------------

/**
 * @brief Less than or equal comparison (`<=`) for two loci in a genome.
 *
 * @copydetails locus_less( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_less_or_equal(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position
) {
    // We could do the simple default way of implementing this as `a == b || a < b`,
    // but this seems wasteful; in this case, we can do with fewer comparisons!
    return l_chromosome < r_chromosome || ( l_chromosome == r_chromosome && l_position <= r_position );
}

/**
 * @copydoc locus_less_or_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_less_or_equal(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position,
    ::genesis::sequence::SequenceDict const& sequence_dict
) {
    // Same logic as above, but using chromosome indices in the dict, instead of their names.
    auto const l_chr_idx = sequence_dict.index_of( l_chromosome );
    auto const r_chr_idx = sequence_dict.index_of( r_chromosome );
    return l_chr_idx < r_chr_idx || ( l_chr_idx == r_chr_idx && l_position <= r_position );
}

// Add all other overloads for GenomeLocus and SequenceDict combinations.
ADD_LOCUS_COMPARISON_OVERLOADS( locus_less_or_equal )

/**
 * @copydoc locus_less_or_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool operator <= ( GenomeLocus const& l, GenomeLocus const& r )
{
    return locus_less_or_equal( l.chromosome, l.position, r.chromosome, r.position );
}

// -------------------------------------------------------------------------
//     Greater than or equal >=
// -------------------------------------------------------------------------

/**
 * @brief Greater than or equal comparison (`>=`) for two loci in a genome.
 *
 * @copydetails locus_less( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_greater_or_equal(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position
) {
    // Just use the existing function, but with reverse l and r.
    return locus_less_or_equal( r_chromosome, r_position, l_chromosome, l_position );
}

/**
 * @copydoc locus_greater_or_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_greater_or_equal(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position,
    ::genesis::sequence::SequenceDict const& sequence_dict
) {
    // Just use the existing function, but with reverse l and r.
    return locus_less_or_equal( r_chromosome, r_position, l_chromosome, l_position, sequence_dict );
}

// Add all other overloads for GenomeLocus and SequenceDict combinations.
ADD_LOCUS_COMPARISON_OVERLOADS( locus_greater_or_equal )

/**
 * @copydoc locus_greater_or_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool operator >= ( GenomeLocus const& l, GenomeLocus const& r )
{
    return locus_greater_or_equal( l.chromosome, l.position, r.chromosome, r.position );
}

// -------------------------------------------------------------------------
//     Clean Up
// -------------------------------------------------------------------------

// We do not need to propagate this macro throughout our code base.
#undef ADD_LOCUS_COMPARISON_OVERLOADS

} // namespace population
} // namespace genesis

#endif // include guard
