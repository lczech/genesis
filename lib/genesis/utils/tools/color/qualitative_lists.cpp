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

const std::vector<Color> color_list_accent_ = {{
    { 0.498039, 0.788235, 0.498039 }, // pale green
    { 0.745098, 0.682353, 0.831373 }, // pale purple
    { 0.992157, 0.752941, 0.525490 }, // pale orange
    { 1.000000, 1.000000, 0.600000 }, // pale yellow
    { 0.219608, 0.423529, 0.690196 }, // blue
    { 0.941176, 0.007843, 0.498039 }, // magenta
    { 0.749020, 0.356863, 0.090196 }, // brown
    { 0.400000, 0.400000, 0.400000 }  // grey
}};

const std::vector<Color> color_list_dark2_ = {{
    { 0.105882, 0.619608, 0.466667 }, // dark teal
    { 0.850980, 0.372549, 0.007843 }, // dark orange
    { 0.458824, 0.439216, 0.701961 }, // dark lilac
    { 0.905882, 0.160784, 0.541176 }, // dark magenta
    { 0.400000, 0.650980, 0.117647 }, // dark lime green
    { 0.901961, 0.670588, 0.007843 }, // dark banana
    { 0.650980, 0.462745, 0.113725 }, // dark tan
    { 0.400000, 0.400000, 0.400000 }  // dark gray
}};

const std::vector<Color> color_list_paired_ = {{
    { 0.650980, 0.807843, 0.890196 }, // light blue
    { 0.121569, 0.470588, 0.705882 }, // dark blue
    { 0.698039, 0.874510, 0.541176 }, // light green
    { 0.200000, 0.627451, 0.172549 }, // dark green
    { 0.984314, 0.603922, 0.600000 }, // light red
    { 0.890196, 0.101961, 0.109804 }, // dark red
    { 0.992157, 0.749020, 0.435294 }, // light orange
    { 1.000000, 0.498039, 0.000000 }, // dark orange
    { 0.792157, 0.698039, 0.839216 }, // light purple
    { 0.415686, 0.239216, 0.603922 }, // dark purple
    { 1.000000, 1.000000, 0.600000 }, // light yellow
    { 0.694118, 0.349020, 0.156863 }  // brown
}};

const std::vector<Color> color_list_pastel1_ = {{
    { 0.984314, 0.705882, 0.682353 }, // pale red
    { 0.701961, 0.803922, 0.890196 }, // pale blue
    { 0.800000, 0.921569, 0.772549 }, // pale green
    { 0.870588, 0.796078, 0.894118 }, // pale purple
    { 0.996078, 0.850980, 0.650980 }, // pale orange
    { 1.000000, 1.000000, 0.800000 }, // pale yellow
    { 0.898039, 0.847059, 0.741176 }, // pale brown
    { 0.992157, 0.854902, 0.925490 }, // pale pink
    { 0.949020, 0.949020, 0.949020 }  // pale grey
}};

const std::vector<Color> color_list_pastel2_ = {{
    { 0.701961, 0.886275, 0.803922 }, // pale teal
    { 0.992157, 0.803922, 0.674510 }, // pale orange
    { 0.803922, 0.709804, 0.909804 }, // pale lilac
    { 0.956863, 0.792157, 0.894118 }, // pale magenta
    { 0.839216, 0.960784, 0.788235 }, // pale lime green
    { 1.000000, 0.949020, 0.682353 }, // pale banana
    { 0.945098, 0.886275, 0.800000 }, // pale tan
    { 0.800000, 0.800000, 0.800000 }  // pale grey
}};

const std::vector<Color> color_list_set1_ = {{
    { 0.894118, 0.101961, 0.109804 }, // red
    { 0.215686, 0.494118, 0.721569 }, // blue
    { 0.301961, 0.686275, 0.290196 }, // green
    { 0.596078, 0.305882, 0.639216 }, // purple
    { 1.000000, 0.498039, 0.000000 }, // orange
    { 1.000000, 1.000000, 0.200000 }, // yellow
    { 0.650980, 0.337255, 0.156863 }, // brown
    { 0.968627, 0.505882, 0.749020 }, // pink
    { 0.600000, 0.600000, 0.600000 }  // grey
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
    { 0.988235, 0.803922, 0.898039 }, // mauve
    { 0.850980, 0.850980, 0.850980 }, // grey
    { 0.737255, 0.501961, 0.741176 }, // purple
    { 0.800000, 0.921569, 0.772549 }, // green
    { 1.000000, 0.929412, 0.435294 }  // yellow
}};

// =================================================================================================
//     Color Lists Functions
// =================================================================================================

std::vector<Color> const& color_list_accent()
{
    return color_list_accent_;
}

std::vector<Color> const& color_list_dark2()
{
    return color_list_dark2_;
}

std::vector<Color> const& color_list_paired()
{
    return color_list_paired_;
}

std::vector<Color> const& color_list_pastel1()
{
    return color_list_pastel1_;
}

std::vector<Color> const& color_list_pastel2()
{
    return color_list_pastel2_;
}

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
    if( palette == QualitativeColorList::kAccent ) {
        return color_list_accent_;
    }
    if( palette == QualitativeColorList::kDark2 ) {
        return color_list_dark2_;
    }
    if( palette == QualitativeColorList::kPaired ) {
        return color_list_paired_;
    }
    if( palette == QualitativeColorList::kPastel1 ) {
        return color_list_pastel1_;
    }
    if( palette == QualitativeColorList::kPastel2 ) {
        return color_list_pastel1_;
    }
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

    if( p == "accent" ) {
        return color_list_accent_;
    }
    if( p == "dark2" ) {
        return color_list_dark2_;
    }
    if( p == "paired" ) {
        return color_list_paired_;
    }
    if( p == "pastel1" ) {
        return color_list_pastel1_;
    }
    if( p == "pastel2" ) {
        return color_list_pastel2_;
    }
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

std::vector<std::string> qualitative_color_list_names()
{
    return {
        "Accent",
        "Dark2",
        "Paired",
        "Pastel1",
        "Pastel2",
        "Set1",
        "Set2",
        "Set3"
    };
}

} // namespace utils
} // namespace genesis
