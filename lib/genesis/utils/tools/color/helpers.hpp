#ifndef GENESIS_UTILS_TOOLS_COLOR_HELPERS_H_
#define GENESIS_UTILS_TOOLS_COLOR_HELPERS_H_

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

#include "genesis/utils/tools/color.hpp"

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class ColorMap;
class ColorNormalization;
class ColorNormalizationDiverging;
class ColorNormalizationLinear;
class ColorNormalizationLogarithmic;
class ColorNormalizationBoundary;

// =================================================================================================
//     Gradients
// =================================================================================================

std::map<double, Color> color_stops( ColorMap const& map, ColorNormalization const& norm );
std::map<double, Color> color_stops( ColorMap const& map, ColorNormalizationLinear const& norm );
std::map<double, Color> color_stops( ColorMap const& map, ColorNormalizationLogarithmic const& norm );
std::map<double, Color> color_stops( ColorMap const& map, ColorNormalizationDiverging const& norm );
std::map<double, Color> color_stops( ColorMap const& map, ColorNormalizationBoundary const& norm );

// =================================================================================================
//     Tickmarks
// =================================================================================================

std::map<double, std::string> color_tickmarks( ColorNormalization const& norm, size_t num_ticks );
std::map<double, std::string> color_tickmarks( ColorNormalizationLinear const& norm, size_t num_ticks );
std::map<double, std::string> color_tickmarks( ColorNormalizationLogarithmic const& norm, size_t num_ticks );
std::map<double, std::string> color_tickmarks( ColorNormalizationDiverging const& norm, size_t num_ticks );
std::map<double, std::string> color_tickmarks( ColorNormalizationBoundary const& norm, size_t num_ticks );

} // namespace utils
} // namespace genesis

#endif // include guard
