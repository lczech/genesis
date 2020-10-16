#ifndef GENESIS_POPULATION_WINDOW_AF_SPECTRUM_H_
#define GENESIS_POPULATION_WINDOW_AF_SPECTRUM_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
 * @ingroup population
 */

#ifdef GENESIS_HTSLIB

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "genesis/population/formats/vcf_record.hpp"
#include "genesis/population/window/sliding_window_generator.hpp"
#include "genesis/population/window/window.hpp"

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/formats/svg/group.hpp"
#include "genesis/utils/formats/svg/matrix.hpp"
#include "genesis/utils/io/output_target.hpp"
#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/map.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Allele Frequency Window
// =================================================================================================

/**
 * @brief
 */
class AlleleFrequencyWindow
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using AFWindow = Window<double>;
    using self_type = AlleleFrequencyWindow;

    struct Spectrum
    {
        Spectrum( std::string const& chromosome )
            : chromosome(chromosome)
        {}

        std::string chromosome;
        std::vector<std::vector<size_t>> values;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    AlleleFrequencyWindow( size_t width, size_t number_of_bins = 100 );
    ~AlleleFrequencyWindow();

    AlleleFrequencyWindow( AlleleFrequencyWindow const& ) = default;
    AlleleFrequencyWindow( AlleleFrequencyWindow&& )      = default;

    AlleleFrequencyWindow& operator= ( AlleleFrequencyWindow const& ) = default;
    AlleleFrequencyWindow& operator= ( AlleleFrequencyWindow&& )      = default;

    // -------------------------------------------------------------------------
    //     Event Plugin Functionals
    // -------------------------------------------------------------------------

    std::function<void( Spectrum const& )> on_chromosome_start;
    std::function<void( Spectrum const& )> on_chromosome_finish;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    size_t number_of_bins() const
    {
        return number_of_bins_;
    }

    bool skip_invalid_records() const
    {
        return skip_invalid_records_;
    }

    self_type& skip_invalid_records( bool value )
    {
        skip_invalid_records_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Window Processing
    // -------------------------------------------------------------------------

    void run_vcf( std::string const& vcf_file );

    void enqueue( std::string const& chromosome, size_t position, double frequency );
    void enqueue( VcfRecord const& record );

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    void on_chromosome_start_( std::string const& chromosome, AFWindow::Accumulator& );

    void on_chromosome_finish_( std::string const& chromosome, AFWindow::Accumulator& );

    void on_emission_( AFWindow const& window );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    size_t number_of_bins_;
    bool skip_invalid_records_ = true;

    SlidingWindowGenerator<double> window_generator_;
    std::vector<Spectrum> spectra_;

};

// =================================================================================================
//     Allele Frequency Heatmap
// =================================================================================================

/**
 * @brief
 */
class AlleleFrequencySpectrumHeatmap
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using Spectrum  = AlleleFrequencyWindow::Spectrum;
    using self_type = AlleleFrequencySpectrumHeatmap;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    AlleleFrequencySpectrumHeatmap()  = default;
    ~AlleleFrequencySpectrumHeatmap() = default;

    AlleleFrequencySpectrumHeatmap( AlleleFrequencySpectrumHeatmap const& ) = default;
    AlleleFrequencySpectrumHeatmap( AlleleFrequencySpectrumHeatmap&& )      = default;

    AlleleFrequencySpectrumHeatmap& operator= ( AlleleFrequencySpectrumHeatmap const& ) = default;
    AlleleFrequencySpectrumHeatmap& operator= ( AlleleFrequencySpectrumHeatmap&& )      = default;

    // -------------------------------------------------------------------------
    //     Heatmap Functions
    // -------------------------------------------------------------------------

    std::pair<utils::Matrix<utils::Color>, size_t> spectrum_to_image(
        Spectrum const& spectrum
    ) const;

    std::pair<utils::SvgGroup, size_t> spectrum_to_svg(
        Spectrum const& spectrum,
        utils::SvgMatrixSettings settings = {}
    ) const;

    size_t spectrum_to_bmp_file(
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

    bool invert_vertically() const
    {
        return invert_vertically_;
    }

    self_type& invert_vertically( bool value )
    {
        invert_vertically_ = value;
        return *this;
    }

    bool normalize_per_column() const
    {
        return normalize_per_column_;
    }

    self_type& normalize_per_column( bool value )
    {
        normalize_per_column_ = value;
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

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    bool log_scale_ = false;
    bool invert_vertically_ = true;
    bool normalize_per_column_ = false;

    bool use_empty_window_color_ = true;
    utils::ColorMap color_map_;

};

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard
