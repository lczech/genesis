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
    // t.overshoot_at_max = true;
    t.undershoot_at_min = true;
    // t.include_max = false;

    auto mins = std::vector<double>({ 0.0, 1, 5.2, 13.9 });
    auto maxs = std::vector<double>({ 1.0, 3.9, 7.5, 17.2 });

    for( auto min : mins ) {
        for( auto max : maxs ) {
            if( min >= max ) {
                continue;
            }

            auto list = t.linear_ticks( min, max, 8 );

            std::string ticks;
            for( auto l : list ) {
                ticks += "\t" + to_string_rounded( l );
            }
            LOG_DBG << "min " << min << ", \tmax " << max << ",  \tticks " << ticks;
        }
    }
    LOG_DBG;

    for( auto min : mins ) {
        for( auto max : maxs ) {
            if( min >= max ) {
                continue;
            }

            auto list = t.linear_labels( min, max, 8 );

            std::string ticks;
            for( auto l : list ) {
                ticks += "\t" + to_string_rounded( l.relative_position ) + " --> " + to_string_rounded( l.label );
            }
            LOG_DBG << "min " << min << ", \tmax " << max << ",  \tticks " << ticks;
        }
    }
    LOG_DBG;

    auto logs = std::vector<double>({ 1, 5.2, 13.9, 350, 44234 });
    for( auto ll : logs ) {
        auto list = t.logarithmic_labels( ll );

        std::string ticks;
        for( auto l : list ) {
            ticks += "\t" + to_string_rounded( l.relative_position ) + " --> " + to_string_rounded( l.label );
        }
        LOG_DBG << "max " << ll << ",\tticks " << ticks;
    }
}
*/
