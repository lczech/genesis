#ifndef GENESIS_UTILS_IO_INPUT_SOURCE_H_
#define GENESIS_UTILS_IO_INPUT_SOURCE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
 * @ingroup utils
 */

#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/file_input_source.hpp>
#include <genesis/utils/io/gzip_input_source.hpp>
#include <genesis/utils/io/gzip.hpp>
#include <genesis/utils/io/stream_input_source.hpp>
#include <genesis/utils/io/string_input_source.hpp>

#include <genesis/utils/core/std.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Input Source Convenience Functions
// =================================================================================================

// ---------------------------------------------------------------------
//   file
// ---------------------------------------------------------------------

/**
 * @brief Obtain an input source for reading from a file.
 *
 * The input source returned from this function can be used in the reader classes, e.g.,
 * placement::JplaceReader or sequence::FastaReader.
 *
 * If the parameter @p detect_compression is `true` (default), it is first determined whether the
 * file is gzip compressed, and if so, a transparent decompression layer is added.
 * That means, gzip-compressed files can be decompressed automatically and on the fly.
 *
 * @see See from_files(), from_string(), from_strings(), and from_stream() for similar
 * helper functions for other types of input sources.
 */
inline std::shared_ptr<BaseInputSource> from_file(
    std::string const& file_name,
    bool detect_compression = true
) {
    if( detect_compression && is_gzip_compressed_file( file_name )) {
        return std::make_shared<utils::GzipInputSource>(
            std::make_shared< FileInputSource >( file_name )
        );
    } else {
        return std::make_shared< FileInputSource >( file_name );
    }
}

/**
 * @brief Obtain a set of input sources for reading from files.
 *
 * See from_file() for details. This version returnes multiple input sources, which can be used
 * for parallely reading from multiple files for speedup.
 * This function can for example be used with the output of utils::dir_list_files().
 *
 * @see See from_file(), from_string(), from_strings(), and from_stream() for similar
 * helper functions for other types of input sources.
 */
inline std::vector<std::shared_ptr<BaseInputSource>> from_files(
    std::vector<std::string> const& file_names,
    bool detect_compression = true
) {
    std::vector<std::shared_ptr<BaseInputSource>> ret;
    for( size_t i = 0; i < file_names.size(); ++i ) {
        ret.emplace_back( from_file( file_names[i], detect_compression ));
    }
    return ret;
}

/**
 * @brief Obtain a set of input sources for reading from files.
 *
 * See from_file() for details. This version returnes multiple input sources from an interator over
 * file names, which can be used for parallely reading from multiple files for speedup.
 *
 * @see See from_file(), from_string(), from_strings(), and from_stream() for similar
 * helper functions for other types of input sources.
 */
template<typename InputIterator>
inline std::vector<std::shared_ptr<BaseInputSource>> from_files(
    InputIterator first,
    InputIterator last,
    bool detect_compression = true
) {
    std::vector<std::shared_ptr<BaseInputSource>> ret;
    while( first != last ) {
        ret.emplace_back( from_file( *first, detect_compression ));
        ++first;
    }
    return ret;
}

// ---------------------------------------------------------------------
//   string
// ---------------------------------------------------------------------

/**
 * @brief Obtain an input source for reading from a `std::string`.
 *
 * The input source returned from this function can be used in the reader classes, e.g.,
 * placement::JplaceReader or sequence::FastaReader.
 *
 * @see See from_file(), from_files(), from_strings(), and from_stream() for similar
 * helper functions for other types of input sources.
 */
inline std::shared_ptr<BaseInputSource> from_string(
    std::string const& input_string
) {
    return std::make_shared< StringInputSource >( input_string );
}

/**
 * @brief Obtain an input source for reading from a moved `std::string`.
 *
 * The input source returned from this function can be used in the reader classes, e.g.,
 * placement::JplaceReader or sequence::FastaReader.
 *
 * @see See from_file(), from_files(), from_strings(), and from_stream() for similar
 * helper functions for other types of input sources.
 */
inline std::shared_ptr<BaseInputSource> from_string(
    std::string&& input_string
) {
    return std::make_shared< StringInputSource >( std::move( input_string ));
}

/**
 * @brief Obtain an input source for reading from a `std::string_view`.
 *
 * The input source returned from this function can be used in the reader classes, e.g.,
 * placement::JplaceReader or sequence::FastaReader.
 *
 * @see See from_file(), from_files(), from_strings(), and from_stream() for similar
 * helper functions for other types of input sources.
 */
inline std::shared_ptr<BaseInputSource> from_string(
    std::string_view input_string_view
) {
    return std::make_shared< StringInputSource >( input_string_view );
}

/**
 * @brief Obtain an input source for reading from a `std::string_view`.
 *
 * Alias for from_string( std::string_view ). See there for details.
 */
inline std::shared_ptr<BaseInputSource> from_string_view(
    std::string_view input_string_view
) {
    return std::make_shared< StringInputSource >( input_string_view );
}

/**
 * @brief Obtain an input source for reading from a string literal.
 *
 * This overload is needed as otherwise, the other overloads of this function are ambiguous
 * when called with a string literal.
 *
 * The input source returned from this function can be used in the reader classes, e.g.,
 * placement::JplaceReader or sequence::FastaReader.
 *
 * @see See from_file(), from_files(), from_strings(), and from_stream() for similar
 * helper functions for other types of input sources.
 */
inline std::shared_ptr<BaseInputSource> from_string(
    char const* input_string
) {
    return std::make_shared< StringInputSource >( std::string_view{input_string} );
}

/**
 * @brief Obtain a set of input sources for reading from strings.
 *
 * See from_string() and from_files() for details.
 *
 * @see See from_file(), from_files(), from_string(), and from_stream() for similar
 * helper functions for other types of input sources.
 */
inline std::vector<std::shared_ptr<BaseInputSource>> from_strings(
    std::vector<std::string> const& input_strings
) {
    std::vector<std::shared_ptr<BaseInputSource>> ret;
    for( size_t i = 0; i < input_strings.size(); ++i ) {
        ret.emplace_back( from_string( input_strings[i] ));
    }
    return ret;
}

/**
 * @brief Obtain a set of input sources for reading from strings.
 *
 * See from_string() and from_files() for details.
 *
 * @see See from_file(), from_files(), from_string(), and from_stream() for similar
 * helper functions for other types of input sources.
 */
template<typename InputIterator>
inline std::vector<std::shared_ptr<BaseInputSource>> from_strings(
    InputIterator first,
    InputIterator last
) {
    std::vector<std::shared_ptr<BaseInputSource>> ret;
    while( first != last ) {
        ret.emplace_back( from_string( *first ));
        ++first;
    }
    return ret;
}

// ---------------------------------------------------------------------
//   stream
// ---------------------------------------------------------------------

/**
 * @brief Obtain an input source for reading from a stream.
 *
 * The input source returned from this function can be used in the reader classes, e.g.,
 * placement::JplaceReader or sequence::FastaReader.
 *
 * @see See from_file(), from_files(), from_string(), and from_strings() for similar
 * helper functions for other types of input sources.
 * @see
 */
inline std::shared_ptr<BaseInputSource> from_stream(
    std::istream& input_stream
) {
    return std::make_shared< StreamInputSource >( input_stream );
}

/**
 * @brief Obtain an input source for reading from standard input (i.e., stdin or cin).
 *
 * The input source returned from this function can be used in the reader classes, e.g.,
 * placement::JplaceReader or sequence::FastaReader.
 *
 * @see See from_file(), from_files(), from_string(), and from_strings() for similar
 * helper functions for other types of input sources, and see from_stream() for the underlying
 * stream input helper function.
 */
inline std::shared_ptr<BaseInputSource> from_stdin()
{
    return std::make_shared< StreamInputSource >( std::cin );
}

} // namespace utils
} // namespace genesis

#endif // include guard
