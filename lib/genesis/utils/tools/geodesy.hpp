#ifndef GENESIS_UTILS_TOOLS_GEODESY_H_
#define GENESIS_UTILS_TOOLS_GEODESY_H_

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
 * @ingroup utils
 */

#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Coordinate System Definitions
// =================================================================================================

/**
 * @brief Geographical coordinates in degrees.
 */
class GeoCoordinate
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    GeoCoordinate()  = default;

    GeoCoordinate( double lat, double lon )
    {
        latitude( lat );
        longitude( lon );
    }

    ~GeoCoordinate() = default;

    GeoCoordinate( GeoCoordinate const& ) = default;
    GeoCoordinate( GeoCoordinate&& )      = default;

    GeoCoordinate& operator= ( GeoCoordinate const& ) = default;
    GeoCoordinate& operator= ( GeoCoordinate&& )      = default;

    // -------------------------------------------------------------------------
    //     Getters and Setters
    // -------------------------------------------------------------------------

    /**
     * @brief Latitude, in range `[ -90.0, 90.0 ]`.
     */
    double latitude() const
    {
        return latitude_;
    }

    /**
     * @brief Longitude, in range `[ -180.0, 180.0 ]`.
     */
    double longitude() const
    {
        return longitude_;
    }

    /**
     * @brief Latitude, in range `[ -90.0, 90.0 ]`.
     */
    GeoCoordinate& latitude( double value )
    {
        if( value < -90.0 || value > 90.0 ) {
            throw std::invalid_argument( "Latitude has to be in range [ -90.0, 90.0 ]." );
        }
        latitude_ = value;
        return *this;
    }

    /**
     * @brief Longitude, in range `[ -180.0, 180.0 ]`.
     */
    GeoCoordinate& longitude( double value )
    {
        if( value < -180.0 || value > 180.0 ) {
            throw std::invalid_argument( "Longitude has to be in range [ -180.0, 180.0 ]." );
        }
        longitude_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Private Members
    // -------------------------------------------------------------------------

private:

    double latitude_;
    double longitude_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
