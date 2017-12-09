#ifndef GENESIS_UTILS_TOOLS_GEODESY_FUNCTIONS_H_
#define GENESIS_UTILS_TOOLS_GEODESY_FUNCTIONS_H_

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

#include "genesis/utils/tools/geodesy.hpp"

#include <iosfwd>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     World Constants
// =================================================================================================

/**
 * @brief Earth is not flat!
 */
constexpr double EARTH_MEAN_RADIUS = 6371.0;

// =================================================================================================
//     Coordinate Conversion
// =================================================================================================

/**
 * @brief Replace non-ascii symbols used in geographic coordinates by their ascii equivalents.
 *
 * The function replaces symbols such as degrees, primes, double primes by simple letters ('dms')
 * that serve the same function.
 *
 * Furthermore, if @p two_components is `true` (default), the function assumes that the given
 * coordinate contains two components (North/South and East/West). In that case, if there is exaclty
 * one comma in the input, it is replaced by a slash. This is meant to avoid ambiguity in parsing
 * of a coordinate that uses commas as decimal separator.
 * Hence, when sanitizing a coordinate with only one component, @p two_components needs to be set
 * to `false` in order to avoid replacing a single decimal separator comma by a slash.
 */
std::string sanitize_geo_coordinate( std::string const& coordinate, bool two_components = true );

/**
 * @brief Parse strings of geographic coordinates.
 *
 * This is the same as convert_geo_coordinate( std::string const& ), with the only different being
 * that the two components (NW and EW) are given separately. See there for details.
 */
GeoCoordinate convert_geo_coordinate( std::string const& latitude, std::string const& longitude );

/**
 * @brief Parse a string of geographic coordinates.
 *
 * The function accepts many different formats for geographical coordinates.
 * Some exemplary valid cases:
 *
 *   * `50d4m17.698N 14d24m2.826E`
 *   * `40:26:46N,79:56:55W`
 *   * `40:26:46.302N 179:56:55.903W`
 *   * `49°59'56.948"N, 15°48'22.989"E`
 *   * `50d4m17.698N 14d24m2.826E`
 *   * `49.9991522N, 150.8063858E`
 *   * `40°26′47″N 79°58′36″W`
 *   * `40d 26′ 47″ N 79d 58′ 36″ W`
 *   * `40.446195N 79.948862W`
 *   * `40,446195° 79,948862°`
 *   * `40° 26.7717 / -79° 56.93172`
 *   * `40.446195, -79.948862`
 *   * <code>N 49° 59.94913', E 15° 48.38315'</code>
 *
 * The hemispheres can be given as `[NS]` for the first component and `[EW]` for the second,
 * and can either preceed or follow the numeric values.
 * Degrees, Minutes and Seconds can be separated by the letters `[dms]`, by degree (`°`),
 * prime (` ′ `), single quotation mark (`'`), double prime (`″`), double quoation mark (`"`),
 * or by colons (`:`), respectively.
 * The two components (NS and EW) can be separated by comma (`,`) or slash (`/`).
 *
 * There are some caveats: The hemisphere strings (`NESW`) have to be in capitial letters,
 * while the letters used for degrees, minute and seconds (`dms`) have to be lower case.
 * This constraint avoids ambiguity between seconds (`s`) and south (`S`).
 * Also, either the decimal separator can be a comma, or the separator between components,
 * but not both. This again avoids ambiguity while parsing.
 */
GeoCoordinate convert_geo_coordinate( std::string const& coordinate );

// =================================================================================================
//     Distance
// =================================================================================================

/**
 * @brief Calculate the distance (in km) between two points on Earth.
 *
 * Uses the [Haversine formula](https://en.wikipedia.org/wiki/Haversine_formula).
 * See @link convert_geo_coordinate( std::string const& ) convert_geo_coordinate()@endlink for
 * a conversion function for geographical coordinates.
 */
double geo_distance( GeoCoordinate const& c1, GeoCoordinate const& c2 );

// =================================================================================================
//     Printing
// =================================================================================================

std::ostream& operator<< ( std::ostream& os, GeoCoordinate const& coord );

} // namespace utils
} // namespace genesis

#endif // include guard
