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

#include "genesis/utils/tools/color/diverging_lists.hpp"

#include "genesis/utils/tools/color/functions.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <map>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Diverging Color Lists
// =================================================================================================

/*
 * These ColorBrewer color palettes are adapted from https://github.com/axismaps/colorbrewer and
 * https://github.com/Gnuplotting/gnuplot-palettes by converting the colors to RGB `double` values.
 * The original ColorBrewer color specifications and designs were developed by Cynthia Brewer
 * (http://colorbrewer.org/), while their gnuplot equivalets are authored by Anna Schneider,
 * and published under the [Apache-2.0 license](http://www.apache.org/licenses/LICENSE-2.0).
 * See also our @link supplement_acknowledgements_code_reuse_colorbrewer Acknowledgements@endlink.
 */

const std::vector<Color> color_list_spectral_ = {{
    { 0.61960784313725492, 0.003921568627450980, 0.25882352941176473 },
    { 0.83529411764705885, 0.24313725490196078 , 0.30980392156862746 },
    { 0.95686274509803926, 0.42745098039215684 , 0.2627450980392157  },
    { 0.99215686274509807, 0.68235294117647061 , 0.38039215686274508 },
    { 0.99607843137254903, 0.8784313725490196  , 0.54509803921568623 },
    { 1.0                , 1.0                 , 0.74901960784313726 },
    { 0.90196078431372551, 0.96078431372549022 , 0.59607843137254901 },
    { 0.6705882352941176 , 0.8666666666666667  , 0.64313725490196083 },
    { 0.4                , 0.76078431372549016 , 0.6470588235294118  },
    { 0.19607843137254902, 0.53333333333333333 , 0.74117647058823533 },
    { 0.36862745098039218, 0.30980392156862746 , 0.63529411764705879 }
}};

// const std::map<double, Color> color_list_spectral_ = {{
//     { -1.0, { 0.61960784313725492, 0.003921568627450980, 0.25882352941176473 }},
//     { -0.8, { 0.83529411764705885, 0.24313725490196078 , 0.30980392156862746 }},
//     { -0.6, { 0.95686274509803926, 0.42745098039215684 , 0.2627450980392157  }},
//     { -0.4, { 0.99215686274509807, 0.68235294117647061 , 0.38039215686274508 }},
//     { -0.2, { 0.99607843137254903, 0.8784313725490196  , 0.54509803921568623 }},
//     {  0.0, { 1.0                , 1.0                 , 0.74901960784313726 }},
//     {  0.2, { 0.90196078431372551, 0.96078431372549022 , 0.59607843137254901 }},
//     {  0.4, { 0.6705882352941176 , 0.8666666666666667  , 0.64313725490196083 }},
//     {  0.6, { 0.4                , 0.76078431372549016 , 0.6470588235294118  }},
//     {  0.8, { 0.19607843137254902, 0.53333333333333333 , 0.74117647058823533 }},
//     {  1.0, { 0.36862745098039218, 0.30980392156862746 , 0.63529411764705879 }}
// }};

// =================================================================================================
//     Color Lists Functions
// =================================================================================================

std::vector<Color> const& color_list_spectral()
{
    return color_list_spectral_;
}

// =================================================================================================
//     Convenience Functions
// =================================================================================================

std::vector<Color> const& diverging_color_list( DivergingColorList palette )
{
    if( palette == DivergingColorList::kSpectral ) {
        return color_list_spectral_;
    }

    throw std::invalid_argument( "Invalid DivergingColorList value." );
}

std::vector<Color> const& diverging_color_list( std::string const& palette )
{
    auto const p = to_lower_ascii( palette );

    if( p == "spectral" ) {
        return color_list_spectral_;
    }

    throw std::invalid_argument( "Invalid DivergingColorList name: '" + palette + "'." );
}

} // namespace utils
} // namespace genesis
