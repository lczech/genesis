#ifndef GENESIS_POPULATION_FORMATS_SAM_VARIANT_INPUT_ITERATOR_H_
#define GENESIS_POPULATION_FORMATS_SAM_VARIANT_INPUT_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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

#include "genesis/population/base_counts.hpp"
#include "genesis/population/functions/genome_locus.hpp"
#include "genesis/population/genome_locus.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/variant.hpp"

#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <cstdint>
#include <string>
#include <vector>

// Forward declarations of htslib structs, so that we do not have to include their headers
// and completely spam our namespace here.
extern "C" {
    struct htsFile;
    struct hts_itr_t;
    struct sam_hdr_t;
    struct bam1_t;

    struct bam_plp_s;
    typedef struct bam_plp_s *bam_plp_t;

    // struct bam_pileup1_t;
    // typedef struct bam_pileup1_t *bam_pileup1_t;
}

namespace genesis {
namespace population {

// =================================================================================================
//     SAM/BAM/CRAM File Input iterator
// =================================================================================================

/**
 * @brief Input iterator for SAM/BAM/CRAM files that produces a Variant per genome position.
 *
 * We expect the input file to be sorted by position.
 * Positions with no reads overlapping are skipped.
 *
 * Exemplary usage:
 *
 *     auto sam_it = SamVariantInputIterator( "/path/to/file.sam" );
 *     sam_it.min_map_qual( 40 );
 *     for( auto const& var : sam_it ) {
 *         std::cout << var.chromosome << "\t" << var.position << "\t";
 *         for( auto const& bs : var.samples ) {
 *             std::cout << "\t";
 *             to_sync( bs, std::cout );
 *         }
 *         std::cout << "\n";
 *     }
 *
 * At the moment, the class produces a single BaseCounts object as part of the Variant, that
 * countains the accumulated counts at the current locus. In the future, we might extend this
 * to be able to split by read group (`@RG`), or offer to read multiple files at the same time
 * (similar to samtools mpileup). We hence recommend to use the above way of accessing the samples.
 */
class SamVariantInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = SamVariantInputIterator;
    using value_type        = Variant;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

    /**
     * @brief %Iterator over loci of the input sources.
     *
     * This is the class that does the actual work. Use the dereference `operator*()`
     * and `operator->()` to get the Variant at the current locus() of the iteration.
     */
    class Iterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type         = SamVariantInputIterator::Iterator;
        using value_type        = Variant;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        using iterator_category = std::input_iterator_tag;

    private:

        Iterator() = default;
        Iterator( SamVariantInputIterator const* parent )
            : parent_( parent )
        {
            init_();
        }

    public:

        ~Iterator();

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        friend SamVariantInputIterator;

        // -------------------------------------------------------------------------
        //     Accessors
        // -------------------------------------------------------------------------

        value_type const * operator->() const
        {
            return &current_variant_;
        }

        value_type * operator->()
        {
            return &current_variant_;
        }

        value_type const & operator*() const
        {
            return current_variant_;
        }

        value_type & operator*()
        {
            return current_variant_;
        }

        // -------------------------------------------------------------------------
        //     Iteration
        // -------------------------------------------------------------------------

        self_type& operator ++ ()
        {
            increment_();
            return *this;
        }

        self_type operator ++(int)
        {
            auto cpy = *this;
            increment_();
            return cpy;
        }

        /**
         * @brief Compare two iterators for equality.
         *
         * Any two iterators that are created by calling begin() on the same SamVariantInputIterator
         * instance will compare equal, as long as neither of them is past-the-end.
         * A valid (not past-the-end) iterator and an end() iterator will not compare equal.
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
        //     Internal Structs
        // -------------------------------------------------------------------------

    private:

        /**
         * @brief Keep per-file data used by htslib/samtools.
         */
        struct SamFileHandle
        {
            // Our main class, for access to settings.
            SamVariantInputIterator const* parent;

            // File handle.
            ::htsFile*   hts_file = nullptr;

            // File header.
            ::sam_hdr_t* sam_hdr  = nullptr;

            // Current iterator.
            bam_plp_t iter;
            // ::hts_itr_t* hts_iter = nullptr;
        };

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

    private:

        /**
         * @brief Initialize the iterator, and start with the first position.
         */
        void init_();

        /**
         * @brief Increment the iterator by moving to the next position.
         */
        void increment_();

        /**
         * @brief Function needed for htslib to process a single read mapped in sam/bam/cram format.
         *
         * This function processed a read, tests its mapping quality and flags, and only lets those
         * reads pass that we actually want to consider for the pileup/variant processing at a given
         * position.
         */
        static int read_sam_( void* data, bam1_t* b );

    private:

        // Parent.
        SamVariantInputIterator const* parent_ = nullptr;

        // htslib specific file handling pointers during iteration.
        SamFileHandle handle_;

        // Current variant object, keeping the base tally of the current locus.
        Variant current_variant_;

    };

    // ======================================================================================
    //      Main Class
    // ======================================================================================

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Create a default instance, with no input. This is also the past-the-end iterator.
     */
    SamVariantInputIterator()
        : SamVariantInputIterator( std::string{} )
    {}

    explicit SamVariantInputIterator(
        std::string const& input_file
    );

    ~SamVariantInputIterator() = default;

    SamVariantInputIterator( SamVariantInputIterator const& ) = delete;
    SamVariantInputIterator( SamVariantInputIterator&& )      = default;

    SamVariantInputIterator& operator= ( SamVariantInputIterator const& ) = delete;
    SamVariantInputIterator& operator= ( SamVariantInputIterator&& )      = default;

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    Iterator begin() const
    {
        return Iterator( this );
    }

    Iterator end() const
    {
        return Iterator();
    }

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    int min_map_qual() const
    {
        return min_map_qual_;
    }

    /**
     * @brief Set the minimum phred-scaled mapping quality score for a read in the input file
     * to be considered.
     *
     * Any read that is below the given value of mapping quality will be completely discarded,
     * and its bases not taken into account.
     */
    self_type& min_map_qual( int value )
    {
        min_map_qual_ = value;
        return *this;
    }

    int min_base_qual() const
    {
        return min_base_qual_;
    }

    /**
     * @brief Set the minimum phred-scaled per-base quality score for a nucleotide to be considered.
     *
     * Any base that has a quality score below the given value is not taken into account in the
     * per-position tally of counts.
     */
    self_type& min_base_qual( int value )
    {
        min_base_qual_ = value;
        return *this;
    }

    int min_depth() const
    {
        return min_depth_;
    }

    /**
     * @brief Set the minimum depth (coverage) at a given position to be considered.
     *
     * Positions in the genome with fewer than the given minimum depth are skipped.
     */
    self_type& min_depth( int value )
    {
        min_depth_ = value;
        return *this;
    }

    int max_depth() const
    {
        return max_depth_;
    }

    /**
     * @brief Set the maximum depth (coverage) at a given position to be considered.
     *
     * Positions in the genome with more than the given minimum depth are skipped.
     * If set to 0 (default), the value is not used as a threshold.
     */
    self_type& max_depth( int value )
    {
        max_depth_ = value;
        return *this;
    }

    int max_accumulation_depth() const
    {
        return max_acc_depth_;
    }

    /**
     * @brief Set the maximum depth (coverage) at a given position that is actually processed.
     *
     * The max_depth() setting excludes sites that have depth/coverage above a given value.
     * However, one might want to keep those sites in the iteration, and yet limit the number of
     * bases being tallied up. This setting is mostly meant as a memory saver, in order to avoid
     * piling up too many sites at the same time. When set to a value greater than 0, only
     * that many bases are considered, and any further reads overlapping the site are not
     * taken into account.
     */
    self_type& max_accumulation_depth( int value )
    {
        max_acc_depth_ = value;
        return *this;
    }

    // bool skip_empty_positions() const
    // {
    //     return skip_empty_positions_;
    // }
    //
    // self_type& skip_empty_positions( bool value )
    // {
    //     skip_empty_positions_ = value;
    //     return *this;
    // }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // Input data
    std::string input_file_;

    // Settings

    // Read filtering flags, as used by htslib
    // TODO: need to make those accessible for being settable by the user
    uint32_t flags_;

    // Minimum mapping and base qualities
    int min_map_qual_ = 0;
    int min_base_qual_ = 0;

    // Read depth / coverage
    int min_depth_ = 0;
    int max_depth_ = 0;
    int max_acc_depth_ = 0;

    // Unused / need to investiage if needed
    // int max_indel_depth_ = 250;
    // bool skip_empty_positions_ = true;
    // bool detect_overlapping_read_pairs_ = false;

};

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard
