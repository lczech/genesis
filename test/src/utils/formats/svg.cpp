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

#include "lib/utils/core/fs.hpp"
#include "lib/utils/formats/svg/svg.hpp"

using namespace genesis::utils;

// -------------------------------------------------------------------------
//     SVG Drawing
// -------------------------------------------------------------------------

TEST( Svg, Basics )
{
    auto doc = SvgDocument();

    auto line = SvgLine( 0, 0, 100, 100 );
    line.stroke.color = Color(128, 192, 255);
    line.stroke.width = 3.0;
    doc.add( line );

    auto rect = SvgRect( 20, 20, 60, 60 );
    rect.stroke.color = Color( 192, 128, 0 );
    rect.fill.color = Color( 255, 192, 0 );
    rect.fill.opacity = 0.3;
    doc << rect;

    auto circle = SvgCircle( 80, 80, 10 );
    circle.stroke.color = Color( 128, 255, 0 );
    circle.fill.color = Color( 192, 255, 128);
    circle.fill.opacity = 0.5;
    doc << circle;

    auto ellipse = SvgEllipse( 80, 20, 20, 10 );
    ellipse.stroke.color = Color( 192, 128, 0 );
    ellipse.fill.color = Color( 255, 192, 128);
    ellipse.fill.opacity = 0.8;
    doc << ellipse;

    auto poly = SvgPolygon();
    poly.stroke.color = Color( 255, 192, 0 );
    poly.fill.color = Color( 255, 255, 0);
    poly.fill.opacity = 0.6;
    poly << SvgPoint( 40, 00 ) << SvgPoint( 50, 20 ) << SvgPoint( 70, 10 ) << SvgPoint( 60, 30 );
    poly << SvgPoint( 80, 40 ) << SvgPoint( 60, 50 ) << SvgPoint( 70, 70 ) << SvgPoint( 50, 60 );
    poly << SvgPoint( 40, 80 ) << SvgPoint( 30, 60 ) << SvgPoint( 10, 70 ) << SvgPoint( 20, 50 );
    poly << SvgPoint( 00, 40 ) << SvgPoint( 20, 30 ) << SvgPoint( 10, 10 ) << SvgPoint( 30, 20 );
    doc << poly;

    auto text = SvgText( SvgPoint( 20, 120 ), "Hello World! ygp", SvgFont( 15 ) );
    auto bb = text.bounding_box();
    doc << SvgRect( bb.top_left, bb.size(), SvgStroke( Color( 255, 128, 128 ) ), SvgFill( Color(), 0.0 ));
    doc << text;

    // doc << SvgLine( 20, 120, 20 + 12.0*10.0/2.0, 120 );
    // doc << SvgLine( 20, 120, 20, 120 - 10.0/1.2 );

    doc.margin = SvgMargin( 10, 30 );

    std::ostringstream out;
    doc.write( out );

    // LOG_DBG << out.str();
    // file_write( out.str(), "/home/lucas/test.svg" );
}
