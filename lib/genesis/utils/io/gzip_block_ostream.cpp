/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/io/gzip_block_ostream.hpp"

#include "genesis/utils/core/options.hpp"

#include <cassert>
#include <fstream>
#include <future>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifdef GENESIS_ZLIB

#    include "zlib.h"

#    if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#       include <fcntl.h>
#       include <io.h>
#   endif

#endif // GENESIS_ZLIB

namespace genesis {
namespace utils {

// We only include all the class definitions if we actually use zlib.
// If not, we later also provide dummy implementations that throw if instanciated.
#ifdef GENESIS_ZLIB

// ================================================================================================
//     Gzip Block
// ================================================================================================

/**
 * @brief Local helper class that stores the input and output buffers of one compression block of
 * a given size, and runs the compression on that block.
 *
 * The input buffer stored here is used as the actual stream buffer of the GzipBlockOStreambuf
 * that this class is used by. This way, we do not have to copy buffer data back and forth.
 * Once that buffer is full, the compress() function is called, which runs gzip zlib compression
 * into the output buffer, which then again is read by GzipBlockOStreambuf and written to its
 * underlying wrapped output stream buffer.
 *
 * After compression, the instance resets itself, so that it can be used again for another block
 * of data to be compressed.
 */
class GzipBlockCompressor
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct a block of a given @p block_size, using a given @p compression_level%.
     *
     * This allocates the zlib stream object, as well as all needed input and output buffers.
     */
    GzipBlockCompressor(
        std::size_t block_size,
        int compression_level = Z_DEFAULT_COMPRESSION
    )
        : in_len_( block_size )
        , out_len_( 2 * block_size )
    {

        // Check compression level validity
        if(
            compression_level < static_cast<int>( GzipCompressionLevel::kDefaultCompression ) ||
            compression_level > static_cast<int>( GzipCompressionLevel::kBestCompression )
        ) {
            throw std::invalid_argument(
                "Compression level " + std::to_string( static_cast<int>( compression_level )) +
                " is invalid for usage in gzip output stream. Valid range is [ -1, 9 ]."
            );
        }

        // Prepare z_stream object
        zstream_ = new z_stream;
        zstream_->next_in = Z_NULL;
        zstream_->zalloc = Z_NULL;
        zstream_->zfree = Z_NULL;
        zstream_->opaque = Z_NULL;
        int ret = deflateInit2(
            zstream_, compression_level, Z_DEFLATED, 15+16, 8, Z_DEFAULT_STRATEGY
        );
        if( ret != Z_OK ) {
            throw GzipError( zstream_->msg, ret );
        }

        // Prepare buffers. We use two times the block size for the output, in the hope that this
        // always suffices for compressing one block of data. If not, we throw some cryptic message...
        in_buff_ = new char [in_len_];
        out_buff_ = new char [out_len_];
    }

    GzipBlockCompressor( GzipBlockCompressor const& ) = delete;
    GzipBlockCompressor( GzipBlockCompressor && )     = default;
    GzipBlockCompressor& operator = ( GzipBlockCompressor const& ) = delete;
    GzipBlockCompressor& operator = ( GzipBlockCompressor && )     = default;

    /**
     * @brief Destruct the block, its zlib stream object, and all input and output buffers.
     */
    ~GzipBlockCompressor()
    {
        deflateEnd( zstream_ );
        delete [] in_buff_;
        delete [] out_buff_;
        delete zstream_;
    }

    // -------------------------------------------------------------
    //     Worker Functions
    // -------------------------------------------------------------

    /**
     * @brief Get the input buffer and its maximum lengths, to which we can write data
     * that we want to be compressed.
     */
    std::pair<char*, size_t> get_input_buffer() const
    {
        return { in_buff_, in_len_ };
    }

    /**
     * @brief Get the output buffer and its used length, from which we can read the compressed data.
     */
    std::pair<char*, size_t> get_output_buffer() const
    {
        return { out_buff_, out_pos_ };
    }

    /**
     * @brief Run compression on the first @p avail_in bytes of the input buffer,
     * writing to the output buffer, and storing how many bytes have been written.
     */
    void compress( size_t avail_in )
    {
        // Start writing to the beginning of the output buffer. Only set that here, where we begin
        // a new block compression. This is then updated automatically from within the deflation loop.
        // We do set it here, so that it is 0 even if we do not compress any data (see next
        // condition).
        out_pos_ = 0;

        // If there is no input, do not write anything, in order to avoid compressing an empty
        // string by accident, which would result in unneccesary gzip headers without content.
        if( avail_in == 0 ) {
            return;
        }

        // Check that we are not asked to compress more data than the input buffer can hold.
        // This is an assertion, because we only use that class and function ourselves locally,
        // so we know what we are doing. If ever moved to the outside, make this an exception.
        assert( avail_in <= in_len_ );

        // Set zstream input buffer pointers. We only process as many bytes as given.
        // This is because the compress function might be called before the full in_len_ buffer
        // is filled, so we only compress what we are told to from the outside.
        zstream_->next_in = reinterpret_cast<decltype( zstream_->next_in )>( in_buff_ );
        zstream_->avail_in = avail_in;

        // Loop until all input is processed
        while( zstream_->avail_in > 0 ) {
            deflate_loop_( Z_NO_FLUSH );
        }

        // All data is done by now.
        assert( zstream_->avail_in == 0 );

        // Then, call deflate again asking to finish the zlib stream
        zstream_->next_in = nullptr;
        zstream_->avail_in = 0;
        deflate_loop_( Z_FINISH );

        // Now reset everything, so that the block can be used again
        deflateReset( zstream_ );
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    void deflate_loop_( int flush )
    {
        while( true ) {
            // When we get here, out_pos_ is already set from the caller to either be 0 for the
            // start of the compression, or left at the current output postion from some earlier
            // deflate loop. So, no need to change it.

            // Set zstream output buffer. It has twice the size, so should fit, but we later still
            // check and throw if not. Ugly, but everything else is just too complicated for now.
            assert( out_len_ >= out_pos_ );
            zstream_->next_out = reinterpret_cast<decltype( zstream_->next_out )>( out_buff_ + out_pos_ );
            zstream_->avail_out = out_len_ - out_pos_;

            // Run the deflate algorithm, and check the result
            int ret = deflate( zstream_, flush );
            if( ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR ) {
                throw GzipError( zstream_->msg, ret );
            }

            // Store the resulting end position in the output buffer after the deflation.
            // If this was too much, throw. Also, we check if nothing was written to the buffer;
            // in that case, we are done.
            auto const old_pos = out_pos_;
            out_pos_ = reinterpret_cast<decltype( out_buff_ )>( zstream_->next_out ) - out_buff_;
            if( out_pos_ >= out_len_ ) {
                throw GzipError( "Block compression ran out of buffer.", Z_MEM_ERROR );
            }

            // If we are done with the input, get out of here. The Z_BUF_ERROR error is not fatal,
            // but indicates that we are done with the input, and can continue.
            if( ret == Z_STREAM_END || ret == Z_BUF_ERROR || old_pos == out_pos_ ) {
                break;
            }
        }
    }

private:

    // Compression object
    z_stream* zstream_;

    // Store the input, and how many bytes are reserved for it.
    char*  in_buff_;
    size_t in_len_;

    // Store the compressed output, how many bytes are reserved,
    // and how many were used by the compression.
    char*  out_buff_;
    size_t out_len_;
    size_t out_pos_ = 0;
};

// ================================================================================================
//     Gzip Output Stream Buffer
// ================================================================================================

/**
 * @brief Output stream buffer that writes gzip-compressed data in individual blocks to a given
 * wrapped/underlying other output stream buffer.
 *
 * We use a ring buffer queue for the input, where each element represents one block to be compressed.
 * The size of each block in the queue corresponds to the desired  block size for compression.
 * We start by using the input buffer of the first block of the queue as our target buffer to write
 * the incoming stream data to. Once the input buffer is full, we send that block to a compression
 * worker thread, and move on, using the next block in the queue as our new input buffer for
 * incoming data, and so on.
 *
 * Eventually, we loop around the ring buffer, coming back to a block that has been send to be
 * compressed before. We can now write the compressed data to our underlying wrapped output sink
 * stream. If the compression is not yet done at that point, we wait, because our buffer is full
 * anyway. Once that data is written, the block is re-used again as our input buffer.
 *
 * Hence, while operating the stream, there is a delay of blocks: we buffer incoming data, send
 * it to compression once an individual block buffer is full, but only write it to the output stream
 * once we return to that block in the next round of the ring. This allows us to keep all worker
 * threads for compression busy all the time. On destruction (or flush/sync), all remaining
 * blocks are of course finished and written. Using that strategy, all blocks are also written in
 * the correct order without any need for special synchronization between compression threads.
 * Also, as the whole main operation of the stream itself is single threaded, we do not have to deal
 * with synchronization for things like access to the ring buffer.
 */
class GzipBlockOStreambuf
    : public std::streambuf
{

    // -------------------------------------------------------------
    //     Structs and Enums
    // -------------------------------------------------------------

private:

    /**
     * @brief Helper struct that stores one block of compression, and its status in form of a future.
     *
     * If the future is valid(), the block was previously assigned to one of the thread pool workers
     * to be compressed. Then, we can call future.get() to wait for the compression to finish,
     * and after that, write the compressed data to our wrapped stream.
     *
     * If the future is not valid, then the block is usable as a buffer, or is currently used as
     * a buffer for our streamed input. Once the block buffer is full, we can then call the
     * compression on it.
     */
    struct BlockTask
    {
        BlockTask( std::size_t block_size, int compression_level )
            : block( block_size, compression_level )
        {}

        GzipBlockCompressor block;
        ProactiveFuture<void> future;
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

public:

    /**
     * @brief Construct a new stream buffer, given the wrapped stream buffer to write gzip-compressed
     * blocks of data to.
     *
     * It is recommended that the underlying wrapped output stream buffer is opened in
     * `std::ios_base::binary` mode, so that the gzip data does not get destroyed.
     *
     * The constructor also takes the desired @p block_size of the uncompressed data blocks to use,
     * as well as the @p compression_level, the @p thread_pool to use for parallel compression, and
     * the @p num_blocks number of consecutive blocks to store as intermediate data (so that the
     * threads can work on them in parallel). Using `num_blocks < thread_pool.size()` does not make
     * much sense, as the threads would not have enough data to work on to all do something in
     * parallel, and we hence recommend to use the default (`num_blocks == 0`), which causes to
     * use `num_blocks = 2 * thread_pool.size()`.
     */
    GzipBlockOStreambuf(
        std::streambuf* sbuf_p,
        std::size_t block_size = GzipBlockOStream::GZIP_DEFAULT_BLOCK_SIZE,
        int compression_level = Z_DEFAULT_COMPRESSION,
        std::shared_ptr<ThreadPool> thread_pool = nullptr,
        size_t num_blocks = 0
    )
        : sbuf_p_( sbuf_p )
        , thread_pool_( thread_pool ? thread_pool : Options::get().global_thread_pool() )
    {
        // Basic setup. We take the number of threads as provided, and if given a number of blocks,
        // also use that. If not, we aim to use twice as many blocks as threads, so that there is
        // enough buffer keeping all worker threads busy. We want at least 2 blocks, so that we
        // have one for current writing operations of the stream, and one that can be compressed
        // at the same time.
        assert( sbuf_p_ );
        assert( thread_pool_ );

        if( num_blocks == 0 ) {
            num_blocks = 2 * thread_pool_->size();
        }
        if( num_blocks < 2 ) {
            num_blocks = 2;
        }
        assert( num_blocks >= 2 );

        // Create as many empty working blocks as needed.
        block_queue_.reserve( num_blocks );
        for( size_t i = 0; i < num_blocks; ++i ) {
            block_queue_.emplace_back( block_size, compression_level );
        }
        assert( block_queue_.size() > 0 );
        assert( block_queue_.size() == num_blocks );
        assert( current_block_ == 0 );

        // Use the first worker block as the current stream target buffer.
        auto block_in = block_queue_[ current_block_ ].block.get_input_buffer();
        setp( block_in.first, block_in.first + block_in.second );
    }

    GzipBlockOStreambuf( GzipBlockOStreambuf const& ) = delete;
    GzipBlockOStreambuf( GzipBlockOStreambuf &&) = delete;
    GzipBlockOStreambuf& operator = ( GzipBlockOStreambuf const& ) = delete;
    GzipBlockOStreambuf& operator = ( GzipBlockOStreambuf &&) = delete;

    /**
     * @brief Destructor that finishes all compression and flushes all data to the wrapped
     * underlying output stream buffer.
     */
    virtual ~GzipBlockOStreambuf()
    {
        // Flush the stream
        //
        // NOTE: Errors here (sync() return value not 0) are ignored, because we
        // cannot throw in a destructor. This mirrors the behaviour of
        // std::basic_filebuf::~basic_filebuf(). To see an exception on error,
        // close the ofstream with an explicit call to close(), and do not rely
        // on the implicit call in the destructor.
        sync();
    }

    // -------------------------------------------------------------
    //     Internal and Virtual Functions
    // -------------------------------------------------------------

    virtual std::streambuf::int_type overflow(std::streambuf::int_type c = traits_type::eof()) override
    {
        // As fas as I understand the usage of the overflow() function, it is only called from the
        // std::streambuf functions (that we inherit from) when there is no more room in the buffer
        // to put the next byte to the stream. As we use blocks in the ring buffer as our (ever
        // changing) output buffer, we should only get here if such a block is fully used.
        // Assert this. If this assertion fails, our assumption is wrong that the overflow() is
        // only called from std::streambuf when there is an actual overflow. In that case, we need
        // to investigate what other std::streambuf functions call overflow, and why.
        // The assertion checks that the difference between the current write pointer of the stream
        // buffer and the beginning of the buffer is the same as the total length of the buffer.
        assert( pptr() >= pbase() );
        assert(
            static_cast<size_t>( pptr() - pbase() ) ==
            block_queue_[ current_block_ % block_queue_.size() ].block.get_input_buffer().second
        );

        // Also, assert that the buffer pointers are correct. In particular, the current
        // write pointer pptr needs to be at the same position as the buffer end pointer epptr.
        // This is a variation of the check above.
        // At the same time, the buffer start pointer pbase shoudl still be at the start of the block.
        assert( pptr() == epptr() );
        assert(
            pbase() ==
            block_queue_[ current_block_ % block_queue_.size() ].block.get_input_buffer().first
        );

        // We have an overflow, so the buffer of the current block is full. We can send it to
        // a worker thread for compression, and move on to the next block in the ring, which we
        // then use as the new buffer for storing our input data.
        // If the ring is full, we wait for the next block in order to finish being
        // compressed, and then write it to the underlying stream.
        // All of this is done in the function call here.
        auto ret = compress_current_block_and_move_to_next_block_();
        if( ret != 0 ) {
            setp( nullptr, nullptr );
            return traits_type::eof();
        }

        return traits_type::eq_int_type(c, traits_type::eof()) ? traits_type::eof() : sputc(c);
    }

    virtual int sync() override
    {
        // No pointer to be used. That is an error.
        if( !pptr() ) {
            return -1;
        }

        // First, send all remaining buffered input of the current block to a compression worker.
        // Return early if there was an issue writing any previously processed compressed blocks
        // to the output sink stream.
        auto ret = compress_current_block_and_move_to_next_block_();
        if( ret != 0 ) {
            return ret;
        }

        // Then, write all blocks that are still in the queue. We need to do a full round, because
        // otherwise we have no way of knowing which blocks were used so far - for very short files,
        // we will not even yet have filled the queue completely.
        size_t cur = current_block_ % block_queue_.size();
        do {

            // Write the compressed block, if it has data, potentially waiting for its compression
            // to be finished by some worker thread first.
            if( block_queue_[ cur ].future.valid() ) {
                ret = write_compressed_block_( cur );

                // Return early if there was an issue writing to the output sink stream.
                if( ret != 0 ) {
                    return ret;
                }
            }

            // Process next, wrapping around the ring.
            ++cur;
            cur %= block_queue_.size();
        } while( cur != current_block_ % block_queue_.size() );
        assert( cur == current_block_ % block_queue_.size() );

        // Assert that we flushed all blocks, that is, we waited for all their compression to be
        // done and all their data to be written to our underlying output sink stream.
        // In that case, none of them should have a valid future, which we check here via a lambda
        // that we immediately call.
        assert(
            [this](){
                for( auto const& block : block_queue_ ) {
                    if( block.future.valid() ) {
                        return false;
                    }
                }
                return true;
            }()
        );

        // If we got here, all previous checks of `ret` were okay. So it still should be okay now.
        assert( ret == 0 );
        return ret;
    }

private:

    /**
     * @brief Local helper function. The name says it all.
     *
     * Okay, maybe not all. The function sends the current block of the queue to a worker thread
     * for compression, up to all bytes that have been written from the user to the stream into
     * that block so far. Any remaining capacity of the block is unused; typically however,
     * this function is called on overflow(), that is, if the buffer is full. In that case, we
     * achive the desired input block size. If called from sync() or other functions, this of course
     * is not true, and the resulting block might be smaller.
     *
     * After that, we move the the next block in the ring queue. If that block was used before
     * (because we use a ring buffer, so eventually, we re-use the blocks), we now need to write
     * its by-now compressed data to our underyling sink stream. If the compression is not yet done,
     * we wait, because that means, we are currently fully using the queue, and so, there is not
     * much else to do for us right now. Once that is done, we can re-use the buffer as our
     * next input buffer for the data that arrives to our stream from the user side.
     *
     * Return 0 if all good, and value other than 0 if some error occurred.
     */
    int compress_current_block_and_move_to_next_block_()
    {
        // Get th current block. We were busy filling it with new input data, so it cannot have
        // been compressed already, meaning it cannot have a valid future.
        auto& cur_block = block_queue_[ current_block_ % block_queue_.size() ];
        assert( ! cur_block.future.valid() );

        // Assert that all pointers are where they should be
        assert( pbase() == cur_block.block.get_input_buffer().first );
        assert(
            epptr() ==
            cur_block.block.get_input_buffer().first + cur_block.block.get_input_buffer().second
        );

        // Send block to a compression worker thread, using all bytes that have been written to it.
        // The thread pool will pick up the task once a thread is available.
        assert( thread_pool_ );
        auto const avail_in = pptr() - pbase();
        cur_block.future = thread_pool_->enqueue_and_retrieve(
            [&]( size_t av_in ){
                cur_block.block.compress( av_in );
            },
            avail_in
        );

        // Move to next block in the ring buffer queue
        ++current_block_;
        auto& next_block = block_queue_[ current_block_ % block_queue_.size() ];

        // If the block has a future, that means that we sent it to compression before.
        // Because we use a ring buffer, that hence means that the ring is full. There are
        // currently only full blocks that are either already compressed or under compression
        // by some worker thread, or waiting to be compressed, but no block that we can use
        // as our next input buffer for writing data to.
        // Hence, we have to wait for the block to finish being compressed and then write it to our
        // underlying sink stream, before we can finally re-use the block as our new target buffer
        // for  the incoming data.
        int ret = 0;
        if( next_block.future.valid() ) {

            // If we are here, the ring buffer queue is full. In that case, all blocks have been
            // added to the thread pool for being compressed.
            // Assert that indeed all bocks contain valid futures, that is, they all have been
            // send to be compressed at some point before. We use a lambda that executes itself.
            assert(
                [this](){
                    for( auto const& block : block_queue_ ) {
                        if( ! block.future.valid() ) {
                            return false;
                        }
                    }
                    return true;
                }()
            );

            // Write the compressed block to the underlying stream,
            // potentially waiting until its compression is finished.
            ret = write_compressed_block_( current_block_ % block_queue_.size() );
        }

        // Now, the block is written, and we can re-use it as the new stream buffer.
        auto block_in = next_block.block.get_input_buffer();
        setp( block_in.first, block_in.first + block_in.second );

        // Assert that all pointers are where they should be
        assert( pbase() == block_in.first );
        assert( pptr()  == block_in.first );
        assert( epptr() == block_in.first + block_in.second );

        // Return value: was the writing of the previously compressed blocks successful.
        // If not, there was an error somewhere.
        return ret;
    }

    /**
     * @brief Local helper function to write a compressed block to the underlying sink stream.
     *
     * The function can only be called for a block that has already been send to be compressed
     * by a worker thread. Such a block will have a valid future, which we assert here.
     * Once the compression of the block is finished (which we wait for here, if necessary), that
     * block is then written to the sink, so that its buffers can be re-used for further data input.
     *
     * Return 0 if all good, and value other than 0 if some error occurred.
     */
    int write_compressed_block_( size_t block_num )
    {
        // Get the block to write. It has to have a future, as we only call this function
        // when the block was previously sent to a worker to be compressed.
        assert( block_num < block_queue_.size() );
        auto& block = block_queue_[ block_num ];
        assert( block.future.valid() );

        // Make sure that the block compression thread is finished
        block.future.get();

        // Get the block output begin and end, and write it to the underlying stream
        auto const block_out = block.block.get_output_buffer();
        std::streamsize sz = sbuf_p_->sputn( block_out.first, block_out.second );

        // Check if there was an error in the sink stream
        if( sz != static_cast<decltype(sz)>( block_out.second )) {
            return -1;
        }
        return 0;
    }

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

private:

    // Target sink stream to write compressed blocks to
    std::streambuf * sbuf_p_;

    // Get a pool of workers that will do the compression of each block
    std::shared_ptr<ThreadPool> thread_pool_;

    // Ring-buffer-like usage of compression blocks: we rotate, and wait if the compression is not
    // yet done for the next block to be re-used in the ring. The current_block_ number only ever
    // increases (that is, it counts the total number of blocks that have been processed so far).
    // This is meant as a helper for future extensions that might want to keep track of byte
    // offsets of output blocks (not yet implemented).
    size_t current_block_ = 0;
    std::vector<BlockTask> block_queue_;

};

// ================================================================================================
//     Gzip Output Stream
// ================================================================================================

// We have all the implementation here, so that we do not need to expose the stream buffer.

GzipBlockOStream::GzipBlockOStream(
    std::ostream& os,
    std::size_t block_size,
    GzipCompressionLevel compression_level,
    std::shared_ptr<ThreadPool> thread_pool
)
    : GzipBlockOStream( os.rdbuf(), block_size, compression_level, thread_pool )
{
    // Nothing to do
}

GzipBlockOStream::GzipBlockOStream(
    std::streambuf* sbuf_p,
    std::size_t block_size,
    GzipCompressionLevel compression_level,
    std::shared_ptr<ThreadPool> thread_pool
)
    : std::ostream( new GzipBlockOStreambuf(
        sbuf_p,
        block_size,
        static_cast<int>(compression_level),
        thread_pool
    ))
{
    exceptions(std::ios_base::badbit);
}

GzipBlockOStream::~GzipBlockOStream()
{
    delete rdbuf();
}

// Up until here, we defined all classes needed for gzip block output streaming.
// However, if genesis is compiled without zlib support, we instead use dummy implementations
// which throw exceptions when being used.
#else // GENESIS_ZLIB

// ================================================================================================
//     Gzip Output Stream
// ================================================================================================

GzipBlockOStream::GzipBlockOStream(
    std::ostream&, std::size_t, GzipCompressionLevel, std::size_t
) {
    throw std::runtime_error( "zlib: Genesis was not compiled with zlib support." );
}

GzipBlockOStream::GzipBlockOStream(
    std::streambuf*, std::size_t, GzipCompressionLevel, std::size_t
) {
    throw std::runtime_error( "zlib: Genesis was not compiled with zlib support." );
}

GzipBlockOStream::~GzipBlockOStream()
{}

#endif // GENESIS_ZLIB

} // namespace utils
} // namespace genesis
