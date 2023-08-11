#ifndef GENESIS_POPULATION_PLOTTING_GENOME_HEATMAP_H_
#define GENESIS_POPULATION_PLOTTING_GENOME_HEATMAP_H_

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

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/formats/svg/svg.hpp"
#include "genesis/utils/io/output_target.hpp"
#include "genesis/utils/color/color.hpp"
#include "genesis/utils/color/map.hpp"

#include <string>

namespace genesis {
namespace population {

// =================================================================================================
//     Drawing
// =================================================================================================

class GenomeHeatmap
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    enum class TextPosition
    {
        kTop,
        kBottom,
        kLeft,
        kRight
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    GenomeHeatmap() = default;
    ~GenomeHeatmap() = default;

    GenomeHeatmap( GenomeHeatmap const& ) = default;
    GenomeHeatmap( GenomeHeatmap&& )      = default;

    GenomeHeatmap& operator= ( GenomeHeatmap const& ) = default;
    GenomeHeatmap& operator= ( GenomeHeatmap&& )      = default;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    double horizontal_scaling() const
    {
        return h_scaling_;
    }

    GenomeHeatmap& horizontal_scaling( double value )
    {
        h_scaling_ = value;
        return *this;
    }

    double vertical_scaling() const
    {
        return v_scaling_;
    }

    GenomeHeatmap& vertical_scaling( double value )
    {
        v_scaling_ = value;
        return *this;
    }

    TextPosition text_position() const
    {
        return text_position_;
    }

    GenomeHeatmap& text_position( TextPosition value )
    {
        text_position_ = value;
        return *this;
    }

    void text_template( utils::SvgText const& tt );
    utils::SvgText& text_template();
    utils::SvgText const& text_template() const;

    // -------------------------------------------------------------------------
    //     Drawing
    // -------------------------------------------------------------------------

    void add(
        std::string const& label,
        utils::Matrix<utils::Color> const& heatmap,
        utils::SvgGroup x_axis = utils::SvgGroup{},
        utils::SvgGroup y_axis = utils::SvgGroup{},
        std::pair<utils::SvgGradientLinear, utils::SvgGroup> color_bar = std::pair<utils::SvgGradientLinear, utils::SvgGroup>{}
    );

    void write( std::shared_ptr<utils::BaseOutputTarget> target ) const;

    utils::SvgDocument const& document() const
    {
        return document_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    double h_scaling_ = 1.0;
    double v_scaling_ = 1.0;

    double current_y_ = 0.0;
    utils::SvgDocument document_;

    utils::SvgText text_template_ = utils::SvgText();
    TextPosition text_position_ = TextPosition::kTop;

};

} // namespace population
} // namespace genesis

#endif // include guard
