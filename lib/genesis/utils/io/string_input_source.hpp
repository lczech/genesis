#ifndef GENESIS_UTILS_IO_STRING_INPUT_SOURCE_H_
#define GENESIS_UTILS_IO_STRING_INPUT_SOURCE_H_

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

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/io/base_input_source.hpp"

#include <cstdio>
#include <cstring>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     String Input Source
// =================================================================================================

/**
 * @brief Input source for reading byte data from a string.
 *
 * The input string is provided via the constructor. It is not owned by this class, thus
 * the owner must keep it alive as long as reading from it is required, and is responsbile for
 * destroying it. This class merely keeps a pointer to it.
 *
 * That implies that the string shall not be modified while this input source is used, thus,
 * only const-members of the string can be called.
 */
class StringInputSource final : public BaseInputSource
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the input source from a char array.
     */
    StringInputSource( char const* str, size_t size )
        : in_str_(    str  )
        , cursor_(    str  )
        , in_size_(   size )
        , rest_size_( size )
    {}

    /**
    * @brief Construct the input source from a `std::string`.
    */
    explicit StringInputSource( std::string const& str )
        : in_str_(    str.c_str() )
        , cursor_(    str.c_str() )
        , in_size_(   str.size()  )
        , rest_size_( str.size()  )
    {}

    StringInputSource( StringInputSource const& ) = default;
    StringInputSource( StringInputSource&& )      = default;

    StringInputSource& operator= ( StringInputSource const& ) = default;
    StringInputSource& operator= ( StringInputSource&& )      = default;

    ~StringInputSource() override
    {}

    // -------------------------------------------------------------
    //     Overloaded Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Override of the read function.
     */
    size_t read_( char* buffer, size_t size ) override
    {
        // Don't overshoot.
        if( size > rest_size_ ) {
            size = rest_size_;
        }

        // Read.
        std::memcpy( buffer, cursor_, size );
        cursor_    += size;
        rest_size_ -= size;
        return size;
    }

    /**
     * @brief Override of the source name funtion. Returns "input string".
     */
    std::string source_name_() const override
    {
        return "input string";
    }

    /**
     * @brief Override of the source string funtion. Returns "string".
     */
    std::string source_string_() const override
    {
        return "string";
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    // Original and current string position pointer.
    char const* in_str_;
    char const* cursor_;

    // Original and current (remaining) string size.
    size_t      in_size_;
    size_t      rest_size_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
