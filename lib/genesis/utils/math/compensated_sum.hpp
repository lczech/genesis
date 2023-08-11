#ifndef GENESIS_UTILS_MATH_COMPENSATED_SUM_H_
#define GENESIS_UTILS_MATH_COMPENSATED_SUM_H_

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
//     Kahan Summation Algorithms
// =================================================================================================

// Forward declaration
template<typename SummationAlgorithm>
class CompensatedSum;

/**
 * @brief Tag for tag dispatching the algorithm in CompensatedSum. See there for details.
 */
struct KahanSummation{};

/**
 * @brief Tag for tag dispatching the algorithm in CompensatedSum. See there for details.
 */
struct NeumaierSummation{};

/**
 * @brief Tag for tag dispatching the algorithm in CompensatedSum. See there for details.
 */
struct KleinSummation{};

/**
 * @brief Typedef shorthand for a Kahan sum.
 */
using KahanSum = CompensatedSum<KahanSummation>;

/**
 * @brief Typedef shorthand for a Kahan Babushka Neumaier sum.
 */
using NeumaierSum = CompensatedSum<NeumaierSummation>;

/**
 * @brief Typedef shorthand for a Kahan Babushka Klein sum.
 */
using KleinSum = CompensatedSum<KleinSummation>;

// =================================================================================================
//     Kahan Sum
// =================================================================================================

/**
 * @brief Compensated summation algorithmm, such as Kahan, Neumaier, and Klein summation.
 *
 * We implement the basic Kahan summation here, taking care of overly aggressive compiler
 * optimizations, as well as several extensions of the algorithm, namely the Kahan Babushka Neumaier
 * summation and the Kahan Babushka Klein summation.
 * See https://en.wikipedia.org/wiki/Kahan_summation_algorithm
 *
 * We use tag dispatch via the tempalte parameter to select the summation algorithm at compile time.
 * Alternatively, we offer typedefs for all three of them, for simplicity.
 * The three currently implemented algorithms can be selected as follows:
 *
 *  - Standard Kahan summation: `CompensatedSum<KahanSummation>`, or KahanSum
 *  - Kahan Babushka Neumaier summation: `CompensatedSum<NeumaierSummation>`, or NeumaierSum
 *  - Kahan Babushka Klein summation: `CompensatedSum<KleinSummation>`, KleinSum
 *
 * Then, use add(), or simply `operator +=` and `operator -=` to add values to the summation,
 * and use get() or implicit conversion via `operator double` to obtain the compensated sum.
 *
 * By default, we use the Neumaier summation, which offers a compromise between accuracy and speed.
 */
template<typename SummationAlgorithm = NeumaierSum>
class CompensatedSum
{
public:

    // ---------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------

    CompensatedSum() = default;

    /**
     * @brief Constructor that initializes the sum to a given @p value.
     */
    CompensatedSum( double value )
        : sum_(value)
    {}

    /**
     * @brief Construct a CompensatedSum, summing over a range of `double`.
     *
     * The given iterator pair @p first to @p last needs to dereference to values
     * that are convertible to `double`. Their sum is computed.
     */
    template<class It>
    CompensatedSum( It first, It last )
    {
        while( first != last ) {
            add( *first );
            ++first;
        }
    }

    ~CompensatedSum() = default;

    CompensatedSum(CompensatedSum const&) = default;
    CompensatedSum(CompensatedSum&&)      = default;

    CompensatedSum& operator= (CompensatedSum const&) = default;
    CompensatedSum& operator= (CompensatedSum&&)      = default;

    // ---------------------------------------------------------
    //     Operators
    // ---------------------------------------------------------

    /**
     * @brief Add a @p value to the sum.
     */
    inline void operator += ( double value )
    {
        add( value );
    }

    /**
     * @brief Subtract a @p value from the sum.
     *
     * This is identical to addting the negative of the @p value.
     */
    inline void operator -= ( double value )
    {
        add( -value );
    }

    /**
     * @brief Set the sum to the given @p value.
     *
     * This will also reset the correction term, as we assume that assining a new value
     * is meant to start a new summation.
     */
    inline CompensatedSum& operator= ( double value )
    {
        sum_ = value;
        cor_0_ = 0.0;
        cor_1_ = 0.0;
        cor_2_ = 0.0;
        return *this;
    }

    /**
     * @brief Conversion to `double`. Return the current compensated sum.
     */
    inline operator double() const
    {
        // We forward to the get() function,
        // as some algorithms do not simply return the sum here.
        return get();
    }

    // ---------------------------------------------------------
    //     Summation Functions
    // ---------------------------------------------------------

    inline void add( double value )
    {
        add_( algorithm_, value );
    }

    inline void reset()
    {
        sum_ = 0.0;
        cor_0_ = 0.0;
        cor_1_ = 0.0;
        cor_2_ = 0.0;
    }

    inline double get() const
    {
        // Corrections of the two advanced algorithms are only applied once in the very end.
        // We always add them here for simplicity, as the unused ones are 0, which is safe to add.
        return sum_ + cor_0_ + cor_1_ + cor_2_;
    }

private:

    // ---------------------------------------------------------
    //     Summation Implementation
    // ---------------------------------------------------------

    inline void add_( KahanSummation, double value )
    {
        // Standard Kahan Summation
        // Use volatile registers to avoid aggressive compiler optimization
        auto const y = value - cor_0_;
        volatile auto const t = sum_ + y;
        volatile auto const z = t - sum_;
        cor_0_ = z - y;
        sum_ = t;

        // Base calculation for reference, without volatile
        // auto const y = value - cor_0_;
        // auto const t = sum_ + y;
        // cor_0_ = ( t - sum_ ) - y;
        // sum_ = t;
    }

    inline void add_( NeumaierSummation, double value )
    {
        // Kahan Babushka Neumaier Summation
        auto const t = sum_ + value;
        if( std::abs( sum_ ) >= std::abs( value )) {
            // If sum is bigger, low-order digits of value are lost.
            volatile auto const v = sum_ - t;
            cor_0_ += v + value;
        } else {
            // Else low-order digits of sum are lost.
            volatile auto const v = value - t;
            cor_0_ += v + sum_;
        }
        sum_ = t;
    }

    inline void add_( KleinSummation, double value )
    {
        // Kahan Babushka Klein Summation
        double c1 = 0.0;
        double c2 = 0.0;
        auto t = sum_ + value;
        if( std::abs( sum_ ) >= std::abs( value )) {
            volatile auto const v = sum_ - t;
            c1 = v + value;
        } else {
            volatile auto const v = value - t;
            c1 = v + sum_;
        }
        sum_ = t;
        t = cor_1_ + c1;
        if( std::abs( cor_1_ ) >= std::abs( c1 )) {
            volatile auto const v = cor_1_ - t;
            c2 = v + c1;
        } else {
            volatile auto const v = c1 - t;
            c2 = v + cor_1_;
        }
        cor_1_ = t;
        cor_2_ = cor_2_ + c2;
    }

    // ---------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------

private:

    // Algorithm, selected at compile time.
    SummationAlgorithm algorithm_{};

    // Sum and correction term.
    double sum_ = 0.0;
    double cor_0_ = 0.0;
    double cor_1_ = 0.0;
    double cor_2_ = 0.0;

};

} // namespace utils
} // namespace genesis

#endif // include guard
