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

#include "genesis/population/formats/variant_parallel_input_iterator.hpp"

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

VariantParallelInputIterator::Iterator::Iterator(
    VariantParallelInputIterator* generator
)
    : generator_(generator)
{
    // We use the generator as a check if this Iterator is intended
    // to be a begin() or end() iterator.
    // We could also just use the default constructor to create end() iterators,
    // this would have the same effect.
    if( ! generator_ ) {
        return;
    }

    // If the generator is valid, initialize our input sources and start iterating them.
    // Init the iterators and variant storage.
    iterators_.reserve( generator_->inputs_.size() );
    variant_sizes_.reserve( generator_->inputs_.size() );
    for( size_t i = 0; i < generator_->inputs_.size(); ++i ) {
        iterators_.emplace_back( generator_->inputs_[i].begin() );

        // We now have stored the iterator and called its begin() function,
        // which in the LambdaIterator already obtains the first element.
        // We use this to get the number of BaseCounts objects in the Variant.
        // We will later need this to default-construct that many BaseCounts
        // for positions where this iterator does not have data.
        // If the iterator does not have any data at all, we store that as well.
        if( iterators_[i] ) {
            variant_sizes_.push_back( iterators_[i]->samples.size() );

            // Let's make sure that the first position is a valid chromosome and
            // position. Later, when we advance the iterator, we repeat the check
            // for every locus we go to as well, just to be sure.
            assert_correct_chr_and_pos_( iterators_[i] );
        } else {
            variant_sizes_.push_back( 0 );
        }
    }

    // We use the sum of all to allocate memory for effciency. Let's compute that sum once.
    variant_size_sum_ = std::accumulate(
        variant_sizes_.begin(),
        variant_sizes_.end(),
        decltype( variant_sizes_ )::value_type( 0 )
    );

    // Init with default constructed Variants.
    variants_.resize( generator_->inputs_.size() );

    // Make sure all have the same size.
    assert( iterators_.size() == generator_->inputs_.size() );
    assert( iterators_.size() == variants_.size() );
    assert( iterators_.size() == variant_sizes_.size() );

    // Lastly, start with the additional carrying loci.
    carrying_locus_it_ = generator_->carrying_loci_.cbegin();

    // Now go to the first locus we want.
    advance_();
}

// =================================================================================================
//     Iterator Accessors
// =================================================================================================

Variant VariantParallelInputIterator::Iterator::joined_variant(
    bool allow_ref_base_mismatches,
    bool allow_alt_base_mismatches,
    bool move_samples
) {
    assert( iterators_.size() == variants_.size() );
    assert( iterators_.size() == variant_sizes_.size() );
    assert( iterators_.size() == generator_->inputs_.size() );

    // Prepare the result.
    Variant res;
    res.chromosome = current_locus_.chromosome;
    res.position   = current_locus_.position;
    res.samples.reserve( variant_size_sum_ );

    // Special edge case: No inputs at all.
    if( variants_.empty() ) {
        return res;
    }
    assert( variants_.size() > 0 );
    assert( variant_sizes_.size() > 0 );

    // Not all variants might have data; some might be the empty optional.
    // We hence need to keep track of whether we already initialized our result or not.
    // This only concernes the ref and alt base fields.
    bool bases_init = false;

    // Go through all variants, and for those that have data, check the data correctness,
    // and add them to the result.
    for( size_t i = 0; i < variants_.size(); ++i ) {

        // If the variant has data, use it.
        if( variants_[i] ) {

            // We already check all of the below when adding the data to variants_.
            // Still, assert that this is all good.
            assert( variants_[i]->chromosome == res.chromosome );
            assert( variants_[i]->position == res.position );
            assert( variants_[i]->samples.size() == variant_sizes_[i] );

            // Set and check the ref and alt bases.
            // This is the first input that has data here. Use it to initialize the bases.
            if( ! bases_init ) {
                res.reference_base   = variants_[i]->reference_base;
                res.alternative_base = variants_[i]->alternative_base;
                bases_init = true;
            }

            // Now check that all inputs have the same bases.
            if( res.reference_base != variants_[i]->reference_base ) {
                if( allow_ref_base_mismatches ) {
                    res.reference_base = 'N';
                } else {
                    throw std::runtime_error(
                        "Mismatching reference bases while iterating input sources in parallel at " +
                        current_locus_.to_string() + ". Some sources have base '" +
                        std::string( 1, res.reference_base ) + "' while others have '" +
                        std::string( 1, variants_[i]->reference_base ) + "'."
                    );
                }
            }
            if( res.alternative_base != variants_[i]->alternative_base ) {
                if( allow_alt_base_mismatches ) {
                    res.alternative_base = 'N';
                } else {
                    throw std::runtime_error(
                        "Mismatching alternative bases while iterating input sources in parallel at " +
                        current_locus_.to_string() + ". Some sources have base '" +
                        std::string( 1, res.alternative_base ) + "' while others have '" +
                        std::string( 1, variants_[i]->alternative_base ) + "'."
                    );
                }
            }

            // Now move or copy the samples. The reserve calls in the following are not necessary,
            // as we already allocate enough capacity above. We keep them here for future reference.
            if( move_samples ) {
                // res.samples.reserve( res.samples.size() + variants_[i]->samples.size() );
                std::move(
                    std::begin( variants_[i]->samples ),
                    std::end(   variants_[i]->samples ),
                    std::back_inserter( res.samples )
                );
                variants_[i]->samples.clear();
            } else {
                // res.samples.reserve( res.samples.size() + variants_[i]->samples.size() );
                std::copy(
                    std::begin( variants_[i]->samples ),
                    std::end(   variants_[i]->samples ),
                    std::back_inserter( res.samples )
                );
            }

        } else {

            // If the variant has no data, put as many dummy samples with empty BaseCounts
            // into the result as the input source has samples in its data positions.
            // res.samples.reserve( res.samples.size() + variant_sizes_[i].size() );
            for( size_t k = 0; k < variant_sizes_[i]; ++k ) {
                res.samples.emplace_back();
            }
        }
    }

    // If none of the input sources had data, that means that we are currently at an
    // additional carrying locus. Check this. We do not need to do anything else,
    // as the resulting Variant already contains all the information that we have at hand.
    assert(
        bases_init ||
        (
            carrying_locus_it_ != generator_->carrying_loci_.cend() &&
            locus_equal( *carrying_locus_it_, current_locus_ )
        )
    );

    // Make sure that the number of samples is the same as the sum of all sample sizes
    // in the variant_sizes_ vector combined.
    assert( res.samples.size() == variant_size_sum_ );

    // Done. Return the merged result.
    return res;
}

// =================================================================================================
//     Iterator Internal Members
// =================================================================================================

// -------------------------------------------------------------------------
//     advance_using_carrying_()
// -------------------------------------------------------------------------

void VariantParallelInputIterator::Iterator::advance_using_carrying_()
{
    // Candidate locus. We look for the earliest position of the carrying iterators,
    // as this is the next one we want to go to.
    GenomeLocus cand_loc;

    // Go through all carrying iterators and find the earliest next position of any of them.
    assert( iterators_.size() == generator_->selections_.size() );
    for( size_t i = 0; i < iterators_.size(); ++i ) {
        auto& iterator = iterators_[i];
        if( ! iterator || generator_->selections_[i] != ContributionType::kCarrying ) {
            continue;
        }

        // In all iterators, we already have moved on to at least the current position.
        // This assumes that all of the inputs are properly sorted. We check this in
        // increment_iterator_()
        // This also works for the very first time this function is called (in the iterator
        // constructor), as the iterator will then also compare greater than the
        // current_locus_, which is empty at this point.
        assert( locus_greater_or_equal(
            iterator->chromosome, iterator->position, current_locus_
        ));

        // If this iterator is currently one of the ones that contain the current
        // position, it's time to move on now. If not, we already asserted that it is
        // greater, which means, it it still waiting for its turn, so nothing to do now.
        if( locus_equal( iterator->chromosome, iterator->position, current_locus_ )) {
            increment_iterator_( iterator );

            // We might now be done with this input source.
            if( ! iterator ) {
                continue;
            }
        }

        // Now comes the part where we find the earliest position that we want to stop at.
        // Stop at the earliest postion of any iterator of type carrying (all of its positions
        // need to be included), or if we are in the first iteration of the loop.
        if(
            cand_loc.empty() ||
            locus_less( iterator->chromosome, iterator->position, cand_loc )
        ) {

            cand_loc = GenomeLocus{ iterator->chromosome, iterator->position };
        }
    }

    // If there are additional carrying loci, use them to find the candidate as well.
    assert( generator_ );
    if( carrying_locus_it_ != generator_->carrying_loci_.cend() ) {
        // All the assertions from above apply here as well.
        assert( ! carrying_locus_it_->empty() );
        assert( locus_greater_or_equal( *carrying_locus_it_, current_locus_ ) );

        // If the carrying locus is at the current locus, we need to move it forward,
        // same as above.
        if( locus_equal( *carrying_locus_it_, current_locus_ ) ) {
            ++carrying_locus_it_;
        }

        // Now, if it still is not at its end, we can use it as a candidate as well,
        // if it is earlier than the current input source based candidate (of it the candidate
        // is empty, which for example happens if all input sources are following, or if all
        // inputs have already reached their end).
        if(
            carrying_locus_it_ != generator_->carrying_loci_.cend() &&
            (
                cand_loc.empty() ||
                locus_less( *carrying_locus_it_, cand_loc )
            )
        ) {
            cand_loc = *carrying_locus_it_;
        }
    }

    // If we have not set any candidate locus, that means that all carrying iterators
    // are at their end. Time to wrap up then.
    if( cand_loc.empty() ) {
        assert( generator_ );
        assert( generator_->has_carrying_input_ );

        // Assert that indeed all carrying iterators are at their end.
        assert( [&](){
            for( size_t i = 0; i < iterators_.size(); ++i ) {
                if( generator_->selections_[i] == ContributionType::kCarrying && iterators_[i] ) {
                    return false;
                }
            }
            return true;
        }() );

        // Also, we must have reached the end of the additional carrying loci,
        // otherwise we would have found a candidate from there.
        assert( carrying_locus_it_ == generator_->carrying_loci_.cend() );

        // We are done here.
        generator_ = nullptr;
        return;
    }

    // We have found a new locus. It needs to be further down from the current
    // (again this also works in the first call of this function, when current is empty).
    assert( cand_loc > current_locus_ );

    // Now that we found the next position to go to, move _all_ iterators to it
    // (or the next one beyond, if it does not have that position).
    for( size_t i = 0; i < iterators_.size(); ++i ) {
        auto& iterator = iterators_[i];

        // Nothing to do if the iterator is already at its end.
        if( ! iterator ) {
            continue;
        }

        // Same assertion as above, this time for all of them, just to be sure.
        assert( locus_greater_or_equal(
            iterator->chromosome, iterator->position, current_locus_
        ));

        // Now move the iterator until we reach the candidate, or one beyond.
        // For carrying iterators, this loop can only get called once at max (or not at all,
        // if this source is already at or beyond the candidate from the loop above),
        // as we never want to skip anything in a carrying iterator. Assert this.
        size_t cnt = 0;
        while( iterator && locus_less( iterator->chromosome, iterator->position, cand_loc )) {
            increment_iterator_( iterator );
            ++cnt;
        }
        (void) cnt;
        assert( generator_->selections_[i] != ContributionType::kCarrying || cnt <= 1 );
    }

    // Finally, update the current locus, and set the variants according to the iterators.
    // The order of these is imporant, as the latter needs the former to be set.
    current_locus_ = cand_loc;
    update_variants_();
}

// -------------------------------------------------------------------------
//     advance_using_only_following_()
// -------------------------------------------------------------------------

void VariantParallelInputIterator::Iterator::advance_using_only_following_()
{
    // If this function is called, we only have following iterators,
    // so there are no addtional carrying loci given.
    assert( carrying_locus_it_ == generator_->carrying_loci_.cend() );
    assert( generator_->carrying_loci_.empty() );

    // Once one of the iterators reaches its end, we are done, as then there cannot
    // be any more intersections.
    bool one_at_end  = false;

    // If this is not the first call of this function (the one that is done in the constructor
    // of the iterator), move all iterators at least once, to get away from the current locus.
    assert( iterators_.size() == generator_->selections_.size() );
    if( ! current_locus_.empty() ) {
        for( size_t i = 0; i < iterators_.size(); ++i ) {
            auto& iterator = iterators_[i];

            // This function is only ever called if all inputs are of type following.
            assert( generator_->selections_[i] == ContributionType::kFollowing );

            // As we are doing the intersection of all iterators here, none of them can be at the
            // end right now. If one were, we would already have reached the end of our
            // parallel iteration before, and never entered this function.
            assert( iterator );

            // In all iterators, we must be at the current locus, as this is only intersections.
            // So now, it's time to move on once. Then, go to the next iterator.
            // The rest of this loop is not needed in this first round.
            assert( locus_equal( iterator->chromosome, iterator->position, current_locus_ ));
            increment_iterator_( iterator );

            // Check if we are done with this iterator. If so, we are completely done,
            // no need to do anything else here.
            if( ! iterator ) {
                one_at_end = true;
                break;
            }
        }
    }

    // Candidate locus. We look for the earliest locus that all inputs share.
    GenomeLocus cand_loc;

    // Loop until we have found a locus that all iterators share,
    // or until one of them is at the end (in which case, there won't be any more intersections
    // and we are done with the parallel iteration).
    bool found_locus = false;
    while( ! found_locus && ! one_at_end ) {

        // Assume that we are done. Below, we will reset these if we are not in fact done.
        found_locus = true;

        // Try to find the candidate in all inputs.
        for( size_t i = 0; i < iterators_.size(); ++i ) {
            auto& iterator = iterators_[i];

            // This function is only ever called if all inputs are of type following.
            assert( generator_->selections_[i] == ContributionType::kFollowing );

            // If the iterator is already at its end, we are done here.
            // This case can here only occur if we have an empty input source,
            // in which case the call to adanvance_() made from the constructor lead us here.
            // We assert that this is indeed the first call of this function by using the
            // current_element_ as a check.
            if( ! iterator ) {
                assert( current_locus_.empty() );
                one_at_end = true;
                break;
            }

            // Init the candidate. This happens in the first iteration of the for loop.
            if( cand_loc.empty() ) {
                assert( i == 0 );
                cand_loc = GenomeLocus{ iterator->chromosome, iterator->position };
            }

            // If the iterator is behind the candidate, move it forward until it either catches
            // up, or overshoots the locus, or reaches its end.
            while( iterator && locus_less( iterator->chromosome, iterator->position, cand_loc )) {
                increment_iterator_( iterator );
            }

            // If the iterator reached its end now, we are done here.
            // No more intersections can occur, we can leave the whole thing.
            if( ! iterator ) {
                one_at_end = true;
                break;
            }

            // If we have an overshoot, the candidate is not good, as this means that not all
            // inputs have that locus (as exemplified by that overshoot). In that case,
            // we store the new candidate, and leave the loop. It does not really matter here
            // if we go on to the next input, or just start the whole outer loop again.
            // Let's keep the pace and continue with the next input.
            assert( iterator );
            if( locus_greater( iterator->chromosome, iterator->position, cand_loc )) {
                cand_loc = GenomeLocus{ iterator->chromosome, iterator->position };
                found_locus = false;
                continue; // or break for the alternative loop order
            }

            // If we are here, we have reached the candidate locus.
            assert( iterator );
            assert( locus_equal( iterator->chromosome, iterator->position, cand_loc ));
        }
    }

    // Only one of the exit conditions can be true (unless there is no input at all).
    assert( iterators_.size() == 0 || ( found_locus ^ one_at_end ));

    // If we have not found any locus, that means that at least one of the iterators is at its end.
    // No more intersections can occur. Time to wrap up then.
    if( one_at_end ) {
        assert( ! generator_->has_carrying_input_ );
        generator_ = nullptr;

        // Assert that exactly one is at its end. Theoretically, in our search, other iterators
        // might also have been about to reach their end, but as we immediately exit the above
        // loop once we find an end, these are not incremented to their end yet.
        assert( [&](){
            size_t at_end_cnt = 0;
            for( size_t i = 0; i < iterators_.size(); ++i ) {
                if( ! iterators_[i] ) {
                    ++at_end_cnt;
                }
            }
            return at_end_cnt == 1;
        }() );

        // We have indicated our end by nulling generator_. Nothing more to do.
        return;
    }

    // If we are here, we have found a good new locus. It needs to be further down from the current
    // (again this also works in the first call of this function, when current is empty).
    assert( iterators_.size() == 0 || cand_loc > current_locus_ );

    // Assert that all are at the given locus, and not at their end.
    assert( iterators_.size() == 0 || found_locus );
    assert( [&](){
        for( size_t i = 0; i < iterators_.size(); ++i ) {
            auto const& iterator = iterators_[i];
            if( ! iterator || ! locus_equal( iterator->chromosome, iterator->position, cand_loc )) {
                return false;
            }
        }
        return true;
    }() );

    // Finally, update the current locus, and set the variants according to the iterators.
    // The order of these is imporant, as the latter needs the former to be set.
    current_locus_ = cand_loc;
    update_variants_();
}

// -------------------------------------------------------------------------
//     increment_iterator_()
// -------------------------------------------------------------------------

void VariantParallelInputIterator::Iterator::increment_iterator_(
    VariantInputIterator::Iterator& iterator
) {
    // If we already reached the end, do nothing.
    // if( ! iterator ) {
    //     return;
    // }
    //
    // Nope, this function should never be called on a finished iterator.
    assert( iterator );

    // We here check that the iterator is in chrom/pos order. This is also already done
    // by default in most of our file format iterators, and we here need an expensive
    // string copy just for this one check, but it feels like this is necessary to be
    // on the safe side. After all, this parallel iterator here is a bit tricky anyway,
    // so we have to live with that cost.
    auto const prev_loc = GenomeLocus{ iterator->chromosome, iterator->position };

    // Now do the increment and check whether we are done with this source.
    ++iterator;
    if( ! iterator ) {
        return;
    }

    // Check that it is has a valid chromosome and position, and
    // make sure that the input is sorted.
    assert_correct_chr_and_pos_( iterator );
    if( locus_less_or_equal( iterator->chromosome, iterator->position, prev_loc )) {
        throw std::runtime_error(
            "Cannot iterate multiple input sources in parallel, as (at least) "
            "one of them is not sorted by chromosome and position. "
            "Offending input source: " + iterator.data().source_name + " at " +
            iterator->chromosome + ":" + std::to_string( iterator->position )
        );
    }
}

// -------------------------------------------------------------------------
//     assert_correct_chr_and_pos_()
// -------------------------------------------------------------------------

void VariantParallelInputIterator::Iterator::assert_correct_chr_and_pos_(
    VariantInputIterator::Iterator const& iterator
) {
    assert( iterator );

    // This is checked already in our file format iterators, but we heavily depend
    // on this here, so let's check it.
    if( iterator->chromosome.empty() || iterator->position == 0 ) {
        throw std::runtime_error(
            "Cannot iterate multiple input sources in parallel, as (at least) "
            "one of them has an invalid chromosome (empty name) or position (0). "
            "Offending input source: " + iterator.data().source_name + " at " +
            iterator->chromosome + ":" + std::to_string( iterator->position )
        );
    }
}

// -------------------------------------------------------------------------
//     update_variants_()
// -------------------------------------------------------------------------

void VariantParallelInputIterator::Iterator::update_variants_()
{
    assert( iterators_.size() == variants_.size() );
    for( size_t i = 0; i < iterators_.size(); ++i ) {
        auto& iterator = iterators_[i];

        // If the iterator is already finished, we store an empty optional variant.
        if( ! iterator ) {
            variants_[i] = utils::nullopt;
            continue;
        }

        // If the iterator is at the current locus, we store its data here,
        // so that users can access it. If not, we store an empty optional variant.
        if( locus_equal( iterator->chromosome, iterator->position, current_locus_ )) {

            // We ideally want to move all data here, for efficiency.
            // The user does not have access to the iterators, so this is okay.
            // We however cannot move all the data, as we will later need access to the
            // chromosome and position of the iterators; so instead, we only move the expensive
            // BaseCounts samples. In order to avoid that when we add more elements to Variant later
            // and then accidentally forget to also set them here, we do a three-step process where
            // we move the BaseCounts over to a temp location first, and then copy the rest.
            // This ensures that whatever other fields Variant gets in the future, we always
            // copy them as well. So future proof!

            // Move the samples and leave them in a well-defined empty state in the iterator.
            auto tmp_samples = std::move( iterator->samples );
            iterator->samples.clear();

            // Now we can copy the rest, which will not copy any samples (as we just cleared them),
            // and then finally move over the samples again.
            variants_[i] = *iterator;
            variants_[i]->samples = std::move( tmp_samples );

            // Check for consistency. This is also already checked in all our input
            // file sources that we have implemented so far, but better safe than sorry.
            // Also, we need to do this in case someone uses a different source that does not check.
            if( variants_[i]->samples.size() != variant_sizes_[i] ) {
                throw std::runtime_error(
                    "Cannot iterate multiple input sources in parallel, as (at least) "
                    "one of them has an inconsistent number of samples. "
                    "Offending input source: " + iterator.data().source_name + " at " +
                    iterator->chromosome + ":" + std::to_string( iterator->position ) + ". " +
                    "Expecting " + std::to_string( variant_sizes_[i] ) +
                    " samples (based on the first used line of input of that source), " +
                    "but found " + std::to_string( variants_[i]->samples.size() ) +
                    " at the indicated locus."
                );
            }

            // Naive version that just makes a full copy.
            // variants_[i] = *iterator;
        } else {
            // The iterator is not at our current locus. We already checked that it is
            // however still valid, so it must be beyond the current one. Assert this.
            assert( locus_greater(
                iterator->chromosome, iterator->position, current_locus_
            ));

            variants_[i] = utils::nullopt;
        }
    }
}

} // namespace population
} // namespace genesis
