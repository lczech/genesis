#ifndef GENESIS_UTILS_IO_GZIP_INPUT_SOURCE_H_
#define GENESIS_UTILS_IO_GZIP_INPUT_SOURCE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/io/base_input_source.hpp"

#include <memory>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Gzip Input Source
// =================================================================================================

/**
 * @brief Input source for reading byte data from a gzip/zlib-compressed source.
 *
 * This input source is a wrapper that takes some other input source
 * (FileInputSource, StringInputSource, StreamInputSource, etc),
 * and de-compresses using gzip/zlib on the fly while reading.
 *
 * The class can be moved, but not copied, because of the internal state that is kept for
 * decompression, and which would mess up the input source if copied.
 */
class GzipInputSource final : public BaseInputSource
{
public:

    // -------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------

    /**
     * @brief Format used by gzip/zlib for decompression.
     */
    enum class Format
    {
        /**
        * @brief Enable automatic header detection, allowing either gzip or zlib.
        */
        kAutomatic,

        /**
         * @brief Use gzip decompression.
         */
        kGzip,

        /**
         * @brief Use zlib decompression.
         */
        kZlib,

        /**
         * @brief Use a pure deflate decompression.
         */
        kDeflate
    };

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the input source using another input source
     * (FileInputSource, StringInputSource, StreamInputSource, etc),
     * and add gzip/zlib decompression on top, using the specified GzipInputSource::Format.
     */
    explicit GzipInputSource(
        std::shared_ptr<BaseInputSource> input_source,
        Format format = Format::kAutomatic
    );

    GzipInputSource( GzipInputSource const& ) = delete;
    GzipInputSource( GzipInputSource&& )      = default;

    GzipInputSource& operator= ( GzipInputSource const& ) = delete;
    GzipInputSource& operator= ( GzipInputSource&& )      = default;

    ~GzipInputSource() override;

    // -------------------------------------------------------------
    //     Overloaded Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Block length for internal input buffering (1MB).
     */
    static const size_t BlockLength = 1 << 20;

    /**
     * @brief Create the internal zlib data structure.
     *
     * C-style, because ZLIB...
     */
    void create_zstream_();

    /**
     * @brief Destroy the internal zlib data structure.
     *
     * C-style, because ZLIB...
     */
    void destroy_zstream_();

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
     * @brief Override of the source string funtion. Returns the source string of its own
     * source, potentially removing a trailing ".gz" (for convenience when reading files).
     */
    std::string source_string_() const override;

    /**
    * @brief Get the zlib internal int form of the format enum.
    */
    int get_format_( Format format ) const;

    /**
     * @brief Translate the format enum into a string representation.
     */
    std::string translate_format_( Format format ) const;

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    std::shared_ptr<BaseInputSource> input_source_;

    Format format_;
    std::string format_name_;

    // We want to avoid including the zlib header in this header,
    // because it totally pollutes everything.
    // So, we use a PIMPL-like data hiding technique for the zlib related members.
    struct ZlibData;

    // Store the implementation and the implementation's deleter as well.
    // Deleter is a pointer to a function with signature `void func(ZlibData *)`.
    // This way, we can use the default move constructor and default move assignment
    // for this class without implementing them on our own.
    std::unique_ptr<ZlibData, void (*)(ZlibData *)> zlib_data_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
