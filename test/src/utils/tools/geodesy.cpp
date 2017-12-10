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
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/tools/geodesy.hpp"
#include "genesis/utils/tools/geodesy/functions.hpp"
#include "genesis/utils/text/string.hpp"

#include <string>
#include <vector>

using namespace genesis;
using namespace utils;

TEST( Geodesy, ConvertPass )
{
    struct CoordPair
    {
        std::string str;
        double      lat;
        double      lon;
    };

    std::vector<CoordPair> cases = {
        { "50d4m17.698N 14d24m2.826E", 50.07158278, 14.40078500 },
        { "40:26:46N,179:56:55W", 40.44611111, -179.94861111 },
        { "40:26:46.302N 79:56:55.903W", 40.44619500, -79.94886194 },
        { "49°59'56.948\"N, 15°48'22.989\"E", 49.99915222, 15.80638583 },
        { "50d4m17.698N 14d24m2.826E", 50.07158278, 14.40078500 },
        { "49.9991522N, 150.8063858E", 49.99915220, 150.80638580 },
        { "N 49° 59.94913', E 15° 48.38315'", 49.99915217, 15.80638583 },
        { "40°26′47″N 79°58′36″W", 40.44638889, -79.97666667 },
        { "40d 26′ 47″ N 79d 58′ 36″ W", 40.44638889, -79.97666667 },
        { "40.446195N 79.948862W", 40.44619500, -79.94886200 },
        { "40,446195° 79,948862°", 40.44619500, 79.94886200 },
        { "40° 26.7717 / -79° 56.93172", 40.44619500, -79.94886200 },
        { "40.446195, -79.948862", 40.44619500, -79.94886200 }
    };

    for( auto const& test : cases ) {
        // std::cout << test.str << "\n";
        auto const res = convert_geo_coordinate( test.str );

        EXPECT_FLOAT_EQ( test.lat, res.latitude()  ) << test.str;
        EXPECT_FLOAT_EQ( test.lon, res.longitude() ) << test.str;

        // std::cout << "        { \"" << test.str << "\", " << to_string_precise( res.latitude, 8 );
        // std::cout << ", " << to_string_precise( res.longitude, 8 ) << " },\n";
    }
}

TEST( Geodesy, ConvertFail )
{
    // Fail for all kinds of different reasons.
    std::vector<std::string> cases = {
        "500d4m17.698N 14d24m2.826E",
        "40:26.12:46N,79:56:55W",
        "40:26:76.302N 179:56:55.903W",
        "N49°59'56.948\"N, W15°48'22.989\"E",
        "50d4m17.698SN 14d24m2.826E",
        "49.9991522N, -150.8063858E",
        "40°26′47″N +79°58′36″W",
        "40d 26′ 47″ N 79.34d 58′ 36″ W",
        "40.446195F 79.948862G",
        "40,,446195° 79,948862°",
        "90° 26.7717 / -79° 56.93172",
        "40.446195, -279.948862",
        "N 49° 79.94913', E 15° 48.38315'"
    };

    for( auto const& test : cases ) {
        EXPECT_THROW( convert_geo_coordinate( test ), std::invalid_argument);
    }
}

TEST( Geodesy, Distances )
{
    struct CoordPair
    {
        GeoCoordinate c1;
        GeoCoordinate c2;
        double dist;
    };

    // Estimated using https://www.movable-type.co.uk/scripts/latlong.html
    // Slight deviations due to different earth radius etc. But generally, fits.
    std::vector<CoordPair> cases = {
        { {  40.7486, -73.9864 }, { 42.5466, -49.9546 },  2000.0211 },
        { { -85.299,  178.4355 }, { 17.454, -159.345  }, 11463.562  },
        { {  65.34,    132.565 }, { 64.23,   131.546  },   132.5256 },
        { {  90.0,     142.56  }, { 90.0,    -17.45   },     0.0    }
    };

    for( auto const& test : cases ) {
        auto const dist_a = geo_distance( test.c1, test.c2 );
        auto const dist_b = geo_distance( test.c2, test.c1 );

        // Due to differences in compilers and implementations, numercial issues and whatver,
        // the last test case does not yield exactly zero. It is close enough, but this means
        // we have to treat this little snowflake special here.
        if( test.dist == 0.0 ) {
            EXPECT_LT( dist_a, 0.0000000001 ) << test.c1 << " " << test.c2;
            EXPECT_LT( dist_b, 0.0000000001 ) << test.c1 << " " << test.c2;
        } else {
            EXPECT_FLOAT_EQ( test.dist, dist_a ) << test.c1 << " " << test.c2;
            EXPECT_FLOAT_EQ( test.dist, dist_b ) << test.c1 << " " << test.c2;
        }
    }
}
