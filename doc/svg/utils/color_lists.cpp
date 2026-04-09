/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include "genesis/util.hpp"

using namespace genesis;
using namespace genesis::util;
using namespace genesis::util::color;
using namespace genesis::util::format;

/**
 * @brief Tool to generate color lists in svg for all the lists that are available.
 */
int main( int argc, char** argv )
{
    // -------------------------------------------------------------------------
    //     Preparation
    // -------------------------------------------------------------------------

    (void) argc;
    (void) argv;

    // Activate logging.
    util::core::Logging::log_to_stdout();
    SvgDocument doc;
    size_t entry = 0;

    auto svgpal = SvgColorBarSettings();
    svgpal.width = 200;
    svgpal.height = 20;
    svgpal.with_tickmarks = false;
    svgpal.with_labels = false;
    svgpal.direction = SvgColorBarSettings::Direction::kLeftToRight;

    // -------------------------------------------------------------------------
    //     Sequential
    // -------------------------------------------------------------------------

    doc.add( SvgText( "Sequential", SvgPoint( 10, 30 * entry + 15 )));
    ++entry;

    for( auto const& listname : color_list_sequential_names() ) {
        auto const& list = color_list_sequential( listname );
        auto map = ColorMap( list );
        auto norm = ColorNormalizationLinear();

        // Palette
        auto sp = make_svg_color_bar( svgpal, map, norm, sanitize_svg_id( listname ));
        sp.second.transform.append( SvgTransform::Translate( 0, 30 * entry ) );
        doc.defs.push_back( sp.first );
        doc.add( sp.second );

        // Label
        doc.add( SvgText( listname, SvgPoint( 220, 30 * entry + 15 )));

        ++entry;
    }

    // -------------------------------------------------------------------------
    //     Diverging
    // -------------------------------------------------------------------------

    ++entry;
    doc.add( SvgText( "Diverging", SvgPoint( 10, 30 * entry + 15 )));
    ++entry;

    // svgpal.diverging_palette = true;
    for( auto const& listname : color_list_diverging_names() ) {
        auto const& list = color_list_diverging( listname );
        auto map = ColorMap( list );
        auto norm = ColorNormalizationLinear();

        // Palette
        auto sp = make_svg_color_bar( svgpal, map, norm, sanitize_svg_id( listname ));
        sp.second.transform.append( SvgTransform::Translate( 0, 30 * entry ) );
        doc.defs.push_back( sp.first );
        doc.add( sp.second );

        // Label
        doc.add( SvgText( listname, SvgPoint( 220, 30 * entry + 15 )));

        ++entry;
    }

    // -------------------------------------------------------------------------
    //     Qualitative
    // -------------------------------------------------------------------------

    ++entry;
    doc.add( SvgText( "Qualitative", SvgPoint( 10, 30 * entry + 15 )));
    ++entry;

    for( auto const& listname : color_list_qualitative_names() ) {
        auto const& list = color_list_qualitative( listname );

        // Single rects for each color.
        for( size_t i = 0; i < list.size(); ++i ) {
            doc.add( SvgRect(
                200.0 * static_cast<double>(i) / list.size(),
                30 * entry,
                200.0 / list.size(),
                20,
                SvgStroke( SvgStroke::Type::kNone ),
                SvgFill( list[i] )
            ));
        }

        // Box around and label
        doc.add( SvgRect(
            0.0, 30 * entry, 200, 20,
            SvgStroke(),
            SvgFill(SvgFill::Type::kNone)
        ));
        doc.add( SvgText( listname, SvgPoint( 220, 30 * entry + 15 )));

        ++entry;
    }


    // -------------------------------------------------------------------------
    //     Miscellaneous
    // -------------------------------------------------------------------------

    ++entry;
    doc.add( SvgText( "Miscellaneous", SvgPoint( 10, 30 * entry + 15 )));
    ++entry;

    for( auto const& listname : color_list_misc_names() ) {
        auto const& list = color_list_misc( listname );

        // Single rects for each color.
        for( size_t i = 0; i < list.size(); ++i ) {
            doc.add( SvgRect(
                200.0 * static_cast<double>(i) / list.size(),
                30 * entry,
                200.0 / list.size() + ( i < list.size() - 1 ? 1.0 : 0.0 ) ,
                20,
                SvgStroke( SvgStroke::Type::kNone ),
                SvgFill( list[i] )
            ));
        }

        // Box around and label
        doc.add( SvgRect(
            0.0, 30 * entry, 200, 20,
            SvgStroke(),
            SvgFill(SvgFill::Type::kNone)
        ));
        doc.add( SvgText( listname, SvgPoint( 220, 30 * entry + 15 )));

        ++entry;
    }

    // -------------------------------------------------------------------------
    //     Write File
    // -------------------------------------------------------------------------

    doc.margin = SvgMargin(10);
    std::ostringstream out;
    doc.write( out );
    util::core::file_write( out.str(), "color_lists.svg" );

    return 0;
}
