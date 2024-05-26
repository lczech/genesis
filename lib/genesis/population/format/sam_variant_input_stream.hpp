#ifndef GENESIS_POPULATION_FORMAT_SAM_VARIANT_INPUT_STREAM_H_
#define GENESIS_POPULATION_FORMAT_SAM_VARIANT_INPUT_STREAM_H_

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

#ifdef GENESIS_HTSLIB

#include "genesis/population/sample_counts.hpp"
#include "genesis/population/function/genome_locus.hpp"
#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/genome_locus.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/population/variant.hpp"
#include "genesis/utils/core/fs.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
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

    struct bam_pileup1_t;
    // typedef struct bam_pileup1_t *bam_pileup1_t;

}

namespace genesis {
namespace population {

// =================================================================================================
//     SAM/BAM/CRAM File Input Stream
// =================================================================================================

/**
 * @brief Input stream for SAM/BAM/CRAM files that produces a Variant per genome position.
 *
 * We expect the input file to be sorted by position.
 * Positions with no reads overlapping are skipped.
 *
 * Exemplary usage:
 *
 *     auto sam_it = SamVariantInputStream( "/path/to/file.sam" );
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
 * By default, as above, all reads are considered to be belonging to the same sample.
 * In that case hence, the above inner loop over samples will only ever go through one SampleCounts
 * object stored in the Variant.
 * We however are also able to split by read group (`@RG`), see split_by_rg() and
 * with_unaccounted_rg() for details. In that case, the Variant contains one SampleCounts object
 * per read group, as well as potentially a special one for unaccounted reads with no proper RG.
 * This can further be filtered by setting rg_tag_filter(), to only consider certain RG tags
 * as samples to be produced.
 */
class SamVariantInputStream
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type         = SamVariantInputStream;
    using value_type        = Variant;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    // ======================================================================================
    //      Sam File Handle
    // ======================================================================================

private:

    /**
     * @brief Implementation detail. Keep per-file data used by htslib/samtools.
     *
     * This allows us to keep the static functions that are needed as htslib callbacks
     * in here, so that we can apply a PIMPL that removes the need to include htslib headers
     * here, so that we do not spam our namespace with them. In particular, the htslib
     * bam_pileup_cd data structure is not properly named (only typdef'ed), so that we cannot
     * forward declare it.
     *
     * It's also convenient for our read_sam_() function, which expects a single (void)* to
     * take its data; this struct here is what we use to cast that void pointer to.
     * We could of course hand over the whole iterator instead. But this seems slighlty cleaner,
     * to just have the actual data that is needed for the file itself available in one place.
     *
     * This could all be direct members of the Iterator class instead. But we keep them here in
     * a separate structure, to make it easier in case we ever want to refactor the class to
     * accept multiple input files at the same time... probably will not happen, but who knows.
     */
    struct SamFileHandle;

    // ======================================================================================
    //      Internal Iterator
    // ======================================================================================

public:

    /**
     * @brief %Iterator over loci of the input sources.
     *
     * This is the class that does the actual work of turning the underlying file data into
     * our Variant and SampleCounts samples. Use the dereference `operator*()`
     * and `operator->()` to get the Variant at the current locus of the iteration.
     */
    class Iterator
    {
    public:

        // -------------------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------------------

        using self_type         = SamVariantInputStream::Iterator;
        using value_type        = Variant;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        using iterator_category = std::input_iterator_tag;

    private:

        Iterator() = default;
        Iterator( SamVariantInputStream const* parent );

    public:

        ~Iterator() = default;

        Iterator( self_type const& ) = default;
        Iterator( self_type&& )      = default;

        Iterator& operator= ( self_type const& ) = default;
        Iterator& operator= ( self_type&& )      = default;

        friend SamVariantInputStream;
        friend SamFileHandle;

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

        // self_type operator ++(int)
        // {
        //     auto cpy = *this;
        //     increment_();
        //     return cpy;
        // }

        /**
         * @brief Compare two iterators for equality.
         *
         * Any two iterators that are created by calling begin() on the same SamVariantInputStream
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
        //     Data Access
        // -------------------------------------------------------------------------

        /**
         * @brief Get the list of read group `RG` tags as used in the iteration,
         * or as found in the SAM/BAM/CRAM file.
         *
         * This function is useful when
         * @link SamVariantInputStream::split_by_rg( bool ) split_by_rg()@endlink is set to `true`,
         * so that the reads are split by their read group tags. The function then returns
         * the RG read group tag names, in the same order that the SampleCounts objects are stored
         * in the resulting Variant of this iterator.
         *
         * When additionally
         * @link SamVariantInputStream::with_unaccounted_rg( bool ) with_unaccounted_rg()@endlink
         * is set to `true`, an additional RG tag "unaccounted" is added to the result as a last
         * element, which is the same position that the unaccounted reads go in the Variant.
         *
         * When using rg_tag_filter() to sub-set the RG tags (samples) being processed, this
         * function by default only returnes those sample names (RG tags) that represent the
         * final SampleCounts samples of the resulting Variant when iterating the data
         * (and potentially including the "unaccounted" group).
         *
         * If @link SamVariantInputStream::split_by_rg( bool ) split_by_rg()@endlink is `false`,
         * we are not splitting by read group tags, so then this function returns an empty vector.
         * Note that the Variant that is produced during iteration still contains one SampleCounts
         * sample, which collects all counts from all reads.
         *
         * All of the above is ignored if the argument @p all_header_tags is set to `true`.
         * In that case, the function instead simply returns those RG tags that are present
         * in the SAM/BAM/CRAM header, without the "unaccounted", and without any filtering.
         * Note that in this case, this function needs to fill the vector when called. Hence,
         * if this list is needed often, it is recommended to call this function once and store
         * the result.
         */
        std::vector<std::string> rg_tags( bool all_header_tags = false ) const;

        /**
         * @brief Return the size of the Variant::sample vector of SampleCounts that is produced
         * by the iterator.
         */
        size_t sample_size() const;

        // -------------------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------------------

    private:

        /**
         * @brief Increment the iterator by moving to the next position.
         */
        void increment_();

        /**
         * @brief For a given read at the current position during incrementation,
         * tally up its base to the sample that it belongs to.
         *
         * This adds the count directly to the sample of the current_variant_ that the @p p read
         * belongs to (taking care of whether we split by RG tags or not).
         */
        void process_base_( bam_pileup1_t const* p );

        /**
         * @brief For a given pileup base, get the sample index it belongs to.
         *
         * When using RG read group tags, this corresponds to the index given by the value of
         * SamFileHandle::rg_tags elements.
         * Without RG tags, we are just using one sample, so this function just returns 0.
         */
        size_t get_sample_index_( bam_pileup1_t const* p ) const;

    private:

        // Parent.
        SamVariantInputStream const* parent_ = nullptr;

        // htslib specific file handling pointers during iteration.
        // We put this in a shared ptr so that the iterator can be copied,
        // but the htslib data gets only freed once all instances are done.
        // This also allows PIMPL to avoid including the htslib headers here.
        std::shared_ptr<SamFileHandle> handle_;

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
    SamVariantInputStream()
        : SamVariantInputStream( std::string{} )
    {}

    explicit SamVariantInputStream(
        std::string const& input_file
    )
        : SamVariantInputStream( input_file, std::unordered_set<std::string>{}, false )
    {}

    SamVariantInputStream(
        std::string const& input_file,
        std::unordered_set<std::string> const& rg_tag_filter,
        bool inverse_rg_tag_filter = false
    );

    ~SamVariantInputStream() = default;

    SamVariantInputStream( SamVariantInputStream const& ) = default;
    SamVariantInputStream( SamVariantInputStream&& )      = default;

    SamVariantInputStream& operator= ( SamVariantInputStream const& ) = default;
    SamVariantInputStream& operator= ( SamVariantInputStream&& )      = default;

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    Iterator begin() const
    {
        // Better error messaging that what htslib would give us if the file did not exist.
        // We check here, as input_file() allows to change the file after construction,
        // so we only do the check once we know that we are good to go.
        std::string err_str;
        if( ! utils::file_is_readable( input_file_, err_str )) {
            throw std::runtime_error(
                "Cannot open input sam/bam/cram file '" + input_file_ + "': " + err_str
            );
        }

        return Iterator( this );
    }

    Iterator end() const
    {
        return Iterator();
    }

    // -------------------------------------------------------------------------
    //     Basic Input Settings
    // -------------------------------------------------------------------------

    std::string const& input_file() const
    {
        return input_file_;
    }

    /**
     * @brief Set the input file.
     *
     * This overwrites the file if it was already given in the constructor.
     * Shall not be called after iteration has been started.
     */
    self_type& input_file( std::string const& value )
    {
        input_file_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Flag Settings
    // -------------------------------------------------------------------------

    uint32_t flags_include_all() const
    {
        return flags_include_all_;
    }

    /**
     * @brief Only use reads with \e all bits set in @p value present in the FLAG field of the read.
     *
     * This is equivalent to the `-f` / `--require-flags` setting in samtools view.
     *
     * The @p value can be specified in hex by beginning with `0x` (i.e., `/^0x[0-9A-F]+/`),
     * in octal by beginning with `0` (i.e., `/^0[0-7]+/`), as a decimal number not beginning
     * with '0', or as a comma-, plus-, or space-separated list of flag names. We are more lenient
     * in parsing flag names then samtools, and allow different capitalization and delimiteres
     * such as dashes and underscores in the flag names as well.
     *
     * See http://www.htslib.org/doc/samtools-flags.html and
     * https://broadinstitute.github.io/picard/explain-flags.html for details on the flag
     * values, and see https://www.htslib.org/doc/samtools-view.html for their usage
     * in samtools.
     *
     * @see flags_include_any( uint32_t ), flags_exclude_all( uint32_t ), flags_exclude_any( uint32_t )
     */
    self_type& flags_include_all( uint32_t value )
    {
        flags_include_all_ = value;
        return *this;
    }

    uint32_t flags_include_any() const
    {
        return flags_include_any_;
    }

    /**
     * @brief Only use reads with \e any bits set in @p value present in the FLAG field of the read.
     *
     * This is equivalent to the `--rf` / `--incl-flags` / `--include-flags` setting in samtools view.
     *
     * The @p value can be specified in hex by beginning with `0x` (i.e., `/^0x[0-9A-F]+/`),
     * in octal by beginning with `0` (i.e., `/^0[0-7]+/`), as a decimal number not beginning
     * with '0', or as a comma-, plus-, or space-separated list of flag names. We are more lenient
     * in parsing flag names then samtools, and allow different capitalization and delimiteres
     * such as dashes and underscores in the flag names as well.
     *
     * See http://www.htslib.org/doc/samtools-flags.html and
     * https://broadinstitute.github.io/picard/explain-flags.html for details on the flag
     * values, and see https://www.htslib.org/doc/samtools-view.html for their usage
     * in samtools.
     *
     * @see flags_include_all( uint32_t ), flags_exclude_all( uint32_t ), flags_exclude_any( uint32_t )
     */
    self_type& flags_include_any( uint32_t value )
    {
        flags_include_any_ = value;
        return *this;
    }

    uint32_t flags_exclude_all() const
    {
        return flags_exclude_all_;
    }

    /**
     * @brief Do not use reads with \e all bits set in @p value present in the FLAG field of the read.
     *
     * This is equivalent to the `-G` setting in samtools view.
     *
     * The @p value can be specified in hex by beginning with `0x` (i.e., `/^0x[0-9A-F]+/`),
     * in octal by beginning with `0` (i.e., `/^0[0-7]+/`), as a decimal number not beginning
     * with '0', or as a comma-, plus-, or space-separated list of flag names. We are more lenient
     * in parsing flag names then samtools, and allow different capitalization and delimiteres
     * such as dashes and underscores in the flag names as well.
     *
     * See http://www.htslib.org/doc/samtools-flags.html and
     * https://broadinstitute.github.io/picard/explain-flags.html for details on the flag
     * values, and see https://www.htslib.org/doc/samtools-view.html for their usage
     * in samtools.
     *
     * @see flags_include_all( uint32_t ), flags_include_any( uint32_t ), flags_exclude_any( uint32_t )
     */
    self_type& flags_exclude_all( uint32_t value )
    {

        flags_exclude_all_ = value;
        return *this;
    }

    uint32_t flags_exclude_any() const
    {
        return flags_exclude_any_;
    }

    /**
     * @brief Do not use reads with \e any bits set in @p value present in the FLAG field of the read.
     *
     * This is equivalent to the `-F` / `--excl-flags` / `--exclude-flags` setting in samtools view.
     *
     * The @p value can be specified in hex by beginning with `0x` (i.e., `/^0x[0-9A-F]+/`),
     * in octal by beginning with `0` (i.e., `/^0[0-7]+/`), as a decimal number not beginning
     * with '0', or as a comma-, plus-, or space-separated list of flag names. We are more lenient
     * in parsing flag names then samtools, and allow different capitalization and delimiteres
     * such as dashes and underscores in the flag names as well.
     *
     * See http://www.htslib.org/doc/samtools-flags.html and
     * https://broadinstitute.github.io/picard/explain-flags.html for details on the flag
     * values, and see https://www.htslib.org/doc/samtools-view.html for their usage
     * in samtools.
     *
     * @see flags_include_all( uint32_t ), flags_include_any( uint32_t ), flags_exclude_all( uint32_t )
     */
    self_type& flags_exclude_any( uint32_t value )
    {
        flags_exclude_any_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Region Filter
    // -------------------------------------------------------------------------

    /**
     * @brief Set a region filter, so that only loci set in the @p loci are used,
     * and all others are skipped.
     *
     * This still needs some basic processing per position, as we are currently not using the
     * htslib internal filters, but apply it afterwards. Still, this skips the base counting,
     * so it is an advantage over filtering later on.
     */
    self_type& region_filter( std::shared_ptr<GenomeLocusSet> region_filter )
    {
        region_filter_ = region_filter;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Quality Settings
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

    // -------------------------------------------------------------------------
    //     Depth Settings
    // -------------------------------------------------------------------------

    int min_depth() const
    {
        return min_depth_;
    }

    /**
     * @brief Set the minimum read depth (coverage) at a given position to be considered.
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
     * @brief Set the maximum read depth (coverage) at a given position to be considered.
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
     * @brief Set the maximum read depth (coverage) at a given position that is actually processed.
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

    // -------------------------------------------------------------------------
    //     Read Group Settings
    // -------------------------------------------------------------------------

    bool split_by_rg() const
    {
        return split_by_rg_;
    }

    /**
     * @brief If set to `true`, instead of reading all mapped reads as a single sample,
     * split them by the `@RG` read group tag.
     *
     * This way, multiple SampleCounts objects are created in the resulting Variant, one for each
     * read group, and potentially an additional one for the unaccounted reads that do not have a
     * read group, if with_unaccounted_rg() is also set.
     */
    self_type& split_by_rg( bool value )
    {
        split_by_rg_ = value;
        return *this;
    }

    bool with_unaccounted_rg() const
    {
        return with_unaccounted_rg_;
    }

    /**
     * @brief Decide whether to add a sample for reads without a read group,
     * when splitting by `@RG` tag.
     *
     * If split_by_rg() and this option are both set to `true`, also add a special sample for
     * the reads without a read group, as the last SampleCounts object of the Variant.
     * If this option here is however set to `false`, all reads without a read group tag or
     * with an invalid read group tag (that does not appear in the header) are ignored.
     * If split_by_rg() is not set to `true`, this option here is completely ignored.
     *
     * See also rg_tag_filter() to sub-set the reads by RG, that is, to ignore reads that have
     * a proper RG tag set, but that belong to a sample that shall be ignored.
     */
    self_type& with_unaccounted_rg( bool value )
    {
        with_unaccounted_rg_ = value;
        return *this;
    }

    std::unordered_set<std::string> const& rg_tag_filter() const
    {
        return rg_tag_filter_;
    }

    /**
     * @brief Set the sample names used for filtering reads by their RG read group tag.
     *
     * Only used when split_by_rg() is set to `true`.
     * Reads that have an RG read group tag that appears in the header of the input file,
     * but is not present in the @p value list given here (or in the constructor of the class),
     * will be ignored. That is, they will also not appear in the "unaccounted" sample,
     * independently of the setting of with_unaccounted_rg(). The unaccounted sample will only
     * contain data from those reads that do not have an RG tag at all, or one that does not
     * appear in the header.
     *
     * See also inverse_rg_tag_filter() to inverse this setting. That is, instead of only using
     * samples based on the RG tags given in this list here, use all _but_ the given RG tags.
     *
     * When the given @p value list is empty, the filtering by RG read group tag is deactivated
     * (which is also the default), independently of the inverse_rg_tag_filter() setting.
     */
    self_type& rg_tag_filter( std::unordered_set<std::string> const& value )
    {
        rg_tag_filter_ = value;
        return *this;
    }

    bool inverse_rg_tag_filter() const
    {
        return inverse_rg_tag_filter_;
    }

    /**
     * @brief Reverse the meaning of the list of sample names given by rg_tag_filter().
     *
     * See there for details.
     */
    self_type& inverse_rg_tag_filter( bool value )
    {
        inverse_rg_tag_filter_ = value;
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

    // Read filtering flags, as used by htslib and samtools
    uint32_t flags_include_all_ = 0;
    uint32_t flags_include_any_ = 0;
    uint32_t flags_exclude_all_ = 0;
    uint32_t flags_exclude_any_ = 0;

    // Filter for which positions to consider or skip
    std::shared_ptr<GenomeLocusSet> region_filter_;

    // Minimum mapping and base qualities
    int min_map_qual_ = 0;
    int min_base_qual_ = 0;

    // Read depth / coverage
    int min_depth_ = 0;
    int max_depth_ = 0;
    int max_acc_depth_ = 0;

    bool split_by_rg_ = false;
    bool with_unaccounted_rg_ = false;
    std::unordered_set<std::string> rg_tag_filter_;
    bool inverse_rg_tag_filter_ = false;

    // Unused / need to investiage if needed
    // int max_indel_depth_ = 250;
    // bool skip_empty_positions_ = true;
    // bool detect_overlapping_read_pairs_ = false;

};

} // namespace population
} // namespace genesis

#endif // htslib guard
#endif // include guard
