/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

#include "genesis/utils/tools/date_time.hpp"

#include <cmath>
#include <string>
#include <vector>

using namespace genesis::utils;

TEST( DateTime, ConversionTM )
{
    std::vector<std::string> valids = {
        "2020-04-17 ", " 20200417", " 2020-04-17T00:27:58 ", "2020-04-17 00:27:58\t", "\t20200417T002758",
        "\n20200417 002758 \t", "    20200417002758", "\n\n\t00:27:58", "002758\t\t\n"
    };

    for( auto const& valid : valids ) {
        EXPECT_NO_THROW( convert_to_tm( valid ));
        EXPECT_TRUE( is_convertible_to_tm( valid ));
    }
    EXPECT_NO_THROW( convert_to_tm( valids.begin(), valids.end() ));
    EXPECT_TRUE( is_convertible_to_tm( valids.begin(), valids.end() ));

    valids.push_back( "xyz" );
    EXPECT_ANY_THROW( convert_to_tm( valids.begin(), valids.end() ));
    EXPECT_FALSE( is_convertible_to_tm( valids.begin(), valids.end() ));

    std::vector<std::string> const invalids = {
        "2020/04/17", "04/17/2020", "2020-04-17T00:27:58Z", "2020-04-17 00:27:58Z", "20200417T002758+0100",
        "20200417x002758", "120200417002758", "00:27:58+0", "02758", "What time is it?"
    };

    for( auto const& invalid : invalids ) {
        EXPECT_ANY_THROW( convert_to_tm( invalid ));
        EXPECT_FALSE( is_convertible_to_tm( invalid ));
    }
    EXPECT_ANY_THROW( convert_to_tm( invalids.begin(), invalids.end() ));
    EXPECT_FALSE( is_convertible_to_tm( invalids.begin(), invalids.end() ));
}

TEST( DateTime, ConversionTime )
{
    std::vector<std::string> const times = {
        "2020-04-17 ", " 20200417", " 2020-04-17T00:27:58 ", "2020-04-17 00:27:58\t", "\t20200417T002758",
        "\n20200417 002758 \t", "    20200417002758", "\n\n\t00:27:58", "002758\t\t\n"
    };

    auto const tms = convert_to_tm( times.begin(), times.end() );
    for( auto const& tm1 : tms ) {
        auto tm2 = time_to_tm( tm_to_time( tm1 ));

        // We cannot directly compare the times, as some blanks are filled in in the conversion.
        // The filled-in values are either 1 or -1, for the fields tested here,
        // except for the day and month, which are maxxed... Weird cpp standard...
        // We leave out tm_wday, tm_yday, tm_isdst here.

        EXPECT_TRUE( tm1.tm_sec == tm2.tm_sec );
        EXPECT_TRUE( tm1.tm_min == tm2.tm_min );
        EXPECT_TRUE( std::abs( tm1.tm_hour - tm2.tm_hour ) <= 1 );
        EXPECT_TRUE( tm1.tm_mday == tm2.tm_mday || tm2.tm_mday == 31 );
        EXPECT_TRUE( tm1.tm_mon == tm2.tm_mon || tm2.tm_mon == 11 );
        EXPECT_TRUE( std::abs( tm1.tm_year - tm2.tm_year ) <= 1 );

        // LOG_DBG << "1: " << tm1.tm_sec << " " << tm1.tm_min << " " << tm1.tm_hour << " " << tm1.tm_mday << " " << tm1.tm_mon << " " << tm1.tm_year << " " << tm1.tm_wday << " " << tm1.tm_yday << " " << tm1.tm_isdst << " ";
        //
        // LOG_DBG << "2: " << tm2.tm_sec << " " << tm2.tm_min << " " << tm2.tm_hour << " " << tm2.tm_mday << " " << tm2.tm_mon << " " << tm2.tm_year << " " << tm2.tm_wday << " " << tm2.tm_yday << " " << tm2.tm_isdst << " ";
    }
}

TEST( DateTime, ConversionString )
{
    std::string const s1 = "2020-04-17";
    EXPECT_EQ( s1, tm_date_to_string( convert_to_tm( s1 )));

    std::string const s2 = "00:27:58";
    EXPECT_EQ( s2, tm_time_to_string( convert_to_tm( s2 )));

    std::string const s3 = "2020-04-17T00:27:58";
    EXPECT_EQ( s3, tm_to_string( convert_to_tm( s3 )));
}
