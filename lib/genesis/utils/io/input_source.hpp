#ifndef GENESIS_UTILS_IO_INPUT_SOURCE_H_
#define GENESIS_UTILS_IO_INPUT_SOURCE_H_

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

#include "genesis/utils/io/base_input_source.hpp"
#include "genesis/utils/io/file_input_source.hpp"
#include "genesis/utils/io/gzip_input_source.hpp"
#include "genesis/utils/io/stream_input_source.hpp"
#include "genesis/utils/io/string_input_source.hpp"

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/tools/gzip.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Input Source Convenience Functions
// =================================================================================================

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

inline std::shared_ptr<BaseInputSource> from_string(
    std::string const& input_string
) {
    return std::make_shared< StringInputSource >( input_string );
}

inline std::vector<std::shared_ptr<BaseInputSource>> from_strings(
    std::vector<std::string> const& input_strings
) {
    std::vector<std::shared_ptr<BaseInputSource>> ret;
    for( size_t i = 0; i < input_strings.size(); ++i ) {
        ret.emplace_back( from_string( input_strings[i] ));
    }
    return ret;
}

inline std::shared_ptr<BaseInputSource> from_stream(
    std::istream& input_stream
) {
    return std::make_shared< StreamInputSource >( input_stream );
}

} // namespace utils
} // namespace genesis

#endif // include guard
