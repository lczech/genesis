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

    Moments() = default;
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
        return (count_ > 0) ? m_new_ : 0.0;
    }

    double variance() const
    {
        return (( count_ > 1 ) ? s_new_ / ( static_cast<double>( count_ ) - 1.0 ) : 0.0 );
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
};


} // namespace utils
} // namespace genesis

#endif // include guard
