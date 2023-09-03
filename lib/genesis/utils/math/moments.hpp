#ifndef GENESIS_UTILS_MATH_MOMENTS_H_
#define GENESIS_UTILS_MATH_MOMENTS_H_

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

#include <cmath>
#include <vector>

namespace genesis {
namespace utils {

// ================================================================================================
//     Moments
// ================================================================================================

/**
 * @brief Compute running mean and variance for an input.
 *
 * The class allows to keep a running mean and variance for some input, without
 * having to know the number of elements beforehand, and in a single pass.
 *
 * This class is modelled after Knuth's algorithm in TAOCP vol 2, 3rd edition, page 232.
 * See also https://www.johndcook.com/blog/standard_deviation/
 * and https://stackoverflow.com/q/2341340
 */
class Moments
{
public:

    // ---------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------

    /**
     * @brief Construct empty Moments.
     *
     * Takes the @p ddof "Delta Degrees of Freedom": the divisor used in the calculation of the
     * variance is N - ddof, where N represents the number of elements. By default @p ddof is zero,
     * meaning that we are computing the population variance (and standard deviation), instead of
     * the sample variance.
     * See for instance https://numpy.org/doc/stable/reference/generated/numpy.var.html for details.
     */
    explicit Moments( size_t ddof = 0 )
        : ddof_(ddof)
    {}

    /**
     * @brief Compute Momemnts by iteration over a range of `double`.
     *
     * The given iterator pair @p first to @p last needs to dereference to values
     * that are convertible to `double`.
     */
    template<class It>
    Moments( It first, It last, size_t ddof = 0 )
        : ddof_(ddof)
    {
        while( first != last ) {
            push( *first );
            ++first;
        }
    }

    /**
     * @brief Compute Moments, over an initializer list of values.
     */
    Moments( std::initializer_list<double> list, size_t ddof = 0 )
        : ddof_(ddof)
    {
        for( auto const e : list ) {
            push( e );
        }
    }

    ~Moments() = default;

    Moments(Moments const&) = default;
    Moments(Moments&&)      = default;

    Moments& operator= (Moments const&) = default;
    Moments& operator= (Moments&&)      = default;

    // ---------------------------------------------------------
    //     Member Functions
    // ---------------------------------------------------------

    void push( double val )
    {
        count_++;
        if( count_ == 1 ) {
            m_old_ = m_new_ = val;
            s_old_ = 0.0;
        } else {
            m_new_ = m_old_ + ( val - m_old_ ) / static_cast<double>( count_ );
            s_new_ = s_old_ + ( val - m_old_ ) * ( val - m_new_ );
            m_old_ = m_new_;
            s_old_ = s_new_;
        }
    }

    size_t count() const
    {
        return count_;
    }

    double mean() const
    {
        return m_new_;
    }

    double variance() const
    {
        return (( count_ > 1 ) ? s_new_ / ( static_cast<double>( count_ - ddof_ )) : 0.0 );
    }

    double stddev() const
    {
        return std::sqrt( variance() );
    }

    double standard_deviation() const
    {
        return std::sqrt( variance() );
    }

    // ---------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------

private:

    size_t count_ = 0;
    double m_old_ = 0.0;
    double m_new_ = 0.0;
    double s_old_ = 0.0;
    double s_new_ = 0.0;
    size_t ddof_  = 0.0;

};


} // namespace utils
} // namespace genesis

#endif // include guard
