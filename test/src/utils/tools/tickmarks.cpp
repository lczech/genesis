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
 * @brief Testing Color class.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/utils/tools/tickmarks.hpp"
#include "genesis/utils/text/string.hpp"

using namespace genesis;
using namespace utils;

/*
TEST( Tools, Tickmarks )
{
    Tickmarks t;
    t.undershoot_at_min = true;
    t.overshoot_at_max = true;
    // t.include_min = false;
    // t.include_max = false;

    auto mins = std::vector<double>({ 0.0, 1, 5.2, 13.9 });
    auto maxs = std::vector<double>({ 1.0, 3.9, 7.5, 17.2 });

    // for( auto min : mins ) {
    //     for( auto max : maxs ) {
    //         if( min >= max ) {
    //             continue;
    //         }
    //
    //         auto list = t.linear_ticks( min, max, 8 );
    //
    //         LOG_DBG << "min " << min << ", \tmax " << max;
    //         for( auto l : list ) {
    //             LOG_DBG1 << to_string_rounded( l );
    //         }
    //     }
    // }
    // LOG_DBG;

    for( auto min : mins ) {
        for( auto max : maxs ) {
            if( min >= max ) {
                continue;
            }

            auto list = t.linear_labels( min, max, 8 );

            LOG_DBG << "min " << min << ", \tmax " << max;
            for( auto l : list ) {
                LOG_DBG1 << to_string_rounded( l.relative_position ) + " --> " + to_string_rounded( l.label );
            }
        }
    }
    LOG_DBG;

    auto log_mins = std::vector<double>({ 0.001, 0.03, 1.0, 70.0, 340.0 });
    auto log_maxs = std::vector<double>({ 1, 5.2, 13.9, 10.0, 350, 44234 });
    for( auto min : log_mins ) {
        for( auto max : log_maxs ) {
            if( min >= max ) {
                continue;
            }

            auto list = t.logarithmic_labels( min, max , 2.0 );

            LOG_DBG << "min " << min << ", max " << max;
            for( auto l : list ) {
                LOG_DBG1 << to_string_rounded( l.relative_position ) + " --> " + to_string_rounded( l.label );
            }
        }
    }
}
*/
