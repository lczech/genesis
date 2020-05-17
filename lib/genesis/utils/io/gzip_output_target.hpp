#ifndef GENESIS_UTILS_IO_GZIP_OUTPUT_TARGET_H_
#define GENESIS_UTILS_IO_GZIP_OUTPUT_TARGET_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/io/base_output_target.hpp"
#include "genesis/utils/io/gzip_stream.hpp"
#include "genesis/utils/core/std.hpp"

#include <cassert>
#include <memory>
#include <string>

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
        GzipCompressionLevel level = GzipCompressionLevel::kDefaultCompression
    )
        : output_target_( output_target )
        , level_( level )
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
            stream_ = make_unique<GzipOStream>( output_target_->ostream(), level_ );
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
        if( level_ == GzipCompressionLevel::kNoCompression ) {
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

    GzipCompressionLevel level_;
    std::unique_ptr<GzipOStream> stream_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
