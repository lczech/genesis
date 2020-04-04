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

#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis::utils;

TEST( Text, ConvertBool )
{
    auto vals = std::vector<std::string>{
        "yes", "no", "1", "0", "true", "false", "on", "off"
    };
    auto const exp = std::vector<bool>{
        true, false, true, false, true, false, true, false
    };
    EXPECT_TRUE( is_convertible_to_bool( vals.begin(), vals.end() ));
    EXPECT_EQ( exp, convert_to_bool( vals.begin(), vals.end() ));

    vals.push_back( "x" );
    EXPECT_FALSE( is_convertible_to_bool( vals.begin(), vals.end() ));
    EXPECT_ANY_THROW( convert_to_bool( vals.begin(), vals.end() ));
}

TEST( Text, ConvertDouble )
{
    auto vals = std::vector<std::string>{
        "3.14", " 42 ", "-1", "-6.023e23", "11e-11"
    };
    auto exp = std::vector<double>{
        3.14, 42.0, -1.0, -6.023e23, 11e-11
    };
    EXPECT_TRUE( is_convertible_to_double( vals.begin(), vals.end() ));
    EXPECT_EQ( exp, convert_to_double( vals.begin(), vals.end() ));

    // Infinity.
    vals.push_back( "inf" );
    vals.push_back( "-infinity" );
    exp.push_back( std::numeric_limits<double>::infinity() );
    exp.push_back( - std::numeric_limits<double>::infinity() );
    EXPECT_TRUE( is_convertible_to_double( vals.begin(), vals.end() ));
    EXPECT_EQ( exp, convert_to_double( vals.begin(), vals.end() ));

    // NaN. Need manual check, as nans do not compare.
    vals.push_back( "nan" );
    vals.push_back( "NAN(abc)" );
    EXPECT_TRUE( is_convertible_to_double( vals.begin(), vals.end() ));
    auto const nan_vals = convert_to_double( vals.begin(), vals.end() );
    EXPECT_TRUE( std::isnan( nan_vals[7] ));
    EXPECT_TRUE( std::isnan( nan_vals[8] ));

    // Invalids.
    vals.push_back( "nope" );
    EXPECT_FALSE( is_convertible_to_double( vals.begin(), vals.end() ));
    EXPECT_ANY_THROW( convert_to_double( vals.begin(), vals.end() ));
}
