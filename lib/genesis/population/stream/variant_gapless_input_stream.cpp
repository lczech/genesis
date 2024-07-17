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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/stream/variant_gapless_input_stream.hpp"

#include "genesis/population/filter/sample_counts_filter.hpp"
#include "genesis/population/filter/variant_filter.hpp"
#include "genesis/population/function/functions.hpp"
#include "genesis/sequence/functions/codes.hpp"
#include "genesis/utils/text/char.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <numeric>
#include <stdexcept>

namespace genesis {
namespace population {

// =================================================================================================
//     Iterator Constructors and Rule of Five
// =================================================================================================

VariantGaplessInputStream::Iterator::Iterator( VariantGaplessInputStream* parent )
    : parent_(parent)
{
    // We use the parent as a check if this Iterator is intended to be a begin() or end()
    // iterator. We here check this to avoid accidentally starting an iteration without data.
    if( ! parent_ ) {
        return;
    }

    // Start the iteration, which already makes the first Variant ready in the input.
    // We don't need to store the end, as the iterator itself is able to tell us that.
    iterator_ = parent_->input_.begin();

    // We get the number of samples in the Variant to initialize the dummy Variant
    // for missing positions where this iterator does not have data.
    auto const sample_name_count = parent_->input_.data().sample_names.size();
    if( iterator_ ) {
        check_input_iterator_();
        num_samples_ = iterator_->samples.size();

        // We assume that the sample_names are of the correct size, if given.
        if( sample_name_count > 0 && iterator_->samples.size() != sample_name_count ) {
            throw std::runtime_error(
                "Input source for VariantGaplessInputStream contains " +
                std::to_string( iterator_->samples.size() ) + " samples, but its sample " +
                "name list contains " + std::to_string( sample_name_count ) + " names."
            );
        }

        // Now we want to start the iteration on the first chromosome where the iterator starts.
        current_locus_ = GenomeLocus( iterator_->chromosome, 1 );
    } else {
        // If we have no data in the input at all (for instance, because of some aggressive
        // filter settings), we use the sample names as an indicator for the number of dummy
        // samples to create. This might still be needed when we want to iterator genome
        // positions from the ref genome or sequence dict.
        num_samples_ = sample_name_count;

        // We have no actual input data. Still let's see if there are extra chromosomes we want.
        // We might not have anything, in which case we are done already.
        auto const chr = find_next_extra_chromosome_();
        if( chr.empty() ) {
            parent_ = nullptr;
            return;
        }
        current_locus_ = GenomeLocus( chr, 1 );
    }

    // If we are here, we have initialized the current locus to the first position on some valid
    // chromosome, and we can start the processing.
    assert( current_locus_.chromosome != "" && current_locus_.position != 0 );
    init_chromosome_();

    // We have just initialized the chrosome, including all cache pointer for the given references.
    // We now use that to check that the position where we started is actually covered by the
    // genome locus set filter. If not, we move on until we find a covered position.
    // This is inefficient, as we are looping over gaps and data that might be filtered out
    // immediately again. But it is good enough for now, and the use case is pretty special anyway.
    if( current_locus_is_covered_by_genome_locus_set_() ) {
        prepare_current_variant_();
    } else {
        // The advance function will loop until it finds a covered locus.
        advance_();
    }
}

// =================================================================================================
//     Iterator Internal Members
// =================================================================================================

// -------------------------------------------------------------------------
//     advance_
// -------------------------------------------------------------------------

void VariantGaplessInputStream::Iterator::advance_()
{
    // Some basic checks.
    assert( parent_ );

    // We need to loop until we find a locus that is actually covered by the genome locus set.
    // If that is not given anyway, this loop will immediately exit. But in case that we have
    // a genome locus set filter, for simplicty, we have implemented this as a loop here.
    // At least, we only do the work of preparing the variant at the very end, once we know
    // that we want to visit it.
    do {
        // Move the current_locus_, and potentially the input iterator,
        // to the next position we want to process.
        advance_current_locus_();

        // If there is no next position, we are done.
        if( current_locus_.empty() ) {
            parent_ = nullptr;
            return;
        }
        assert( current_locus_.chromosome != "" && current_locus_.position != 0 );

        // If the next position is the start of a chromosome,
        // we need to set it up correctly first.
        if( current_locus_.position == 1 ) {
            init_chromosome_();
        }
    } while( ! current_locus_is_covered_by_genome_locus_set_() );


    // Now we have everything to populate our variant as needed.
    prepare_current_variant_();
}

// -------------------------------------------------------------------------
//     init_chromosome_
// -------------------------------------------------------------------------

void VariantGaplessInputStream::Iterator::init_chromosome_()
{
    // Check that we are not done yet (parent still valid), and that we have either
    // a ref genome or a seq dict, but not both (neither is also fine).
    assert( parent_ );
    assert( !( parent_->ref_genome_ && parent_->seq_dict_ ));

    // Check that we are indeed at the beginning of a new chromosome.
    assert( current_locus_.chromosome != "" );
    assert( current_locus_.position == 1 );
    std::string const& chr = current_locus_.chromosome;

    // Check that we do not accidentally duplicate any chromosomes.
    if( processed_chromosomes_.count( chr ) > 0 ) {
        throw std::runtime_error(
            "In VariantGaplessInputStream: Chromosome \"" + chr + "\" occurs multiple times. "
            "Likely, this means that the input is not sorted by chromosome and position."
        );
    }
    processed_chromosomes_.insert( chr );

    // If we have a reference genome, set the cache value for fast finding of the sequence.
    if( parent_->ref_genome_ ) {
        ref_genome_it_ = parent_->ref_genome_->find( chr );
        if( ref_genome_it_ == parent_->ref_genome_->end() ) {
            throw std::runtime_error(
                "In VariantGaplessInputStream: Chromosome \"" + chr + "\" requested "
                "in the input data, which does not occur in the reference genome."
            );
        }
    }

    // Same for sequence dict.
    if( parent_->seq_dict_ ) {
        seq_dict_it_ = parent_->seq_dict_->find( chr );
        if( seq_dict_it_ == parent_->seq_dict_->end() ) {
            throw std::runtime_error(
                "In VariantGaplessInputStream: Chromosome \"" + chr + "\" requested "
                "in the input data, which does not occur in the sequence dictionary."
            );
        }
    }

    // Lastly, for the genome locus set, we do the same check, but slightly different:
    // The set might not contain chromosomes that are filtered out completely anyway,
    // and in that case we do not want to fail, but just indicate that be setting the
    // cache iterator to the end. That is done by the find function anyway,
    // so no further steps or checks are needed in that case.
    if( parent_->genome_locus_set_ ) {
        genome_locus_set_it_ = parent_->genome_locus_set_->find( chr );
    }
}

// -------------------------------------------------------------------------
//     advance_current_locus_
// -------------------------------------------------------------------------

void VariantGaplessInputStream::Iterator::advance_current_locus_()
{
    // If we have no more input data, we are processing positions (and potential extra chromosomes)
    // as provided by the ref genome or seq dict.
    if( !iterator_ ) {
        advance_current_locus_beyond_input_();
        return;
    }

    // If the input data is at exactly where we are in our iteration (i.e., there was data
    // for the current position), we need to advance the iterator. That could lead to its end,
    // in which case we do the same as above: See if there are positions beyond.
    // If this is not the case, the iterator is somewhere ahead of us here, and so we just leave
    // it there until we reach its position (in which case the condition here will then trigger).
    if( iterator_->chromosome == current_locus_.chromosome ) {
        assert( iterator_->position >= current_locus_.position );
        if( iterator_->position == current_locus_.position ) {
            ++iterator_;
            if( !iterator_ ) {
                advance_current_locus_beyond_input_();
                return;
            }
            check_input_iterator_();
        }
    }
    assert( iterator_ );

    // If the iterator still has data on the chromosome, or the ref genome or seq dict has,
    // we just move a position forward. We here do not care if the iterator actually has
    // data for that next position; this is checked when populating the data. All we need
    // to know here is that there will be some more data at some point on this chromosome.
    // If not, we start a new chromosome.
    if(
        iterator_->chromosome == current_locus_.chromosome ||
        has_more_ref_loci_on_current_chromosome_()
    ) {
        ++current_locus_.position;
    } else {
        current_locus_ = GenomeLocus( iterator_->chromosome, 1 );
    }
}

// -------------------------------------------------------------------------
//     advance_current_locus_beyond_input_
// -------------------------------------------------------------------------

void VariantGaplessInputStream::Iterator::advance_current_locus_beyond_input_()
{
    // Assumptions about the caller. We only get called when there is no more data in the iterator,
    // but we are not yet fully done with the ref genome or seq dict extra chromosomes.
    assert( parent_ );
    assert( !iterator_ );

    // We first check if the next incremental position is still valid according to the
    // ref genome or seq dict. If so, we just move to it and are done.
    if( has_more_ref_loci_on_current_chromosome_() ) {
        ++current_locus_.position;
        return;
    }

    // Once we are here, we have processed a chromosome and might want to move to the next.
    // As we are already beyond the input data when this function is called, this can only
    // mean that we want to check for extra chromosomes that are only in the ref genome or
    // seq dict, but not in the input data. Check if we want to do that at all.
    if( !parent_->iterate_extra_chromosomes_ ) {
        current_locus_.clear();
        return;
    }

    // If not, we reached the end of one extra chr and want to move to the next,
    // or if there isn't any, indicate to the caller that we are done.
    auto const next_chr = find_next_extra_chromosome_();
    if( next_chr.empty() ) {
        current_locus_.clear();
    } else {
        current_locus_.chromosome = next_chr;
        current_locus_.position = 1;
    }
}

// -------------------------------------------------------------------------
//     has_more_ref_loci_on_current_chromosome_
// -------------------------------------------------------------------------

bool VariantGaplessInputStream::Iterator::has_more_ref_loci_on_current_chromosome_()
{
    assert( parent_ );
    assert( !( parent_->ref_genome_ && parent_->seq_dict_ ));

    // Check if there is a next position on the chromosome that we are currently at.
    // Positions are 1-based, so we need smaller or equal comparison here.
    // If neither ref genome nor seq dict are given, we just return false.
    if( parent_->ref_genome_ ) {
        assert( ref_genome_it_ != parent_->ref_genome_->end() );
        assert( ref_genome_it_->label() == current_locus_.chromosome );
        if( current_locus_.position + 1 <= ref_genome_it_->size() ) {
            return true;
        }
    }
    if( parent_->seq_dict_ ) {
        assert( seq_dict_it_ != parent_->seq_dict_->end() );
        assert( seq_dict_it_->name == current_locus_.chromosome );
        if( current_locus_.position + 1 <= seq_dict_it_->length ) {
            return true;
        }
    }
    return false;
}

// -------------------------------------------------------------------------
//     find_next_extra_chromosome_
// -------------------------------------------------------------------------

std::string VariantGaplessInputStream::Iterator::find_next_extra_chromosome_()
{
    assert( parent_ );

    // We might not want to do extra chromosomes at all.
    if( !parent_->iterate_extra_chromosomes_ ) {
        return "";
    }

    // Check for extra ref genome chromosomes.
    if( parent_->ref_genome_ ) {
        // Check if there are any more chromosomes that we have not done yet.
        // During the nornal iteration with data, we always check that a chromosome that is found
        // in the data also is in the ref genome (or seq dict, same there below). So, when we reach
        // the end of the data, and then process the extra chromosomes here, we can be sure that
        // there are no chromosomes in the data that are not also in the ref genome - hence, the
        // size check here works to test completness of the chromosomes.
        if( processed_chromosomes_.size() == parent_->ref_genome_->size() ) {
            return "";
        }

        // If yes, find the next one.
        for( auto const& ref_chr : *parent_->ref_genome_ ) {
            if( ref_chr.label().empty() ) {
                throw std::runtime_error(
                    "Invalid empty chromosome name in reference genome."
                );
            }
            if( processed_chromosomes_.count( ref_chr.label() ) == 0 ) {
                return ref_chr.label();
            }
        }

        // We are guaranteed to find one, so this here cannot be reached.
        assert( false );
    }

    // Same for extra seq dict chromosomes.
    // Unfortunate code duplication due to the slightly different interfaces.
    if( parent_->seq_dict_ ) {
        // Check if there are any more chromosomes that we have not done yet.
        if( processed_chromosomes_.size() == parent_->seq_dict_->size() ) {
            return "";
        }

        // If yes, find the next one.
        for( auto const& entry : *parent_->seq_dict_ ) {
            if( entry.name.empty() ) {
                throw std::runtime_error(
                    "Invalid empty chromosome name in sequence dictionary."
                );
            }
            if( processed_chromosomes_.count( entry.name ) == 0 ) {
                return entry.name;
            }
        }

        // We are guaranteed to find one, so this here cannot be reached.
        assert( false );
    }

    // If neither is given, just return an empty string to indiate that we do not have
    // any extra chromosomes to process.
    return "";
}

// -------------------------------------------------------------------------
//     prepare_current_variant_
// -------------------------------------------------------------------------

void VariantGaplessInputStream::Iterator::prepare_current_variant_()
{
    // We expect to be at a valid current locus.
    assert( parent_ );
    assert( current_locus_.chromosome != "" && current_locus_.position != 0 );

    // Check that the current locus is valid according to the ref genome or seq dict.
    if( parent_->ref_genome_ ) {
        assert( ref_genome_it_ != parent_->ref_genome_->end() );
        assert( ref_genome_it_->label() == current_locus_.chromosome );

        // We use 1-based positions here, hence the greater-than comparison.
        if( current_locus_.position > ref_genome_it_->length() ) {
            throw std::runtime_error(
                "In VariantGaplessInputStream: Invalid input data, which has data "
                "beyond the reference genome at " + to_string( current_locus_ )
            );
        }
    }
    if( parent_->seq_dict_ ) {
        assert( seq_dict_it_ != parent_->seq_dict_->end() );
        assert( seq_dict_it_->name == current_locus_.chromosome );
        if( current_locus_.position > seq_dict_it_->length ) {
            throw std::runtime_error(
                "In VariantGaplessInputStream: Invalid input data, which has data "
                "beyond the reference genome at " + to_string( current_locus_ )
            );
        }
    }

    // Check if the current locus has data. If so, we point our data to the input data.
    // If not, we point to our internal "missing" variant dummy, and reset it from prev iterations.
    if( iterator_ && locus_equal( iterator_->chromosome, iterator_->position, current_locus_ )) {
        current_variant_is_missing_ = false;

        // Error check for consistent sample size.
        if( iterator_->samples.size() != num_samples_ ) {
            throw std::runtime_error(
                "In VariantGaplessInputStream: Invalid input data that has an inconsistent "
                "number of samples throughout, first occurring at " + to_string( current_locus_ ) +
                ". Expected " + std::to_string( num_samples_ ) +
                " samples based on first iteration, but found " +
                std::to_string( iterator_->samples.size() ) + " samples instead."
            );
        }
    } else {
        current_variant_is_missing_ = true;
        missing_variant_.chromosome = current_locus_.chromosome;
        missing_variant_.position   = current_locus_.position;
        missing_variant_.status.reset( VariantFilterTag::kMissing );
        missing_variant_.reference_base   = 'N';
        missing_variant_.alternative_base = 'N';

        // In case that the Variant is moved-from, we need to reset the sample size. In case it was
        // modified (by some filter or transformation), we also need to reset the counts.
        missing_variant_.samples.resize( num_samples_ );
        for( auto& sample : missing_variant_.samples ) {
            sample = SampleCounts();
            sample.status.reset( SampleCountsFilterTag::kMissing );
        }
    }

    prepare_current_variant_ref_base_();
}

// -------------------------------------------------------------------------
//     prepare_current_variant_ref_base_
// -------------------------------------------------------------------------

void VariantGaplessInputStream::Iterator::prepare_current_variant_ref_base_()
{
    // Shorthand. Points to either the missing variant or the input iterator variant.
    auto& cur_var = current_variant_();

    // This function expects current_variant to be set up for the locus already,
    // and only be missing the ref base setup.
    assert( parent_ );
    assert( !current_locus_.chromosome.empty() && current_locus_.position > 0 );
    assert( locus_equal(
        cur_var.chromosome, cur_var.position, current_locus_
    ));

    // If we have a ref genome, we use it to get or check the reference base.
    // If not, we are done.
    if( !parent_->ref_genome_ ) {
        return;
    }
    assert( ref_genome_it_ != parent_->ref_genome_->end() );
    assert( ref_genome_it_->label() == current_locus_.chromosome );

    // Get the reference base and check it against the Variant.
    // This throws an exception for mismatches; sets the ref and alt base if not
    // already done so; does nothing if already set in the Variant.
    // We use 1-based positions, but the ref genome is a simple sequence in string
    // format, so we need to offset by one here.
    assert(
        current_locus_.position > 0 &&
        current_locus_.position <= ref_genome_it_->length()
    );
    auto const ref_base = utils::to_upper( ref_genome_it_->site_at( current_locus_.position - 1 ));
    // guess_and_set_ref_and_alt_bases( cur_var, ref_base, true );
    if( is_valid_base( utils::to_upper( cur_var.reference_base ))) {
        bool contains = false;
        try {
            using genesis::sequence::nucleic_acid_code_containment;
            contains = nucleic_acid_code_containment(
                ref_base, utils::to_upper( cur_var.reference_base )
            );
        } catch(...) {
            // The above throws an error if the given bases are not valid.
            // Catch this, and re-throw a nicer, more understandable exception instead.
            throw std::runtime_error(
                "At chromosome \"" + current_locus_.chromosome + "\" position " +
                std::to_string( current_locus_.position ) + ", the reference genome has base '" +
                std::string( 1, ref_base ) + "', which is not a valid nucleic acid code"
            );
        }
        if( ! contains ) {
            throw std::runtime_error(
                "At chromosome \"" + current_locus_.chromosome + "\" position " +
                std::to_string( current_locus_.position ) +
                ", the reference base in the data is '" +
                std::string( 1, cur_var.reference_base ) + "'. " +
                "However, the reference genome has base '" + std::string( 1, ref_base ) +
                "', which does not code for that base, and hence likely points to "
                "some kind of mismatch"
            );
        }
    } else {
        cur_var.reference_base = utils::to_upper( ref_base );
    }
}

// -------------------------------------------------------------------------
//     check_input_iterator_
// -------------------------------------------------------------------------

void VariantGaplessInputStream::Iterator::check_input_iterator_()
{
    if( iterator_->chromosome.empty() || iterator_->position == 0 ) {
        throw std::runtime_error(
            "In VariantGaplessInputStream: Invalid position "
            "with empty chromosome name or zero position."
        );
    }
}

// -------------------------------------------------------------------------
//     current_locus_is_covered_by_genome_locus_set_
// -------------------------------------------------------------------------

bool VariantGaplessInputStream::Iterator::current_locus_is_covered_by_genome_locus_set_()
{
    assert( parent_ );
    assert( current_locus_.chromosome != "" );
    assert( current_locus_.position > 0 );

    // Without a given genome locus set, we always consider this position to be covered.
    if( ! parent_->genome_locus_set_ ) {
        return true;
    }

    // If we are here, we have a genome locus set filter given.
    // If the chromosome is not in the set, that means that the position is not covered.
    if( genome_locus_set_it_ == parent_->genome_locus_set_->end() ) {
        return false;
    }

    // If it contains the given chromosome, we use that to determine if the locus is covered.
    // We assume that the iterator is at the current chromosome.
    assert( genome_locus_set_it_->first == current_locus_.chromosome );
    return parent_->genome_locus_set_->is_covered( genome_locus_set_it_, current_locus_.position );
}

} // namespace population
} // namespace genesis
