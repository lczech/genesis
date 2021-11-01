#ifndef GENESIS_POPULATION_FORMATS_VARIANT_PARALLEL_INPUT_ITERATOR_H_
#define GENESIS_POPULATION_FORMATS_VARIANT_PARALLEL_INPUT_ITERATOR_H_

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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/formats/variant_input_iterator.hpp"
#include "genesis/population/functions/genome_locus.hpp"
#include "genesis/population/genome_locus.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/utils/containers/optional.hpp"

#include <cassert>
#include <functional>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//      Variant Parallel Input Iterator
// =================================================================================================

/**
 * @brief Iterate multiple input sources that yield Variant%s in parallel.
 *
 * This iterator allows to traverse multiple sources of data in parallel, where each stop of the
 * traversal is a Locus in the input sources.
 * Using @link VariantParallelInputIterator::ContributionType ContributionType@endlink,
 * one can select the contribution of loci of each input, that is, whether all its loci get used,
 * or just the ones that also overlap with other input sources. See also
 * @link VariantParallelInputIterator::add_carrying_locus() add_carrying_locus()@endlink for
 * other ways to specify the loci to iterate over.
 *
 * At each visited locus, the iterator yields the data of the underlying input sources as a vector
 * of @link genesis::utils::Optional Optional@endlink Variant%s, with one Variant per input source.
 * If a source does not have data at the current locus,
 * the @link genesis::utils::Optional Optional@endlink is empty.
 * Use the dereference `operator*()` and `operator->()` of the iterator or the access functions
 * variants() and variant_at() to get the set of variants at the current locus() of the iteration,
 * or use joined_variant() to get one Variant that has all sample BaseCounts joined into it.
 *
 * Furthermore, using the @link VariantParallelInputIterator::inputs() inputs()@endlink and
 * @link VariantParallelInputIterator::input_at( size_t ) input_at()@endlink functions, which are
 * also available from the iterator itself, one can access additional information about the
 * underlying iterators, such as the file name and sample names that are being read.
 * This is particularly useful if input sources are added as in the example below, where we use
 * functions such as make_variant_input_iterator_from_pileup_file() to get access to the files,
 * which encapsulate and hence would otherwise hide this information from us.
 * See VariantInputIteratorData for the data structure that is used to store these
 * additional information, and see VariantInputIterator for details on the underlying iterator.
 *
 * Example:
 *
 * ~~~{.cpp}
 *     // Add input sources to a parallel iterator, one carrying, so that all its loci are visited,
 *     // and one following, meaning that its loci are only visited if the first one also
 *     // as those loci.
 *     auto parallel = VariantParallelInputIterator();
 *     parallel.add_variant_input_iterator(
 *         make_variant_input_iterator_from_pileup_file( "path/to/file.pileup.gz" ),
 *         VariantParallelInputIterator::ContributionType::kCarrying
 *     );
 *     parallel.add_variant_input_iterator(
 *         make_variant_input_iterator_from_sync_file( "path/to/file.sync" ),
 *         VariantParallelInputIterator::ContributionType::kFollowing
 *     );
 *
 *     for( auto it = parallel.begin(); it != parallel.end(); ++it ) {
 *         // Work with the iterator, which stops at every locus of the first input source.
 *         std::cout << "At: " << it.locus() << "\n";
 *         for( auto const& var : *it ) {
 *             if( var ) {
 *                 // The optional has data, and the variant is valid, which means that
 *                 // the input has data at the current locus.
 *                 // For example, get the number of samples of that variant
 *                 // (which has to be constant over the genome for each input -
 *                 // we do not allow inputs to change their number of samples during iteration).
 *                 auto const s = var->samples.size();
 *             }
 *         }
 *
 *         // Or get all data combined into one Variant.
 *         auto const joined_var = it.joined_variant();
 *     }
 *
 *     // or
 *
 *     for( auto const& opt_variant : parallel ) {
 *         // Work directly with the std::vector<utils::Optional<Variant>> opt_variant that is
 *         // returned by dereferencing the iterator. Note that this does not allow access to other
 *         // members of the iterator, such as its locus() and joined_variant() functions.
 *     }
 * ~~~
 *
 * See the VariantParallelInputIterator::Iterator class for details on access to the data
 * during traversal.
 */
class VariantParallelInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Select which loci of an input are used.
     *
     * We offer two ways an input can be traversed over: Either take all its loci (carrying),
     * or only those which also appear in other inputs as well (following).
     *
     * For the most part, the `kCarrying` type acts as a set union of the input loci;
     * all loci of all sources that are added with that type get visited.
     * The `kFollowing` type on the other hand does not contribute its unique loci
     * (i.e., the ones that are private to itself / do not appear in any other input source),
     * but also does not change or constrain the ones that are visited by the carrying inputs.
     *
     * A notable case happens if all inputs are added as type `kFollowing`:
     * In the absence of a carrying set of loci, only those loci are visited that are in _all_
     * inputs; in other words, in this case, the `kFollowing` type acts as an intersection of loci.
     *
     * This model does not allow more complex subset operations of loci, such as intersections,
     * complements, (symmetrical) differences, and exclusions. For these cases, one can use the
     * add_carrying_locus() and add_carrying_loci() functions that allow a pre-defined set of
     * loci to be iterated over.
     */
    enum class ContributionType
    {
        /**
         * @brief For a given input, stop at all its positions.
         *
         * Other input sources that do not have data at these loci will then have the
         * @link genesis::utils::Optional Optional@endlink be empty in the iterator at this locus.
         */
        kCarrying,

        /**
         * @brief For a given input, only stop at positions where other inputs also want to stop.
         *
         * In other words, this input does not contribute the loci that are unique to it
         * to the traversal, but contributes its data only at the loci that are visited by
         * others (or has an empty @link genesis::utils::Optional Optional@endlink Variant,
         * if it does not have data at a visited Locus).
         */
        kFollowing
    };

    using self_type         = VariantParallelInputIterator;
    using value_type        = Variant;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

    /**
     * @brief %Iterator over loci of the input sources.
     *
     * This is the class that does the actual work. Use the dereference `operator*()`
     * and `operator->()` or the access functions variants() and variant_at() to get the set of
     * variants at the current locus() of the iteration, or use joined_variant() to get one
     * Variant that has all sample BaseCounts joined into it.
     */
    class Iterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type         = VariantParallelInputIterator::Iterator;
        using value_type        = std::vector<utils::Optional<Variant>>;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        using iterator_category = std::input_iterator_tag;

    private:

        Iterator() = default;
        Iterator( VariantParallelInputIterator* generator );

    public:

        ~Iterator() = default;

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        friend VariantParallelInputIterator;

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        std::vector<utils::Optional<Variant>> const * operator->() const
        {
            return &variants_;
        }

        std::vector<utils::Optional<Variant>> * operator->()
        {
            return &variants_;
        }

        std::vector<utils::Optional<Variant>> const & operator*() const
        {
            return variants_;
        }

        std::vector<utils::Optional<Variant>> & operator*()
        {
            return variants_;
        }

        /**
         * @brief Return the data of all input iterators at the current locus.
         *
         * Any input sources that do not have data at the current locus() have an empty
         * optional in the vector.
         */
        std::vector<utils::Optional<Variant>> const& variants() const
        {
            return variants_;
        }

        /**
         * @copydoc ::genesis::population::VariantParallelInputIterator::Iterator::variants() const
         */
        std::vector<utils::Optional<Variant>>& variants()
        {
            return variants_;
        }

        /**
         * @brief Get access to the input iterators that have been added to this parallel iterator.
         *
         * This is just a concenience function that forwards the
         * @link VariantParallelInputIterator::inputs() inputs()@endlink function of
         * VariantParallelInputIterator. See there for details.
         */
        std::vector<VariantInputIterator> const& inputs() const
        {
            // We assume that the user only does this when the iterator is not an end() iterator.
            assert( generator_ );
            return generator_->inputs_;
        }

        /**
         * @brief Get access to an input iterator that has been added to this parallel iterator.
         *
         * This is just a concenience function that forwards the
         * @link VariantParallelInputIterator::input_at() input_at()@endlink function of
         * VariantParallelInputIterator. See there for details.
         */
        VariantInputIterator const& input_at( size_t index ) const
        {
            return generator_->inputs_[index];
        }

        /**
         * @brief Return the data of the input iterators at the given @p index at the current locus.
         *
         * The indexing follows the order in which inputs have been added to the
         * VariantParallelInputIterator.
         * See also VariantParallelInputIterator::input_size() to get their count.
         *
         * An input source that do not have data at the current locus() has an empty optional.
         */
        utils::Optional<Variant> const& variant_at( size_t index ) const
        {
            // Return with boundary check.
            return variants_.at( index );
        }

        /**
         * @copydoc ::genesis::population::VariantParallelInputIterator::Iterator::variant_at( size_t ) const
         */
        utils::Optional<Variant>& variant_at( size_t index )
        {
            // Return with boundary check.
            return variants_.at( index );
        }

        /**
         * @brief Create a single Variant instance that combines all Variant%s from the input
         * sources at the current locus.
         *
         * This joins all BaseCounts of all Variant%s of the input sources at the current locus.
         * For sources that have no data at the current position, as many empty BaseCounts
         * (with all zero counts) are inserted as the iterator has samples; hence, the number
         * of BaseCounts in the Variant::samples of the returned Variant (as indicated by
         * Variant.samples.size()) is kept consistent at each locus.
         *
         * By default, we expect that the Variant%s of each iterator have the same
         * Variant::reference_base; if not, the function throws an exception.
         * For the Variant::alternative_base, by default we allow different bases, as not every
         * file format contains alternative bases, meaning that it might be set to `'N'` instead
         * of the actual value in those file formats.
         * To change the default behaviour, use @p allow_ref_base_mismatches and/or
         * @p allow_alt_base_mismatches as needed. When a mismatch is allowed, in cases of
         * a mismatch, the returned Variant will contain an `'N'` as the base.
         *
         * Lastly, by default, we copy the BaseCounts of all Variant::samples into the resulting
         * Variant. If however these are not needed at the current iterator position any more
         * (that is, if this iterator is not dereferenced or the variants() function is not called)
         * after calling this function, we can instead move them, for efficiency.
         */
        Variant joined_variant(
            bool allow_ref_base_mismatches = false,
            bool allow_alt_base_mismatches = true,
            bool move_samples = false
        );

        /**
         * @brief Return the current locus where the iteration is at.
         */
        GenomeLocus const& locus() const
        {
            return current_locus_;
        }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++ ()
        {
            advance_();
            return *this;
        }

        self_type operator ++(int)
        {
            auto cpy = *this;
            advance_();
            return cpy;
        }

        operator bool() const
        {
            return generator_ != nullptr;
        }

        /**
         * @brief Compare two iterators for equality.
         *
         * Any two iterators that are created by calling begin() on the same LambdaIterator
         * instance will compare equal, as long as neither of them is past-the-end.
         * A valid (not past-the-end) iterator and an end() iterator will not compare equal,
         * no matter from which LambdaIterator they were created.
         */
        bool operator==( self_type const& it ) const
        {
            return generator_ == it.generator_;
        }

        bool operator!=( self_type const& it ) const
        {
            return !(*this == it);
        }

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

    private:

        /**
         * @brief Move to the next locus.
         */
        void advance_()
        {
            // Some basic checks.
            assert( generator_ );
            assert( generator_->inputs_.size() == generator_->selections_.size() );
            assert( generator_->inputs_.size() == iterators_.size() );

            // Depending on what type of inputs we have, we need two different algorithms
            // to find the next position to iterate to.
            if( generator_->has_carrying_input_ ) {
                advance_using_carrying_();
            } else {
                advance_using_only_following_();
            }
        }

        /**
         * @brief Advance the iterator, for the case that at least one of them is carrying.
         */
        void advance_using_carrying_();

        /**
         * @brief Special case for advancing for when all inputs are of type following.
         *
         * The above advance_using_carrying_() does not work in that case, as we have no guaranteed
         * stopping positions, and hence need a different approach.
         */
        void advance_using_only_following_();

        /**
         * @brief Increment an iterator by one position,
         * and check that the chromosome and position are good and their order is correct.
         */
        void increment_iterator_( VariantInputIterator::Iterator& iterator );

        /**
         * @brief Helper function to assert that each iterator at each position has a valid
         * chromosome name and position value.
         */
        void assert_correct_chr_and_pos_( VariantInputIterator::Iterator const& iterator );

        /**
         * @brief Set the variants_ to the data of their iterator variants
         * if the iterators are at the current locus (and hence have data at that locus),
         * or to empty (optional) data otherwise.
         */
        void update_variants_();

    private:

        // Parent
        VariantParallelInputIterator* generator_ = nullptr;

        // Keep track of the locus that the iterator currently is at.
        // Not all sources have to be there (if they don't have data for that locus), in which case
        // we want them to be at the next position in their data beyond the current locus.
        GenomeLocus current_locus_;

        // Keep the iterators that we want to traverse. We only need the begin() iteratos,
        // as they are themselves able to tell us if they are still good (via their operator bool).
        std::vector<VariantInputIterator::Iterator> iterators_;

        // We need to store how many samples (BaseCounts objects) the Variant of each iterator has,
        // in order to fill in the empty ones at the iterator positions where they don't have data.
        // We cannot always look that up from the iterators themselves, as they might already have
        // reached their end of the data while others are still having data, so we store it here.
        std::vector<size_t> variant_sizes_;
        size_t variant_size_sum_;

        // Storage for the variants of the iterators. We need these copies, as not all iterators
        // are expected to have all loci in the genome, so if we'd instead gave access to the
        // iterators directly to the user of this class, they'd have to check if the iterator is at
        // the correct locus, and so on. So instead, we offer a user-friendly interface that they
        // can simply iterator over and check if the optional is empty or not. Bit of copying,
        // but then again, each layer of abstraction comes at some cost...
        // At least, we move (not copy) data into here, for efficiency.
        std::vector<utils::Optional<Variant>> variants_;

        // Store the current additional carrying locus that we are at (if those have been
        // added; if not, we just store the end iterator here).
        std::set<GenomeLocus>::const_iterator carrying_locus_it_;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    VariantParallelInputIterator() = default;
    ~VariantParallelInputIterator() = default;

    VariantParallelInputIterator( self_type const& ) = default;
    VariantParallelInputIterator( self_type&& )      = default;

    self_type& operator= ( self_type const& ) = default;
    self_type& operator= ( self_type&& )      = default;

    friend Iterator;

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    /**
     * @brief Begin the iteration.
     *
     * Use this to obtain an VariantParallelInputIterator::Iterator that starts traversing
     * the input sources.
     */
    Iterator begin()
    {
        return Iterator( this );
    }

    /**
     * @brief End marker for the iteration.
     */
    Iterator end()
    {
        return Iterator( nullptr );
    }

    // -------------------------------------------------------------------------
    //     Input Sources
    // -------------------------------------------------------------------------

    /**
     * @brief Add an input to the parallel iterator.
     */
    self_type& add_variant_input_iterator(
        VariantInputIterator const& input,
        ContributionType selection
    ) {
        inputs_.emplace_back( input );
        selections_.emplace_back( selection );
        assert( inputs_.size() == selections_.size() );

        if( selection == ContributionType::kCarrying ) {
            has_carrying_input_ = true;
        }
        return *this;
    }

    /**
     * @brief Add an input to the parallel iterator.
     *
     * This version of the function takes the function to obtain elements from the underlying
     * data iterator, same as VariantInputIterator. See there and LambdaIterator for details.
     */
    self_type& add_variant_input(
        std::function<utils::Optional<Variant>()> input_element_generator,
        ContributionType selection
    ) {
        add_variant_input_iterator( VariantInputIterator( input_element_generator ), selection );
        return *this;
    }

    /**
     * @brief Get access to the input iterators that have been added to this parallel iterator.
     */
    std::vector<VariantInputIterator> const& inputs() const
    {
        return inputs_;
    }

    /**
     * @brief Get access to the input iterators that have been added to this parallel iterator.
     *
     * This non-const version of the function can for exmple be used to bulk-add filters
     * and transformations to the iterators, using their functions
     * @link utils::LambdaIterator::add_transform() add_transform()@endlink,
     * @link utils::LambdaIterator::add_filter() add_filter()@endlink, and
     * @link utils::LambdaIterator::add_transform_filter() add_transform_filter()@endlink;
     * see utils::LambdaIterator for details.
     */
    std::vector<VariantInputIterator>& inputs()
    {
        return inputs_;
    }

    /**
     * @brief Get access to an input iterator that has been added to this parallel iterator.
     */
    VariantInputIterator const& input_at( size_t index ) const
    {
        return inputs_[index];
    }

    /**
     * @brief Get access to an input iterator that has been added to this parallel iterator.
     */
    VariantInputIterator& input_at( size_t index )
    {
        return inputs_[index];
    }

    /**
     * @brief Return the number of input sourced added.
     */
    size_t input_size() const
    {
        assert( inputs_.size() == selections_.size() );
        return inputs_.size();
    }

    // -------------------------------------------------------------------------
    //     Input Loci
    // -------------------------------------------------------------------------

    /**
     * @brief Add a set of @link GenomeLocus GenomeLoci@endlink that are used as carrying loci
     * in the iteration.
     *
     * This allows to iterate over a pre-defined set of loci. The iterator stops at each of these
     * loci, independently of whether any of the underlying input sources have data at this locus.
     * That means, it acts as an "empty" input that only contributes loci, as if it were added
     * with ContributionType::kCarrying, but without any actual variants.
     * Duplicate loci in these additional carrying loci are ignored.
     *
     * Using this is particularly useful for more complex subset operations of loci, such as
     * intersections, complements, (symmetrical) differences, and exclusions. These cases cannot
     * be modelled with our simple ContributionType based approach; so instead, one can externally
     * prepare the list of loci that need to be visited, and provide these to this function.
     * In these cases, to use _exactly_ the list of provided loci, all actual input sources can
     * be added as ContributionType::kFollowing, to make sure that none of them adds additional
     * loci to the traversal.
     *
     * Note that in addition to the loci added via this function, all loci of input sources that are
     * of ContributionType::kCarrying are also visited.
     */
    self_type& add_carrying_locus( GenomeLocus const& locus )
    {
        // Error check.
        if( locus.chromosome.empty() || locus.position == 0 ) {
            throw std::invalid_argument(
                "Cannot add a carrying locus with empty chromosome or position 0 "
                "to VariantParallelInputIterator"
            );
        }

        // Add to the list. Also, if loci are added with this function, these serve as carrying loci,
        // and so we can always use advance_using_carrying_() to find the next locus;
        // mark this by setting has_carrying_input_.
        carrying_loci_.insert( locus );
        has_carrying_input_ = true;
        return *this;
    }

    /**
     * @brief Add a set of @link GenomeLocus GenomeLoci@endlink that are used as carrying loci
     * in the iteration.
     *
     * @see VariantParallelInputIterator::add_carrying_locus( GenomeLocus const& )
     */
    self_type& add_carrying_loci( std::vector<GenomeLocus> const& loci )
    {
        add_carrying_loci( loci.begin(), loci.end() );
        return *this;
    }

    /**
     * @copydoc add_carrying_loci( std::vector<GenomeLocus> const& )
     */
    template<class ForwardIterator>
    self_type& add_carrying_loci( ForwardIterator first, ForwardIterator last )
    {
        while( first != last ) {
            add_carrying_locus( *first );
            ++first;
        }

        // Version for if we wanted to switch the set for a vector.
        // Sort the list of loci. All this is so inefficient, as we store the chromosome names
        // again and again for each locus. The sorting is okay though, we need to have that
        // complexity somewhere - using a std::set for example would just shift the place where
        // we do the sorting, but would make iteration a bit more tricky, and would need even more
        // memory.
        // std::sort( carrying_loci_.begin(), carrying_loci_.end() );
        // carrying_loci_.erase(
        //     std::unique( carrying_loci_.begin(), carrying_loci_.end() ),
        //     carrying_loci_.end()
        // );

        return *this;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Store all input sources, as well as the type (carrying or following) of how we want
    // to traverse them. We keep track whether at least one of them is of type carrying.
    // If not (all following), the advance function of the iterator needs to be special.
    std::vector<VariantInputIterator> inputs_;
    std::vector<ContributionType> selections_;
    bool has_carrying_input_ = false;

    // Store all additional loci that we want to include as stops in the iterator.
    // Memory-wise, this is highly inefficient, as we store the chromosome name for each of them.
    // But for now, this is easiest and fastest. We use a set, so that adding loci one after another
    // always results in a sorted container, without having to re-sort every time.
    // This again has a bit of a higher memory impact, but that should be okay for now.
    std::set<GenomeLocus> carrying_loci_;

};

} // namespace population
} // namespace genesis

#endif // include guard
