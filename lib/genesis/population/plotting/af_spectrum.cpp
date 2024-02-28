/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

#ifdef GENESIS_HTSLIB

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <limits>
#include <stdexcept>

#include "genesis/population/plotting/af_spectrum.hpp"
#include "genesis/population/formats/vcf_input_stream.hpp"

#include "genesis/utils/formats/bmp/writer.hpp"

namespace genesis {
namespace population {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

AlleleFrequencyWindow::AlleleFrequencyWindow( size_t width, size_t number_of_bins )
    : number_of_bins_( number_of_bins )
    , window_generator_( SlidingWindowType::kInterval, width )
{
    // Set the plugin functions.
    window_generator_.add_chromosome_start_plugin( [&]( std::string const& chromosome, AFWindow::Accumulator& accu ){
        on_chromosome_start_( chromosome, accu );
    });
    window_generator_.add_chromosome_finish_plugin( [&]( std::string const& chromosome, AFWindow::Accumulator& accu ){
        on_chromosome_finish_( chromosome, accu );
    });
    window_generator_.add_emission_plugin( [&](
        AFWindow const& window
    ){
        on_emission_( window );
    });
}

AlleleFrequencyWindow::~AlleleFrequencyWindow()
{
    window_generator_.finish_chromosome();
}

// =================================================================================================
//     Window Processing
// =================================================================================================

void AlleleFrequencyWindow::run_vcf( std::string const& vcf_file )
{
    for( auto record = VcfInputStream( vcf_file ); record; ++record ) {
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
    window_generator_.enqueue( chromosome, position, frequency );
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
    for( auto const& ad_field : record.get_format_int("AD") ) {
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
    window_generator_.enqueue( record.get_chromosome(), record.get_position(), freq );

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
    // window_generator_.enqueue( record.get_chromosome(), record.get_position(), af[0] );
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
    (void) chromosome;

    // Make bitmap or whatever the user wants.
    if( on_chromosome_finish ) {
        on_chromosome_finish( spectra_.back() );
    }
}

void AlleleFrequencyWindow::on_emission_(
    AFWindow const& window
) {
    assert( ! spectra_.empty() );
    auto& values = spectra_.back().values;
    values.emplace_back( number_of_bins_, 0 );
    auto& bins = values.back();

    // Collect all data from the window and fill the frequency bins.
    for( auto const& entry : window ) {
        if( ! std::isfinite(entry.data) || entry.data < 0.0 || entry.data > 1.0 ) {
            throw std::invalid_argument(
                "Invalid allele frequency " + std::to_string(entry.data) + " at " +
                spectra_.back().chromosome + ":" + std::to_string( entry.position )
            );
        }

        // Bring the value into the bins and store it. We need a special case for the exact value
        // of 1.0 here, so that we don't get an overflow of the bin index.
        size_t index = std::floor( entry.data * static_cast<double>( number_of_bins_ ));
        index = std::min( index, number_of_bins_ - 1 );
        ++bins[ index ];
    }
}

} // namespace population
} // namespace genesis

#endif // htslib guard
