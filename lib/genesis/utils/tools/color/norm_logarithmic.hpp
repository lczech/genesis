#ifndef GENESIS_UTILS_TOOLS_COLOR_NORM_LOGARITHMIC_H_
#define GENESIS_UTILS_TOOLS_COLOR_NORM_LOGARITHMIC_H_

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
 * @ingroup utils
 */

#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/norm_linear.hpp"
#include "genesis/utils/tools/tickmarks.hpp"

#include <cmath>
#include <limits>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Normalization Logarithmic
// =================================================================================================

/**
 * @brief Color normalization for a logarithmic scale.
 */
class ColorNormalizationLogarithmic
    : public ColorNormalizationLinear
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Constructor that sets `min == 1.0` and `max == 100.0`.
     */
    ColorNormalizationLogarithmic()
        : ColorNormalizationLinear( 1.0, 100.0 )
    {}

    /**
     * @brief Constructor that sets min() and max() to the provided values.
     */
    ColorNormalizationLogarithmic( double min, double max )
        : ColorNormalizationLinear( min, max )
    {}

    /**
     * @brief Constructor that sets min() and max() to the min and max of the provided @p values,
     * and mid() to their midpoint.
     */
    ColorNormalizationLogarithmic( std::vector<double> const& values )
    {
        autoscale( values.begin(), values.end() );
    }

    /**
     * @brief Constructor that sets min() and max() to the min and max of the provided range,
     * and mid() to their midpoint.
     */
    template <class ForwardIterator>
    ColorNormalizationLogarithmic( ForwardIterator first, ForwardIterator last )
    {
        autoscale( first, last );
    }

    virtual ~ColorNormalizationLogarithmic() = default;

    ColorNormalizationLogarithmic(ColorNormalizationLogarithmic const&) = default;
    ColorNormalizationLogarithmic(ColorNormalizationLogarithmic&&)      = default;

    ColorNormalizationLogarithmic& operator= (ColorNormalizationLogarithmic const&) = default;
    ColorNormalizationLogarithmic& operator= (ColorNormalizationLogarithmic&&)      = default;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    double base() const
    {
        return base_;
    }

    ColorNormalizationLogarithmic& base( double value )
    {
        base_ = value;
        return *this;
    }

    bool exponential_labels() const
    {
        return exp_labels_;
    }

    ColorNormalizationLogarithmic& exponential_labels( bool value )
    {
        exp_labels_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Virtual Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether the ranges are correct.
     */
    virtual bool is_valid_() const override
    {
        return 0.0 < min_value() && min_value() < max_value();
    }

protected:

    /**
     * @brief Throw if the ranges are incorrect.
     */
    virtual void is_valid_or_throw_() const override
    {
        if( min_value() >= max_value() ) {
            throw std::runtime_error( "Invalid Color Normalization with min >= max" );
        }
        if( min_value() <= 0.0 ) {
            throw std::runtime_error( "Invalid Color Normalization with min <= 0.0" );
        }
    }

    virtual double normalize_( double value ) const override
    {
        // Already checked by base class.
        assert( min_value() <= value && value <= max_value() );
        assert( is_valid_() );

        // As we have 0 < min <= value, this must hold. Otherwise, log won't work.
        assert( 0.0 < value );

        // Bring value into the range [ 0.0, 1.0 ].
        auto const lg_min = std::log( min_value() ) / std::log( base_ );
        auto const lg_max = std::log( max_value() ) / std::log( base_ );
        auto const lg_val = std::log( value ) / std::log( base_ );
        return ( lg_val - lg_min ) / ( lg_max - lg_min );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    double base_ = 10.0;
    bool exp_labels_ = false;

};

} // namespace utils
} // namespace genesis

#endif // include guard
