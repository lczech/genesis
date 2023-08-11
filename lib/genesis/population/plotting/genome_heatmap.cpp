/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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

#include "genesis/population/plotting/genome_heatmap.hpp"

#include "genesis/utils/formats/bmp/writer.hpp"

#include <cassert>

namespace genesis {
namespace population {

// =================================================================================================
//     Genome Heatmap
// =================================================================================================

void GenomeHeatmap::add(
    std::string const& label,
    utils::Matrix<utils::Color> const& heatmap,
    utils::SvgGroup x_axis,
    utils::SvgGroup y_axis,
    std::pair<utils::SvgGradientLinear, utils::SvgGroup> color_bar
) {
    using namespace utils;

    // Make a text, based on the template
    auto txt = text_template_;
    txt.text = label;

    // Add the label, unless it's supposed to be at the bottom, in which case we add it later.
    if( ! label.empty() ) {
        switch( text_position_ ) {
            case TextPosition::kTop: {
                txt.position = SvgPoint{ 0.0, current_y_ };
                document_ << txt;
                current_y_ += text_template_.font.size + 5.0;
                break;
            }
            case TextPosition::kLeft: {
                txt.position = SvgPoint{ -100.0, current_y_ };
                document_ << txt;
                break;
            }
            case TextPosition::kRight: {
                auto const r = static_cast<double>( heatmap.cols() ) * h_scaling_;
                txt.position = SvgPoint{ r + 10.0, current_y_ };
                document_ << txt;
                break;
            }
            case TextPosition::kBottom: {
                break;
            }
            default: {
                assert( false );
            }
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

    // If we have other objects at the same position provided, add them too, and move accordingly.
    if( color_bar.second ) {
        auto const xpos = 30.0 + static_cast<double>( heatmap.cols() ) * h_scaling_;
        color_bar.second.transform.append( SvgTransform::Translate( xpos, current_y_ ));
        document_ << std::move( color_bar.second );
        document_.defs.push_back( color_bar.first );
    }
    if( y_axis ) {
        auto const ypos = current_y_ + static_cast<double>( heatmap.rows() ) * v_scaling_;
        y_axis.transform.append( SvgTransform::Translate( 0.0, ypos ));
        document_ << std::move( y_axis );
    }
    if( x_axis ) {
        // We have added the other two extra groups to the doc first,
        // so that we can move the current y pos here without affecting them.
        auto const ypos = current_y_ + static_cast<double>( heatmap.rows() ) * v_scaling_;
        x_axis.transform.append( SvgTransform::Translate( 0.0, ypos ));
        current_y_ += x_axis.bounding_box().height();
        document_ << std::move( x_axis );
    }

    // Move to below the added image
    current_y_ += static_cast<double>( heatmap.rows() ) * v_scaling_;

    // Add the label if its supposed to be at the bottom, or simply add vertical space as needed.
    if( ! label.empty() && text_position_ == TextPosition::kBottom ) {
        current_y_ += text_template_.font.size + 5.0;
        txt.position = SvgPoint{ 0.0, current_y_ };
        document_ << txt;
    }
    current_y_ += 20.0;
}

void GenomeHeatmap::write( std::shared_ptr<utils::BaseOutputTarget> target ) const
{
    document_.write( target->ostream() );
}

} // namespace population
} // namespace genesis
