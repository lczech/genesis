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
#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>

#include "genesis/population/tools/af_spectrum.hpp"
#include "genesis/population/formats/vcf_input_iterator.hpp"

#include "genesis/utils/formats/bmp/writer.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

AlleleFrequencyWindow::AlleleFrequencyWindow( size_t width, size_t number_of_bins )
    : number_of_bins_( number_of_bins )
    , window_( AFWindow::Type::kInterval, width )
{
    // Set the plugin functions.
    window_.on_chromosome_start = [&]( std::string const& chromosome, AFWindow::Accumulator& accu ){
        on_chromosome_start_( chromosome, accu );
    };
    window_.on_chromosome_finish = [&]( std::string const& chromosome, AFWindow::Accumulator& accu ){
        on_chromosome_finish_( chromosome, accu );
    };
    window_.on_emission = [&](
        size_t first_position, size_t last_position, size_t reported_position,
        AFWindow::const_iterator begin, AFWindow::const_iterator end, AFWindow::Accumulator& accu
    ){
        on_emission_( first_position, last_position, reported_position, begin, end, accu);
    };
}

AlleleFrequencyWindow::~AlleleFrequencyWindow()
{
    window_.finish_chromosome();
}

// =================================================================================================
//     Window Processing
// =================================================================================================

void AlleleFrequencyWindow::run_vcf( std::string const& vcf_file )
{
    for( auto record = VcfInputIterator( vcf_file ); record; ++record ) {
        enqueue( *record );
    }
}

void AlleleFrequencyWindow::enqueue(
    std::string const& chromosome, size_t position, double frequency
) {
    if( ! std::isfinite(frequency) || frequency < 0.0 || frequency > 1.0 ) {
        throw std::invalid_argument(
            "Invalid allele frequency " + std::to_string(frequency) + " at " +
            chromosome + ":" + std::to_string( position )
        );
    }
    window_.enqueue( chromosome, position, frequency );
}

void AlleleFrequencyWindow::enqueue( VcfRecord const& record )
{
    // Check that the record is one that we can use, and either skip or throw if not.
    if( ! record.is_snp() || record.get_alternatives_count() != 1 || ! record.has_format( "AD" ) ) {
        if( skip_invalid_records_ ) {
            return;
        } else {
            throw std::runtime_error(
                "Invalid VCF Record for Allele Frequency Window that is either not a biallelic SNP "
                "or does not have the FORMAT field `AD`."
            );
        }
    }

    // Sum up all allelic depth values for all samples of the record line.
    size_t ref = 0;
    size_t alt = 0;
    for( auto& ad_field : record.get_format_int("AD") ) {
        if( ad_field.values_per_sample() != 2 ) {
            throw std::runtime_error(
                "Invalid VCF Record that claims to be biallelic, but in fact contains more than "
                "two values for the FORMAT field `AD` for a sample."
            );
        }

        ref += ad_field.get_value_at(0);
        alt += ad_field.get_value_at(1);
    }

    // Compute the allele frequency based on the counts, and store them in the window.
    // Here, we simply skip invalid values, which can happen if all AD fields are zero.
    // Rare, so we don't want to fail here. Simply ignore them.
    double freq = static_cast<double>(alt) / static_cast<double>( alt + ref );
    if( ! std::isfinite(freq) ) {
        return;
    }
    window_.enqueue( record.get_chromosome(), record.get_position(), freq );

    // The AF field just counts frequencies based on the called genotype data (e.g. `0|1`),
    // but does not take the actual frequencies / allelic depths into account.
    // Hence, we only keep the following code for reference, in case that this information
    // is needed at some point.
    // auto const af = record.get_info_float("AF");
    // if( af.size() != 1 ) {
    //     throw std::runtime_error(
    //         "Invalid allele frequency (`AF`) field in VCF record at " + record.at() + " with size" +
    //         std::to_string( af.size() ) + " instead of expected size 1."
    //     );
    // }
    // window_.enqueue( record.get_chromosome(), record.get_position(), af[0] );
}

// =================================================================================================
//     Internal Members
// =================================================================================================

void AlleleFrequencyWindow::on_chromosome_start_(
    std::string const& chromosome,
    AFWindow::Accumulator&
) {
    spectra_.emplace_back( chromosome );
    assert( ! spectra_.empty() );
    assert( spectra_.back().chromosome == chromosome );

    // Not sure how that function might be handy, but let's offer it anyways.
    if( on_chromosome_start ) {
        on_chromosome_start( spectra_.back() );
    }
}

void AlleleFrequencyWindow::on_chromosome_finish_(
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

void AlleleFrequencyWindow::on_emission_(
    size_t, size_t, size_t,
    AFWindow::const_iterator begin, AFWindow::const_iterator end, AFWindow::Accumulator&
) {
    assert( ! spectra_.empty() );
    auto& values = spectra_.back().values;
    values.emplace_back( number_of_bins_, 0 );
    auto& bins = values.back();

    // Collect all data from the window and fill the frequency bins.
    for( auto it = begin; it != end; ++it ) {
        if( ! std::isfinite(it->data) || it->data < 0.0 || it->data > 1.0 ) {
            throw std::invalid_argument(
                "Invalid allele frequency " + std::to_string(it->data) + " at " +
                spectra_.back().chromosome + ":" + std::to_string( it->position )
            );
        }

        // Bring the value into the bins and store it. We need a special case for the exact value
        // of 1.0 here, so that we don't get an overflow of the bin index.
        size_t index = std::floor( it->data * static_cast<double>( number_of_bins_ ));
        index = std::min( index, number_of_bins_ - 1 );
        ++bins[ index ];
    }
}

// =================================================================================================
//     Allele Frequency Heatmap
// =================================================================================================

std::pair<utils::Matrix<utils::Color>, size_t> AlleleFrequencySpectrumHeatmap::spectrum_to_image(
    Spectrum const& spectrum
) const {
    using namespace utils;

    // Check.
    if( color_map_.empty() ) {
        throw std::runtime_error(
            "ColorMap has to be assigned a palette before using AlleleFrequencySpectrumHeatmap."
        );
    }

    // Edge case.
    if( spectrum.values.empty() ) {
        return { {}, 0 };
    }

    // Get the row size that we need. We later also confirm that this is consistent
    // across all spetra, to make sure that the data is actually a matrix/image.
    assert( ! spectrum.values.empty() );
    size_t rows = spectrum.values[0].size();

    // We need two passes through the data: first, find the max entry, then convert to scale.
    // While doing the first pass, make sure that the data is actually a matrix.
    size_t abs_max = 0;
    for( auto const& col : spectrum.values ) {
        if( col.size() != rows ) {
            throw std::runtime_error(
                "Invalid allele frequency spectrum with inconsistent number of rows."
            );
        }
        for( auto const& val : col ) {
            abs_max = std::max( abs_max, val );
        }
    }

    // Now convert to color values.
    auto image = Matrix<Color>( rows, spectrum.values.size() );
    for( size_t c = 0; c < spectrum.values.size(); ++c ) {
        auto const& col = spectrum.values[c];
        assert( col.size() == rows );

        // Get the max value of the current column.
        size_t col_max = 0;
        for( auto const& val : col ) {
            col_max = std::max( col_max, val );
        }

        // Get the max value that we want to use for normalization.
        double const used_max = static_cast<double>( normalize_per_column_ ? col_max : abs_max );

        // Do the actual per-bin convertion to color.
        for( size_t r = 0; r < col.size(); ++r ) {
            assert( col[r] <= abs_max );
            assert( col[r] <= col_max );

            // Get the row where to write the color to.
            size_t row_idx = invert_vertically_ ? rows - r - 1 : r;
            assert( row_idx < image.rows() );

            // Special case: no bin filled at all in this window. That means, there were no variants
            // in the whole window. If needed, mark with special "empty" color, which we
            // stored in the mask color of the color map.
            if( col_max == 0 && use_empty_window_color_ ) {
                image( row_idx, c ) = color_map_( std::numeric_limits<double>::quiet_NaN() );
                continue;
            }

            // Set the color for the current pixel.
            if( log_scale_ ) {
                // Special case for log scaling: If either the pixel value or the total max
                // for the colum is 1 or below, we cannot use log scaling for them (as we are working
                // with integer number counts here), so we simply use the min value instead.
                if( col[r] <= 1 || used_max <= 1 ) {
                    image( row_idx, c ) = color_map_( 0.0 );
                } else {
                    double frac = std::log( static_cast<double>( col[r] )) / std::log( used_max );
                    image( row_idx, c ) = color_map_( frac );
                }
            } else {
                double frac = static_cast<double>( col[r] ) / used_max;
                image( row_idx, c ) = color_map_( frac );
            }
        }
    }

    // Return the image and the appropriate max value used for the color scaling.
    return { image, normalize_per_column_ ? 1 : abs_max };
}

std::pair<utils::SvgGroup, size_t> AlleleFrequencySpectrumHeatmap::spectrum_to_svg(
    Spectrum const& spectrum,
    utils::SvgMatrixSettings settings
) const {
    // Generate the pixel color image matrix.
    auto const spec_img_and_max = spectrum_to_image( spectrum );

    // Return the svg group and the max value here.
    return { make_svg_matrix( spec_img_and_max.first, settings ), spec_img_and_max.second };
}

size_t AlleleFrequencySpectrumHeatmap::spectrum_to_bmp_file(
    AlleleFrequencySpectrumHeatmap::Spectrum const& spectrum,
    std::shared_ptr<utils::BaseOutputTarget> target
) const {

    // Generate the pixel color image matrix, and write the image to file.
    auto const spec_img_and_max = spectrum_to_image( spectrum );
    utils::BmpWriter().write( spec_img_and_max.first, target );

    // Return only the max value here.
    return spec_img_and_max.second;
}

} // namespace population
} // namespace genesis
