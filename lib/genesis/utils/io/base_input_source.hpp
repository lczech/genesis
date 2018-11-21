#ifndef GENESIS_UTILS_IO_BASE_INPUT_SOURCE_H_
#define GENESIS_UTILS_IO_BASE_INPUT_SOURCE_H_

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
     * @brief Read `size` many bytes into the char `buffer`.
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

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    virtual size_t read_( char* buffer, size_t size ) = 0;

    virtual std::string source_name_() const = 0;
    virtual std::string source_string_() const = 0;

};

} // namespace utils
} // namespace genesis

#endif // include guard
