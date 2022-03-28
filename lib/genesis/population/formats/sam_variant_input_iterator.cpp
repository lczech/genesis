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

#include "genesis/population/formats/sam_variant_input_iterator.hpp"

#include "genesis/population/functions/base_counts.hpp"

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
//     Iterator Public Members
// =================================================================================================

std::vector<std::string> SamVariantInputIterator::Iterator::rg_tags() const
{
    // Empty list when we do not split by read group tag.
    assert( parent_ );
    if( ! parent_->split_by_rg_ ) {
        return {};
    }

    // We store the rg tags in a form that is fastest for per-read access. However, turning this
    // into a neat ordered list is a bit cumbersome... but this usually is only called in the
    // beginning of the iteration, so that cost is okay.
    std::vector<std::string> result;
    for( size_t i = 0; i < handle_.rg_tags.size(); ++i ) {
        // For each tag in the order that we actually want, find where it is in the map.
        // We need to scan the whole map every time, as the indicies are stored as values...
        for( auto const& tag : handle_.rg_tags ) {
            if( tag.second == i ) {
                // Found the index for this tag. Add it, and move on.
                result.push_back( tag.first );
                break;
            }
        }
        // If we get here, that means that the result size was changed in the above loop.
        // If not, we did not find the tag that has index i. Assert this.
        assert( result.size() == i + 1 );
    }

    // Add the unaccounted tag if needed.
    if( parent_->with_unaccounted_rg_ ) {
        result.push_back( "unaccounted" );
    }

    // Check that we have the correct size, and return.
    assert(
        ( ! parent_->with_unaccounted_rg_ && result.size() == handle_.rg_tags.size() ) ||
        (   parent_->with_unaccounted_rg_ && result.size() == handle_.rg_tags.size() + 1 )
    );
    return result;
}

// =================================================================================================
//     Iterator Private Members
// =================================================================================================

void SamVariantInputIterator::Iterator::init_()
{
    // Assert that the nucleotide codes in htslib are as we expect them here.
    // This should be a static assertion, but C char* does not support that,
    // so instead we do it once here on construction - an acceptable overhead.
    assert( strcmp( seq_nt16_str, "=ACMGRSVTWYHKDBN" ) == 0 );

    // Edge case: empty iterator,
    assert( parent_ );
    if( parent_->input_file_.empty() ) {
        return;
    }

    // Set the pointer to parent.
    handle_.parent = parent_;

    // Open the file and read its header.
    // We use hts_open() here instead of sam_open(), as the latter is a macro whose expansion
    // differs between htslib and samtools - WTF???
    // In htslib, it simply forwards to hts_open() anyway, so let's just do that directly.
    handle_.hts_file = hts_open( parent_->input_file_.c_str(), "r" );
    if( ! handle_.hts_file ) {
        throw std::runtime_error( "Cannot open file " + parent_->input_file_ );
    }
    handle_.sam_hdr = sam_hdr_read( handle_.hts_file );
    if( ! handle_.sam_hdr ) {
        throw std::runtime_error( "Cannot read header of file " + parent_->input_file_ );
    }

    // Init the iterator, and set the max depth, to keep memory usage limited.
    // Not sure that we also need to do the additional max depth check during plp porcessing below,
    // but also doesn't hurt to keep it in there.
    handle_.iter = bam_plp_init( read_sam_, static_cast<void*>( &handle_ ));
    if( parent_->max_acc_depth_ > 0 ) {
        bam_plp_set_maxcnt( handle_.iter, parent_->max_acc_depth_ );
    }
    // assert( handle_.hts_iter == nullptr );
    // handle_.hts_iter = sam_itr_queryi(idx[i], HTS_IDX_START, 0, 0);

    // If wanted, get the @RG read group tags from the header.
    if( parent_->split_by_rg_ ) {
        handle_.rg_tags = get_header_rg_tags_( handle_.sam_hdr );
    }

    // Finally, get the first line.
    increment_();
}

void SamVariantInputIterator::Iterator::increment_()
{
    // Only to be called when the iterator is still valid (not past-the-end).
    assert( parent_ );

    // Find the next input position that we want to consider (it has data and fitting coverage).
    // tid is the chromosome name index, pos the position on the chromosome, and n is coverage/depth
    int tid, pos, n;
    bam_pileup1_t const* plp;
    while( true ) {
        plp = bam_plp_auto( handle_.iter, &tid, &pos, &n );

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
        if( tid < 0 ) {
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

    // Set current chromosome/locus, make 1-based for our case.
    current_variant_.chromosome = std::string( handle_.sam_hdr->target_name[tid] );
    current_variant_.position = pos + 1;
    current_variant_.reference_base = 'N';
    current_variant_.alternative_base = 'N';

    // Resize to the number of samples, and reset the variant base count tallies for all samples.
    // We fully resize here, in case that the current_variant_ has been moved from by the user,
    // see https://stackoverflow.com/a/55054380/4184258.
    // All other member variables of the Variant are in defined states after being moved from,
    // but the vector might be empty or of the wrong size afterwards.
    if( parent_->split_by_rg_ ) {
        if( parent_->with_unaccounted_rg_ ) {
            current_variant_.samples.resize( handle_.rg_tags.size() + 1 );
        } else {
            current_variant_.samples.resize( handle_.rg_tags.size() );
        }
    } else {
        current_variant_.samples.resize( 1 );
    }
    for( auto& sample : current_variant_.samples ) {
        sample.clear();
    }

    // Go through the read data at the current position and tally up.
    // The loop goes through all bases that cover the position. We access the reads that these
    // bases belong to, in order to get information on base quality and the actual nucleotide, etc.
    // See https://github.com/samtools/samtools/blob/ae1f9d8809/bam_plcmd.c#L62
    for( int i = 0; i < n; ++i ) {
        bam_pileup1_t const* p = plp + i;

        // Check per base quality. If it does not meet our threshold, we skip this base,
        // and go to the next one (next iteration of the for loop).
        int const qual = p->qpos < p->b->core.l_qseq
            ? bam_get_qual(p->b)[p->qpos]
            : 0
        ;
        if( qual < parent_->min_base_qual_ ) {
            continue;
        }

        // Get the sample, according to the read tag if set, or, if not set,
        // just get the one sample that we have initialized.
        auto const smp_idx = get_sample_index_( p );
        if( smp_idx == std::numeric_limits<size_t>::max() ) {
            // If we are here, we have an unaccounted read and want to skip it.
            continue;
        }
        assert( smp_idx < current_variant_.samples.size() );
        auto& sample = current_variant_.samples[smp_idx];

        // Check deletions. If it is one, note that, and then we are done for this base.
        if( p->is_del || p->is_refskip ){
            ++sample.d_count;
            continue;
        }

        // Get the htslib internal code for the nucleotide as defined in seq_nt16_str, in 0-15,
        // which is what bam_seqi() returns, and use it to tally up (increment) our base counts.
        uint8_t* seq = bam_get_seq(p->b);
        uint8_t  nuc = bam_seqi(seq, p->qpos);
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
                    "Invalid base in sam/bam/cram file " + handle_.parent->input_file_ +
                    " at " + current_variant_.chromosome + ":" +
                    std::to_string( current_variant_.position ) + ". Found " +
                    std::string( 1, seq_nt16_str[nuc] ) + ", but expected [ACGTN]."
                );
            }
        }
    }

    // Status of the iteration, as given by the result of bam_mplp_auto().
    // If greater than zero, we are still iterating.
    // int iter_status_;
    // iter_status_ = bam_mplp_auto( mplp, & tid, & pos, n_plp, plp );
}

size_t SamVariantInputIterator::Iterator::get_sample_index_( bam_pileup1_t const* p ) const
{
    // If we are not using splitting by read groups, we just return the index of the single sample
    // that we are using for all reads.
    if( ! parent_->split_by_rg_ ) {
        return 0;
    }

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
    auto tag_itr = handle_.rg_tags.end();

    // Get RG tag from read and look it up in hash to find its index.
    // Not obvious from htslib documentatin at all, but let's hope that this
    // is the correct way to do this...
    uint8_t* tag = bam_aux_get( p->b, "RG" );
    if( tag != nullptr ) {
        // Unfortunately, it seems that we have to take the detour via the string
        // value of the tag here, instead of htslib directly offering the index.
        // Hence all the shenannigans with the unordered map of indices...
        // Also, please don't ask me why the htslib function for this is called bam_aux2Z().
        // I guess Z is their notation for a string, and a tag such as RG is an "auxiliary" thing,
        // so this function means "tag 2 string", in a sense?!
        char* rg = bam_aux2Z( tag );
        tag_itr = handle_.rg_tags.find( std::string( rg ));

        // Potential future extension: RG tag in read is not in the header.
        // That indicates that something is wrong with the file. For now, we just treat this
        // as an unaccounted read.
        // if( tag_itr == handle_.rg_tags.end() ) {
        //     throw std::runtime_error(
        //         "Invalid @RG tag in read that is not listed in the header. "
        //         "Offending tag: '" + std::string( rg ) + "' in file " +
        //         parent_->input_file_
        //     );
        // }
    }

    size_t smp_idx = 0;
    if( tag_itr != handle_.rg_tags.end() ) {
        // We found the RG tag index.
        // Assert that it is within the bounds - if we also use unaccounted,
        // the base counts contains an additional element which should never be found
        // by the index lookup. If we are here, we also need to have at least one actual
        // sample plus potentially the unaccounted one, otherwise we would not be here.
        smp_idx = tag_itr->second;
        assert(
            current_variant_.samples.size() >=
            static_cast<size_t>( parent_->with_unaccounted_rg_ ) + 1
        );
        assert(
            smp_idx < (
                current_variant_.samples.size() -
                static_cast<size_t>( parent_->with_unaccounted_rg_ )
            )
        );
    } else {
        // One of the two error cases occurred.
        // Decide based on whether we want to use unaccounted reads or skip them.
        if( parent_->with_unaccounted_rg_ ) {
            // If we are here, we have at least initialized the samples to have the
            // unaccounted base counts object.
            // Get the last base counts object, which is the unaccounted one.
            assert( current_variant_.samples.size() > 0 );
            smp_idx = current_variant_.samples.size() - 1;
        } else {
            // If we are here, we have an unaccounted read and want to skip it.
            // Use max size_t to indicate that.
            smp_idx = std::numeric_limits<size_t>::max();
        }
    }
    return smp_idx;
}

int SamVariantInputIterator::Iterator::read_sam_( void* data, bam1_t* bam )
{
    // The function processes a single mapped read. Reads that make it through here are then
    // used by htslib for pileup processing, and subsequencly by us in the increment_() function
    // to build our Variant object from it. Inspired from bam2depth and bedcov programs.
    // Comment from bam2depth: read level filters better go here to avoid pileup.

    // Data in fact is a pointer to our handle.
    auto handle = static_cast<SamFileHandle*>( data );

    // Loop until we find a read that we want to use.
    int ret;
    while( true ) {

        // Read the data. Not sure why we need two behaviours depending on iter, but this is how it
        // is done in samtools bam2depth, just that we here use the sam instead of the bam functions.
        // int ret = handle->hts_iter
        //     ? sam_itr_next( handle->hts_file, handle->hts_iter, bam )
        //     : sam_read1( handle->hts_file, handle->sam_hdr, bam )
        // ;

        // Get the read, and check result.
        // According to htslib, 0 on success, -1 on EOF, and <-1 on error, so let's check this.
        ret = sam_read1( handle->hts_file, handle->sam_hdr, bam );
        if( ret == -1 ) {
            break;
        }
        if( ret < -1 ) {
            throw std::runtime_error( "Error reading file " + handle->parent->input_file_ );
        }

        // Check per-read properties, and skip the read if not matching requirements.
        // We check for some basic flags, as well as minimum mapping quality here.
        if( bam->core.flag & handle->parent->flags_ ) {
            continue;
        }
        if( static_cast<int>( bam->core.qual ) < handle->parent->min_map_qual_ ) {
            continue;
        }
        break;
    }
    return ret;
}

std::unordered_map<std::string, size_t>
SamVariantInputIterator::Iterator::get_header_rg_tags_(
    ::sam_hdr_t* sam_hdr
) const {
    // Inspired by https://github.com/samtools/samtools/blob/2ece68ef9d0bd302a74c952b55df1badf9e61aae/bam_split.c#L217

    // Prepare result.
    std::unordered_map<std::string, size_t> result;

    // Get the number of RG tags in the header of the file.
    auto const n_rg = sam_hdr_count_lines( sam_hdr, "RG" );
    if( n_rg < 0 ) {
        throw std::runtime_error(
            "Failed to get @RG ID tags in file " + parent_->input_file_
        );
        return result;
    }

    // Go through all RG tags and store them.
    kstring_t id_val = KS_INITIALIZE;
    for( size_t i = 0; i < static_cast<size_t>( n_rg ); ++i ) {

        // Get the next tag.
        if( sam_hdr_find_tag_pos(sam_hdr, "RG", i, "ID", &id_val) < 0 ) {
            ks_free( &id_val );
            throw std::runtime_error(
                "Failed to get @RG ID tags in file " + parent_->input_file_
            );
        }

        // Get the name of this rg tag. Need to free it afterwards ourselves.
        // We set the index to the current size (pre emplacement), meaning that each entry
        // gets the index according to the how many-th element in the `result` map it is.
        char* name = ks_release( &id_val );
        result.emplace( name, result.size() );
        free( name );
    }

    return result;
}

SamVariantInputIterator::Iterator::~Iterator()
{
    // if( mpileup_ ) {
    //     bam_mplp_destroy( mpileup_ );
    //     mpileup_ = nullptr;
    // }
    // if( handle.hts_iter ) {
    //     // destroy( handle.hts_iter );
    //     // handle.hts_iter = nullptr;
    // }

    if( handle_.iter ) {
        bam_plp_destroy( handle_.iter );
        handle_.iter = nullptr;
    }
    if( handle_.sam_hdr ) {
        sam_hdr_destroy( handle_.sam_hdr );
        handle_.sam_hdr = nullptr;
    }
    if( handle_.hts_file ) {
        hts_close( handle_.hts_file );
        handle_.hts_file = nullptr;
    }
}

// =================================================================================================
//     Sam Variant Input Iterator
// =================================================================================================

SamVariantInputIterator::SamVariantInputIterator( std::string const& infile )
{
    // Set the file, including error checking.
    input_file( infile );

    // Skip unmapepd and duplicates by default. We set the flags here in the cpp,
    // so that our header file can stay free of htslib includes and constants.
    flags_ = ( BAM_FUNMAP | BAM_FDUP );
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
