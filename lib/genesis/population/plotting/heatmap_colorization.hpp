#ifndef GENESIS_POPULATION_PLOTTING_HEATMAP_COLORIZATION_H_
#define GENESIS_POPULATION_PLOTTING_HEATMAP_COLORIZATION_H_

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
#include "genesis/utils/formats/svg/group.hpp"
#include "genesis/utils/formats/svg/matrix.hpp"
#include "genesis/utils/io/output_target.hpp"
#include "genesis/utils/color/color.hpp"
#include "genesis/utils/color/map.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Heatmap Colorization
// =================================================================================================

/**
 * @brief
 */
class HeatmapColorization
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = HeatmapColorization;

    struct Spectrum
    {
        Spectrum() = default;
        Spectrum( std::string const& chromosome )
            : chromosome(chromosome)
        {}

        std::string chromosome;
        std::vector<std::vector<double>> values;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    HeatmapColorization() = default;

    explicit HeatmapColorization( std::vector<utils::Color> const& palette )
    {
        color_map_.palette( palette );
    }

    ~HeatmapColorization() = default;

    HeatmapColorization( HeatmapColorization const& ) = default;
    HeatmapColorization( HeatmapColorization&& )      = default;

    HeatmapColorization& operator= ( HeatmapColorization const& ) = default;
    HeatmapColorization& operator= ( HeatmapColorization&& )      = default;

    // -------------------------------------------------------------------------
    //     Heatmap Functions
    // -------------------------------------------------------------------------

    std::pair<utils::Matrix<utils::Color>, double> spectrum_to_image(
        Spectrum const& spectrum
    ) const;

    std::pair<utils::SvgGroup, double> spectrum_to_svg(
        Spectrum const& spectrum,
        utils::SvgMatrixSettings settings = {}
    ) const;

    double spectrum_to_bmp_file(
        Spectrum const& spectrum,
        std::shared_ptr<utils::BaseOutputTarget> target
    ) const;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    bool log_scale() const
    {
        return log_scale_;
    }

    self_type& log_scale( bool value )
    {
        log_scale_ = value;
        return *this;
    }

    bool diverging_scale() const
    {
        return diverging_scale_;
    }

    self_type& diverging_scale( bool value )
    {
        diverging_scale_ = value;
        return *this;
    }

    bool invert_vertically() const
    {
        return invert_vertically_;
    }

    self_type& invert_vertically( bool value )
    {
        invert_vertically_ = value;
        return *this;
    }

    bool max_per_column() const
    {
        return max_per_column_;
    }

    self_type& max_per_column( bool value )
    {
        max_per_column_ = value;
        return *this;
    }

    utils::Color const& empty_window_color() const
    {
        return color_map_.mask_color();
    }

    self_type& empty_window_color( utils::Color const& value )
    {
        color_map_.mask_color( value );
        return *this;
    }

    bool use_empty_window_color() const
    {
        return use_empty_window_color_;
    }

    self_type& use_empty_window_color( bool value )
    {
        use_empty_window_color_ = value;
        return *this;
    }

    self_type& palette( std::vector<utils::Color> const& value )
    {
        color_map_.palette( value );
        return *this;
    }

    utils::ColorMap& color_map()
    {
        return color_map_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    bool log_scale_ = false;
    bool diverging_scale_ = false;
    bool invert_vertically_ = true;
    bool max_per_column_ = false;

    bool use_empty_window_color_ = true;
    utils::ColorMap color_map_;

};

} // namespace population
} // namespace genesis

#endif // include guard
