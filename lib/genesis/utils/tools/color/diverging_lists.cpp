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

const std::vector<Color> color_list_brbg_ = {{
    { 0.329412, 0.188235, 0.019608 },
    { 0.549020, 0.317647, 0.039216 }, // dark brown
    { 0.749020, 0.505882, 0.176471 }, // medium brown
    { 0.874510, 0.760784, 0.490196 },
    { 0.964706, 0.909804, 0.764706 }, // pale brown
    { 0.960784, 0.960784, 0.960784 },
    { 0.780392, 0.917647, 0.898039 }, // pale blue-green
    { 0.501961, 0.803922, 0.756863 },
    { 0.207843, 0.592157, 0.560784 }, // medium blue-green
    { 0.003922, 0.400000, 0.368627 }, // dark blue-green
    { 0.000000, 0.235294, 0.188235 }
}};

const std::vector<Color> color_list_piyg_ = {{
    { 0.556863, 0.003922, 0.321569 },
    { 0.772549, 0.105882, 0.490196 }, // dark pink
    { 0.870588, 0.466667, 0.682353 }, // medium pink
    { 0.945098, 0.713725, 0.854902 },
    { 0.992157, 0.878431, 0.937255 }, // pale pink
    { 0.968627, 0.968627, 0.968627 },
    { 0.901961, 0.960784, 0.815686 }, // pale yellow-green
    { 0.721569, 0.882353, 0.525490 },
    { 0.498039, 0.737255, 0.254902 }, // medium yellow-green
    { 0.301961, 0.572549, 0.129412 }, // dark yellow-green
    { 0.152941, 0.392157, 0.098039 }
}};

const std::vector<Color> color_list_prgn_ = {{
    { 0.250980, 0.000000, 0.294118 },
    { 0.462745, 0.164706, 0.513725 }, // dark purple
    { 0.600000, 0.439216, 0.670588 }, // medium purple
    { 0.760784, 0.647059, 0.811765 },
    { 0.905882, 0.831373, 0.909804 }, // pale purple
    { 0.968627, 0.968627, 0.968627 },
    { 0.850980, 0.941176, 0.827451 }, // pale green
    { 0.650980, 0.858824, 0.627451 },
    { 0.352941, 0.682353, 0.380392 }, // medium green
    { 0.105882, 0.470588, 0.215686 }, // dark green
    { 0.000000, 0.266667, 0.105882 }
}};

const std::vector<Color> color_list_puor_ = {{
    { 0.498039, 0.231373, 0.031373 },
    { 0.701961, 0.345098, 0.023529 }, // dark orange
    { 0.878431, 0.509804, 0.078431 }, // medium orange
    { 0.992157, 0.721569, 0.388235 },
    { 0.996078, 0.878431, 0.713725 }, // pale orange
    { 0.968627, 0.968627, 0.968627 },
    { 0.847059, 0.854902, 0.921569 }, // pale purple
    { 0.698039, 0.670588, 0.823529 },
    { 0.501961, 0.450980, 0.674510 }, // medium purple
    { 0.329412, 0.152941, 0.533333 }, // dark purple
    { 0.176471, 0.000000, 0.294118 }
}};

const std::vector<Color> color_list_rdbu_ = {{
    { 0.403922, 0.000000, 0.121569 },
    { 0.698039, 0.094118, 0.168627 }, // red
    { 0.839216, 0.376471, 0.301961 }, // red-orange
    { 0.956863, 0.647059, 0.509804 },
    { 0.992157, 0.858824, 0.780392 }, // pale orange
    { 0.968627, 0.968627, 0.968627 },
    { 0.819608, 0.898039, 0.941176 }, // pale blue
    { 0.572549, 0.772549, 0.870588 },
    { 0.262745, 0.576471, 0.764706 }, // medium blue
    { 0.129412, 0.400000, 0.674510 }, // dark blue
    { 0.019608, 0.188235, 0.380392 }
}};

const std::vector<Color> color_list_rdgy_ = {{
    { 0.403922, 0.000000, 0.121569 },
    { 0.698039, 0.094118, 0.168627 }, // red
    { 0.839216, 0.376471, 0.301961 }, // red-orange
    { 0.956863, 0.647059, 0.509804 },
    { 0.992157, 0.858824, 0.780392 }, // pale orange
    { 1.000000, 1.000000, 1.000000 },
    { 0.878431, 0.878431, 0.878431 }, // pale grey
    { 0.729412, 0.729412, 0.729412 },
    { 0.529412, 0.529412, 0.529412 }, // medium grey
    { 0.301961, 0.301961, 0.301961 }, // dark grey
    { 0.101961, 0.101961, 0.101961 }
}};

const std::vector<Color> color_list_rdylbu_ = {{
    { 0.647059, 0.000000, 0.149020 },
    { 0.843137, 0.188235, 0.152941 }, // red
    { 0.956863, 0.427451, 0.262745 }, // orange
    { 0.992157, 0.682353, 0.380392 },
    { 0.996078, 0.878431, 0.564706 }, // pale orange
    { 1.000000, 1.000000, 0.749020 },
    { 0.878431, 0.952941, 0.972549 }, // pale blue
    { 0.670588, 0.850980, 0.913725 },
    { 0.454902, 0.678431, 0.819608 }, // medium blue
    { 0.270588, 0.458824, 0.705882 }, // blue
    { 0.192157, 0.211765, 0.584314 }
}};

const std::vector<Color> color_list_rdylgn_ = {{
    { 0.647059, 0.000000, 0.149020 },
    { 0.843137, 0.188235, 0.152941 }, // red
    { 0.956863, 0.427451, 0.262745 }, // orange
    { 0.992157, 0.682353, 0.380392 }, // pale orange
    { 0.996078, 0.878431, 0.545098 }, // pale yellow-orange
    { 1.000000, 1.000000, 0.749020 },
    { 0.850980, 0.937255, 0.545098 }, // pale yellow-green
    { 0.650980, 0.850980, 0.415686 }, // pale green
    { 0.400000, 0.741176, 0.388235 }, // medium green
    { 0.101961, 0.596078, 0.313725 }, // green
    { 0.000000, 0.407843, 0.215686 }
}};

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

std::vector<Color> const& color_list_brbg()
{
    return color_list_brbg_;
}

std::vector<Color> const& color_list_piyg()
{
    return color_list_piyg_;
}

std::vector<Color> const& color_list_prgn()
{
    return color_list_prgn_;
}

std::vector<Color> const& color_list_puor()
{
    return color_list_puor_;
}

std::vector<Color> const& color_list_rdbu()
{
    return color_list_rdbu_;
}

std::vector<Color> const& color_list_rdgy()
{
    return color_list_rdgy_;
}

std::vector<Color> const& color_list_rdylbu()
{
    return color_list_rdylbu_;
}

std::vector<Color> const& color_list_rdylgn()
{
    return color_list_rdylgn_;
}

std::vector<Color> const& color_list_spectral()
{
    return color_list_spectral_;
}

// =================================================================================================
//     Convenience Functions
// =================================================================================================

std::vector<Color> const& diverging_color_list( DivergingColorList palette )
{
    if( palette == DivergingColorList::kBrbg ) {
        return color_list_brbg_;
    }
    if( palette == DivergingColorList::kPiyg ) {
        return color_list_piyg_;
    }
    if( palette == DivergingColorList::kPrgn ) {
        return color_list_prgn_;
    }
    if( palette == DivergingColorList::kPuor ) {
        return color_list_puor_;
    }
    if( palette == DivergingColorList::kRdbu ) {
        return color_list_rdbu_;
    }
    if( palette == DivergingColorList::kRdgy ) {
        return color_list_rdgy_;
    }
    if( palette == DivergingColorList::kRdylbu ) {
        return color_list_rdylbu_;
    }
    if( palette == DivergingColorList::kRdylgn ) {
        return color_list_rdylgn_;
    }
    if( palette == DivergingColorList::kSpectral ) {
        return color_list_spectral_;
    }

    throw std::invalid_argument( "Invalid DivergingColorList value." );
}

std::vector<Color> const& diverging_color_list( std::string const& palette )
{
    auto const p = to_lower_ascii( palette );

    if( p == "brbg" ) {
        return color_list_brbg_;
    }
    if( p == "piyg" ) {
        return color_list_piyg_;
    }
    if( p == "prgn" ) {
        return color_list_prgn_;
    }
    if( p == "puor" ) {
        return color_list_puor_;
    }
    if( p == "rdbu" ) {
        return color_list_rdbu_;
    }
    if( p == "rdgy" ) {
        return color_list_rdgy_;
    }
    if( p == "rdylbu" ) {
        return color_list_rdylbu_;
    }
    if( p == "rdylgn" ) {
        return color_list_rdylgn_;
    }
    if( p == "spectral" ) {
        return color_list_spectral_;
    }

    throw std::invalid_argument( "Invalid DivergingColorList name: '" + palette + "'." );
}

std::vector<std::string> diverging_color_list_names()
{
    return {
        "BrBg",
        "PiYg",
        "PrGn",
        "PuOr",
        "RdBu",
        "RdGy",
        "RdYlBu",
        "RdYlGn",
        "Spectral"
    };
}

} // namespace utils
} // namespace genesis
