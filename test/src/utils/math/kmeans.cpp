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

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/math/kmeans.hpp"
#include "genesis/utils/formats/svg/svg.hpp"

#include <array>
#include <utility>

using namespace genesis;
using namespace utils;

// =================================================================================================
//     Test Cases
// =================================================================================================

TEST( Math, Kmeans )
{
    using Point = std::vector<double>;

    // utils::Logging::details.time = true;
    // utils::Options::get().number_of_threads( 4 );
    // utils::Options::get().random_seed( 174333141 );
    // LOG_BOLD << utils::Options::get().info();
    // LOG_BOLD;

    // -------------------------------------------------------------------------
    //     Prepare data
    // -------------------------------------------------------------------------

    // LOG_DBG << "Prepare data";

    size_t factor = 100;
    // size_t factor = 1000000;

    auto& e = Options::get().random_engine();

    std::normal_distribution<double> c1_d0( 5.0, 0.7 );
    std::normal_distribution<double> c1_d1( 8.0, 1.0 );

    std::normal_distribution<double> c2_d0( -2.0, 0.8 );
    std::normal_distribution<double> c2_d1( 3.0, 0.4 );

    std::normal_distribution<double> c3_d0( 3.0, 1.5 );
    std::normal_distribution<double> c3_d1( -4.0, 1.8 );

    auto data = std::vector<Point>();
    for( size_t i = 0; i < 10 * factor; ++i ) {
        auto p = Point({ c1_d0(e), c1_d1(e) });
        data.emplace_back( p );
    }
    for( size_t i = 0; i < 5 * factor; ++i ) {
        auto p = Point({ c2_d0(e), c2_d1(e) });
        data.emplace_back( p );
    }
    for( size_t i = 0; i < 20 * factor; ++i ) {
        auto p = Point({ c3_d0(e), c3_d1(e) });
        data.emplace_back( p );
    }

    // -------------------------------------------------------------------------
    //     Run Kmeans
    // -------------------------------------------------------------------------

    // LOG_DBG << "Run Kmeans with " << data.size() << " points";

    auto kmeans = EuclideanKmeans( 2 );
    kmeans.initialization_strategy( EuclideanKmeans::InitializationStrategy::kKmeansPlusPlus );
    kmeans.run( data, 3 );

    // -------------------------------------------------------------------------
    //     Show result
    // -------------------------------------------------------------------------

    // LOG_DBG << "Show result";

    return;

    auto doc = SvgDocument();
    auto const stroke = SvgStroke( SvgStroke::Type::kNone );

    for( size_t i = 0; i < data.size(); ++i ) {
        auto const a = kmeans.assignments()[i];

        Color c;
        switch( a ) {
            case 0: {
                c = Color( 255, 0, 0 );
                break;
            }
            case 1: {
                c = Color( 0, 255, 0 );
                break;
            }
            case 2: {
                c = Color( 0, 0, 255 );
                break;
            }
            default: {
                LOG_WARN << "Invalid assignment: " << a;
            }
        }

        doc << SvgCircle(
            data[i][0], data[i][1], 0.1, stroke, SvgFill( c, 0.5 )
        );
    }

    std::ostringstream out;
    doc.write( out );
    file_write( out.str(), "/home/lucas/test.svg" );
}
