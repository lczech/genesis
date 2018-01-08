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

#include "genesis/utils/tools/color/qualitative_lists.hpp"

#include "genesis/utils/tools/color/functions.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <map>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Qualitative Color Lists
// =================================================================================================

/*
 * These ColorBrewer color palettes are adapted from https://github.com/axismaps/colorbrewer and
 * https://github.com/Gnuplotting/gnuplot-palettes by converting the colors to RGB `double` values.
 * The original ColorBrewer color specifications and designs were developed by Cynthia Brewer
 * (http://colorbrewer.org/), while their gnuplot equivalets are authored by Anna Schneider,
 * and published under the [Apache-2.0 license](http://www.apache.org/licenses/LICENSE-2.0).
 * See also our @link supplement_acknowledgements_code_reuse_colorbrewer Acknowledgements@endlink.
 */

const std::vector<Color> color_list_set1_ = {{
    { 0.894118, 0.101961, 0.109804 }, // red
    { 0.215686, 0.494118, 0.721569 }, // blue
    { 0.301961, 0.686275, 0.290196 }, // green
    { 0.596078, 0.305882, 0.639216 }, // purple
    { 1.000000, 0.498039, 0.000000 }, // orange
    { 1.000000, 1.000000, 0.200000 }, // yellow
    { 0.650980, 0.337255, 0.156863 }, // brown
    { 0.968627, 0.505882, 0.749020 }  // pink
}};

const std::vector<Color> color_list_set2_ = {{
    { 0.400000, 0.760784, 0.647059 }, // teal
    { 0.988235, 0.552941, 0.384314 }, // orange
    { 0.552941, 0.627451, 0.796078 }, // lilac
    { 0.905882, 0.541176, 0.764706 }, // magenta
    { 0.650980, 0.847059, 0.329412 }, // lime green
    { 1.000000, 0.850980, 0.184314 }, // banana
    { 0.898039, 0.768627, 0.580392 }, // tan
    { 0.701961, 0.701961, 0.701961 }  // grey
}};

const std::vector<Color> color_list_set3_ = {{
    { 0.552941, 0.827451, 0.780392 }, // teal
    { 1.000000, 1.000000, 0.701961 }, // banana
    { 0.745098, 0.729412, 0.854902 }, // lilac
    { 0.984314, 0.501961, 0.447059 }, // red
    { 0.501961, 0.694118, 0.827451 }, // steel blue
    { 0.992157, 0.705882, 0.384314 }, // adobe orange
    { 0.701961, 0.870588, 0.411765 }, // lime green
    { 0.988235, 0.803922, 0.898039 }  // mauve
}};

// =================================================================================================
//     Color Lists Functions
// =================================================================================================

std::vector<Color> const& color_list_set1()
{
    return color_list_set1_;
}

std::vector<Color> const& color_list_set2()
{
    return color_list_set2_;
}

std::vector<Color> const& color_list_set3()
{
    return color_list_set3_;
}

// =================================================================================================
//     Convenience Functions
// =================================================================================================

std::vector<Color> const& qualitative_color_list( QualitativeColorList palette )
{
    if( palette == QualitativeColorList::kSet1 ) {
        return color_list_set1_;
    }
    if( palette == QualitativeColorList::kSet2 ) {
        return color_list_set2_;
    }
    if( palette == QualitativeColorList::kSet3 ) {
        return color_list_set3_;
    }

    throw std::invalid_argument( "Invalid QualitativeColorList value." );
}

std::vector<Color> const& qualitative_color_list( std::string const& palette )
{
    auto const p = to_lower_ascii( palette );

    if( p == "set1" ) {
        return color_list_set1_;
    }
    if( p == "set2" ) {
        return color_list_set2_;
    }
    if( p == "set3" ) {
        return color_list_set3_;
    }

    throw std::invalid_argument( "Invalid QualitativeColorList name: '" + palette + "'." );
}

} // namespace utils
} // namespace genesis
