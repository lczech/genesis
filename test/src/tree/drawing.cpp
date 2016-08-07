/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include "common.hpp"

#include "lib/tree/default/newick_reader.hpp"
#include "lib/tree/drawing/rectangular_layout.hpp"
#include "lib/tree/tree.hpp"

#include "lib/utils/formats/svg/svg.hpp"
#include "lib/utils/core/fs.hpp"
#include "lib/utils/tools/color/names.hpp"

using namespace genesis;
using namespace tree;

TEST(Tree, Drawing)
{
    std::string input = "((A,(B,C)D)E,((F,(G,H)I)J,K)L)R;";

    Tree tree;
    EXPECT_TRUE( DefaultTreeNewickReader().from_string( input, tree ));

    auto layout = RectangularLayout();

    layout.from_tree( tree );

    std::vector<std::string> scheme = {
        "Crimson",
        "DarkCyan",
        "DarkGoldenRod",
        "DarkGreen",
        "DarkOrchid",
        "DeepPink",
        "DodgerBlue",
        "DimGray",
        "GreenYellow",
        "Indigo",
        "MediumVioletRed",
        "MidnightBlue",
        "Olive",
        "Orange",
        "OrangeRed",
        "Peru",
        "Purple",
        "SeaGreen",
        "DeepSkyBlue",
        "RoyalBlue",
        "SlateBlue",
        "Tomato",
        "YellowGreen"
    };

    std::vector<utils::SvgStroke> strokes;
    for( size_t i = 0; i < 12; ++i ) {
        strokes.push_back( utils::SvgStroke() );
        strokes.back().color = utils::get_named_color( scheme[i] );
    }
    layout.set_edge_strokes( strokes );

    std::ostringstream out;
    layout.to_svg_document().write( out );

    // LOG_DBG << out.str();
    // utils::file_write( out.str(), "/home/lucas/tree.svg" );

}
