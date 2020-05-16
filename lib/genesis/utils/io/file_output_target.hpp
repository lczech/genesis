#ifndef GENESIS_UTILS_IO_FILE_OUTPUT_TARGET_H_
#define GENESIS_UTILS_IO_FILE_OUTPUT_TARGET_H_

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

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/io/base_output_target.hpp"
#include "genesis/utils/io/output_stream.hpp"

#include <fstream>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     File Output Target
// =================================================================================================

/**
 * @brief Output target for writing data to a file.
 *
 * The output file name is provided via the constructor.
 */
class FileOutputTarget final : public BaseOutputTarget
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the output target from a file with the given file name.
     */
    explicit FileOutputTarget(
        std::string const& file_name,
        std::ios_base::openmode mode = std::ios_base::out
    )
        : file_name_( file_name )
        , mode_( mode )
    {}

    FileOutputTarget( FileOutputTarget const& ) = delete;
    FileOutputTarget( FileOutputTarget&& )      = default;

    FileOutputTarget& operator= ( FileOutputTarget const& ) = delete;
    FileOutputTarget& operator= ( FileOutputTarget&& )      = default;

    ~FileOutputTarget() override = default;

    // -------------------------------------------------------------
    //     Overloaded Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Override of the output stream function.
     */
    std::ostream& out_stream_() override
    {
        // Lazy loading.
        if( !stream_.is_open() ) {
            file_output_stream( file_name_, stream_, mode_ );
        }
        return stream_;
    }

    /**
     * @brief Override of the target name funtion. Returns "output file (<file_name>)".
     */
    std::string target_name_() const override
    {
        return "output file (" + file_name_ + ")";
    }

    /**
     * @brief Override of the target string funtion. Returns the file name.
     */
    std::string target_string_() const override
    {
        return file_name_;
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    std::string file_name_;
    std::ofstream stream_;

    std::ios_base::openmode mode_ = std::ios_base::out;
};

} // namespace utils
} // namespace genesis

#endif // include guard
