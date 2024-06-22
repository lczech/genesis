#ifndef GENESIS_UTILS_IO_BASE_INPUT_SOURCE_H_
#define GENESIS_UTILS_IO_BASE_INPUT_SOURCE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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

#include "genesis/utils/core/fs.hpp"

#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Base Input Source
// =================================================================================================

/**
 * @brief Abstract base class for reading byte data from input sources.
 *
 * It offers to read() a certain amount of bytes into a char buffer.
 */
class BaseInputSource
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    BaseInputSource() = default;

    BaseInputSource( BaseInputSource const& ) = default;
    BaseInputSource( BaseInputSource&& )      = default;

    BaseInputSource& operator= ( BaseInputSource const& ) = default;
    BaseInputSource& operator= ( BaseInputSource&& )      = default;

    virtual ~BaseInputSource()
    {}

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    /**
     * @brief Read @p size many bytes into the char @p buffer.
     *
     * Returns the number of bytes that have actually been read, which might be lower than
     * the initial target @p size, e.g., if the end of the input source was reached.
     */
    size_t read( char* buffer, size_t size )
    {
        // Non-virtual interface.
        return read_( buffer, size );
    }

    /**
     * @brief Get a name of the input source. This is intended for user output.
     */
    std::string source_name() const
    {
        // Non-virtual interface.
        return source_name_();
    }

    /**
     * @brief Get a string representing the input source. This is intended for the reader classes,
     * which for example might want to examine the input file name.
     */
    std::string source_string() const
    {
        // Non-virtual interface.
        return source_string_();
    }

    /**
     * @brief Return whether reading from this source is trivial, such as reading from a file,
     * or involves more complex operations, such as reading from a compressed input that needs
     * to be decompressed first.
     *
     * This helps downstream processes to decide if reading can be done in a cheap async thread,
     * or needs to use the global thread pool in order to not oversubscribe the system when many
     * such inputs (e.g., many gzipped files) are read in parallel. A thread that is just reading
     * a block of data from a file is not using computation, and will spend its time in I/O wait,
     * so it's okay to have many of those. However, compressed files need computation, and are
     * hence not trivial.
     */
    bool is_trivial() const
    {
        // Non-virtual interface.
        return is_trivial_();
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    virtual size_t read_( char* buffer, size_t size ) = 0;

    virtual std::string source_name_() const = 0;
    virtual std::string source_string_() const = 0;

    // By default, we just assume trivial reading unless the derived class overrides this.
    virtual bool is_trivial_() const
    {
        return true;
    }

};

} // namespace utils
} // namespace genesis

#endif // include guard
