#ifndef GENESIS_POPULATION_WINDOW_REGION_WINDOW_STREAM_H_
#define GENESIS_POPULATION_WINDOW_REGION_WINDOW_STREAM_H_

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

#include "genesis/population/genome_locus.hpp"
#include "genesis/population/genome_region_list.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/window/base_window_stream.hpp"
#include "genesis/population/window/window_view_stream.hpp"
#include "genesis/population/window/window_view.hpp"
#include "genesis/population/window/window.hpp"
#include "genesis/utils/containers/range.hpp"

#include <cassert>
#include <deque>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Region Window Stream
// =================================================================================================

/**
 * @brief Stream for Window%s representing regions of a genome.
 *
 * This class allows to iterate a set of regions, each yielded to the caller as a Window.
 * That is, with each step of the iteration of this class, one region (interval) of the input
 * region list is yielded as a Window, containing the data of the input `InputStreamIterator`
 * (which is of type `DataType`). These regions can for example be genes, linkage blocks, or
 * other genomic intervals of interest. Each can have an arbitrary length, and the can potentially
 * be overlapping or be nested.
 *
 * The three functors
 *
 *  * #entry_input_function,
 *  * #chromosome_function, and
 *  * #position_function
 *
 * have to be set prior to the iteration.
 * All other settings are option and/or defaulted to reasonable values.
 * See make_region_window_stream() and make_default_region_window_stream()
 * for helper functions that take care of this for most of our data types.
 *
 * See BaseWindowStream for more details on the three functors, the template parameters,
 * and general usage examples of the class.
 *
 * @see make_region_window_stream()
 * @see make_default_region_window_stream()
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
class RegionWindowStream final : public BaseWindowStream<InputStreamIterator, DataType>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = RegionWindowStream<InputStreamIterator, DataType>;
    using base_type = BaseWindowStream<InputStreamIterator, DataType>;

    using Window            = ::genesis::population::Window<DataType>;
    using Entry             = typename Window::Entry;
    using InputType         = typename InputStreamIterator::value_type;

    using iterator_category = std::input_iterator_tag;
    using value_type        = Window;
    using pointer           = value_type*;
    using reference         = value_type&;
    using const_reference   = value_type const&;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

    /**
     * @brief Internal iterator that produces Window%s.
     */
    class DerivedIterator final : public BaseWindowStream<InputStreamIterator, DataType>::BaseIterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type = typename RegionWindowStream<
            InputStreamIterator, DataType
        >::DerivedIterator;

        using base_iterator_type = typename BaseWindowStream<
            InputStreamIterator, DataType
        >::BaseIterator;

        using Window            = ::genesis::population::Window<DataType>;
        using Entry             = typename Window::Entry;
        using InputType         = typename InputStreamIterator::value_type;

        using iterator_category = std::input_iterator_tag;
        using value_type        = Window;
        using pointer           = value_type*;
        using reference         = value_type&;
        using const_reference   = value_type const&;

    private:

        DerivedIterator() = default;

        DerivedIterator(
            RegionWindowStream const* parent
        )
            : base_iterator_type( parent )
            , parent_( parent )
        {
            // Edge case check. See Base for details.
            if( ! parent_ ) {
                return;
            }
            assert( parent_ );

            // Saveguard. This might be called on an empty range, in which case we just do nothing,
            // with the exception being that we do not want to skip empty regions; in that case,
            // we continue, so that - despite there being no data - all regions are added and
            // processed by the iterator.
            if(
                base_iterator_type::current_ == base_iterator_type::end_ &&
                parent_->skip_empty_regions_
            ) {
                parent_ = nullptr;
                return;
            }

            // Let's get going. This takes care of finding the first window that we want to emit.
            find_next_chromosome_regions_();
            fill_next_windows_();

            // We here need a special case for the first/last window markers,
            // to cover the edge case that there is only a single window.
            base_iterator_type::is_first_window_ = ( windows_.size() > 0 );
            base_iterator_type::is_last_window_ = (
                windows_.size() == 1 ||
                ( windows_.size() > 1 && windows_[0].chromosome() != windows_[1].chromosome() )
            );

            // We might have finished already, if there were no windows at all that we needed to fill.
            // Could be that we did not have any regions, or skip empty regions, and there was no
            // overlap with any data. Either way, in that case we have processed all our input
            // already, and are done.
            if( windows_.empty() ) {
                assert( base_iterator_type::current_ == base_iterator_type::end_ );
                parent_ = nullptr;
            }
        }

    public:

        virtual ~DerivedIterator() override = default;

        DerivedIterator( self_type const& ) = default;
        DerivedIterator( self_type&& )      = default;

        DerivedIterator& operator= ( self_type const& ) = default;
        DerivedIterator& operator= ( self_type&& )      = default;

        friend RegionWindowStream;

        // -------------------------------------------------------------------------
        //     Internal and Virtual Members
        // -------------------------------------------------------------------------

    private:

        /**
         * @brief Increment the iterator, i.e., move to the next window.
         */
        void increment_() override final
        {
            // Check that there is at least one window, which needs to be the case,
            // as otherwise we would already have finished the iteration, or the user has called
            // an increment on a past-the-end iterator, so we can asserted that as well here.
            // For the same reason, we do not have marked this as the end yet, assert that as well.
            assert( windows_.size() > 0 );
            assert( parent_ );
            assert( parent_->region_list_ );

            // Here we fill at least the next two windows (if possible), to know when we switch
            // chromosomes, so that we can process our chromosome boundary first/last markers.
            // In other words, whenever we use fill_next_windows_(), we end up with two or more
            // windows in the queue while possible, and only end up with 1 window at the very end.
            if( windows_.size() == 1 ) {
                // We reached the end. Make sure that we are actually at the end,
                // and did not forget to mark the window that we just left as the last.
                // We also either have processed all regions in the list, or wanted to skip
                // the empty ones anyway (which then are the ones for which there was no data).
                // We don't really need to pop that last window, but seems cleaner that way.
                assert( base_iterator_type::current_ == base_iterator_type::end_ );
                assert( base_iterator_type::is_last_window_ );
                assert(
                    seen_chromosomes_.size() == parent_->region_list_->chromosome_count() ||
                    parent_->skip_empty_regions_
                );
                windows_.pop_front();
                parent_ = nullptr;

            } else {
                assert( windows_.size() > 1 );

                // If we are about to enter a new chromosome, mark this.
                // We are going to pop window 0 next, so then what is now window 1 becomes
                // the current one, which in this case is the first of its chromosome.
                base_iterator_type::is_first_window_ = (
                    windows_[0].chromosome() != windows_[1].chromosome()
                );

                // We go to the next window, meaning we are done with this one,
                // and fill enough to know where we are at - whether the next one is the last window
                // of the chromosome or not.
                windows_.pop_front();
                fill_next_windows_();

                // Same as above, but for the end marker. This happens if either we are at the very
                // end and there is only one window left, or if with the next increment of the
                // iterator, we are going to have a new chromosome.
                assert( windows_.size() > 0 );
                base_iterator_type::is_last_window_ = (
                    windows_.size() == 1 ||
                    ( windows_[0].chromosome() != windows_[1].chromosome() )
                );

                // We either have data in the window, or do not skip empty windows.
                assert( ! windows_.front().empty() || ! parent_->skip_empty_regions_ );
            }
        }

        /**
         * @brief Fill at least the next two windows (if possible), to make sure that we know
         * where we are at in the iteration (end of a chromosome or not).
         *
         * The window to be filled is the one that will be dereferenced next by the user of this
         * class. We furthermore move along in the input data stream at least long enough to know
         * whether this is the last window on the chromosome, or if there are more windows later
         * that will be used (i.e., either have data, or are not skipped due to skip_empty_regions_).
         */
        void fill_next_windows_()
        {
            // Shorthands and same basic assertions as before.
            assert( parent_ );
            assert( parent_->region_list_ );
            auto const& region_list = *parent_->region_list_;
            auto& cur_it = base_iterator_type::current_;
            auto& end_it = base_iterator_type::end_;

            // We always need to have at least one window completely ready (filled with data),
            // as this is the one that the iterator is currently at when it gets dereferenced.
            // However, we also need to peek ahead to see if there is at least one more region
            // down the line for which we have data, so that (depending on skip_empty_regions_) we
            // can tell whether we are currently at the last window of the chromosome or not.

            // Fill all regions that overlap with our current pos (as we are only processing
            // the input data once, so we have to do this at the same time). We stop once we have
            // filled our current window _and_ determined whether it's the last of the chromosome.
            // We might also reach the end of the data, or the next chromosome while looping.
            while( cur_it != end_it ) {

                // Each loop iteration here processes one position of the input data stream.
                // We need the chr and pos often, so let's store them here.
                auto const cur_chr = parent_->chromosome_function( *cur_it );
                auto const cur_pos = parent_->position_function( *cur_it );

                // We cannot assume to have any windows here... All might have been deleted
                // in a previous iteration, when the position on the chromosome is after
                // all regions in the list. So, no assertion about windows here.

                // The current window is done when we are either already at the next chromosome,
                // or, if we are still on the same chromosome, we have processed all data in
                // the interval of the window.
                auto const cur_win_done = (
                    windows_.size() > 0 && (
                        cur_chr != windows_.front().chromosome() ||
                        cur_pos >  windows_.front().last_position()
                    )
                );

                // The next window is determined when we know whether it has data or won't be
                // skipped anyway (if we dont skip_empty_regions_). It does not matter if this
                // window is on the same chromosome as the current one - we just need to know
                // that there is one more window coming that we want to yield to the user of this
                // iterator, and from there we can determine the rest in increment_().
                auto const next_win_determined = (
                    windows_.size() > 1 && (
                        ! windows_[1].empty() ||
                        ! parent_->skip_empty_regions_
                    )
                );

                // Our actual exit condition: We have filled the current window, _and_ we know
                // whether we are currently at the last window of the chromosome or not.
                // It might happen that we never break out of here, and instead the loop exit
                // condition kicks in, which is when we have reached the end of the data.
                if( cur_win_done && next_win_determined ) {
                    break;
                }

                // If we reach the end of the current chromosome, we need to find the next one
                // that we want to use. There might be chromosomes in the data with no regions,
                // and we might even reach the end of the input in that case.
                // This means that we process windows in the same order of the data input stream,
                // but independently of their order in the genome region list.
                // The edge case that the windows are empty can occur when the first data entry
                // on a chr is after the last region, so that all regions are deleted by
                // fill_windows_at_current_position_() already in the previous loop iteration.
                if( windows_.empty() || cur_chr != windows_.back().chromosome() ) {
                    // Let's find the next chromosome for which we have regions, and skip all data
                    // that does not have any regions in the list. Once we have found this, we add
                    // all its regions to our window list. This might keep quite a long list of
                    // empty windows in the queue, but that's okay, as this is waaaay easier to
                    // implement than trying to iterate through the regions at the same time as well.
                    find_next_chromosome_regions_();

                    // After the above call, our position in the input stream has changed, so we
                    // need to restart the loop. As cur_it is a reference, it is also updated,
                    // so this is okay to check it here this way.
                    if( cur_it == end_it ) {
                        // If we reached the end of the data, this loop has done its purpose.
                        // (We could just continue here, as this is also the loop exit condition...)
                        break;
                    } else {
                        // If we are not yet at the end, we have moved our cur_it to a new chromosome.
                        // Assert that this was successfull. Then, start the loop again, to reset
                        // our shorthands, and check our exit conditions again for the cur it.
                        assert( ! windows_.empty() );
                        assert(
                            windows_.back().chromosome() == parent_->chromosome_function( *cur_it )
                        );
                        assert( cur_index_ == 0 );
                        continue;
                    }
                }

                // Now add all data of the current position to all windows that span the locus.
                // Let's keep the function a bit smaller, and outsource this step.
                // We here assert that we are actually where we think we are, as the condition
                // above might have changed our position on the input data.
                assert( ! windows_.empty() );
                assert( parent_->chromosome_function( *cur_it ) == cur_chr );
                assert( parent_->position_function( *cur_it ) == cur_pos );
                fill_windows_at_current_position_( cur_chr, cur_pos );

                // Edge case: The filling of the windows has determined that all windows (or all
                // but one) were empty and to be deleted, so that none (or just one) are left.
                // So now we are at a point in the input data where we know that no regions will
                // come any more on that chromosome. That happens if at the end of chromosomes
                // there is data, but no regions.
                // Edge cases covered below:
                // (1) Every window was removed by the above function, as none had data. This
                //     happens when in the beginning of the iteration, all regions on a chromosome
                //     get deleted, as none contain data.
                // (2) Same case, but after we already have processed one with data and are now
                //     looking for another region on the same chr, but don't find data for it.
                // (3) We already moved to the next chr in the data, but while trying to find
                //     a region with data, we also deleted all regions on there, because none
                //     had data. We need to cover this case here, as otherwise we'd be attempting
                //     to add the regions of the chr again by calling find_next_chromosome_regions_()
                //     which we do not want, so we skip the rest of the chr in the data.
                // In all those cases, we kinda already know that the current window (if there is one)
                // is the last on the chromosome, but our surrounding functions assume that we have
                // two windows in the queue at all times (while possible), so we need to move to the
                // next chromosome here, so that we can get it's regions added to the queue.
                // Hence, we simply finish the data input for that chromosome.
                if(
                    windows_.empty() ||
                    (
                        windows_.size() == 1 &&
                        cur_chr == windows_.front().chromosome() &&
                        cur_pos >  windows_.front().last_position()
                    ) || (
                        windows_.size() == 1 &&
                        cur_chr != windows_.front().chromosome()
                    )
                ) {
                    while( cur_it != end_it && parent_->chromosome_function( *cur_it ) == cur_chr ) {
                        ++cur_it;
                    }
                    continue;
                }
                assert( cur_it != end_it );
                assert(
                    windows_.size() > 1 || (
                        windows_.size() == 1 &&
                        cur_chr == windows_.front().chromosome() &&
                        cur_pos <= windows_.front().last_position()
                    )
                );

                // Move to next input.
                ++cur_index_;
                ++cur_it;

                // Make sure that input is at least sorted by position.
                if(
                    cur_it != end_it &&
                    parent_->chromosome_function( *cur_it ) == cur_chr &&
                    parent_->position_function( *cur_it ) <= cur_pos
                ) {
                    throw std::runtime_error(
                        "Input not sorted or containing duplicate positions,"
                        " on chromosome '" + cur_chr + "',"
                        " position " + std::to_string( parent_->position_function( *cur_it )) +
                        " found in the input after already having seen position " +
                        std::to_string( cur_pos )
                    );
                }
            }

            // Opposite edge case of above: We reached the end of the data stream before
            // all positions for which we have regions were covered by the above loop.
            // In that case, the deletion in fill_windows_at_current_position_() was never executed.
            // That means, we might have regions for which there is no data. Might need to delete.
            if( cur_it == end_it && parent_->skip_empty_regions_ ) {

                // Delete all empty windows from the back of the list for which we have not
                // seen data, so that we did not discard them yet.
                while( ! windows_.empty() && windows_.back().empty() ) {
                    windows_.pop_back();
                }
            }

            // Another edge case: We have reached the end of the data, but did not process all
            // regions yet in the case that we don't want to skip the empty ones. In that case,
            // we just all add them to the end of the list, so that they get processed.
            // This needs to come here at the end, as we might get here after the above call to
            // find_next_chromosome_regions_()
            if(
                cur_it == end_it &&
                ! parent_->skip_empty_regions_ &&
                seen_chromosomes_.size() != region_list.chromosome_count()
            ) {
                // It would be a bit more memory efficient to only add one of the remaning chrs
                // here at a time. However, there might be chrs with exactly one region in the list,
                // in which case our increment_() function would assume that this is the last
                // window of the whole iteration... So instead of adding more bookkeeping here,
                // we simply add all missing remaining regions now, and are done with it.
                for( auto const& chr : region_list.chromosome_names() ) {
                    assert( region_list.region_count( chr ) > 0 );
                    if( seen_chromosomes_.count( chr ) == 0 ) {
                        add_chromosome_windows_( chr );
                    }
                }
            }
        }

        /**
         * @brief Helper function that adds the data of the current locus (where we are at in the
         * input data stream) to all windows that span it.
         *
         * We take the @p cur_chr and @p cur_pos here as arguments simply as a small optimization,
         * in order to not have to obtain them from the data iterator again.
         */
        void fill_windows_at_current_position_(
            std::string const& cur_chr,
            size_t cur_pos
        ) {
            auto& cur_it = base_iterator_type::current_;
            auto& end_it = base_iterator_type::end_;
            (void) end_it;

            assert( parent_ );
            assert( cur_it != end_it );
            assert( ! cur_chr.empty() );
            assert( cur_pos > 0 );
            assert( seen_chromosomes_.count( cur_chr ) > 0 );
            assert( windows_.size() > 0 );

            // Add the current locus to all windows that span it. We only visit each locus
            // once, so we need to make sure that all windows that span the locus get the data.
            // We use a while loop here, as we might delete empty windows that did not receive
            // any data while in the loop, and hence want some control over the iteration.
            size_t i = 0;
            while( i < windows_.size() ) {
                auto& window = windows_[i];

                // The windows are sorted by start position, and so is the input data stream,
                // so as soon as we reach one where we are not yet at,
                // we can stop - none of the following windows in this loop need the current pos.
                if( cur_chr == window.chromosome() && cur_pos < window.first_position() ) {
                    break;
                }

                // If the window spans the current locus, add it.
                if(
                    cur_chr == window.chromosome() &&
                    cur_pos >= window.first_position() &&
                    cur_pos <= window.last_position()
                ) {

                    // Make absolutely sure that we are adding the right stuff.
                    assert( cur_it != end_it );
                    assert( cur_chr == parent_->chromosome_function( *cur_it ));
                    assert( cur_pos == parent_->position_function( *cur_it ) );

                    window.entries().emplace_back(
                        cur_index_,
                        cur_pos,
                        parent_->entry_input_function( *cur_it )
                    );
                }

                // Finally, if we are past a window (either past is position, or on a different
                // chromosome altogether), and that window is empty, we might want to delete it.
                // We do not increment i in this case, as after earsing,
                // the next window will have the current i.
                if(
                    window.empty() &&
                    parent_->skip_empty_regions_ &&
                    ( cur_chr != window.chromosome() || cur_pos > window.last_position() )
                ) {
                    windows_.erase( windows_.begin() + i );
                    continue;
                }

                // If we are here, we filled the current position into the current window of
                // the inner loop, or accept emmpty windows, and can continue with the next one.
                assert( ! window.empty() || ! parent_->skip_empty_regions_ );
                ++i;
            }
        }

        /**
         * @brief Helper function that scans the input data stream until we find a chromosome
         * for which we have regions in the list, and adds them to the queue.
         *
         * This does not yet guarantee that any of the regions acutally contain data;
         * checking this is the job of fill_next_windows_().
         *
         * The function assumes that we are at the start of a chromosome that we have not seen yet,
         * and otherwise throws - and hence also throws if we have repeated chromosomes in the data.
         */
        void find_next_chromosome_regions_()
        {
            // Checks and shorthands.
            assert( parent_ );
            assert( parent_->region_list_ );
            auto const& region_list = *parent_->region_list_;
            auto& cur_it = base_iterator_type::current_;
            auto& end_it = base_iterator_type::end_;

            // The function is only called when we have data, or in the edge case that we have no
            // data but don't want to skip empty regions, in which case there should be no windows.
            assert( cur_it != end_it || windows_.size() == 0 );

            // Find the next chromosome for which the region list has regions.
            // We use empty cur_chr to mark that we have not found a good chromosome yet.
            std::string cur_chr = "";
            while( cur_chr.empty() && cur_it != end_it ) {

                // Get the new chromosome that we are at now, and check that we have not seen it yet.
                cur_chr = parent_->chromosome_function( *cur_it );
                if( seen_chromosomes_.count( cur_chr ) > 0 ) {
                    throw std::runtime_error(
                        "Input not sorted, chromosome '" + cur_chr + "' has been in the input before."
                    );
                }

                // If the chromosome does not have any regions in the list, we skip the whole
                // input data for this chromosome, and move on to the next.
                if(
                    ! region_list.has_chromosome( cur_chr ) ||
                    region_list.region_count( cur_chr ) == 0
                ) {
                    while( cur_it != end_it && parent_->chromosome_function( *cur_it ) == cur_chr ) {
                        ++cur_it;
                    }
                    // Not adding the chr to the seen list here, as we want that list to only
                    // contain chrs from the region list, but not also the ones frmo the data.
                    // We might add later a check for data chrs as well, let's see.
                    // seen_chromosomes_.insert( cur_chr );
                    cur_chr = "";
                }
            }

            // We might have reached the end of the input data, with nothing left to do here.
            // At this point, we do not know anything about the windows. We might have n windows
            // from the prev chr for which we are trying to find data, but there is none, hence we
            // ended up in this function here.
            if( cur_it == end_it ) {
                assert( cur_chr.empty() );
                return;
            }

            // Now we are at a chromosome for which there is data, and regions in the list.
            // That does not yet mean that there is an overlap between the data and the regions,
            // but that will be checked by fill_next_windows_().
            assert( cur_it != end_it );
            assert( cur_chr == parent_->chromosome_function( *cur_it ) );
            assert( region_list.has_chromosome( cur_chr ));
            assert( region_list.region_count( cur_chr ) > 0 );

            // Add those windows to the list. There needs to have been something added then.
            add_chromosome_windows_( cur_chr );
            assert( windows_.size() > 0 );

            // Reset the index for this chromosome.
            cur_index_ = 0;
        }

        /**
         * @brief Helper function to add all regions of a chromosome in the region list as empty
         * windows to our list.
         */
        void add_chromosome_windows_( std::string const& chromosome )
        {
            // Checks and shorthands.
            assert( parent_ );
            assert( parent_->region_list_ );
            auto const& region_list = *parent_->region_list_;

            // We only call this function for chromosomes for which there are regions,
            // and that we have not processed before. Both are ensured by the callers already.
            assert( region_list.has_chromosome( chromosome ));
            assert( region_list.region_count( chromosome ) > 0 );
            assert( seen_chromosomes_.count( chromosome ) == 0 );

            // For simplicity, we add _all_ regions of the region list of this chromosome to our
            // window list here already. We could be slightly more efficient, and only ever add
            // them based on the position that we are at, but we add them empty, and fill in the
            // data while we iterate the underlying data stream, so it's not much overhead,
            // but makes the code here way simpler, as we do not have to iterate in parallel over
            // the data _and_ the regions.
            for( auto const& region : region_list.chromosome_regions( chromosome ) ) {
                if( region.low() == 0 || region.high() == 0 ) {
                    throw std::runtime_error(
                        "Cannot process whole chromosomes with RegionWindowStream"
                    );
                };

                // Check that they come in the right order, sorted by starting position,
                // which should be guaranteed as IntervalTree is sorted that way.
                assert(
                    windows_.empty() ||
                    windows_.back().chromosome() != chromosome ||
                    region.low() >= windows_.back().first_position()
                );

                // Now add the window, with all its properties.
                windows_.emplace_back();
                auto& window = windows_.back();
                window.chromosome( chromosome );
                window.first_position( region.low() );
                window.last_position(  region.high() );
            }

            // Mark that we have seen this chromosome and processed its windows.
            seen_chromosomes_.insert( chromosome );
        }

        value_type& get_current_window_() const override final
        {
            // If the window list is empty, we have reached the end of the iteration. Calling this
            // function here is then a user error. Catch it a bit nicer with the assertion.
            assert( ! windows_.empty() );
            return const_cast<value_type&>( windows_.front() );
        }

        BaseWindowStream<InputStreamIterator, DataType> const* get_parent_() const override final
        {
            return parent_;
        }

    private:

        // Parent. Needs to live here to have the correct derived type.
        RegionWindowStream const* parent_ = nullptr;

        // We store the chromosome names from the region list that we already processed,
        // as a double check so that we don't start multiple times, but allow chromosomes
        // in any order, and only expect positions within a chromosome to be sorted.
        std::unordered_set<std::string> seen_chromosomes_;

        // Store the windows that we are filling. This stores all windows of a chromosome,
        // and gets cleared and filled with new windows when the underlying iterator reaches
        // the next chromosome.
        std::deque<Window> windows_;
        size_t cur_index_ = 0;
    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    RegionWindowStream(
        InputStreamIterator begin, InputStreamIterator end,
        std::shared_ptr<GenomeRegionList> region_list
    )
        : base_type( begin, end )
        , region_list_( region_list )
    {
        if( ! region_list ) {
            throw std::invalid_argument(
                "No GenomeRegionList provided for creating RegionWindowStream"
            );
        }
    }

    virtual ~RegionWindowStream() override = default;

    RegionWindowStream( RegionWindowStream const& ) = default;
    RegionWindowStream( RegionWindowStream&& )      = default;

    RegionWindowStream& operator= ( RegionWindowStream const& ) = default;
    RegionWindowStream& operator= ( RegionWindowStream&& )      = default;

    friend DerivedIterator;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    self_type& skip_empty_regions( bool value )
    {
        skip_empty_regions_ = value;
        return *this;
    }

    bool skip_empty_regions() const
    {
        return skip_empty_regions_;
    }

    // -------------------------------------------------------------------------
    //     Virtual Members
    // -------------------------------------------------------------------------

protected:

    std::unique_ptr<typename BaseWindowStream<InputStreamIterator, DataType>::BaseIterator>
    get_begin_iterator_() override final
    {
        // Cannot use make_unique here, as the Iterator constructor is private,
        // and trying to make make_unique a friend does not seem to be working...
        return std::unique_ptr<DerivedIterator>( new DerivedIterator( this ));
        // return utils::make_unique<DerivedIterator>( this );
    }

    std::unique_ptr<typename BaseWindowStream<InputStreamIterator, DataType>::BaseIterator>
    get_end_iterator_() override final
    {
        return std::unique_ptr<DerivedIterator>( new DerivedIterator() );
        // return utils::make_unique<DerivedIterator>( nullptr );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // List of regions that we want to iterate over; each region will be represented as a Window.
    std::shared_ptr<GenomeRegionList> region_list_;

    // Settings
    bool skip_empty_regions_ = true;

};

// =================================================================================================
//     Make Region Window Stream
// =================================================================================================

/**
 * @brief Helper function to instantiate a RegionWindowStream
 * without the need to specify the template parameters manually.
 *
 * The three functors `entry_input_function`, `chromosome_function`, and `position_function`
 * of the RegionWindowStream have to be set in the returned stream before using it.
 * See make_default_region_window_stream() for an alternative make function
 * that sets these three functors to reasonable defaults that work for the Variant data type.
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
RegionWindowStream<InputStreamIterator, DataType>
make_region_window_stream(
    InputStreamIterator begin, InputStreamIterator end,
    std::shared_ptr<GenomeRegionList> region_list
) {
    auto it = RegionWindowStream<InputStreamIterator, DataType>( begin, end, region_list );
    return it;
}

/**
 * @brief Helper function to instantiate a RegionWindowStream for a default use case.
 *
 * This helper assumes that the underlying type of the input data stream and of the Window%s
 * that we are iterating over are of the same type, that is, we do no conversion in the
 * `entry_input_function` functor of the RegionWindowStream. It further assumes that this
 * data type has public member variables `chromosome` and `position` that are accessed by the
 * `chromosome_function` and `position_function` functors of the RegionWindowStream.
 * For example, a data type that this works for is Variant data.
 */
template<class InputStreamIterator>
RegionWindowStream<InputStreamIterator>
make_default_region_window_stream(
    InputStreamIterator begin, InputStreamIterator end,
    std::shared_ptr<GenomeRegionList> region_list
) {
    using DataType = typename InputStreamIterator::value_type;

    // Set functors.
    auto it = RegionWindowStream<InputStreamIterator>( begin, end, region_list );
    it.entry_input_function = []( DataType const& variant ) {
        return variant;
    };
    it.chromosome_function = []( DataType const& variant ) {
        return variant.chromosome;
    };
    it.position_function = []( DataType const& variant ) {
        return variant.position;
    };

    // Set properties.
    return it;
}

/**
 * @brief Helper class that creates a RegionWindowStream
 * and wraps it in a WindowViewStream.
 *
 * See make_default_region_window_stream() for the base functionality,
 * and see make_window_view_stream() for the wrapping behaviour.
 *
 * Note that because this is a simple wrapper around the constructor of RegionWindowStream,
 * we lose access to that class itself, so that its more specialized member functions cannot be called
 * any more. If this is needed, use the two aforementioned `make_...()` functions individually.
 */
template<class InputStreamIterator>
WindowViewStream<InputStreamIterator>
make_default_region_window_view_stream(
    InputStreamIterator begin, InputStreamIterator end,
    std::shared_ptr<GenomeRegionList> region_list
) {
    return make_window_view_stream(
        make_default_region_window_stream( begin, end, region_list )
    );
}

} // namespace population
} // namespace genesis

#endif // include guard
