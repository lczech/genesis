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

#include "genesis/utils/tools/geodesy/functions.hpp"

#include "genesis/utils/math/common.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <ostream>
#include <regex>
#include <stdexcept>
#include <utility>

namespace genesis {
namespace utils {

// =================================================================================================
//     Coordinate Conversion
// =================================================================================================

/**
 * @brief Local helper enum that indicates which component of a coordinate we are dealing with.
 */
enum class GeoCoordinateComponent
{
    kLatitude,
    kLongitude
};

/**
 * @brief Local helper function that takes parts of the regex matches and converts them to double.
 *
 * The strings @p h1, @p d, @p m, @p s and @p h2 are the matches of the regex.
 * The parameter @p component is the hemisphere of this coordinate component, NS or EW,
 * which is determined by whether the matches are the first or the second component of the coordinate.
 */
double convert_single_geo_coordinate(
    std::string const& h1, // Hemisphere
    std::string const& d,  // Degrees
    std::string const& m,  // Minutes
    std::string const& s,  // Seconds
    std::string const& h2, // Hemispheres
    GeoCoordinateComponent  component
) {

    // Depending on the component, store which chars determine pos and neg sign,
    // and the valid range for the hemisphere degrees.
    assert(
        component == GeoCoordinateComponent::kLatitude ||
        component == GeoCoordinateComponent::kLongitude
    );
    char   const pos_h = ( component == GeoCoordinateComponent::kLatitude ? 'n' : 'e' );
    char   const neg_h = ( component == GeoCoordinateComponent::kLatitude ? 's' : 'w' );
    double const d_min = ( component == GeoCoordinateComponent::kLatitude ? -90.0 : -180.0 );
    double const d_max = ( component == GeoCoordinateComponent::kLatitude ? +90.0 : +180.0 );

    // Get hemisphere string.
    std::string h;
    if( ! h1.empty() && ! h2.empty() ) {
        throw std::invalid_argument( "Invalid coordinate: Has two hemisphere directives [NESW]." );
    } else if( ! h1.empty() ) {
        h = h1;
    } else if( ! h2.empty() ) {
        h = h2;
    }

    // By design of the regex, the hemisphere string can only be 0 or 1 char long.
    assert( h.size() < 2 );

    // Get hemisphere sign.
    int sgn = 1;
    if( h.size() == 1 ) {
        if( tolower( h[0] ) == neg_h ) {
            sgn = -1;
        } else if( tolower( h[0] ) != pos_h ) {
            if( component == GeoCoordinateComponent::kLatitude ) {
                throw std::invalid_argument(
                    "Invalid coordinate: Latitude hemisphere directive not in [NS]."
                );
            } else {
                throw std::invalid_argument(
                    "Invalid coordinate: Longitude hemisphere directive not in [EW]."
                );
            }
        }
    }

    // Calc degrees.
    if( d.empty() ) {
        throw std::invalid_argument( "Invalid coordinate: No degrees." );
    }
    double const dd = stod(d);
    if( dd < d_min || dd > d_max ) {
        throw std::invalid_argument(
            "Invalid coordinate: Degrees outside of range [ " + std::to_string( d_min ) + ", " +
            std::to_string( d_max ) + " ]."
        );
    }

    // If a hemisphere was explicitly set, and a sign for the degrees was explicitly set,
    // we need to make sure that they match, and avoid double negatives.
    if( h.size() == 1 && ( d[0] == '+' || d[0] == '-' )) {
        if( ( sgn < 0 && dd > 0.0 ) || ( sgn > 0 && dd < 0.0 ) ) {
            throw std::invalid_argument(
                "Invalid coordinate: Hemisphere does not match sign of the degrees."
            );
        }
        if( sgn < 0 && dd < 0.0 ) {
            sgn = 1;
        }
    }

    // Calc minutes.
    double md = 0.0;
    if( ! m.empty() ) {
        md = stod(m) ;

        if( std::count( d.begin(), d.end(), '.') == 1 ) {
            throw std::invalid_argument(
                "Invalid coordinate: Degrees cannot have decimal part if Minutes are provided."
            );
        }
    }
    if( md < 0.0 || md > 60.0 ) {
        throw std::invalid_argument(
            "Invalid coordinate: Minutes outside of range [ 0.0, 60.0 ]."
        );
    }

    // Calc seconds.
    double sd = 0.0;
    if( ! s.empty() ) {
        sd = stod(s) ;

        if( std::count( m.begin(), m.end(), '.') == 1 ) {
            throw std::invalid_argument(
                "Invalid coordinate: Minutes cannot have decimal part if Seconds are provided."
            );
        }
    }
    if( sd < 0.0 || sd > 60.0 ) {
        throw std::invalid_argument(
            "Invalid coordinate: Seconds outside of range [ 0.0, 60.0 ]."
        );
    }

    // Calculate the final value. We need to take care of adding up negative degrees correctly.
    double v = 0.0;
    if( dd < 0.0 ) {
        v = static_cast<double>( sgn ) * -1.0 * ( ( -1.0 * dd ) + md / 60.0 + sd / 3600.0 );
    } else {
        v = static_cast<double>( sgn ) * ( dd + md / 60.0 + sd / 3600.0 );
    }

    // Finally, we need to check again, as the degrees were added up now.
    if( v < d_min || v > d_max ) {
        throw std::invalid_argument(
            "Invalid coordinate: Degrees outside of range [ " + std::to_string( d_min ) + ", " +
            std::to_string( d_max ) + " ]."
        );
    }
    return v;
}

std::string sanitize_geo_coordinate( std::string const& coordinates, bool two_components )
{
    // So far, this is the only use case of multi byte characters that we have.
    // Would be way too much effort to introduce proper encoding here,
    // so instead we simply treat each multi byte char as just that,
    // a series of bytes, and replace them by single byte ascii characters as needed.

    // Lookup list of what we want to replace.
    // Converted using "UTF-8 code units" from https://r12a.github.io/apps/conversion/
    // The order of the list is important. E.g., we have to replace two primes before one prime!
    std::vector<std::pair<std::string, std::string>> list = {
        { "\xC2\xB0",                 "d" },    // Degree Symbol
        { "\xE2\x80\xB2\xE2\x80\xB2", "s" },    // Two Primes = Seconds
        { "\xE2\x80\xB2",             "m" },    // Prime = Minute
        { "\xE2\x80\xB3",             "s" },    // Double Prime = Seconds
    };

    // Replace all bad (multi byte) chars by their ascii counterparts.
    auto res = coordinates;
    for( auto const& bad : list ) {
        size_t pos = 0;
        while( pos < res.size() ) {

            // Check if we find the bad chars...
            pos = res.find( bad.first, pos );
            if( pos == std::string::npos ) {
                break;
            }

            // ...and replace them.
            res = res.replace( pos, bad.first.size(), bad.second );
        }
    }

    // If there is only one comma, replace it with a slash, so that there is no confusion
    // between comma as decimal separator and comma as separator between two component of the coord.
    if( two_components && std::count( res.begin(), res.end(), ',') == 1 ) {
        auto const pos = res.find( ',' );
        assert( pos < res.size() );
        res[ pos ] = '/';
    }

    // Now replace all commas with dots, so that parsing to double works.
    for( size_t i = 0; i < res.size(); ++i ) {
        if( res[i] == ',' ) {
            res[i] = '.';
        }
    }

    return res;
}

GeoCoordinate convert_geo_coordinate( std::string const& latitude, std::string const& longitude )
{
    return convert_geo_coordinate( latitude + " / " + longitude );
}

GeoCoordinate convert_geo_coordinate( std::string const& coordinate )
{
    // ISO Format, which we probably also should recognize, but currently don't:
    // https://www.w3.org/2005/Incubator/geo/Wiki/LatitudeLongitudeAltitude

    // Prepare static regex (no need to re-compile it on every function call).
    // Alternative regex: https://gist.github.com/moole/3707127
    static std::string const single_expr =
        "([NESW])?[\\s]*"                                       // Group 1: Hemisphere
        "((?:[\\+-]?[0-9]*[\\.][0-9]+)|(?:[\\+-]?[0-9]+))"      // Group 2: Degrees
        "(?:"                                                   // Optional A: All or nothing
        "(?:(?:[\\s]*[d:][\\s]*)|[\\s]+)"                       // Degree Delimiter
        "(?:"                                                   // Optional Minutes
        "((?:[0-9]*[\\.][0-9]+)|(?:[0-9]+))"                    // Group 3: Minutes
        "(?:"                                                   // Optional B: All or nothing
        "(?:(?:[\\s]*['m:][\\s]*)|[\\s]+)"                      // Minutes Delimiter
        "(?:"                                                   // Optional Seconds
        "((?:[0-9]*[\\.][0-9]+)|(?:[0-9]+))"                    // Group 4: Seconds
        "(?:(?:[\\s]*(?:[\"s:]|(?:''))[\\s]*)|[\\s]*)"          // Seconds Delimiter
        ")?"                                                    // Optional Seconds
        ")?"                                                    // Optional B
        ")?"                                                    // Optional Minutes
        ")?"                                                    // Optional A
        "[\\s]*([NESW])?"                                       // Group 5: Hemisphere
    ;
    static std::string const double_expr =
        "^[\\s]*" + single_expr + "(?:(?:[\\s]*[/][\\s]*)|[\\s]+)" + single_expr + "[\\s]*$"
    ;
    static std::regex pattern( double_expr );

    // Run the expression.
    std::smatch matches;
    auto const sanitized = sanitize_geo_coordinate( coordinate );
    if( ! std::regex_search( sanitized, matches, pattern )) {
        throw std::invalid_argument( "Invalid coordinate format." );
    }

    // Print for debug.
    // for (size_t i = 0; i < matches.size(); ++i) {
    //     std::cout << "  " << i << ": '" << matches[i].str() << "'\n";
    // }

    // Calculate component values.
    double const lat = convert_single_geo_coordinate(
        matches[1].str(), matches[2].str(), matches[3].str(), matches[4].str(), matches[5].str(),
        GeoCoordinateComponent::kLatitude
    );
    double const lon = convert_single_geo_coordinate(
        matches[6].str(), matches[7].str(), matches[8].str(), matches[9].str(), matches[10].str(),
        GeoCoordinateComponent::kLongitude
    );
    assert( - 90.0 <= lat && lat <=  90.0  );
    assert( -180.0 <= lon && lon <= 180.0  );

    // Make and return result.
    return { lat, lon };
}

// =================================================================================================
//     Distance
// =================================================================================================

double geo_distance( GeoCoordinate const& c1, GeoCoordinate const& c2 )
{
    // Function adapted from https://rosettacode.org/wiki/Haversine_formula#C

    // Convert to rad, for trigonometry.
    auto to_rad = []( double d ){
        return PI * d / 180.0;
    };

    // Get proper angles.
    auto const th1 = to_rad( c1.latitude() );
    auto const ph1 = to_rad( c1.longitude() );
    auto const th2 = to_rad( c2.latitude() );
    auto const ph2 = to_rad( c2.longitude() );

    // Get parts of the formula.
    auto const dx = cos( ph1 - ph2 ) * cos( th1 ) - cos( th2 );
    auto const dy = sin( ph1 - ph2 ) * cos( th1 );
    auto const dz = sin( th1 ) - sin( th2 );

    return asin( sqrt( dx * dx + dy * dy + dz * dz ) / 2.0 ) * 2.0 * EARTH_MEAN_RADIUS;
}

// =================================================================================================
//     Printing
// =================================================================================================

std::ostream& operator<< ( std::ostream& os, GeoCoordinate const& coord )
{
    os << "( " << coord.latitude() << ", " << coord.longitude() << " )";
    return os;
}

} // namespace utils
} // namespace genesis
