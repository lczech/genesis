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

#ifdef GENESIS_HTSLIB

#include "genesis/population/formats/vcf_input_iterator.hpp"

extern "C" {
    #include <htslib/hts.h>
    #include <htslib/vcf.h>
}

#include <cstring>

namespace genesis {
namespace population {

// =================================================================================================
//     Private Members
// =================================================================================================

void VcfInputIterator::init_()
{
    assert( file_ );
    assert( header_ );
    assert( current_block_ );
    assert( buffer_block_ );

    // Init the records and create empty VcfRecord to read into.
    current_block_->reserve( block_size_ );
    buffer_block_->reserve( block_size_ );
    for( size_t i = 0; i < block_size_; ++i ) {
        current_block_->emplace_back( *header_ );
        buffer_block_->emplace_back( *header_ );
    }
    assert( current_block_->size() == block_size_ );
    assert( buffer_block_->size() == block_size_ );

    // Read the first block synchronously, so that there is data initialized to be dereferenced.
    end_pos_ = VcfInputIterator::read_block_( file_, current_block_, block_size_ );
    assert( current_pos_ == 0 );

    // If there is less data than the block size, the file is already done.
    // No need to start the async buffering.
    if( end_pos_ < block_size_ ) {
        return;
    }

    // Now start the worker thread to fill the buffer.
    fill_buffer_block_();
}

void VcfInputIterator::increment_()
{
    // Finish the reading (potentially waiting if not yet finished in the worker thread).
    // The future returns how much data there was to be read, which we use as our status.
    // After that, swap the buffer and start a new reading operation in the worker thread.

    // If needed, we check the correct order of chromosomes and positions in the input file.
    // To avoid making expensive temporary strings for this,
    // we simply use char pointers to the htslib record data directly here.
    // We might need a copy of the chromosome, in case we go across blocks - we put this in chr_cpy,
    // which we might free later if we end up using it.
    char const* cur_chr = nullptr;
    size_t      cur_pos = 0;
    char*       chr_cpy = nullptr;
    if( expect_ordered_ ) {
        auto const& cur_rec = (*current_block_)[current_pos_];
        cur_chr = ::bcf_hdr_id2name( cur_rec.header().data(), cur_rec.data()->rid );
        cur_pos = cur_rec.data()->pos + 1;
    }

    // Move to the next element in the vector. If we are at the end of the record vector,
    // and if that vector was full (complete block size), there is more data, so start reading.
    // Depending on whether we need a new block, we need a bit of a special treatment of
    // the chromosome and position check here.
    ++current_pos_;
    if( current_pos_ == end_pos_ && end_pos_ == block_size_ ) {
        assert( future_ );
        assert( future_->valid() );

        // If we go across block borders, we need to copy the current chrom/pos for the check.
        // This is a minor waste, that we probably have to live with.
        if( expect_ordered_ ) {
            chr_cpy = strdup( cur_chr );
            cur_chr = chr_cpy;
        }

        // Get how many records were read into the buffer, which also waits for the reading
        // if necessary. After that, we can swao the buffer, start reading again, and set
        // or internal current location to the first element of the vector again.
        end_pos_ = future_->get();
        std::swap( buffer_block_, current_block_ );
        fill_buffer_block_();
        current_pos_ = 0;
    }

    // Do the correct order check if needed.
    if( expect_ordered_ && current_pos_ < end_pos_ ) {
        assert( cur_chr != nullptr );
        assert( cur_pos > 0 );

        // Get the new chr/pos.
        auto const& new_rec = (*current_block_)[current_pos_];
        char const* new_chr = ::bcf_hdr_id2name( new_rec.header().data(), new_rec.data()->rid );
        size_t      new_pos = new_rec.data()->pos + 1;

        // Check!
        if(
            ( strcmp( new_chr, cur_chr )  < 0 ) ||
            ( strcmp( new_chr, cur_chr ) == 0 && new_pos <= cur_pos )
        ) {
            throw std::runtime_error(
                "Malformed VCF file " + filename_ + ": " +
                "unordered chromosomes and positions going from " +
                std::string( cur_chr ) + ":" + std::to_string( cur_pos ) + " to " +
                std::string( new_chr ) + ":" + std::to_string( new_pos )
            );
        }
    }

    // Release copy, if we made one.
    if( chr_cpy ) {
        assert( expect_ordered_ );
        free( chr_cpy );
    }
}

void VcfInputIterator::fill_buffer_block_()
{;
    assert( thread_pool_ );
    assert( future_ );

    // This function is only every called after we finished any previous operations,
    // so let's assert that the thread pool and future are in the states that we expect.
    assert( thread_pool_->load() == 0 );
    assert( ! future_->valid() );

    // In order to use lambda captures by copy for class member variables in C++11, we first
    // have to make local copies, and then capture those. Capturing the class members direclty
    // was only introduced later. Bit cumbersome, but gets the work done.
    auto file         = file_;
    auto buffer_block = buffer_block_;
    auto block_size   = block_size_;

    // The lambda returns the result of read_block_ call, that is, the number of records
    // that have been read, and which we later (in the future_) use to see how much data we got.
    *future_ = thread_pool_->enqueue(
        [ file, buffer_block, block_size ](){
            return VcfInputIterator::read_block_( file, buffer_block, block_size );
        }
    );
}

size_t VcfInputIterator::read_block_(
    std::shared_ptr<HtsFile> file,
    std::shared_ptr<std::vector<VcfRecord>> target,
    size_t block_size
) {
    // This is a static function that does not depend on the class member data, so that
    // we can use it from the future lambda in the thread pool above without having to worry
    // about lambda captures of `this` going extinct... which was an absolutely nasty bug to
    // find! Such a rookie mistake! For that reason, we also take all arguments as shared
    // pointers, so that they are kept alive while the thread pool is working.
    // However, once its done with its work, the function (the one that we give to the thread
    // pool with a lambda) is popped from the thread queue, so that the shared pointer can
    // be freed again - that is, we do not need to worry about the lambda keeping the shared
    // pointer from freeing its memory indefinitely.

    assert( file );
    assert( target->size() == block_size );

    // Read as long as there is data. Return number of read records.
    size_t i = 0;
    for( ; i < block_size; ++i ) {
        if( ! (*target)[i].read_next( *file ) ) {
            break;
        }
    }
    return i;
}

} // namespace population
} // namespace genesis

#endif // htslib guard
