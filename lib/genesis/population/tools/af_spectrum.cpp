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

#include <algorithm>
#include <cassert>
#include <functional>
#include <stdexcept>

#include "genesis/population/tools/af_spectrum.hpp"

#include "genesis/utils/containers/matrix.hpp"
#include "genesis/utils/formats/bmp/writer.hpp"
#include "genesis/utils/tools/color.hpp"

#include "genesis/utils/core/logging.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

AlleleFrequencySpectrum::AlleleFrequencySpectrum( size_t width, size_t bins )
    : bins_( bins )
    , window_( AFWindow::Type::kInterval, width )
{
    // Set the plugin functions.
    window_.on_chromosome_start = [&]( std::string const& chromosome, AFWindow::Accumulator& accu ){
        on_chromosome_start_( chromosome, accu );
    };
    window_.on_chromosome_end = [&]( std::string const& chromosome, AFWindow::Accumulator& accu ){
        on_chromosome_end_( chromosome, accu );
    };
    window_.on_emission = [&](
        size_t first_position, size_t last_position, size_t reported_position,
        AFWindow::const_iterator begin, AFWindow::const_iterator end, AFWindow::Accumulator& accu
    ){
        on_emission_( first_position, last_position, reported_position, begin, end, accu);
    };
}

AlleleFrequencySpectrum::~AlleleFrequencySpectrum()
{
    window_.finish_chromosome();
}

// =================================================================================================
//     Modifiers
// =================================================================================================

void AlleleFrequencySpectrum::enqueue( std::string const& chromosome, size_t position, double frequency )
{
    window_.enqueue( chromosome, position, frequency );
}

void AlleleFrequencySpectrum::enqueue( VcfRecord const& record )
{
    auto const af = record.get_info_float("AF");
    if( af.size() != 1 ) {
        throw std::runtime_error(
            "Invalid allele frequency (`AF`) field in VCF record at " + record.at() + " with size" +
            std::to_string( af.size() ) + " instead of expected size 1."
        );
    }
    window_.enqueue( record.get_chromosome(), record.get_position(), af[0] );
}

// =================================================================================================
//     Helper Functions
// =================================================================================================

size_t AlleleFrequencySpectrum::spectrum_to_bmp(
    Spectrum const& spectrum,
    std::shared_ptr<utils::BaseOutputTarget> target,
    bool log_scale
) {
    return spectrum_to_bmp( spectrum, {}, target, log_scale );
}

size_t AlleleFrequencySpectrum::spectrum_to_bmp(
    Spectrum const& spectrum,
    std::vector<utils::Color> const& palette,
    std::shared_ptr<utils::BaseOutputTarget> target,
    bool log_scale
) {
    // Edge case.
    if( spectrum.values.empty() ) {
        return 0;
    }
    assert( ! spectrum.values.empty() );
    size_t rows = spectrum.values[0].size();

    // We need two passes through the data: first, find the max entry, then convert to scale.
    // While doing so, make sure that the data is actually a matrix.
    size_t max = 0;
    for( auto const& col : spectrum.values ) {
        if( col.size() != rows ) {
            throw std::runtime_error(
                "Invalid allele frequency spectrum with inconsistent number of rows."
            );
        }
        for( auto const& val : col ) {
            max = std::max( max, val );
        }
    }

    // Now convert to byte values.
    static_assert( sizeof( unsigned char ) == 1, "Not working with full bytes." );
    auto image = utils::Matrix<unsigned char>( rows, spectrum.values.size() );
    for( size_t c = 0; c < spectrum.values.size(); ++c ) {
        auto const& col = spectrum.values[c];
        assert( col.size() == rows );
        for( size_t r = 0; r < col.size(); ++r ) {
            assert( col[r] <= max );

            if( log_scale ) {
                if( col[r] == 0 ) {
                    image(r, c) = 0;
                } else {
                    image(r, c) = 255 * std::log( col[r] ) / std::log( max );
                }
            } else {
                image(r, c) = 255 * col[r] / max;
            }
        }
    }

    // Finally, write to the target stream. We use two versions here, either just grayscale, or
    // with a palette of colors. The latter throws if the palette does not have exaclty 256 entries.
    if( palette.size() == 0 ) {
        utils::BmpWriter().write( image, target );
    } else {
        utils::BmpWriter().write( image, palette, target );
    }

    return max;
}

// =================================================================================================
//     Internal Members
// =================================================================================================

void AlleleFrequencySpectrum::on_chromosome_start_(
    std::string const& chromosome,
    AFWindow::Accumulator&
) {
    spectra_.emplace_back( chromosome );
}

void AlleleFrequencySpectrum::on_chromosome_end_(
    std::string const& chromosome,
    AFWindow::Accumulator&
) {
    assert( ! spectra_.empty() );
    assert( spectra_.back().chromosome == chromosome );

    // Make bitmap or whatever the user wants.
    if( on_chromosome_finish ) {
        on_chromosome_finish( spectra_.back() );
    }
}

void AlleleFrequencySpectrum::on_emission_(
    size_t, size_t, size_t,
    AFWindow::const_iterator begin, AFWindow::const_iterator end, AFWindow::Accumulator&
) {
    assert( ! spectra_.empty() );
    auto& values = spectra_.back().values;
    values.emplace_back( bins_, 0 );
    auto& bins = values.back();

    for( auto it = begin; it != end; ++it ) {
        if( ! std::isfinite(it->data) || it->data < 0.0 || it->data > 1.0 ) {
            throw std::invalid_argument(
                "Invalid allele frequency " + std::to_string(it->data) + " at " +
                spectra_.back().chromosome + ":" + std::to_string( it->position )
            );
        }
        size_t index = it->data * bins_;
        index = std::min( index, bins_ - 1 );
        ++bins[ index ];
    }
}

} // namespace population
} // namespace genesis
