#ifndef GENESIS_UTILS_IO_GZIP_INPUT_SOURCE_H_
#define GENESIS_UTILS_IO_GZIP_INPUT_SOURCE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

// Found zlib
#ifdef GENESIS_ZLIB

#include "genesis/utils/io/base_input_source.hpp"

#include <memory>
#include <string>

#include "zlib.h"

namespace genesis {
namespace utils {

// =================================================================================================
//     Gzip Input Source
// =================================================================================================

/**
 * @brief Input source for reading byte data from a gzip-compressed source.
 *
 * This input source is a wrapper that takes some other source (string, file, etc),
 * and de-compresses using gzip on the fly while reading.
 */
class GzipInputSource : public BaseInputSource
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    /**
     * @brief Block length for internal buffering (1MB).
     */
    static const size_t BlockLength = 1 << 20;

    /**
     * @brief Format used by zlib for decompression.
     */
    enum class Format : int
    {
        /**
         * @brief Use gzip decompression.
         */
        kGzip = MAX_WBITS | 16,

        /**
         * @brief Use zlib decompression.
         */
        kZlib = MAX_WBITS,

        /**
         * @brief Use a pure deflate decompression.
         */
        kDeflate = -MAX_WBITS
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the input source from a file with the given file name.
     */
    explicit GzipInputSource(
        std::unique_ptr<BaseInputSource> input_source,
        Format format = Format::kGzip
    );

    GzipInputSource( GzipInputSource const& ) = default;
    GzipInputSource( GzipInputSource&& )      = default;

    GzipInputSource& operator= ( GzipInputSource const& ) = default;
    GzipInputSource& operator= ( GzipInputSource&& )      = default;

    ~GzipInputSource();

    // -------------------------------------------------------------
    //     Overloaded Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Override of the read function.
     */
    size_t read_( char* buffer, size_t size ) override;

    /**
     * @brief Override of the source name funtion. Returns "gzip-compressed <original_source>".
     */
    std::string source_name_() const override
    {
        return format_name_ + "-compressed " + input_source_->source_name();
    }

    /**
     * @brief Throw an exception for zlib related errors.
     */
    void report_zlib_error_( int error_code ) const;

    /**
     * @brief Translate the format enum into a string representation.
     */
    std::string translate_format_( Format format ) const;

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    std::unique_ptr<BaseInputSource> input_source_;
    std::string format_name_;

    z_stream z_stream_;
    char     in_buf_[ BlockLength ];
    size_t   in_pos_ = 0;
    size_t   in_end_ = 0;

};

} // namespace utils
} // namespace genesis

#endif // found zlib

#endif // include guard
