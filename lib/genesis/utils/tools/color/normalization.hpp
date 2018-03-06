#ifndef GENESIS_UTILS_TOOLS_COLOR_NORMALIZATION_H_
#define GENESIS_UTILS_TOOLS_COLOR_NORMALIZATION_H_

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

#include <cassert>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Color Normalization
// =================================================================================================

/**
 * @brief Base class for color normalization.
 */
class ColorNormalization
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    ColorNormalization() = default;
    virtual ~ColorNormalization() = default;

    ColorNormalization(ColorNormalization const&) = default;
    ColorNormalization(ColorNormalization&&)      = default;

    ColorNormalization& operator= (ColorNormalization const&) = default;
    ColorNormalization& operator= (ColorNormalization&&)      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Mask value that identifies invalid values.
     *
     * When querying a value that compares equal to the mask value, the color functions
     * return mask_color() instead of a Color from the palette. This is meant as a simple means
     * of filtering and visualizing invalid values.
     *
     * The mask value is initialized to `quiet_NaN`, so that it never compares equal to anything.
     * Thus, it is not used by default has to be explicitly set to a different value.
     */
    double mask_value() const
    {
        return mask_value_;
    }

    /**
     * @brief Return whether ranges and other values are correct.
     */
    bool is_valid() const
    {
        return is_valid_();
    }

    // -------------------------------------------------------------------------
    //     Modificators
    // -------------------------------------------------------------------------

    /**
     * @copydoc mask_value()
     */
    ColorNormalization& mask_value( double value )
    {
        mask_value_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Normalize
    // -------------------------------------------------------------------------

    /**
     * @brief Normalize a @p value into range `[ 0.0, 1.0 ]`.
     *
     * The function first checks whether the value is finite or mask_value(),
     * and returns `quiet_NaN` if so. If not, the normalization is applied.
     */
    double operator()( double value ) const
    {
        if( ! std::isfinite( value ) || value == mask_value_ ) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return normalize_( value );
    }

    /**
     * @brief Return the normalized values for a vector of @p values.
     *
     * Simply applies operator() ( double value ) to all values.
     */
    std::vector<double> operator() ( std::vector<double> const& values ) const
    {
        using it_type = std::vector<double>::const_iterator;
        return operator()<it_type>( values.begin(), values.end() );
    }

    /**
     * @brief Return the normalized values for a range of values.
     *
     * Simply applies operator() ( double value ) to all values.
     */
    template <class ForwardIterator>
    std::vector<double> operator()( ForwardIterator first, ForwardIterator last ) const
    {
        std::vector<double> result;
        while( first != last ) {
            result.push_back( operator()( *first ) );
            ++first;
        }
        return result;
    }

    // -------------------------------------------------------------------------
    //     (Pure) Virtual Functions
    // -------------------------------------------------------------------------

protected:

    /**
     * @brief Normalization function.
     *
     * Return a value in range `[ 0.0, 1.0 ]` for valid values, or outside of this for clipping values.
     * Derived classes need to override this to provide their specific normalization.
     *
     * By having this function separated from the operator(), we implement a non-virtual interface.
     */
    virtual double normalize_( double value ) const = 0;

    /**
     * @brief Return whether ranges and other values are correct.
     */
    virtual bool is_valid_() const = 0;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    double mask_value_ = std::numeric_limits<double>::quiet_NaN();

};

} // namespace utils
} // namespace genesis

#endif // include guard
