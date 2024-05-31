#ifndef GENESIS_POPULATION_WINDOW_QUEUE_WINDOW_STREAM_H_
#define GENESIS_POPULATION_WINDOW_QUEUE_WINDOW_STREAM_H_

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

#include "genesis/population/window/base_window_stream.hpp"
#include "genesis/population/window/window_view_stream.hpp"
#include "genesis/population/window/window_view.hpp"
#include "genesis/population/window/window.hpp"

#include <cassert>
#include <deque>
#include <functional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace genesis {
namespace population {

// =================================================================================================
//     Queue Window Stream
// =================================================================================================

/**
 * @brief Stream for Window%s contaiing a queue of entries, i.e., sliding Window%s of a fixed number
 * of selected positions in a genome.
 *
 * With each step of the iteration, a window of entries of the underlying input stream is yielded.
 * The window contains count() many selected entries (entries that we want to consider), which is
 * based on the result of the user-provided entry_selection_function() functor. Any entries of the
 * underlying stream for which entry_selection_function() returns `false` are still included in the
 * windows, but do not count towards the total count() of entries that we want in the window. This
 * is meant as a way for instance to create windows of `n` SNP positions, while also being able to
 * retain all non-SNP entries, if they are needed for the downstream algorithm that is using the
 * windows.
 *
 * If no such extra positions should be included in the window (e.g., if we _only_ want the SNPs),
 * an approriate filter needs to be applied beforhand to the input stream, for instance by adding
 * it to the VariantInputStream that is typically used as input stream here, so that these entries
 * are removed already before being assembled into windows here.
 *
 * The way the windowing then works is as follows: Any window contains count() many selected
 * entries, as well as all not-selected entries that come _before_ each of these selected entries.
 * For instance, `x` marking selected (for which `entry_selection_function` returns `true`),
 * and `-` marking non-selected entries, and with non-overlapping windows of size 2
 * (`stride == count == 2`), we get the following windows:
 *
 *     ---x--x---x-xx---x-xx-x--x----x-x-x---x--
 *     [     ][    ][   ][ ][   ][     ][      ]
 *
 * where brackets mark each window as a closed interval. This is only considering entries based on
 * them being selected or not - their position on the chromosome is not considerered here at all,
 * which also means that any missing data does not affect this - it will just appear as jumps in
 * the positions when iterating the windows, but as this windowing approach here is not based on
 * these positions, there is no other effect.
 *
 * Notice the special last case here: the last window also contains the trailing non-selected
 * positions. This is done so that these positions are part of _some_ window. This is also the case
 * if the last window contains fewer entries than the count(), which happens if the number of
 * selected entries in the chromosome is not a multiple of the count() (which is very likely).
 *
 * The default helper functions to make an instance of this window stream,
 * make_default_queue_window_stream() and make_default_queue_window_view_stream()
 * set the entry_selection_function() so that _all_ entries are selected.
 * On the other hand, the specialized versions of these functions for underlying data of type
 * Variant, make_passing_variant_queue_window_stream() and
 * make_passing_variant_queue_window_view_stream() instead set the
 * entry_selection_function() so that only Variant%s are selected for which the Variant::status
 * is passing. Thus, for instance, if the underlying data stream has filters applied to it using
 * the VariantFilterNumericalParams, this can be used to filter for SNPs. See there for details.
 *
 * During the stream iteration, when incrementing the iterator, we move forward stride() many
 * entries, which can correspond to an arbitrary number of positions in the genome, depending on
 * how far the entries are spread out. Hence, any filtering on positions in the genome should be
 * done beforehand on the inputs, so that only those entries are used by this stream that are
 * meant to be selected.
 *
 * The four functors
 *
 *  * #entry_input_function,
 *  * #chromosome_function,
 *  * #position_function, and
 *  * #entry_selection_function
 *
 * have to be set in the class prior to starting the iteration, as well as the count() of how many
 * entries shall be used in each iteration.
 * All other settings are optional and/or defaulted to reasonable values.
 *
 * See BaseWindowStream for more details on the four functors, the template parameters,
 * and general usage examples of the class.
 *
 * For helper functions that take care of setting up this class for the most common use cases:
 * @see make_queue_window_stream()
 * @see make_default_queue_window_stream()
 * @see make_default_queue_window_view_stream()
 * @see make_passing_variant_queue_window_stream()
 * @see make_passing_variant_queue_window_view_stream()
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
class QueueWindowStream final : public BaseWindowStream<InputStreamIterator, DataType>
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = QueueWindowStream<InputStreamIterator, DataType>;
    using base_type = BaseWindowStream<InputStreamIterator, DataType>;

    using Window            = ::genesis::population::Window<DataType>;
    using Entry             = typename Window::Entry;
    using InputType         = typename InputStreamIterator::value_type;

    using iterator_category = std::input_iterator_tag;
    using value_type        = Window;
    using pointer           = value_type*;
    using reference         = value_type&;
    using const_reference   = value_type const&;

    // -------------------------------------------------------------------------
    //     Public Functors
    // -------------------------------------------------------------------------

    /**
     * @brief Functor that takes an entry of the underlying input stream and returns whether
     * that entry should be selected towards the window size count() or not.
     *
     * Any entry that is selected (the function here returning `true`) will be used to fill the
     * window and are considered for the count() of entries per window. Entries for which the
     * function returns `false` are also enqueued in the window, but do not count towards the
     * number of entries in the window.
     */
    std::function<bool( InputType const& )> entry_selection_function;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

    /**
     * @brief Internal iterator that produces Window%s.
     */
    class DerivedIterator final : public BaseWindowStream<InputStreamIterator, DataType>::BaseIterator
    {
        // -------------------------------------------------------------------------
        //     Typedefs and Enums
        // -------------------------------------------------------------------------

    public:

        using self_type = typename QueueWindowStream<
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

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

    private:

        DerivedIterator() = default;

        DerivedIterator(
            QueueWindowStream const* parent
        )
            : base_iterator_type( parent )
            , parent_( parent )
        {
            // Edge case check. See Base for details.
            if( ! parent_ ) {
                return;
            }

            // Check that our selection functor is set up. The other three are already checked
            // in the base class, which is called from the delegated constructor call above.
            if( ! parent->entry_selection_function ) {
                throw std::runtime_error(
                    "Need to set BaseWindowStream::entry_selection_function "
                    "before iterating over Windows with a QueueWindowStream."
                );
            }

            // Check our own the settings.
            if( parent_->count_ == 0 ) {
                throw std::runtime_error(
                    "Cannot use QueueWindowStream with count == 0."
                );
            }
            if( parent_->stride_ == 0 ) {
                parent_->stride_ = parent_->count_;
            }
            if( parent_->stride_ > parent_->count_ ) {
                throw std::runtime_error(
                    "Cannot use QueueWindowStream with stride > count."
                );
            }

            // Let's get going.
            init_chromosome_();

            // If the input is empty (no data at all), we might already be done.
            // If not, fill the window with data.
            if( parent_ ) {
                update_();
            }
        }

    public:

        virtual ~DerivedIterator() override = default;

        DerivedIterator( self_type const& ) = default;
        DerivedIterator( self_type&& )      = default;

        DerivedIterator& operator= ( self_type const& ) = default;
        DerivedIterator& operator= ( self_type&& )      = default;

        friend QueueWindowStream;

        // -------------------------------------------------------------------------
        //     Internal and Virtual Members
        // -------------------------------------------------------------------------

    private:

        void init_chromosome_()
        {
            // Check that we are still good. If not, this function being called is likely a user
            // error by trying to increment a past-the-end iterator.
            assert( parent_ );

            // At the beginning of a chromosome, we should never have anything buffered.
            assert( tail_buffer_.empty() );
            assert( input_chromosome_finished_() );

            // Saveguard. This might be called on an empty range, in which case we just do nothing.
            if( base_iterator_type::current_ == base_iterator_type::end_ ) {
                parent_ = nullptr;
                return;
            }

            // Clear the window and prepare for new chromosome.
            window_.clear();
            window_.chromosome( parent_->chromosome_function( *base_iterator_type::current_ ));
            next_index_ = 0;
            current_queue_pop_count_.clear();
            base_iterator_type::is_first_window_ = true;
            base_iterator_type::is_last_window_ = false;
        }

        void increment_() override final
        {
            // Basic check again.
            assert( parent_ );

            // Special case: If we have no more underlying data, the iterator still needs to stop
            // at the last window(s), so that they can be processed. After that, when this
            // increment_() function is called again by the user, we then set parent_ = nullptr
            // to indicate that now we are done for good.
            if(
                base_iterator_type::current_ == base_iterator_type::end_ &&
                tail_buffer_.empty()
            ) {
                // If current_ == end_, we have definitely reached the end of the input, so we need
                // to have set is_last_window_ previously. If not set, that means it was already
                // reset, so that this is an iteration past the end.
                if( ! base_iterator_type::is_last_window_ ) {
                    throw std::runtime_error( "QueueWindowStream: Incrementing past the end" );
                }

                // Indicate that we are done now.
                parent_ = nullptr;
                return;
            }

            // Check if this call moves to the next chromosome. If so, we clear everything and
            // start all windows and buffers fresh for the new chromosome.
            if( base_iterator_type::is_last_window_ ) {
                assert( input_chromosome_finished_() );
                init_chromosome_();
            } else {
                base_iterator_type::is_first_window_ = false;
            }

            // Fill window with data.
            update_();
        }

        void update_()
        {
            // This function is only called when there is still data to be processed, which is
            // either the case when the input has data, or at least the buffer has some.
            assert( parent_ );
            assert(
                base_iterator_type::current_ != base_iterator_type::end_ ||
                ! tail_buffer_.empty()
            );

            // Two steps to update the queue.
            pop_old_entries_();
            push_new_entries_();

            // Update the window positions.
            assert( ! window_.empty() );
            window_.first_position( window_.entries().front().position );
            window_.last_position( window_.entries().back().position );

            // Assert that all is good. We have some data, either the right amount,
            // or less if we are at the end of the input or chromosome.
            // In particular, after the update, we either have the window full of entries
            // according to the count, or not, if it is the last window for the chromosome.
            assert( window_.size() > 0 );
            assert(
                base_iterator_type::is_last_window_ ||
                current_queue_pop_count_.size() == parent_->count_
            );
            assert(
                base_iterator_type::is_last_window_ ||
                window_.size() >= current_queue_pop_count_.front()
            );

            // If we are at the last window, for sure we also must have finished the chromosome.
            // The other way is not necessarily true (the two are not equivalent): It can be that
            // the buffer contains the very last element of the current chromosome, but the input
            // is already at the next. In that case, we are not yet at the last window (because we
            // need to move and process the buffer first), but the input already is done with the
            // chromosome. We test this as a logical implication here.
            assert( ! base_iterator_type::is_last_window_ || input_chromosome_finished_() );

            // Unless it's the last window, we have some data in the buffer.
            // If we are not yet in the last window, the push above will have put one entry in the
            // buffer. If we _are_ at the last window, we have moved all remaining buffer data
            // into the window already.
            assert( tail_buffer_.empty() ^ !base_iterator_type::is_last_window_ );
        }

        void pop_old_entries_()
        {
            // Before we update the window, we either have it full of entries, or its the first
            // window for the chromosome (or first window at all).
            assert(
                ( current_queue_pop_count_.size() == parent_->count_ ) ||
                ( current_queue_pop_count_.size() == 0 && base_iterator_type::is_first_window_ )
            );

            // Dequeue everything that we do not want to keep. If stride == count (default case),
            // we can simply remove everything at once, for speed. Otherwise, we pop as many
            // entries as we have stride, but only counting the considered ones.
            if( parent_->stride_ == parent_->count_ ) {
                window_.entries().clear();
                current_queue_pop_count_.clear();
            } else if( current_queue_pop_count_.empty() ) {
                // Edge case when we start with a new empty window.
                assert( window_.empty() );
                assert( current_queue_pop_count_.empty() );
                assert( base_iterator_type::is_first_window_ );
            } else {
                assert( ! window_.empty() );
                assert( ! current_queue_pop_count_.empty() );
                assert( ! base_iterator_type::is_first_window_ );

                // Remove as many selected entries as the stride tells us.
                // This means we also need to remove all non-selected entries along,
                // which is what current_queue_pop_count_ tells us. For each selected entry,
                // it tells us the number of total entries that we need to remove: itself,
                // and all non-selected ones that came before it.
                size_t removed_cnt = 0;
                while( removed_cnt < parent_->stride_ ) {
                    // Remove a single selected entry form the window, along with all preceeding
                    // non-selected ones. The current_queue_pop_count_ queue tells us how many
                    // those are.
                    assert( ! current_queue_pop_count_.empty() );
                    assert( window_.size() >= current_queue_pop_count_.front() );
                    for( size_t i = 0; i < current_queue_pop_count_.front(); ++i ) {
                        assert( ! window_.empty() );
                        window_.entries().pop_front();
                    }
                    current_queue_pop_count_.pop_front();
                    ++removed_cnt;
                }
                assert( current_queue_pop_count_.size() > 0 );
                assert( removed_cnt == parent_->stride_ );
            }

            // We always have either an empty window, or one where we made room for another stride.
            assert(
                current_queue_pop_count_.size() == 0 ||
                current_queue_pop_count_.size() == parent_->count_ - parent_->stride_
            );
        }

        void push_new_entries_()
        {
            // We need to keep track of how many entries were added for each stride.
            // Otherwise, the user might modify the entries, and so we cannot check the
            // entry_selection_function again when popping. Hence, instead, when pushing entries,
            // we keep a queue of how many entries need to be popped in the next iteration.

            // Keep track of how many entries this function added, for some assertions only.
            size_t add_cnt = 0;
            (void) add_cnt;

            // First make sure we process any trailing entries from the previous iteration. If there
            // is data in the tail buffer, that means it contains the next selected entry (and all
            // the unselected ones before it) on the current chromosome.
            if( tail_buffer_.empty() ) {
                // The buffer is only empty when we start the iteration or end it, at the data end
                // or at the end of the current chromosome. The assertion captures that.
                assert(
                    ( base_iterator_type::is_first_window_ && window_.empty() ) ||
                    ( base_iterator_type::is_last_window_  && input_chromosome_finished_() )
                );
            } else {
                // Here, we have a tail buffer, which means, we are still on the same chromosome,
                // and have a selected entry at the end of the buffer. We first assert that then
                // our window has some data from the previous iteration (or not, if the stride
                // is the count, so that each window starts empty).
                assert( ! window_.empty() || parent_->stride_ == parent_->count_ );
                assert(
                    parent_->chromosome_function( tail_buffer_.back().data ) == window_.chromosome()
                );
                assert( parent_->entry_selection_function( tail_buffer_.back().data ) );

                // We now move the tail with the selected entry and all unselected before it
                // from the buffer to our window, and increment the queue count accordingly.
                current_queue_pop_count_.push_back( tail_buffer_.size() );
                move_tail_buffer_to_window_();
                assert( tail_buffer_.empty() );
                ++add_cnt;
            }

            // Now enqueue new entries to fill the queue.
            bool finished_chromosome = false;
            while( current_queue_pop_count_.size() < parent_->count_ ) {
                // Try to find and enqueue the next selected entry into the window,
                // as well as all unselected entries before it.
                auto const& cur_chr = window_.chromosome();
                auto const cur_pos = window_.size() == 0 ? 0 : window_.entries().back().position;
                auto const add_result = add_entries_until_selected_to_queue_(
                    cur_chr, cur_pos, window_.entries()
                );

                // Check if we got a selected entry. If so, we count it.
                // If not, we reached the end of the chromosome or data, and so we leave this loop
                // prematurely, without having filled the full queue.
                if( add_result.second ) {
                    assert( add_result.first > 0 );
                    current_queue_pop_count_.push_back( add_result.first );
                    ++add_cnt;
                } else {
                    finished_chromosome = true;
                    break;
                }
            }

            // The above loop filled the window with as many selected entries as we need.
            // It could however be that there are no more selected entries on the chromosome
            // after that, which we need to know now.
            // So we read into the buffer, and test that. If so, we need to add that tail to
            // the window as well. If not, we keep it in the buffer for the next iteration.
            if( ! finished_chromosome ) {
                assert( tail_buffer_.empty() );
                assert( ! window_.empty() );

                // Read the next selected entry into the tail buffer
                auto const& cur_chr = window_.chromosome();
                auto const cur_pos = window_.size() == 0 ? 0 : window_.entries().back().position;
                auto const add_result = add_entries_until_selected_to_queue_(
                    cur_chr, cur_pos, tail_buffer_
                );

                // If we did not find a selected entry (only unselected, or nothing at all on this
                // chromosome), we mark that we reached the end of the chromosome, for below.
                if( add_result.second == false ) {
                    finished_chromosome = true;
                }
            }

            // If we ended the above loop without fully filling the window, or in the condition
            // afterwards found that we are at the end of the chromosome, we are done with a
            // chromosome (or the whole data). The tail buffer then contains all remaining
            // unselected entries, which we hence need to add to the window, as this is the last
            // window on the chromosome.
            if( finished_chromosome ) {
                assert(
                    tail_buffer_.empty() ||
                    ! parent_->entry_selection_function( tail_buffer_.back().data )
                );
                assert( input_chromosome_finished_() );
                move_tail_buffer_to_window_();
                base_iterator_type::is_last_window_ = true;
                assert( tail_buffer_.empty() );
            }

            // Either we have added as many new entries as the stride tells us, or, if this
            // was a new empty window, we have added a full count of entries,
            // or we reached the end of the data or the end of the chromosome.
            // Also, we can never have _more_ entries in the window, and we cannot have an empty
            // window, as in that case this update function should not have been called at all.
            assert(
                add_cnt == parent_->stride_ ||
                ( add_cnt == parent_->count_ && base_iterator_type::is_first_window_ ) ||
                ( base_iterator_type::is_last_window_ && input_chromosome_finished_() )
            );
            assert( add_cnt <= parent_->count_ );
            assert( add_cnt <= window_.size() );
        }

        std::pair<size_t, bool> add_entries_until_selected_to_queue_(
            std::string const& prev_chr, size_t prev_pos, std::deque<typename Window::Entry>& target
        ) {
            // The caller needs to know how many entries we added in total,
            // and whether we found a selected entry, or reached the end of the chr/data instead.
            size_t added_count = 0;
            bool found_selected = false;

            // Add entries from the input source to the target queue, stopping at the first entry
            // for which the selection function is true, or when we reach the end of the chr or data.
            while( true ) {
                // If we are at the end of the data, there is no selected entry here.
                if( base_iterator_type::current_ == base_iterator_type::end_ ) {
                    break;
                }

                // Get the chr and pos of this entry.
                auto const cur_chr = parent_->chromosome_function( *base_iterator_type::current_ );
                auto const cur_pos = parent_->position_function( *base_iterator_type::current_ );

                // If we are at the next chromosome, we are done with this window,
                // again not having found a selected entry.
                if( cur_chr != prev_chr ) {
                    break;
                }

                // Check that we are not going backwards in the chromosome,
                // i.e., if we got unsorted data. That would lead to unwanted behaviour.
                if( prev_pos >= cur_pos ) {
                    throw std::runtime_error(
                        "Invalid entry in queue window that not in sequence with other entries. "
                        "Previous entry is " + prev_chr + ":" + std::to_string( prev_pos ) +
                        ", current (invalid) entry is " + prev_chr + ":" + std::to_string( cur_pos )
                    );
                }

                // Finally, enqueue the entry, and move to the next entry of the input,
                // as well as update all involved counters and helpers.
                using WindowEntry = typename Window::Entry;
                target.push_back(
                    WindowEntry(
                        next_index_,
                        cur_pos,
                        parent_->entry_input_function( *base_iterator_type::current_ )
                    )
                );
                ++base_iterator_type::current_;
                ++next_index_;
                ++added_count;
                prev_pos = cur_pos;

                // We check if this entry is a selected one according to the function.
                // If so, we are done here - we found the next selected entry.
                assert( ! target.empty() );
                if( parent_->entry_selection_function( target.back() )) {
                    found_selected = true;
                    break;
                }
            }
            return { added_count, found_selected };
        }

        void move_tail_buffer_to_window_()
        {
            // Move everything from the trailing list to our actual window.
            // We can move, because we are sure not to need those entries in the buffer any more.
            while( ! tail_buffer_.empty() ) {
                // The trailing ones need to be non-selected, except potentially for the very last
                // entry. This is because the add_entries_until_selected_to_queue_() functions,
                // which we use to add entries to the trail buffer, stops when it reaches the first
                // selected entry. As we have not exposed those entries to the user, they are
                // unmodified from the input data, so we can evaluate the selection function here
                // again, to assert this.
                assert(
                    ! parent_->entry_selection_function( tail_buffer_.front().data ) ||
                    tail_buffer_.size() == 1
                );

                // We also check that the chromosome is the same and the entries are in order.
                assert(
                    window_.empty() || (
                        window_.chromosome() == parent_->chromosome_function( tail_buffer_.front() ) &&
                        window_.entries().back().position <
                        parent_->position_function( tail_buffer_.front() )
                    )
                );

                // Now move the entry from the trailing list to our actual window.
                window_.entries().push_back( std::move( tail_buffer_.front() ));
                tail_buffer_.pop_front();
            }
        }

        inline bool input_chromosome_finished_() const
        {
            // Helper function used in assertions to test that the input iterator is at another
            // chromosome compared to the current window (or the end of the data).
            return (
                base_iterator_type::current_ == base_iterator_type::end_ ||
                parent_->chromosome_function( *base_iterator_type::current_ ) !=
                window_.chromosome()
            );
        }

        value_type& get_current_window_() const override final
        {
            return const_cast<value_type&>( window_ );
        }

        BaseWindowStream<InputStreamIterator, DataType> const* get_parent_() const override final
        {
            return parent_;
        }

    private:

        // Parent. Needs to live here to have the correct derived type.
        QueueWindowStream const* parent_ = nullptr;

        // Current window and its position
        Window window_;
        size_t next_index_ = 0;

        // Keep track of the current number of queued (selected) entries in the window, as well
        // as how many non-selected there are as well. We do this as follows: each item in the
        // current_queue_pop_count_ list represents one entry of our input data for which the
        // entry_selection_function returned true (i.e., one entry that we queue in the window).
        // The value of the item here then indicates the _total_ number of entries from our input
        // that came along with that one selected entry - in addition to that entry itself, this
        // is the number of all non-selected entries from our input that came before the selected
        // one (but after the previous selected one).
        // We need this to keep track of how many selected entries (the ones considered due to the
        // entry_selection_function) we have in each step of the iteration, so that we can remove
        // the right amount for each stride. This counts _all_ entries. We need this as otherwise
        // we would need to evaluate the entry_selection_function again when dequeueing, which
        // could lead to wrong results if the user has changed the data during iteration.
        // So, we only evaluate entry_selection_function when pushing items, and here keep track
        // of how many need to be popped later again.
        // For each selected entry, the queue contains the total number of entries that need to be
        // popped from the front in order to remove that selected entry. That is, it contains the
        // count of all non-selected entries before the selected one, plus that one itself.
        std::deque<size_t> current_queue_pop_count_;

        // Furthermore, in order to be able to handle the last window correclty, we need to have
        // an intermediate storage of the trailing entries up until either the next selected entry,
        // or the end of the chromosome or data. Otherwise, we could have a last window
        // that just happens to have the exact right number of selected entries, but when there are
        // no more selected entries afterwards in the input data, all not-selected ones would be
        // missed. So whenever we have filled a window with all selected entries, we need to keep
        // one next set of entries (up until the next selected one) here first.
        std::deque<typename Window::Entry> tail_buffer_;
    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    QueueWindowStream(
        InputStreamIterator begin, InputStreamIterator end,
        size_t count = 0, size_t stride = 0
    )
        : base_type( begin, end )
        , count_( count )
        , stride_( stride )
    {}

    virtual ~QueueWindowStream() override = default;

    QueueWindowStream( QueueWindowStream const& ) = default;
    QueueWindowStream( QueueWindowStream&& )      = default;

    QueueWindowStream& operator= ( QueueWindowStream const& ) = default;
    QueueWindowStream& operator= ( QueueWindowStream&& )      = default;

    friend DerivedIterator;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Number of selected entries in each Window.
     *
     * The count has to be `> 0`. This is the number of selected entries that are put into each
     * Window. Typically, we process, e.g., variant positions only - in that case, the count()
     * is the number of Variant%s in each Window. This depends on the entry_selection_function(),
     * see there for details on how the selection of entries works.
     */
    self_type& count( size_t value )
    {
        count_ = value;
        return *this;
    }

    size_t count() const
    {
        return count_;
    }

    /**
     * @brief Stride of the Window, that is, how many entries to move forward with each iteration.
     *
     * The stride his is the shift towards the next interval, determining how many entries are
     * dropped from the beginning and added to the end of each Window when iterating.
     * It has to be `<= count`. If stride is set to 0 (default), it is set automatically to the
     * count() when starting the iteration, meaning that each Window contains the next count()
     * many new entries from the underlying input iterator.
     */
    self_type& stride( size_t value )
    {
        stride_ = value;
        return *this;
    }

    size_t stride() const
    {
        return stride_;
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
        return std::unique_ptr<DerivedIterator>( new DerivedIterator( nullptr ));
        // return utils::make_unique<DerivedIterator>( nullptr );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Settings. We make stride_ mutable so that the iterator can set it to the count.
    size_t count_ = 0;
    mutable size_t stride_ = 0;

};

// =================================================================================================
//     Make Queue Window Stream
// =================================================================================================

/**
 * @brief Helper function to instantiate a QueueWindowStream
 * without the need to specify the template parameters manually.
 *
 * This still requires to set the four needed functionals for processing the input stream, as
 * described in QueueWindowStream.
 */
template<class InputStreamIterator, class DataType = typename InputStreamIterator::value_type>
QueueWindowStream<InputStreamIterator, DataType>
make_queue_window_stream(
    InputStreamIterator begin, InputStreamIterator end, size_t count = 0, size_t stride = 0
) {
    auto it = QueueWindowStream<InputStreamIterator, DataType>( begin, end );
    it.count( count );
    it.stride( stride );
    return it;
}

/**
 * @brief Helper function to instantiate a QueueWindowStream for a default use case.
 *
 * This helper assumes that the underlying type of the input data stream and of the Window%s
 * that we are sliding over are of the same type, that is, we do no conversion in the
 * `entry_input_function` functor of the QueueWindowStream. It further assumes that this
 * data type has public member variables `chromosome` and `position` that are accessed by the
 * `chromosome_function` and `position_function` functors of the QueueWindowStream.
 * For example, a data type that this works for is Variant data.
 *
 * The QueueWindowStream::entry_selection_function is set so that _all_ entries are selected to
 * be considered towards the QueueWindowStream::count() of entries per window. This can be
 * re-set afterwards if a different criterion is needed. See also
 * make_passing_variant_queue_window_stream() and
 * make_passing_variant_queue_window_view_stream() for specializations of this for data
 * type Variant, which instead only select entries that have Variant::status passing.
 *
 * @see make_default_queue_window_view_stream() wraps the returned stream in an additional
 * WindowViewStream.
 */
template<class InputStreamIterator>
QueueWindowStream<InputStreamIterator>
make_default_queue_window_stream(
    InputStreamIterator begin, InputStreamIterator end, size_t count = 0, size_t stride = 0
) {
    using DataType = typename InputStreamIterator::value_type;

    // Set functors.
    auto it = QueueWindowStream<InputStreamIterator>( begin, end );
    it.entry_input_function = []( DataType const& variant ) {
        return variant;
    };
    it.chromosome_function = []( DataType const& variant ) {
        return variant.chromosome;
    };
    it.position_function = []( DataType const& variant ) {
        return variant.position;
    };
    it.entry_selection_function = []( DataType const& variant ) {
        (void) variant;
        return true;
    };

    // Set properties.
    it.count( count );
    it.stride( stride );
    return it;
}

/**
 * @brief Helper class that creates a QueueWindowStream with default functors
 * and wraps it in a WindowViewStream.
 *
 * See make_default_queue_window_stream() for the base functionality,
 * and see make_window_view_stream() for the wrapping behaviour.
 *
 * Note that because this is a simple wrapper around the constructor of QueueWindowStream,
 * we lose access to that class itself, so that its more specialized member functions cannot be called
 * any more. If this is needed, use the two aforementioned `make_...()` functions individually.
 */
template<class InputStreamIterator>
WindowViewStream<InputStreamIterator>
make_default_queue_window_view_stream(
    InputStreamIterator begin, InputStreamIterator end, size_t count, size_t stride = 0
) {
    return make_window_view_stream(
        make_default_queue_window_stream( begin, end, count, stride )
    );
}

/**
 * @brief Helper function to instantiate a QueueWindowStream for a default use case with
 * underlying data of type Variant, where only Variant%s with passing status are selected.
 *
 * This helper assumes that the underlying type of the input data stream and of the Window%s
 * that we are sliding over are of type Variant. It is hence a more specialized version of
 * make_default_queue_window_stream(). Here, we check the Variant::status, and only
 * select those Variants%s towards the QueueWindowStream::count() of each window That
 * have a passing FilterStatus. The QueueWindowStream::entry_selection_function is
 * set accordingly.
 *
 * @see make_passing_variant_queue_window_view_stream() wraps the returned stream in an
 * additional WindowViewStream.
 */
template<class InputStreamIterator>
QueueWindowStream<InputStreamIterator>
make_passing_variant_queue_window_stream(
    InputStreamIterator begin, InputStreamIterator end, size_t count = 0, size_t stride = 0
) {
    using DataType = typename InputStreamIterator::value_type;

    // Set functors.
    auto it = QueueWindowStream<InputStreamIterator>( begin, end );
    it.entry_input_function = []( DataType const& variant ) {
        return variant;
    };
    it.chromosome_function = []( DataType const& variant ) {
        return variant.chromosome;
    };
    it.position_function = []( DataType const& variant ) {
        return variant.position;
    };
    it.entry_selection_function = []( DataType const& variant ) {
        return variant.status.passing();
    };

    // Set properties.
    it.count( count );
    it.stride( stride );
    return it;
}

/**
 * @brief Helper class that creates a QueueWindowStream with default functions for Variant
 * data, and wraps it in a WindowViewStream.
 *
 * See make_passing_variant_queue_window_stream() for the base functionality,
 * and see make_window_view_stream() for the wrapping behaviour.
 *
 * Note that because this is a simple wrapper around the constructor of QueueWindowStream,
 * we lose access to that class itself, so that its more specialized member functions cannot be called
 * any more. If this is needed, use the two aforementioned `make_...()` functions individually.
 */
template<class InputStreamIterator>
WindowViewStream<InputStreamIterator>
make_passing_variant_queue_window_view_stream(
    InputStreamIterator begin, InputStreamIterator end, size_t count, size_t stride = 0
) {
    return make_window_view_stream(
        make_passing_variant_queue_window_stream( begin, end, count, stride )
    );
}

} // namespace population
} // namespace genesis

#endif // include guard
