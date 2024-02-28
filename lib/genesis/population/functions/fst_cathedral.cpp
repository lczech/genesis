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

#include "genesis/population/functions/fst_cathedral.hpp"

#include "genesis/utils/formats/json/document.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Fst Cathedral Accumulator
// =================================================================================================

bool all_finite_( FstCathedralPlotRecord::Entry const& entry )
{
    auto const pi_w_finite = std::isfinite( entry.pi_within );
    auto const pi_b_finite = std::isfinite( entry.pi_between );
    auto const pi_t_finite = std::isfinite( entry.pi_total );
    return pi_w_finite && pi_b_finite && pi_t_finite;
}

void FstCathedralAccumulator::accumulate( FstCathedralPlotRecord::Entry const& entry )
{
    if( ! all_finite_( entry )) {
        return;
    }
    pi_within_sum_  += entry.pi_within;
    pi_between_sum_ += entry.pi_between;
    pi_total_sum_   += entry.pi_total;
    ++value_count_;
}

void FstCathedralAccumulator::dissipate( FstCathedralPlotRecord::Entry const& entry )
{
    // Boundary cases.
    if( ! all_finite_( entry )) {
        return;
    }
    if( value_count_ == 0 ) {
        throw std::runtime_error(
            "FstCathedralAccumulator: Cannot dissipate with value_count_==0"
        );
    }

    // Remove the values from the accumulator.
    pi_within_sum_  -= entry.pi_within;
    pi_between_sum_ -= entry.pi_between;
    pi_total_sum_   -= entry.pi_total;
    --value_count_;

    // Special case: all was removed. Even though we are using compensated summation,
    // this serves as a small additional hard reset for cases where we know the state.
    if( value_count_ == 0 ) {
        reset();
    }
}

double FstCathedralAccumulator::aggregate() const
{
    double result = 0.0;
    switch( fst_estimator_ ) {
        case FstPoolCalculatorUnbiased::Estimator::kNei: {
            result = 1.0 - ( pi_within_sum_ / pi_total_sum_ );
            break;
        }
        case FstPoolCalculatorUnbiased::Estimator::kHudson: {
            result = 1.0 - ( pi_within_sum_ / pi_between_sum_ );
            break;
        }
        default: {
            throw std::invalid_argument(
                "FstCathedralAccumulator: Invalid FstPoolCalculatorUnbiased::Estimator"
            );
        }
    }
    return result;
}

void FstCathedralAccumulator::reset()
{
    pi_within_sum_  = 0.0;
    pi_between_sum_ = 0.0;
    pi_total_sum_   = 0.0;
}

// =================================================================================================
//     Compute Data Functions
// =================================================================================================

std::vector<FstCathedralPlotRecord> prepare_fst_cathedral_records_for_chromosome_(
    std::string const& chromosome,
    FstPoolProcessor const& processor,
    FstPoolCalculatorUnbiased::Estimator fst_estimator,
    std::vector<std::string> const& sample_names
) {
    // Make as many fst data objects as we have pairs of samples in the processor.
    // Each item in the vector is one pair of samples, contaning their per-position partial fst data.
    // We cannot resize the value field inside the results, as we do not know how large the
    // chromosome is that we are using beforehand.
    std::vector<FstCathedralPlotRecord> result;
    result.resize( processor.size() );
    assert( processor.size() == processor.calculators().size() );

    // If sample names are given, use those to name the results.
    auto sample_name_pairs = fst_pool_processor_sample_names( processor, sample_names );
    assert( sample_name_pairs.empty() || sample_name_pairs.size() == processor.size() );

    // We are at some chromosome now. We want to iterate while on that chromosome.
    // We have a seq dict, so we know the length of the chromosome. First thought was to use that
    // to pre-allocate the value vectors, but that might waaay over-allocate, as likely non-snp
    // positions will be filtered out beforehand in the input iterator. So we don't do that.
    auto const fst_name = fst_pool_unbiased_estimator_to_string( fst_estimator );
    for( size_t i = 0; i < result.size(); ++i ) {
        auto& record = result[i];

        record.chromosome_name = chromosome;
        record.fst_estimator = fst_estimator;

        // If we have sample names, use them.
        if( !sample_name_pairs.empty() ) {
            assert( sample_name_pairs.size() == result.size() );
            record.sample_name_1 = std::move( sample_name_pairs[i].first );
            record.sample_name_2 = std::move( sample_name_pairs[i].second );
        }

        // We also make a title, for user convenience, to be used in the plot by default.
        // Could make that configurable, but good enough for now.
        record.title = "Fst (" + fst_name + ")";
        if( !record.sample_name_1.empty() && !record.sample_name_2.empty() ) {
            record.title += " " + record.sample_name_1 + " vs " + record.sample_name_2;
        }
        if( !record.chromosome_name.empty() ) {
            record.title += ", chromosome: " + record.chromosome_name;
        }
    }
    return result;
}

void fill_fst_cathedral_records_from_processor_(
    FstPoolProcessor const& processor,
    std::vector<FstCathedralPlotRecord>& records,
    size_t position
) {
    // We need to cast the calculators in the processor to get the correct derived type,
    // so that we can access the partial pi values from there.
    // Bit hacky, but good enough for now. Then, store the results.
    assert( processor.size() == records.size() );
    for( size_t i = 0; i < processor.size(); ++i ) {
        auto& raw_calc = processor.calculators()[i];
        auto fst_calc = dynamic_cast<FstPoolCalculatorUnbiased const*>( raw_calc.get() );
        if( ! fst_calc ) {
            throw std::runtime_error(
                "In compute_fst_cathedral_records_for_chromosome(): "
                "Invalid FstPoolCalculator that is not FstPoolCalculatorUnbiased"
            );
        }

        // Now add the entry for the current calculator to its respective records entry.
        // We rely on the amortized complexity here - cannot pre-allocate the size,
        // as we do not know how many positions will actually be in the input beforehand.
        records[i].entries.emplace_back(
            position,
            fst_calc->get_pi_within(),
            fst_calc->get_pi_between(),
            fst_calc->get_pi_total()
        );
    }
}

std::vector<FstCathedralPlotRecord> compute_fst_cathedral_records_for_chromosome(
    VariantInputStream::Iterator& iterator,
    FstPoolProcessor& processor,
    FstPoolCalculatorUnbiased::Estimator fst_estimator,
    std::vector<std::string> const& sample_names,
    std::shared_ptr<genesis::sequence::SequenceDict> const& sequence_dict
) {
    // Boundary check.
    if( ! iterator ) {
        return {};
    }

    // Prepare a vector of records, one for each fst calculator, with their respective meta-data.
    std::string const chromosome = iterator->chromosome;
    auto result = prepare_fst_cathedral_records_for_chromosome_(
        chromosome, processor, fst_estimator, sample_names
    );
    assert( result.size() == processor.size() );

    // Process all variants in the input as long as we are on the same chromosome,
    // and run them through the processor, storing all results in the result.
    size_t cur_pos = 0;
    while( iterator && iterator->chromosome == chromosome ) {

        // Process a single Variant, so reset at every step.
        auto const& variant = *iterator;
        processor.reset();
        processor.process( variant );

        // Make sure that we are in order. Otherwise the whole downstream approach fails.
        if( variant.position <= cur_pos ) {
            throw std::runtime_error(
                "Unsorted positions in input: On chromosome \"" + chromosome + "\", position " +
                std::to_string( variant.position ) + " follows position " +
                std::to_string( cur_pos ) + ", which is not in strict ordering."
            );
        }
        cur_pos = variant.position;

        // Create entries in the records of each processor, and move to next position.
        fill_fst_cathedral_records_from_processor_( processor, result, variant.position );
        ++iterator;
    }

    // If we have an entry for it in the seq dict, we use that as the total length,
    // so that downstream plots show the correct length. If not, we use what's in the data.
    if( sequence_dict && sequence_dict->contains( chromosome )) {
        cur_pos = sequence_dict->get( chromosome ).length;
    }
    for( auto& data : result ) {
        data.chromosome_length = cur_pos;
    }

    return result;
}

std::vector<FstCathedralPlotRecord> compute_fst_cathedral_records(
    VariantInputStream& iterator,
    FstPoolProcessor& processor,
    FstPoolCalculatorUnbiased::Estimator fst_estimator,
    std::vector<std::string> const& sample_names,
    std::shared_ptr<genesis::sequence::SequenceDict> const& sequence_dict
) {
    // We make one big result vector with all entries from all pairs of samples and chromosomes.
    std::vector<FstCathedralPlotRecord> result;

    // Start the iteration, process each chromosome, and move over the results.
    auto it = iterator.begin();
    while( it ) {
        auto chr_result = compute_fst_cathedral_records_for_chromosome(
            it, processor, fst_estimator, sample_names, sequence_dict
        );
        assert( chr_result.size() == processor.size() );

        // Move the data for one chromsome (for each pair of samples) to the result.
        result.reserve( result.size() + chr_result.size() );
        for( size_t i = 0; i < chr_result.size(); ++i ) {
            result.push_back( std::move( chr_result[i] ));
        }
    }
    assert( it == iterator.end() );
    return result;
}

// =================================================================================================
//     Storage Functions
// =================================================================================================

genesis::utils::JsonDocument fst_cathedral_plot_record_to_json_document(
    FstCathedralPlotRecord const& record
) {
    using namespace genesis::utils;

    // Get the base class fields. This also sets up the document.
    auto document = cathedral_plot_record_to_json_document( record );

    // We expect a top-level Json object, to be filled with our data.
    auto& obj = document.get_object();
    auto const fst_name = fst_pool_unbiased_estimator_to_string( record.fst_estimator );
    obj["sampleName1"]  = JsonDocument::string( record.sample_name_1 );
    obj["sampleName2"]  = JsonDocument::string( record.sample_name_2 );
    obj["fstEstimator"] = JsonDocument::string( fst_name );
    obj["entryCount"]   = JsonDocument::number_unsigned( record.entries.size() );

    return document;
}

} // namespace population
} // namespace genesis
