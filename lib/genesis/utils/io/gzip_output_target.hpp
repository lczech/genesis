#ifndef GENESIS_UTILS_IO_GZIP_OUTPUT_TARGET_H_
#define GENESIS_UTILS_IO_GZIP_OUTPUT_TARGET_H_

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
 * @ingroup utils
 */

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/base_output_target.hpp"
#include "genesis/utils/io/gzip_block_ostream.hpp"
#include "genesis/utils/io/gzip_stream.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Gzip Output Target
// =================================================================================================

/**
 * @brief Output target for writing byte data to a gzip/zlib-compressed target.
 *
 * This output target is a wrapper that takes some other output target
 * (FileOutputTarget, StringOutputTarget, StreamOutputTarget, etc),
 * and compresses using the gzip format on the fly while writing to that other target.
 *
 * The class can be moved, but not copied, because of the internal state that is kept for
 * compression, and which would mess up the output if copied.
 */
class GzipOutputTarget final : public BaseOutputTarget
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the output target using another output target
     * (FileOutputTarget, StringOutputTarget, StreamOutputTarget, etc),
     * and add gzip/zlib compression on top, using the specified ::GzipCompressionLevel%.
     */
    explicit GzipOutputTarget(
        std::shared_ptr<BaseOutputTarget> output_target,
        GzipCompressionLevel compression_level = GzipCompressionLevel::kDefaultCompression
    )
        : output_target_( output_target )
        , compression_level_( compression_level )
    {}

    GzipOutputTarget( GzipOutputTarget const& ) = delete;
    GzipOutputTarget( GzipOutputTarget&& )      = default;

    GzipOutputTarget& operator= ( GzipOutputTarget const& ) = delete;
    GzipOutputTarget& operator= ( GzipOutputTarget&& )      = default;

    ~GzipOutputTarget() override = default;

    // -------------------------------------------------------------
    //     Overloaded Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Override of the output stream function.
     */
    std::ostream& out_stream_() override
    {
        // Lazy loading. Needed in case we want to write in parallel to many files - having all
        // open when creating the output targets might overflow the file pointers.
        if( !stream_ || !stream_->good() ) {
            // Although we are in untils namespace here, we specify the namespace full,
            // in order to avoid ambiguous overload when compiled with C++17.
            stream_ = genesis::utils::make_unique<GzipOStream>(
                output_target_->ostream(), compression_level_
            );
        }
        assert( stream_ );
        assert( stream_->good() );
        return *stream_;
    }

    /**
     * @brief Override of the target name funtion.
     *
     * Returns "gzip-compressed <target_name>", using the wrapped target name, if the stream
     * actually compresses, that is, if the compression level is not
     * GzipCompressionLevel::kNoCompression%. Otherwise, simply the wrapped output target name
     * is forwarded.
     */
    std::string target_name_() const override
    {
        if( compression_level_ == GzipCompressionLevel::kNoCompression ) {
            return output_target_->target_name();
        }
        return "gzip-compressed " + output_target_->target_name();
    }

    /**
     * @brief Override of the target string funtion. Returns the wrapped output target string.
     */
    std::string target_string_() const override
    {
        return output_target_->target_name();
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    // Need the wrapped target in order to keep it alive when it was handed over from
    // a function such as to_file()
    std::shared_ptr<BaseOutputTarget> output_target_;

    GzipCompressionLevel compression_level_;
    std::unique_ptr<GzipOStream> stream_;

};

// =================================================================================================
//     Gzip Block Output Target
// =================================================================================================

/**
 * @brief Output target for writing byte data to a gzip-compressed target in blocks of gzip data.
 *
 * This output target is a wrapper that takes some other output target
 * (FileOutputTarget, StringOutputTarget, StreamOutputTarget, etc),
 * and compresses using the gzip format on the fly while writing to that other target.
 *
 * Using gzip blocks allows us to compress in parallel using multiple threads. Furthermore,
 * it should allow for downstream indexing and random access into the compressed file, although
 * we currently have not tested this. See the GzipBlockOStream class for details on gzip block
 * compression.
 *
 * The class can be moved, but not copied, because of the internal state that is kept for
 * compression, and which would mess up the output if copied.
 *
 * @see GzipBlockOStream
 * @see to_gzip_block_file
 */
class GzipBlockOutputTarget final : public BaseOutputTarget
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the output target using another output target
     * (FileOutputTarget, StringOutputTarget, StreamOutputTarget, etc),
     * and add gzip/zlib compression on top, using the specified ::GzipCompressionLevel%.
     *
     * See GzipBlockOStream for details on the parameters.
     */
    explicit GzipBlockOutputTarget(
        std::shared_ptr<BaseOutputTarget> output_target,
        std::size_t block_size = GzipBlockOStream::GZIP_DEFAULT_BLOCK_SIZE,
        GzipCompressionLevel compression_level = GzipCompressionLevel::kDefaultCompression,
        std::size_t num_threads = 0
    )
        : output_target_( output_target )
        , block_size_( block_size )
        , compression_level_( compression_level )
        , num_threads_( num_threads )
    {
        if( compression_level_ == GzipCompressionLevel::kNoCompression ) {
            throw std::invalid_argument(
                "Cannot use compression level kNoCompression with a gzip block output."
            );
        }
    }

    GzipBlockOutputTarget( GzipBlockOutputTarget const& ) = delete;
    GzipBlockOutputTarget( GzipBlockOutputTarget&& )      = default;

    GzipBlockOutputTarget& operator= ( GzipBlockOutputTarget const& ) = delete;
    GzipBlockOutputTarget& operator= ( GzipBlockOutputTarget&& )      = default;

    ~GzipBlockOutputTarget() override = default;

    // -------------------------------------------------------------
    //     Overloaded Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Override of the output stream function.
     */
    std::ostream& out_stream_() override
    {
        // Lazy loading. Needed in case we want to write in parallel to many files - having all
        // open when creating the output targets might overflow the file pointers.
        if( !stream_ || !stream_->good() ) {
            // Although we are in untils namespace here, we specify the namespace full,
            // in order to avoid ambiguous overload when compiled with C++17.
            stream_ = genesis::utils::make_unique<GzipBlockOStream>(
                output_target_->ostream(), block_size_, compression_level_, num_threads_
            );
        }
        assert( stream_ );
        assert( stream_->good() );
        return *stream_;
    }

    /**
     * @brief Override of the target name funtion.
     *
     * Returns "gzip-compressed <target_name>", using the wrapped target name.
     */
    std::string target_name_() const override
    {

        return "gzip-compressed " + output_target_->target_name();
    }

    /**
     * @brief Override of the target string funtion. Returns the wrapped output target string.
     */
    std::string target_string_() const override
    {
        return output_target_->target_name();
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    // Need the wrapped target in order to keep it alive when it was handed over from
    // a function such as to_file()
    std::shared_ptr<BaseOutputTarget> output_target_;

    // Compressing block gzip stream
    std::unique_ptr<GzipBlockOStream> stream_;

    // Need to store our settings, as we use lazy instanciation of the output stream
    std::size_t          block_size_;
    GzipCompressionLevel compression_level_;
    std::size_t          num_threads_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
