#ifndef GENESIS_UTILS_IO_STREAM_INPUT_SOURCE_H_
#define GENESIS_UTILS_IO_STREAM_INPUT_SOURCE_H_

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
#include <istream>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Stream Input Source
// =================================================================================================

/**
 * @brief Input source for reading byte data from an istream.
 *
 * The input stream is provided via the constructor. It is not owned by this class, thus
 * the owner must keep it alive as long as reading from it is required, and is responsbile for
 * destroying it. This class merely keeps a reference to it.
 */
class StreamInputSource final : public BaseInputSource
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the input source from an `std::istream`.
     */
    explicit StreamInputSource( std::istream& in )
        : source_( &in )
    {}

    StreamInputSource( StreamInputSource const& ) = default;
    StreamInputSource( StreamInputSource&& )      = default;

    StreamInputSource& operator= ( StreamInputSource const& ) = default;
    StreamInputSource& operator= ( StreamInputSource&& )      = default;

    ~StreamInputSource() override
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
        source_->read( buffer, size );
        return source_->gcount();
    }

    /**
     * @brief Override of the source name funtion. Returns "input stream".
     */
    std::string source_name_() const override
    {
        return "input stream";
    }

    /**
     * @brief Override of the source string funtion. Returns "stream".
     */
    std::string source_string_() const override
    {
        return "stream";
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    std::istream* source_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
