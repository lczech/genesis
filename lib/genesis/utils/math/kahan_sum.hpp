#ifndef GENESIS_UTILS_MATH_KAHAN_SUM_H_
#define GENESIS_UTILS_MATH_KAHAN_SUM_H_

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
 * @ingroup utils
 */

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     KahanSum
// =================================================================================================

/**
 * @brief Kahan summation algorithm.
 *
 * See https://en.wikipedia.org/wiki/Kahan_summation_algorithm
 * We do not implement Neumaier's improvement here as of now.
 */
class KahanSum
{
public:

    // ---------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------

    KahanSum() = default;

    /**
     * @brief Constructor that initializes the sum to a given @p value.
     */
    KahanSum( double value )
        : sum_(value)
    {}

    /**
     * @brief Construct a KahanSum, summing over a range of `double`.
     *
     * The given iterator pair @p first to @p last needs to dereference to values
     * that are convertible to `double`. Their sum is computed.
     */
    template<class It>
    KahanSum( It first, It last )
    {
        while( first != last ) {
            add( *first );
            ++first;
        }
    }

    ~KahanSum() = default;

    KahanSum(KahanSum const&) = default;
    KahanSum(KahanSum&&)      = default;

    KahanSum& operator= (KahanSum const&) = default;
    KahanSum& operator= (KahanSum&&)      = default;

    // ---------------------------------------------------------
    //     Operators
    // ---------------------------------------------------------

    inline void operator += ( double value )
    {
        add( value );
    }

    inline void operator -= ( double value )
    {
        add( -value );
    }

    // ---------------------------------------------------------
    //     Other Functions
    // ---------------------------------------------------------

    inline void reset()
    {
        sum_ = 0.0;
        cor_ = 0.0;
    }

    inline void add( double value )
    {
        auto const y = value - cor_;
        auto const t = sum_ + y;
        cor_ = ( t - sum_ ) - y;
        sum_ = t;

        // Use volatile registers to avoid aggressive compiler optimization.
        // auto const y = value - cor_;
        // volatile auto const t = sum_ + y;
        // volatile auto const z = t - sum_;
        // cor_ = z - y;
        // sum_ = t;
    }

    inline double get() const
    {
        return sum_;
    }

    // ---------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------

private:

    // Sum and correction term.
    double sum_ = 0.0;
    double cor_ = 0.0;

};

} // namespace utils
} // namespace genesis

#endif // include guard
