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

#include "genesis/utils/text/string.hpp"
#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/map.hpp"
#include "genesis/utils/tools/tickmarks.hpp"

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
 * @brief Default Color normalization, using a sequential linear scaling in the
 * range `[ min, max ]`.
 */
class ColorNormalization
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Constructor that sets `min == 0.0` and `max == 1.0`.
     */
    ColorNormalization() = default;

    /**
     * @brief Constructor that sets min() and max() to the provided values.
     */
    ColorNormalization( double min, double max )
        : min_( min )
        , max_( max )
    {
        range_check_throw_();
    }

    /**
     * @brief Constructor that sets min() and max() to the min and max of the provided @p values.
     */
    ColorNormalization( std::vector<double> const& values )
    {
        autoscale( values.begin(), values.end() );
    }

    /**
     * @brief Constructor that sets min() and max() to the min and max of the provided range.
     */
    template <class ForwardIterator>
    ColorNormalization( ForwardIterator first, ForwardIterator last )
    {
        autoscale( first, last );
    }

    virtual ~ColorNormalization() = default;

    ColorNormalization(ColorNormalization const&)     = default;
    ColorNormalization(ColorNormalization&&) noexcept = default;

    ColorNormalization& operator= (ColorNormalization const&)     = default;
    ColorNormalization& operator= (ColorNormalization&&) noexcept = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    /**
     * @brief Minimum value, that is, where to begin the color scale.
     */
    double min() const
    {
        return min_;
    }

    /**
     * @brief Minimum value, that is, where to end the color scale.
     */
    double max() const
    {
        return max_;
    }

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

    // -------------------------------------------------------------------------
    //     Modificators
    // -------------------------------------------------------------------------

    /**
     * @brief
     */
    ColorNormalization& scale( double min, double max )
    {
        min_ = min;
        max_ = max;
        update_hook_( min, max );
        return *this;
    }

    /**
     * @brief
     */
    ColorNormalization& autoscale( std::vector<double> const& values )
    {
        return autoscale( values.begin(), values.end() );
    }

    /**
     * @brief Set the min and max of the Palette so that they reflect the min and max valid values
     * that are found in the range `[ first, last )`.
     *
     * The provided iterator range needs to contain values that are convertible and comparable
     * to `double`. Any non-finite values or values that are equal to the mask_value() are skipped.
     * If then no value is found at all, the min and max are not changed.
     *
     * In derived classes, other values might also be set from this. For example,
     * ColorNormalizationDiverging also sets the mid value accordingly.
     */
    template <class ForwardIterator>
    ColorNormalization& autoscale( ForwardIterator first, ForwardIterator last )
    {
        // New values, so that we first do not override the current ones.
        auto min = std::numeric_limits<double>::max();
        auto max = std::numeric_limits<double>::lowest();

        double sum = 0.0;
        size_t cnt = 0;

        while( first != last ) {
            if( ! std::isfinite( *first ) || *first == mask_value_ ) {
                continue;
            }
            if( *first < min ) {
                min = *first;
            }
            if( *first > max ) {
                max = *first;
            }

            sum += *first;
            ++cnt;

            ++first;
        }

        // Only update if we found values.
        if( cnt == 0 ) {
            return *this;
        }

        // Set the values simply to what we found.
        min_ = min;
        max_ = max;
        update_hook_( min, max );

        return *this;
    }

    /**
     * @copydoc min()
     */
    ColorNormalization& min( double value )
    {
        min_ = value;
        update_hook_( min_, max_ );
        return *this;
    }

    /**
     * @copydoc max()
     */
    ColorNormalization& max( double value )
    {
        max_ = value;
        update_hook_( min_, max_ );
        return *this;
    }

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
     * whether it is less than min() or greater than max(),
     * and returns `quiet_NaN`, `-1.0` or `2.0`, respectively.
     * If neither applies, the normalization is applied.
     */
    double operator()( double value ) const
    {
        // Make sure that the norm is set up correctly.
        range_check_throw_();

        if( ! std::isfinite( value ) || value == mask_value_ ) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        if( value < min_ ) {
            return -1.0;
        }
        if( value > max_ ) {
            return 2.0;
        }
        assert( min_ <= value && value <= max_ );
        auto const r = normalize_( value );
        assert( 0.0 <= r && r <= 1.0 );
        return r;
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

    virtual std::map<double, Color> gradient( ColorMap const& map ) const
    {
        std::map<double, Color> result;
        for( size_t i = 0; i < map.size(); ++i ) {
            auto const offset = static_cast<double>( i ) / static_cast<double>( map.size() - 1 );
            result[ offset ] = map.color( i );
        }
        return result;
    }

    virtual std::map<double, std::string> tickmarks( size_t num_ticks ) const
    {
        std::map<double, std::string> result;
        auto tm = Tickmarks();
        auto const tm_labels = tm.linear_labels( min(), max(), num_ticks );
        for( auto const& tm_label : tm_labels ) {
            result[ tm_label.relative_position ] = utils::to_string( tm_label.label );
        }
        return result;
    }

    /**
     * @brief Return whether the ranges are correct.
     */
    virtual bool range_check() const
    {
        return min_ < max_;
    }

protected:

    /**
     * @brief Throw if the ranges are incorrect.
     */
    virtual void range_check_throw_() const
    {
        if( min_ >= max_ ) {
            throw std::runtime_error( "Invalid Color Normalization with min >= max." );
        }
    }

    /**
     * @brief Normalization function.
     *
     * Return a value in range `[ 0.0, 1.0 ]`. Derived classes can override this
     * to provide their specific normalization.
     */
    virtual double normalize_( double value ) const
    {
        // Already checked by operator().
        assert( min() <= value && value <= max() );
        assert( range_check() );

        // Bring value into the range [ 0.0, 1.0 ].
        auto const pos = ( value - min_ ) / ( max_ - min_ );
        return pos;
    }

    /**
     * @brief Called whenever the min and max are set automatically.
     * Gives derived classes a chance to update their values.
     */
    virtual void update_hook_( double min, double max )
    {
        // Nothing to do for this class.
        (void) min;
        (void) max;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    double min_ = 0.0;
    double max_ = 1.0;

    double mask_value_ = std::numeric_limits<double>::quiet_NaN();

};

} // namespace utils
} // namespace genesis

#endif // include guard
