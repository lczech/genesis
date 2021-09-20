#ifndef GENESIS_POPULATION_FUNCTIONS_GENOME_LOCUS_H_
#define GENESIS_POPULATION_FUNCTIONS_GENOME_LOCUS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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

#include <cassert>
#include <iostream>
#include <string>

namespace genesis {
namespace population {

// =================================================================================================
//     Output
// =================================================================================================

inline std::ostream& operator << ( std::ostream& os, GenomeLocus const& l )
{
    os << l.to_string();
    return os;
}

// =================================================================================================
//     Comparion Operators
// =================================================================================================

// Really good example of the claim that C++ needs a lot of boilerplate code...
// We here provide all comparison operators for GenomeLocus, but also comparing loci given
// as chromosome (`std::string`) and position (`size_t`) separately.
// Maybe there is a smarter way to generate this, probably using macros, but then again,
// most operators are optimized, and macros are ugly...
// So, we live with the boilerplate for clarity - this also works better with code documentation!

// -------------------------------------------------------------------------
//     Spaceship <=>
// -------------------------------------------------------------------------

/**
 * @brief Three-way comparison (spaceship operator `<=>`) for two loci in a genome.
 *
 * The comparison returns `-1` if the left locus is before the right locus,
 * `+1` for the opposite, and `0` if the two loci are equal.
 *
 * Note that for our purposes, chromosome names are also sorted in lexicographical order,
 * hence, two loci on different chromosomes will first compare the ordering of their chromosome
 * names.
 */
inline int locus_compare(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position
) {
    // This is a lot of branching. Maybe there is a smarter way. Good enough for now though.
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
    assert( l_chromosome == r_chromosome );
    assert( l_position == r_position );
    return 0;
}

/**
 * @copydoc locus_compare( std::string const&, size_t, std::string const&, size_t )
 */
inline int locus_compare(
    GenomeLocus const& l,
    std::string const& r_chromosome, size_t r_position
) {
    return locus_compare( l.chromosome, l.position, r_chromosome, r_position );
}

/**
 * @copydoc locus_compare( std::string const&, size_t, std::string const&, size_t )
 */
inline int locus_compare(
    std::string const& l_chromosome, size_t l_position,
    GenomeLocus const& r
) {
    return locus_compare( l_chromosome, l_position, r.chromosome, r.position );
}

/**
 * @copydoc locus_compare( std::string const&, size_t, std::string const&, size_t )
 */
inline int locus_compare(
    GenomeLocus const& l,
    GenomeLocus const& r
) {
    return locus_compare( l.chromosome, l.position, r.chromosome, r.position );
}

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
 * @brief Inquality comparison (`!=`) for two loci in a genome.
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
 * Note that for our purposes, chromosome names are also sorted in lexicographical order,
 * hence, two loci on different chromosomes will first compare the ordering of their chromosome
 * names.
 */
inline bool locus_less(
    std::string const& l_chromosome, size_t l_position,
    std::string const& r_chromosome, size_t r_position
) {
    return l_chromosome < r_chromosome || ( l_chromosome == r_chromosome && l_position < r_position );
}

/**
 * @copydoc locus_less( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_less(
    GenomeLocus const& l,
    std::string const& r_chromosome, size_t r_position
) {
    return locus_less( l.chromosome, l.position, r_chromosome, r_position );
}

/**
 * @copydoc locus_less( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_less(
    std::string const& l_chromosome, size_t l_position,
    GenomeLocus const& r
) {
    return locus_less( l_chromosome, l_position, r.chromosome, r.position );
}

/**
 * @copydoc locus_less( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_less(
    GenomeLocus const& l,
    GenomeLocus const& r
) {
    return locus_less( l.chromosome, l.position, r.chromosome, r.position );
}

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
    return locus_less( r_chromosome, r_position, l_chromosome, l_position );
}

/**
 * @copydoc locus_greater( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_greater(
    GenomeLocus const& l,
    std::string const& r_chromosome, size_t r_position
) {
    return locus_greater( l.chromosome, l.position, r_chromosome, r_position );
}

/**
 * @copydoc locus_greater( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_greater(
    std::string const& l_chromosome, size_t l_position,
    GenomeLocus const& r
) {
    return locus_greater( l_chromosome, l_position, r.chromosome, r.position );
}

/**
 * @copydoc locus_greater( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_greater(
    GenomeLocus const& l,
    GenomeLocus const& r
) {
    return locus_greater( l.chromosome, l.position, r.chromosome, r.position );
}

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
    GenomeLocus const& l,
    std::string const& r_chromosome, size_t r_position
) {
    return locus_less_or_equal( l.chromosome, l.position, r_chromosome, r_position );
}

/**
 * @copydoc locus_less_or_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_less_or_equal(
    std::string const& l_chromosome, size_t l_position,
    GenomeLocus const& r
) {
    return locus_less_or_equal( l_chromosome, l_position, r.chromosome, r.position );
}

/**
 * @copydoc locus_less_or_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_less_or_equal(
    GenomeLocus const& l,
    GenomeLocus const& r
) {
    return locus_less_or_equal( l.chromosome, l.position, r.chromosome, r.position );
}

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
    return locus_less_or_equal( r_chromosome, r_position, l_chromosome, l_position );
}

/**
 * @copydoc locus_greater_or_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_greater_or_equal(
    GenomeLocus const& l,
    std::string const& r_chromosome, size_t r_position
) {
    return locus_greater_or_equal( l.chromosome, l.position, r_chromosome, r_position );
}

/**
 * @copydoc locus_greater_or_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_greater_or_equal(
    std::string const& l_chromosome, size_t l_position,
    GenomeLocus const& r
) {
    return locus_greater_or_equal( l_chromosome, l_position, r.chromosome, r.position );
}

/**
 * @copydoc locus_greater_or_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool locus_greater_or_equal(
    GenomeLocus const& l,
    GenomeLocus const& r
) {
    return locus_greater_or_equal( l.chromosome, l.position, r.chromosome, r.position );
}

/**
 * @copydoc locus_greater_or_equal( std::string const&, size_t, std::string const&, size_t )
 */
inline bool operator >= ( GenomeLocus const& l, GenomeLocus const& r )
{
    return locus_greater_or_equal( l.chromosome, l.position, r.chromosome, r.position );
}

} // namespace population
} // namespace genesis

#endif // include guard
