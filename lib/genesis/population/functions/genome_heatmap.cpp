/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/functions/genome_heatmap.hpp"

#include "genesis/utils/formats/bmp/writer.hpp"

#include <cassert>

namespace genesis {
namespace population {

// =================================================================================================
//     Genome Heatmap
// =================================================================================================

void GenomeHeatmap::add( std::string const& label, utils::Matrix<utils::Color> const& heatmap )
{
    using namespace utils;

    // Add the label, unless it's supposed to be at the bottom, in which case we add it later.
    switch( text_position_ ) {
        case TextPosition::kTop: {
            document_ << SvgText( label, SvgPoint{ 0.0, current_y_ });
            current_y_ += text_template_.font.size + 5.0;
            break;
        }
        case TextPosition::kLeft: {
            document_ << SvgText( label, SvgPoint{ -100.0, current_y_ });
            break;
        }
        case TextPosition::kRight: {
            auto const r = static_cast<double>( heatmap.cols() ) * h_scaling_;
            document_ << SvgText( label, SvgPoint{ r + 10.0, current_y_ });
            break;
        }
        case TextPosition::kBottom: {
            break;
        }
        default: {
            assert( false );
        }
    }

    // Add the heatmap as a bitmap, embedded by encoding it in base64, and using pixelated rendering
    // (which is not really well defined in svg, but we try hard).
    std::string matrix_bmp;
    BmpWriter().write( heatmap, genesis::utils::to_string( matrix_bmp ));
    auto img = SvgImage(
        svg_data_uri( "image/bmp", matrix_bmp, true ),
        SvgPoint{ 0.0, current_y_ },
        SvgSize{
            static_cast<double>( heatmap.cols() ) * h_scaling_,
            static_cast<double>( heatmap.rows() ) * v_scaling_
        }
    );
    img.rendering = SvgImage::ImageRendering::kPixelated;
    document_ << std::move( img );

    // Add the label if its supposed to be at the bottom, or simply add vertical space as needed.
    current_y_ += static_cast<double>( heatmap.rows() ) * v_scaling_;
    switch( text_position_ ) {
        case TextPosition::kBottom: {
            auto txt = text_template_;
            txt.text = label;
            current_y_ += text_template_.font.size + 5.0;
            txt.position = SvgPoint{ 0.0, current_y_ };
            document_ << txt;
            current_y_ += 20.0;
            break;
        }
        default: {
            current_y_ += 20.0;
            break;
        }
    }
}

void GenomeHeatmap::write( std::shared_ptr<utils::BaseOutputTarget> target ) const
{
    document_.write( target->ostream() );
}

} // namespace population
} // namespace genesis
