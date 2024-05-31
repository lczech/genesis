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

#include "genesis/population/format/sam_variant_input_stream.hpp"

#include "genesis/population/function/functions.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>

extern "C" {
    #include <htslib/cram.h>
    #include <htslib/hts.h>
    #include <htslib/khash.h>
    #include <htslib/kstring.h>
    #include <htslib/sam.h>
}

namespace genesis {
namespace population {

// =================================================================================================
//     Sam File Handle
// =================================================================================================

struct SamVariantInputStream::SamFileHandle
{
public:

    // This whole class is private to its parent class (and the iterator).
    // To keep variable naming in line, we hence use underscores for all variables here,
    // despite them being public in the context of this class itself.

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

    // Our main class, for access to settings.
    SamVariantInputStream const* parent_;

    // File handle.
    ::htsFile* hts_file_ = nullptr;

    // File header.
    ::sam_hdr_t* sam_hdr_ = nullptr;

    // Current iterator. The `bam_plp_t` type is in fact a pointer. Messy htslib.
    bam_plp_t iter_ = nullptr;
    // ::hts_itr_t* hts_iter = nullptr;

    // List of the @RG read group tags as present in the header, filtered by any potential
    // rg_tag_filter() and inverse_rg_tag_filter() settings. That is, this maps from
    // RG tags to their index of the SampleCounts object in the Variant that is produced
    // by this iterator at each position. We use a map in this direction for speed,
    // as we get the RG tag name from htslib for each read.
    std::unordered_map<std::string, size_t> rg_tags_;

    // We additionally keep a plain list around, just for the iterator to access.
    // This list might include the "unaccounted" sample as well as a string, which is not part
    // of the above map. Hence, this list here is purely meant for user output.
    std::vector<std::string> target_sample_names_;

    // We furthermore store the number of SampleCounts samples needed in each Variant,
    // as a result of the rg_tag splitting. With no splitting, this is 1, as we then only
    // ever produce one sample, containing the base counts of all reads, independently of
    // their RG tag.
    // With splitting, this depends on rg_tag_filter() and with_unaccounted_rg() settings.
    // See init_() for details.
    size_t target_sample_count_;

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

    /**
     * @brief Init the handle, by creating all htslib structures, and the RG tag list if needed.
     *
     * To be called only when we actually have a file. Hence, this function is called from
     * the Iterator, once it has done its own checks.
     *
     * If split_by_rg() is set, this function also sets the list of all `@RG` read group tags
     * to be used while iterating. The function creates the map has the read group tags as keys,
     * and their index in the RG list of the input file as values. This makes it possible
     * to do a fast index lookup for each read, given its RG tag in the file.
     */
    void init_( SamVariantInputStream const* parent );

    /**
     * @brief Get all `@RG` read group tags that are present in the header of the input file.
     */
    std::vector<std::string> get_header_rg_tags_() const;

    /**
     * @brief Destructor needs to destroy all htslib pointers.
     *
     * We keep this data in a shared pointer in the Iterator, so that any copies of the
     * iterator (which are usually happening incidentally when creating the loop for
     * iteration) don't accidentally destroy the htslib structs already.
     * That was a bug that we had, and this is how we fixed it.
     */
    ~SamFileHandle();

    // -------------------------------------------------------------------------
    //     Static Callbacks
    // -------------------------------------------------------------------------

    /**
     * @brief Function needed for htslib to process a single read mapped in sam/bam/cram format.
     *
     * This function processed a read, tests its mapping quality and flags, and only lets those
     * reads pass that we actually want to consider for the pileup/variant processing at a given
     * position.
     */
    static int read_sam_( void* data, bam1_t* b );

    /**
     * @brief Store the RG read group tag of the bam record in the callball client data structure.
     *
     * This data will then be used by get_sample_index_() of the iterator to get the sample
     * index to write the base tally to, for each read, without having to obtain the RG tag from
     * the bam data anew for every base. Major speedup.
     */
    static int pileup_cd_create_( void* data, bam1_t const* b, bam_pileup_cd* cd );

    /**
     * @brief Destroy the client data for htslib.
     *
     * Currently does nothing, as we do not allocate anything in the creating function.
     */
    static int pileup_cd_destroy_( void* data, bam1_t const* b, bam_pileup_cd* cd );

};

// =================================================================================================
//     Sam File Handle Members
// =================================================================================================

// -------------------------------------------------------------------------
//     init_
// -------------------------------------------------------------------------

void SamVariantInputStream::SamFileHandle::init_( SamVariantInputStream const* parent )
{
    // ----------------------------------
    //     General Setup
    // ----------------------------------

    // Set the pointer to parent.
    // We only create a handle if we have an actual file to process; that is, this function should
    // not be called from Iterator when its a past-the-end default constructed Iterator that
    // does not have any file.
    parent_ = parent;
    assert( ! parent_->input_file_.empty() );

    // Open the file and read its header.
    // We use hts_open() here instead of sam_open(), as the latter is a macro whose expansion
    // differs between htslib and samtools - WTF???
    // In htslib, it simply forwards to hts_open() anyway, so let's just do that directly.
    hts_file_ = hts_open( parent_->input_file_.c_str(), "r" );
    if( ! hts_file_ ) {
        throw std::runtime_error( "Cannot open file " + parent_->input_file_ );
    }
    sam_hdr_ = sam_hdr_read( hts_file_ );
    if( ! sam_hdr_ ) {
        throw std::runtime_error( "Cannot read header of file " + parent_->input_file_ );
    }

    // Init the iterator, and set the max depth, to keep memory usage limited.
    // Not sure that we also need to do the additional max depth check during plp porcessing below,
    // but also doesn't hurt to keep it in there.
    iter_ = bam_plp_init(
        read_sam_,
        static_cast<void*>( this )
    );
    if( ! iter_ ) {
        throw std::runtime_error( "Cannot initialize to traverse file " + parent_->input_file_ );
    }
    if( parent_->max_acc_depth_ > 0 ) {
        bam_plp_set_maxcnt( iter_, parent_->max_acc_depth_ );
    }
    // assert( hts_iter == nullptr );
    // hts_iter = sam_itr_queryi(idx[i], HTS_IDX_START, 0, 0);

    // Once here, all is set up.
    assert( parent_ );
    assert( hts_file_ );
    assert( sam_hdr_ );
    assert( iter_ );

    // ----------------------------------
    //     RG Tag Setup
    // ----------------------------------

    // If wanted, use the RG tags of the header to set the samples that we want to use,
    // including any filtering if wanted, and taking care of the unaccounted setting as well.
    // If not, we just init to one sample that catches all reads independently of their RG tag.

    // Prepare result. This is its default anyway, but why not.
    rg_tags_.clear();
    target_sample_names_.clear();
    target_sample_count_ = 0;

    // If we do not want to split by RG tag, we simply leave rg_tag empty,
    // and set the target count to 1, as in that case, we only ever produce one sample,
    // containing the base counts of all reads, independently of their RG tag.
    if( ! parent_->split_by_rg_ ) {

        // Some error checks. If we do not split, some settings shall not be set.
        if(
            ! parent_->rg_tag_filter_.empty() ||
            parent_->inverse_rg_tag_filter_ ||
            parent_->with_unaccounted_rg_
        ) {
            throw std::runtime_error(
                "Input settings for filtering samples based on their RG tag are set in the "
                "SAM/BAM/CRAM reader, but the RG tag splitting is not activated in the reader."
            );
        }

        // Set the target count to 1. We keep the target_sample_names_ empty in this case.
        target_sample_count_ = 1;
        return;
    }

    // From here on, we know that we want to use the @RG read group tags from the header,
    // and set the rg_tags_ to the samples (rg tags) that are there, including any potential
    // filtering.

    // Get all RG read group tags from the header, and add the tags we want to the list.
    // We check that the filter only uses tags that are actually in the header,
    // by making a copy of the filter tags, and removing the ones that are in the header.
    // Anything that remains is an error.
    auto tags = get_header_rg_tags_();
    auto filter_tags_copy = parent->rg_tag_filter_;
    for( auto& tag : tags ) {

        // Remove the tag from the filter tag list copy if present. This also works if the tag
        // is not actually present in the set (erase just doesn't do anything in that case),
        // which of course it might be, as not all tags will be in the filter (otherwise,
        // we'd not filter at all, or filter everything).
        // We do this here before the tag has been moved from below.
        filter_tags_copy.erase( tag );

        // Check if we want to include this rg tag. If not, we indicate that by setting
        // the index to max, so that the function where samples are assessed can ignore them.
        // This also makes sure that the rg tag filter setting is independent of the unaccounted
        // filter, as we still will have listed all proper sample names from the header in the
        // rg_tags_ list, so that when iterating, we can distinguish between tags that appear
        // in the header but are filtered out, and those that are actually unaccounted for or
        // are lacking the RG tag altogether.
        if(
            (   parent_->rg_tag_filter_.empty() ) ||
            ( ! parent_->inverse_rg_tag_filter_ && parent_->rg_tag_filter_.count( tag ) >  0 ) ||
            (   parent_->inverse_rg_tag_filter_ && parent_->rg_tag_filter_.count( tag ) == 0 )
        ) {
            // We set the index to the current target count, which only counts the valid samples
            // that we do not want to filter out, meaning that each entry gets the index according
            // to the how many-th valid element in the result map it is. Also add it to the list
            // of names. We can now move it, as the tag is not needed any more.
            rg_tags_.emplace( tag, target_sample_count_ );
            target_sample_names_.emplace_back( std::move( tag ));
            ++target_sample_count_;

        } else {
            // RG tags that we want to filter out get assigned max int,
            // as an indicator that the reading step shall skip them.
            rg_tags_.emplace( std::move( tag ), std::numeric_limits<size_t>::max() );
        }
    }

    // Make sure that all given filters are actually valid according to the header.
    // If any name remains in the list of copies, we have an error.
    if( filter_tags_copy.size() > 0 ) {
        // Nice output of some tags in the header. We need to fetch the tags from the header again,
        // as we moved from the `tags` list in the loop above. But this is the error case,
        // so the cost for this is fine.
        tags = get_header_rg_tags_();
        std::string hd_tags_msg;
        if( tags.empty() ) {
            hd_tags_msg = " Header does not contain any RG tags; there can hence be no filtering.";
        } else {
            hd_tags_msg = " First @RG tag that appears in the header: \"" + (*tags.begin()) + "\".";
        }

        // Error. The filter tags are not empty, as we checked in the condition above.
        assert( ! filter_tags_copy.empty() );
        throw std::runtime_error(
            "Invalid list of @RG read group tags given for filtering the SAM/BAM/CRAM file, "
            "which do not occur in the @RG list in the header of the file." + hd_tags_msg +
            " First offending RG tag that appears in the given filter list, "
            "but not in the header: \"" + (*filter_tags_copy.begin()) + "\"."
        );
    }

    // After the above, we have added all tags to our map. We moved the strings, but the `tags`
    // vector size is still the same, so let's assert this.
    assert( rg_tags_.size() == tags.size() );
    assert( parent_->split_by_rg_ );

    // If we use the unaccounted group, we need to make room for that additional sample.
    if( parent_->with_unaccounted_rg_ ) {
        target_sample_names_.emplace_back( "unaccounted" );
        ++target_sample_count_;
    }
    assert( target_sample_names_.size() == target_sample_count_ );

    // Finally, set the callback functions that take care of finding the RG group per read.
    // By only determining the RG tag once per read and storing it in the cliend data of the
    // pileup, we have a tremendous speedup compared to our previous implementation where
    // we determiend the RG tag for every base of every read again and again.
    // See https://github.com/samtools/htslib/issues/1417#issuecomment-1088398131 for details.
    // Functionality adapted from
    // https://github.com/samtools/samtools/blob/4be69864304f4e7ac0113fdff9e4ff764b9d9267/bam_plcmd.c#L554-L557
    // https://github.com/samtools/samtools/blob/4be69864304f4e7ac0113fdff9e4ff764b9d9267/bam_plcmd.c#L336-L351
    // https://github.com/samtools/samtools/blob/4be69864304f4e7ac0113fdff9e4ff764b9d9267/bam_plcmd.c#L76
    // as mentioned in the GitHub issue (but we use permalinks here, for future stability).
    // We currently do not use the destructor, as we have nothing to free.
    bam_plp_constructor( iter_, pileup_cd_create_ );
    // bam_plp_destructor(  iter_, pileup_cd_destroy_ );
}

// -------------------------------------------------------------------------
//     get_header_rg_tags_
// -------------------------------------------------------------------------

std::vector<std::string> SamVariantInputStream::SamFileHandle::get_header_rg_tags_() const
{
    // Inspired by https://github.com/samtools/samtools/blob/2ece68ef9d0bd302a74c952b55df1badf9e61aae/bam_split.c#L217

    // Prepare result.
    assert( parent_ );
    assert( sam_hdr_ );
    std::vector<std::string> result;

    // Get the number of RG tags in the header of the file.
    auto const n_rg = sam_hdr_count_lines( sam_hdr_, "RG" );
    if( n_rg < 0 ) {
        throw std::runtime_error(
            "Failed to get @RG ID tags in file " + parent_->input_file_ +
            ". Cannot split by RG read group tags."
        );
    }

    // Go through all RG tags and store them.
    kstring_t id_val = KS_INITIALIZE;
    for( size_t i = 0; i < static_cast<size_t>( n_rg ); ++i ) {

        // Get the next tag.
        if( sam_hdr_find_tag_pos( sam_hdr_, "RG", i, "ID", &id_val ) < 0 ) {
            ks_free( &id_val );
            throw std::runtime_error(
                "Failed to get @RG ID tags in file " + parent_->input_file_
            );
        }

        // Get the name of this rg tag. Need to free it afterwards ourselves.
        // We need to turn it into a string anyways, so let's do this here.
        char* tag = ks_release( &id_val );
        result.emplace_back( tag );
        free( tag );
    }

    return result;
}

// -------------------------------------------------------------------------
//     Destructor
// -------------------------------------------------------------------------

SamVariantInputStream::SamFileHandle::~SamFileHandle()
{
    // if( mpileup_ ) {
    //     bam_mplp_destroy( mpileup_ );
    //     mpileup_ = nullptr;
    // }
    // if( handle.hts_iter ) {
    //     // destroy( handle.hts_iter );
    //     // handle.hts_iter = nullptr;
    // }

    if( iter_ ) {
        bam_plp_destroy( iter_ );
        iter_ = nullptr;
    }
    if( sam_hdr_ ) {
        sam_hdr_destroy( sam_hdr_ );
        sam_hdr_ = nullptr;
    }
    if( hts_file_ ) {
        hts_close( hts_file_ );
        hts_file_ = nullptr;
    }
}

// =================================================================================================
//     Sam File Handle Static Members
// =================================================================================================

// -------------------------------------------------------------------------
//     read_sam_
// -------------------------------------------------------------------------

// static
int SamVariantInputStream::SamFileHandle::read_sam_(
    void* data, bam1_t* bam
) {
    // The function processes a single mapped read. Reads that make it through here are then
    // used by htslib for pileup processing, and subsequencly by us in the increment_() function
    // to build our Variant object from it. Inspired from bam2depth and bedcov programs.
    // Comment from bam2depth: read level filters better go here to avoid pileup.

    // Data in fact is a pointer to our handle.
    auto handle = static_cast<SamVariantInputStream::SamFileHandle*>( data );
    assert( handle );
    assert( handle->parent_ );
    assert( handle->hts_file_ );
    assert( handle->sam_hdr_ );

    // Loop until we find a read that we want to use.
    int ret;
    while( true ) {

        // Read the data. Not sure why we need two behaviours depending on iter, but this is how it
        // is done in samtools bam2depth, just that we here use the sam instead of the bam functions.
        // int ret = handle->hts_iter
        //     ? sam_itr_next( handle->hts_file_, handle->hts_iter, bam )
        //     : sam_read1( handle->hts_file_, handle->sam_hdr_, bam )
        // ;

        // Get the read, and check result.
        // According to htslib, 0 on success, -1 on EOF, and <-1 on error, so let's check this.
        ret = sam_read1( handle->hts_file_, handle->sam_hdr_, bam );
        if( ret == -1 ) {
            break;
        }
        if( ret < -1 ) {
            throw std::runtime_error( "Error reading file " + handle->parent_->input_file() );
        }

        // Define flag shorthands for readability.
        auto const& flags_in_all = handle->parent_->flags_include_all_;
        auto const& flags_in_any = handle->parent_->flags_include_any_;
        auto const& flags_ex_all = handle->parent_->flags_exclude_all_;
        auto const& flags_ex_any = handle->parent_->flags_exclude_any_;

        // Check per-read flags. Skip reads that match one of the conditions.
        if( flags_in_all && (( bam->core.flag & flags_in_all ) != flags_in_all )) {
            // from sam_view.c
            // keep   if (FLAG & N) == N             (all on)
            // int flag_on;
            // case 'f': settings.flag_on |= bam_str2flag(optarg); break;
            // if ( (b->core.flag & settings->flag_on) != settings->flag_on )
            //     return 1; // skip read

            continue;
        }
        if( flags_in_any && (( bam->core.flag & flags_in_any ) == 0 )) {
            // from sam_view.c
            // keep   if (FLAG & N) != 0             (any on)
            // int flag_anyon;
            // case LONGOPT('g'):
            //     settings.flag_anyon |= bam_str2flag(optarg); break;
            // if (settings->flag_anyon && ((b->core.flag & settings->flag_anyon) == 0))
            //   return 1; // skip read

            continue;
        }
        if( flags_ex_all && (( bam->core.flag & flags_ex_all ) == flags_ex_all )) {
            // from sam_view.c
            // reject if (FLAG & N) == N             (any off)
            // NB I think the "any off" in the previous line is wrong in samtools,
            // and should be "all off"?!
            // But the usage of "off" is misleading... we are looking for on bits...
            // int flag_alloff;
            // case 'G': settings.flag_alloff |= bam_str2flag(optarg); break;
            // if (settings->flag_alloff && ((b->core.flag & settings->flag_alloff) == settings->flag_alloff))
            //     return 1; // skip read

            continue;
        }
        if( flags_ex_any && (( bam->core.flag & flags_ex_any ) != 0 )) {
            // from sam_view.c
            // keep   if (FLAG & N) == 0             (all off)
            // int flag_off;
            // case 'F': settings.flag_off |= bam_str2flag(optarg); break;
            // if (b->core.flag & settings->flag_off)
            //     return 1; // skip read

            continue;
        }

        // Check minimum mapping quality as well.
        if( static_cast<int>( bam->core.qual ) < handle->parent_->min_map_qual_ ) {
            continue;
        }
        break;
    }
    return ret;
}

// -------------------------------------------------------------------------
//     pileup_cd_create_
// -------------------------------------------------------------------------

// static
int SamVariantInputStream::SamFileHandle::pileup_cd_create_(
    void* data, bam1_t const* b, bam_pileup_cd* cd
) {
    // Data in fact is a pointer to our handle, same as for the read function above.
    auto handle = static_cast<SamVariantInputStream::SamFileHandle*>( data );

    // Only called when splitting by read groups.
    assert( handle );
    assert( handle->parent_ );
    assert( handle->parent_->split_by_rg_ );

    // Details inspired by
    // https://github.com/samtools/samtools/blob/2ece68ef9d0bd302a74c952b55df1badf9e61aae/bam_split.c#L476

    // Look up the RG tag of the current read.
    // We have two fail cases: the tag is not in the header, or there is no tag at all.
    // Either way, we make our decision dependend on whether we want to use the unaccounted
    // reads or not. We could further distinguish between the cases, and offer a choice
    // to throw an exception if the header does not contain all tags of the reads...
    // Maybe do that if needed later.

    // Init a lookup into our tag index list, with its end as an indicator of failure.
    // It will stay at this unless we find a proper RG index.
    auto tag_itr = handle->rg_tags_.end();

    // Get RG tag from read and look it up in hash to find its index.
    // Not obvious from htslib documentatin at all, but let's hope that this
    // is the correct way to do this...
    uint8_t* tag = bam_aux_get( b, "RG" );
    if( tag != nullptr ) {
        // Unfortunately, it seems that we have to take the detour via the string
        // value of the tag here, instead of htslib directly offering the index.
        // Hence all the shenannigans with the unordered map of indices...
        // Also, please don't ask me why the htslib function for this is called bam_aux2Z().
        // I guess Z is their notation for a string, and a tag such as RG is an "auxiliary" thing,
        // so this function means "tag 2 string", in a sense?!
        char* rg = bam_aux2Z( tag );
        tag_itr = handle->rg_tags_.find( std::string( rg ));

        // Potential future extension: RG tag in read is not in the header.
        // That indicates that something is wrong with the file. For now, we just treat this
        // as an unaccounted read.
        // if( tag_itr == handle->rg_tags_.end() ) {
        //     throw std::runtime_error(
        //         "Invalid @RG tag in read that is not listed in the header. "
        //         "Offending tag: '" + std::string( rg ) + "' in file " +
        //         parent_->input_file_
        //     );
        // }
    }

    size_t smp_idx = 0;
    if( tag_itr != handle->rg_tags_.end() ) {
        // We found the RG tag index.
        // Assert that it is within the bounds - if we also use unaccounted,
        // the base counts contains an additional element which should never be found
        // by the index lookup. If we are here, we also need to have at least one actual
        // sample plus potentially the unaccounted one, otherwise we would not be here.
        // A special case are samples ignored due to being filtered by sample_names;
        // in that case, their index is set to max, to indicate to the caller of this function
        // to ignore them.
        smp_idx = tag_itr->second;
        if( handle->parent_->with_unaccounted_rg_ ) {
            assert(
                smp_idx < handle->target_sample_count_ - 1 ||
                smp_idx == std::numeric_limits<size_t>::max()
            );
        } else {
            assert(
                smp_idx < handle->target_sample_count_ ||
                smp_idx == std::numeric_limits<size_t>::max()
            );
        }
    } else {
        // One of the two error cases occurred.
        // Decide based on whether we want to use unaccounted reads or skip them.
        if( handle->parent_->with_unaccounted_rg_ ) {
            // If we are here, we have at least initialized the samples to have the
            // unaccounted base counts object.
            // Get the last base counts object, which is the unaccounted one.
            assert( handle->target_sample_count_ > 0 );
            smp_idx = handle->target_sample_count_ - 1;
        } else {
            // If we are here, we have an unaccounted read and want to skip it.
            // Use max size_t to indicate that.
            smp_idx = std::numeric_limits<size_t>::max();
        }
    }

    // Final check. Then, we convert max to -1 for storage, as the bam_pileup_cd::i field
    // is signed. We do not want to use -1 internally here, in order to keep unsigned integer
    // comparisons for the indices simple.
    assert(
        smp_idx < handle->target_sample_count_ ||
        smp_idx == std::numeric_limits<size_t>::max()
    );
    if( smp_idx == std::numeric_limits<size_t>::max() ) {
        cd->i = -1;
    } else {
        cd->i = smp_idx;
    }

    return 0;
}

// -------------------------------------------------------------------------
//     pileup_cd_destroy_
// -------------------------------------------------------------------------

// static
int SamVariantInputStream::SamFileHandle::pileup_cd_destroy_(
    void* data, bam1_t const* b, bam_pileup_cd* cd
) {
    // Nothing to do, as we only use bam_pileup_cd::i, which does not need freeing.
    // We keep this function around for reference, in case it's needed later.
    (void) data;
    (void) b;
    (void) cd;
    return 0;
}

// =================================================================================================
//     Iterator Public Members
// =================================================================================================

// -------------------------------------------------------------------------
//     Concstructor
// -------------------------------------------------------------------------

SamVariantInputStream::Iterator::Iterator( SamVariantInputStream const* parent )
    : parent_( parent )
    , handle_( std::make_shared<SamFileHandle>() )
{
    // The constructor needs to be here in the cpp, as we just defined SamFileHandle above.
    // Otherwise, the init for the shared pointer to handle_ above cannot work.

    // Assert that the nucleotide codes in htslib are as we expect them here.
    // This should be a static assertion, but C char* does not support that,
    // so instead we do it once here on construction - an acceptable overhead.
    assert( strcmp( seq_nt16_str, "=ACMGRSVTWYHKDBN" ) == 0 );

    // Edge case: empty iterator,
    assert( parent_ );
    assert( handle_ );
    if( parent_->input_file_.empty() ) {
        return;
    }

    // Initialize the data structures of the handle. This sets up all htslib data strcutures,
    // and if needed the RG tag filtering.
    handle_->init_( parent_ );

    // Finally, get the first line.
    increment_();
}

// -------------------------------------------------------------------------
//     rg_tags
// -------------------------------------------------------------------------

std::vector<std::string> SamVariantInputStream::Iterator::rg_tags( bool all_header_tags ) const
{
    // Previously, this function used to rely on a re-filling of the list of tags, using
    // the settings of the parent_ SamVariantInputStream. However, in cases where the per-read
    // filter settings would fitler out _all_ reads (e.g., super high min map qual filter),
    // the iterator would already reach its end when being intitialized, as no read would ever be
    // used, and hence parent_ would already be nullptr when this function here was called from
    // the user. So no we do not rely on the parent_ any more, and instead store the list
    // in the handle already upon construction.
    // In the future, it might still be necessary to fix the underlying issue, i.e., the iterator
    // being ended, and the parent being null, if the use case of this class changes. But for
    // now, it seems that the approach still works, so we keep it that way.

    assert( handle_ );
    if( all_header_tags ) {
        // Special case, if we just want to get all the rg tags from the file header.
        // This is a bit trippy in case that the iteration already finished... In that case,
        // this iterator will already have it's parent_ pointer nulled, but the handle_ will not,
        // so that we can use this to get the rg header tags again from the file.
        // In hindsight, it might have been better to not use the parent_ == nullptr as an indicator
        // that we have finished iteration... On the other hand, this is a pretty good way to
        // ensure that no part of the iterator is called accidentally afterwards, as this will
        // immediately lead to failure.
        return handle_->get_header_rg_tags_();
    }

    // Normal case: get the tags that are acutally used, potentially after splitting and filtering.
    return handle_->target_sample_names_;
}

// -------------------------------------------------------------------------
//     sample_size
// -------------------------------------------------------------------------

size_t SamVariantInputStream::Iterator::sample_size() const
{
    assert( handle_ );
    return handle_->target_sample_count_;
}

// =================================================================================================
//     Iterator Private Members
// =================================================================================================

// -------------------------------------------------------------------------
//     increment_
// -------------------------------------------------------------------------

void SamVariantInputStream::Iterator::increment_()
{
    // Only to be called when the iterator is still valid (not past-the-end).
    assert( parent_ );
    assert( handle_ );
    assert( handle_->parent_ );
    assert( handle_->sam_hdr_ );
    assert( handle_->iter_ );

    // Find the next input position that we want to consider (it has data and fitting read depth,
    // and is not filtered out due to the region list).
    // tid is the chromosome name index, pos the position on the chromosome, and n is read depth ("coverage")
    int tid, pos, n;
    bam_pileup1_t const* plp;
    while( true ) {
        plp = bam_plp_auto( handle_->iter_, &tid, &pos, &n );

        // Check for end of the iteration and error.
        // Unfortunately, we cannot check the internal iter->error here, as the iter type bam_plp_t
        // is only defined in `sam.c`, and hence not accessible to us here. So instead, any errors
        // will be printed to stderr by htslib, and we will instead just end our iteration here,
        // not being able to know whether this is due to the end of the file or an error.
        // Typical errors that we would want to catch here are that the reads or chromosomes are not
        // sorted, in which case an iterator such as this one here does not work.
        // Users will get the stderr output that hints at that though, which is something.
        if( plp == nullptr ) {
            parent_ = nullptr;
            return;
        }
        if( tid < 0 || n < 0 ) {
            continue;
        }

        // Region filter, used only if one was provided.
        if(
            parent_->region_filter_ &&
            ! parent_->region_filter_->is_covered(
                std::string( handle_->sam_hdr_->target_name[tid] ),
                static_cast<size_t>( pos ) + 1
            )
        ) {
            continue;
        }

        // Coverage / read depth check.
        if( parent_->min_depth_ && n < parent_->min_depth_ ) {
            continue;
        }
        if( parent_->max_depth_ && n > parent_->max_depth_ ) {
            continue;
        }
        break;
    }

    // htslib takes care of ordering along chromosomes already, so let's just assert this.
    // If the position is not incrementing, that means we switched chromosome.
    // We do not assert that chromosomes are in correct lexicographical order, as apparently
    // that is not necessary for the correct functioning of htslib sam iteration.
    // assert( std::string( handle_->sam_hdr_->target_name[tid] ) >= current_variant_.chromosome);
    assert( pos >= 0 );
    assert(
        ( static_cast<size_t>( pos ) + 1 > current_variant_.position ) ||
        ( std::string( handle_->sam_hdr_->target_name[tid] ) != current_variant_.chromosome )
    );

    // Set current chromosome/locus, make 1-based for our case.
    current_variant_.chromosome = std::string( handle_->sam_hdr_->target_name[tid] );
    current_variant_.position = static_cast<size_t>( pos ) + 1;
    current_variant_.reference_base = 'N';
    current_variant_.alternative_base = 'N';
    current_variant_.status.reset();

    // Resize to the number of samples, and reset the variant base count tallies for all samples.
    // We fully resize here, in case that the current_variant_ has been moved from by the user,
    // see https://stackoverflow.com/a/55054380/4184258. All other member variables of the Variant
    // are in defined states after being moved from, so the above initializations are okay,
    // but the vector might be empty or of the wrong size afterwards.
    // This also resets all counts to 0, and the status to passing.
    current_variant_.samples.resize( handle_->target_sample_count_ );
    for( auto& sample : current_variant_.samples ) {
        sample = SampleCounts();
    }

    // Go through the read data at the current position and tally up their base counts.
    // The loop goes through all bases that cover the position. We access the reads that these
    // bases belong to, in order to get information on base quality and the actual nucleotide, etc.
    // See https://github.com/samtools/samtools/blob/ae1f9d8809/bam_plcmd.c#L62
    for( int i = 0; i < n; ++i ) {
        bam_pileup1_t const* p = plp + i;
        process_base_( p );
    }
}

// -------------------------------------------------------------------------
//     process_base_
// -------------------------------------------------------------------------

void SamVariantInputStream::Iterator::process_base_( bam_pileup1_t const* p )
{
    assert( parent_ );
    assert( handle_ );

    // Check per base quality. If it does not meet our threshold, we skip this base,
    // and go to the next one (next iteration of the for loop from where this function is called).
    int const qual = p->qpos < p->b->core.l_qseq
        ? bam_get_qual(p->b)[p->qpos]
        : 0
    ;
    if( qual < parent_->min_base_qual_ ) {
        return;
    }

    // Get the sample, according to the read tag if set, or, if not set,
    // just get the one sample that we have initialized.
    auto const smp_idx = get_sample_index_( p );
    if( smp_idx == std::numeric_limits<size_t>::max() ) {
        // If we are here, we have an unaccounted read or one of a sample that we want to filter out,
        // and hence skip it.
        return;
    }
    assert( current_variant_.samples.size() == handle_->target_sample_count_ );
    assert( smp_idx < current_variant_.samples.size() );
    assert( handle_->target_sample_count_ == current_variant_.samples.size() );
    auto& sample = current_variant_.samples[smp_idx];

    // Check deletions. If it is one, note that, and then we are done for this base.
    if( p->is_del || p->is_refskip ){
        ++sample.d_count;
        return;
    }

    // Get the htslib internal code for the nucleotide as defined in seq_nt16_str, in 0-15,
    // which is what bam_seqi() returns, and use it to tally up (increment) our base counts.
    uint8_t* seq = bam_get_seq (p->b );
    uint8_t  nuc = bam_seqi( seq, p->qpos );
    switch( nuc ) {
        case  1: {
            ++sample.a_count;
            break;
        }
        case  2: {
            ++sample.c_count;
            break;
        }
        case  4: {
            ++sample.g_count;
            break;
        }
        case  8: {
            ++sample.t_count;
            break;
        }
        case 15: {
            ++sample.n_count;
            break;
        }
        default: {
            throw std::runtime_error(
                "Invalid base in sam/bam/cram file " + parent_->input_file_ +
                " at " + current_variant_.chromosome + ":" +
                std::to_string( current_variant_.position ) + ". Found " +
                std::string( 1, seq_nt16_str[nuc] ) + ", but expected [ACGTN]."
            );
        }
    }
}

// -------------------------------------------------------------------------
//     get_sample_index_
// -------------------------------------------------------------------------

size_t SamVariantInputStream::Iterator::get_sample_index_( bam_pileup1_t const* p ) const
{
    assert( parent_ );
    assert( handle_ );

    // If we are not using splitting by read groups, we just return the index of the single sample
    // that we are using for all reads.
    if( ! parent_->split_by_rg_ ) {
        return 0;
    }

    // If we are here, we have read group splitting, so we have set the callbacks.
    assert( parent_->split_by_rg_ );

    // Now get the bam client data, and translate it back to our format, where -1 is turned
    // into max int instead, in order to work easier with unsigned int comparisons for the indexing.
    auto const index = p->cd.i;
    if( index < 0 ) {
        assert( index == -1 );
        return std::numeric_limits<size_t>::max();
    } else {
        assert( static_cast<size_t>( index ) < handle_->target_sample_count_ );
        return static_cast<size_t>( index );
    }
}

// =================================================================================================
//     Sam Variant Input Iterator
// =================================================================================================

SamVariantInputStream::SamVariantInputStream(
    std::string const& infile,
    std::unordered_set<std::string> const& rg_tag_filter,
    bool inverse_rg_tag_filter
) {
    // Set the file, including error checking.
    input_file( infile );

    // Skip unmapepd and duplicates by default. We set the flags here in the cpp,
    // so that our header file can stay free of htslib includes and constants.
    // flags_ = ( BAM_FUNMAP | BAM_FDUP );

    // Set the rg tag filter
    rg_tag_filter_ = rg_tag_filter;
    inverse_rg_tag_filter_ = inverse_rg_tag_filter;
}

// =================================================================================================
//     Validity check
// =================================================================================================

// Make sure that the bases are as we expect them.
// Nope, cannot check, because seq_nt16_str is not declared constexpr... :-(
// static_assert(
//     seq_nt16_str[ 1] == 'A' &&
//     seq_nt16_str[ 2] == 'C' &&
//     seq_nt16_str[ 4] == 'G' &&
//     seq_nt16_str[ 8] == 'T' &&
//     seq_nt16_str[15] == 'N',
//     "Definition of the nucleotide codes in seq_nt16_str[] differs between htslib and genesis. "
//     "Please submit a bug report at https://github.com/lczech/genesis/issues"
// );

// Usage examples:
// mpileup command: https://github.com/samtools/samtools/blob/develop/bam_plcmd.c
// bedcov:          https://github.com/samtools/samtools/blob/develop/bedcov.c

} // namespace population
} // namespace genesis

#endif // htslib guard
