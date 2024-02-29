#ifndef GENESIS_POPULATION_STREAMS_VARIANT_GAPLESS_INPUT_STREAM_H_
#define GENESIS_POPULATION_STREAMS_VARIANT_GAPLESS_INPUT_STREAM_H_

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

#include "genesis/population/functions/functions.hpp"
#include "genesis/population/functions/genome_locus.hpp"
#include "genesis/population/genome_locus.hpp"
#include "genesis/population/streams/variant_input_stream.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/sequence/reference_genome.hpp"
#include "genesis/sequence/sequence_dict.hpp"

#include <cassert>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Variant Gapless Input Stream
// =================================================================================================

/**
 * @brief Stream adapter that visits every position in the genome.
 *
 * The iterator takes some other VariantInputStream as input. It then iterates _all_ positions
 * in the chromosomes of that input, starting at 1, and until the last position per chromosome
 * of the input. All positions where the input does not have data (missing) instead dereference to
 * a dummy Variant that is set up with the same number of samples as the input.
 *
 * If additionally a reference genome or sequence dictionary is provided, the chromosomes are further
 * iterated for the full length as specified in these references. This expects that the input data
 * does not contain positions beyond the reference (otherwise, an exception is thrown), and we also
 * check that the reference genome bases are compatible with the bases provided by the input data
 * (the Variant::reference_base).
 *
 * Furthermore, if a reference genome or sequence dictionary is provided, and iterate_extra_chromosomes()
 * is set to `true` (which it is by default), we also iterate any chromosomes that appear in the
 * reference but not in the input data at all (of course, all of them will then only contain missing
 * data). This makes sure that the full reference is iterated over.
 *
 * The iterator is useful in siutations where input is expected to have missing data, so that it's
 * skipped by its iterator, but some external algorithm or processing wants to use all the positions.
 * For instance, when writing a sync file, this can be used to make a "gsync" file that contains
 * all positions, instead of skipping missing data positions.
 */
class VariantGaplessInputStream
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type         = VariantGaplessInputStream;
    using value_type        = Variant;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

    /**
     * @brief %Iterator over loci of the input source.
     */
    class Iterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type         = VariantGaplessInputStream::Iterator;
        using value_type        = Variant;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        using iterator_category = std::input_iterator_tag;

    public:

        Iterator() = default;

    private:

        Iterator( VariantGaplessInputStream* parent );

    public:

        ~Iterator() = default;

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        friend VariantGaplessInputStream;

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        Variant const * operator->() const
        {
            return &current_variant_();
        }

        // Variant * operator->()
        // {
        //     return &current_variant_();
        // }

        Variant const & operator*() const
        {
            return current_variant_();
        }

        // Variant & operator*()
        // {
        //     return current_variant_();
        // }

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

        // self_type operator ++(int)
        // {
        //     auto cpy = *this;
        //     advance_();
        //     return cpy;
        // }

        operator bool() const
        {
            return parent_ != nullptr;
        }

        /**
         * @brief Compare two iterators for equality.
         *
         * Any two iterators that are created by calling begin() on the same LambdaStream
         * instance will compare equal, as long as neither of them is past-the-end.
         * A valid (not past-the-end) iterator and an end() iterator will not compare equal,
         * no matter from which LambdaStream they were created.
         */
        bool operator==( self_type const& it ) const
        {
            return parent_ == it.parent_;
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
            assert( parent_ );

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
                start_chromosome_();
            }

            // Now we have everything to populate our variant as needed.
            prepare_current_variant_();
        }

        /**
         * @brief Set up everything at the beginning of a new chromosome.
         *
         * This can be either when the input data moves to the next chromosome, or, once we are done
         * with that, be a chromosome that is only provided by the ref genome or seq dict, in case
         * that iterate_extra_chromosomes_ is set.
         *
         * The function expects current_locus_ to be set to position 1 on the new chromosome.
         * It then does some consistenty checks and sets up the ref genome or seq dict for the new
         * chromosome.
         */
        void start_chromosome_();

        /**
         * @brief Find the next locus to process.
         *
         * This function modifies current_locus_, and sets it to the next position of the
         * iteration. If set to an empty locus, we reached the end. If set to position 1, this
         * means we are starting a new chromosome, and the caller needs to act accordingly.
         * If necessary, the input data iterator is also advanced.
         */
        void advance_current_locus_();

        /**
         * @brief Move to the next locus on a chromosome after the input iterator does not have
         * data for it any more.
         *
         * This is called when there is no more data in the input iterator for the chromosome where
         * we are currently at. In that case, we move along chromosomes and positions in the
         * ref genome or seq dict that were not in the input data.
         *
         * Once we are done with that, this function then might move on to the extra chromosomes
         * provided in the ref genome or seq dict for which the input did not have any data at all.
         * Whether that is done depends on the iterate_extra_chromosomes_ setting.
         */
        void advance_current_locus_beyond_input_();

        /**
         * @brief Return whether the ref dict genome or seq dict has more positions on the current
         * chromosome.
         */
        bool has_more_ref_loci_on_current_chromosome_();

        /**
         * @brief Find the next chromosome of the ref genome or seq dict that isn't in the input
         * data and has not been processed yet.
         *
         * If iterate_extra_chromosomes_ is set, we want to iterate over extra chromosomes.
         * Here, we find the next one that has not been done yet. If there are no more, and we are
         * done with all chromosomes, this function returns an empty string. Also, this function
         * might be called if neither ref gneome or seq dict are given. In that case as well,
         * the returned string is empty, indicating that there is nothing more to do. This works
         * because we disallow empty chromosome names in actual data.
         */
        std::string find_next_extra_chromosome_();

        /**
         * @brief Check the current locus and set up the Variant for it.
         *
         * This assumes that current_locus_ is already at the position that we want to be at for
         * the current iteration. This locus is then prepared, by setting properties of the Variant.
         */
        void prepare_current_variant_();

        /**
         * @brief Prepare the reference base of the Variant for the current locus.
         *
         * This checks the samples and checks or sets the reference base.
         * Everything else is already set by the other functions.
         */
        void prepare_current_variant_ref_base_();

        /**
         * @brief Check the iterator for validity, that is, not empty. Throw otherwise.
         */
        void check_input_iterator_();

        /**
         * @brief Get the Variant at the current position.
         *
         * Either points to the input iterator Variant, or our missing variant.
         */
        Variant& current_variant_()
        {
            if( current_variant_is_missing_ ) {
                return missing_variant_;
            } else {
                assert( iterator_ );
                return *iterator_;
            }
        }

        /**
         * @brief Get the Variant at the current position.
         *
         * Either points to the input iterator Variant, or our missing variant.
         */
        Variant const& current_variant_() const
        {
            if( current_variant_is_missing_ ) {
                return missing_variant_;
            } else {
                assert( iterator_ );
                return *iterator_;
            }
        }

    private:

        // Parent
        VariantGaplessInputStream* parent_ = nullptr;

        // Keep track of the locus that the iterator currently is at.
        GenomeLocus current_locus_;

        // Is the current variant missing? If so, we are using the dummy missing_variant_,
        // otherwise the one of the input iterator.
        bool current_variant_is_missing_ = false;

        // Storage for the missing variants of the iterators. This serves as a dummy variant
        // for all positions of the input without data, so that we do not need to re-allocate
        // every time for this. It is initialized to zero counts for all base counts.
        // In each iteration step, the position and refrence base are updated.
        // To allow manupulation or move-from, we reset the whole Variant each time though.
        // To this end, we store the number of samples here as well.
        Variant missing_variant_;
        size_t num_samples_;

        // Keep the iterators that we want to traverse. We only need the begin() iterators,
        // as they are themselves able to tell us if they are still good (via their operator bool).
        VariantInputStream::Iterator iterator_;

        // Cache for the ref genome and seq dict sequences, so that we do not have to find them
        // on every iteration here.
        ::genesis::sequence::ReferenceGenome::const_iterator ref_genome_it_;
        ::genesis::sequence::SequenceDict::const_iterator seq_dict_it_;

        // We keep track of which chromosomes we have seen yet, in order to avoid errors due to
        // unordered input, and also to know which chromosomes to process later for the case that
        // iterate_extra_chromosomes() is set and there are unprocessed chromosomes left.
        std::unordered_set<std::string> processed_chromosomes_;
    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    VariantGaplessInputStream() = default;

    VariantGaplessInputStream(
        VariantInputStream const& input
    )
        : input_( input )
    {}

    VariantGaplessInputStream(
        VariantInputStream&& input
    )
        : input_( std::move( input ))
    {}

    ~VariantGaplessInputStream() = default;

    VariantGaplessInputStream( VariantGaplessInputStream const& ) = default;
    VariantGaplessInputStream( VariantGaplessInputStream&& )      = default;

    VariantGaplessInputStream& operator= ( VariantGaplessInputStream const& ) = default;
    VariantGaplessInputStream& operator= ( VariantGaplessInputStream&& )      = default;

    friend Iterator;

    // -------------------------------------------------------------------------
    //     Input
    // -------------------------------------------------------------------------

    VariantInputStream const& input() const
    {
        return input_;
    }

    VariantInputStream& input()
    {
        return input_;
    }

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    /**
     * @brief Begin the iteration.
     */
    Iterator begin()
    {
        if( started_ ) {
            throw std::runtime_error( "Cannot start VariantGaplessInputStream multiple times" );
        }
        started_ = true;
        return Iterator( this );
    }

    /**
     * @brief End marker for the iteration.
     */
    Iterator end()
    {
        return Iterator( nullptr );
    }

    // ---------------------------------------------------------------------
    //     Settings
    // ---------------------------------------------------------------------

    bool iterate_extra_chromosomes() const
    {
        return iterate_extra_chromosomes_;
    }

    /**
     * @brief Determine whether extra chromosomes without any data in the input are itereated.
     *
     * If a reference_genome() or sequence_dict() is provided, there might be chromosomes in there
     * that do not appear in the input data at all. With this setting, which is `true` by default,
     * these chromosomes are iterated over, of course solely consisting of missing data then.
     * If set to false, these are skipped instead and the iteration is ended with the end of the data.
     */
    self_type& iterate_extra_chromosomes( bool value )
    {
        iterate_extra_chromosomes_ = value;
        return *this;
    }

    /**
     * @brief Get the currently set reference genome to be used for the chromosome lengths and bases.
     */
    std::shared_ptr<::genesis::sequence::ReferenceGenome> reference_genome() const
    {
        return ref_genome_;
    }

    /**
     * @brief Set a reference genome to be used for the chromosome lengths and bases.
     *
     * When provided, this is used to determine the length of each chromosome during iteration,
     * as well as the reference base at each position.
     *
     * If iterate_extra_chromosomes() is set (`true` by default), this also is used to determine
     * chromosomes that are not in the input at all, and iterate those as well (consisting solely
     * of missing data then, of course).
     *
     * For simplicity, reference_genome() and sequence_dict() cannot be used at the same time.
     */
    self_type& reference_genome( std::shared_ptr<::genesis::sequence::ReferenceGenome> value )
    {
        if( value && seq_dict_ ) {
            throw std::invalid_argument(
                "Cannot set reference_genome() in VariantGaplessInputStream "
                "when sequence_dict() is already provided."
            );
        }
        ref_genome_ = value;
        return *this;
    }

    /**
     * @brief Get the currently set sequence dictionary used for the chromosome lengths.
     */
    std::shared_ptr<genesis::sequence::SequenceDict> sequence_dict() const
    {
        return seq_dict_;
    }

    /**
     * @brief Set a sequence dictionary to be used for the chromosome lengths.
     *
     * See @link reference_genome(std::shared_ptr<::genesis::sequence::ReferenceGenome>) reference_genome()@endlink
     * for details. Using a SequenceDict is similar, but without the ability to infer reference
     * bases at the positions along the genome. Other than that, it behaves the same.
     * For simplicity, sequence_dict() and reference_genome() cannot be used at the same time.
     */
    self_type& sequence_dict( std::shared_ptr<genesis::sequence::SequenceDict> value )
    {
        if( value && ref_genome_ ) {
            throw std::invalid_argument(
                "Cannot set sequence_dict() in VariantGaplessInputStream "
                "when reference_genome() is already provided."
            );
        }
        seq_dict_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    VariantInputStream input_;
    bool started_ = false;

    // We offer two ways of specifying chromosome lengths.
    // With ref genome, we additionally gain access to the bases.
    std::shared_ptr<::genesis::sequence::ReferenceGenome> ref_genome_;
    std::shared_ptr<::genesis::sequence::SequenceDict> seq_dict_;
    bool iterate_extra_chromosomes_ = true;

};

} // namespace population
} // namespace genesis

#endif // include guard
