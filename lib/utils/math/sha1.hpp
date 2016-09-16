#ifndef GENESIS_UTILS_MATH_SHA1_H_
#define GENESIS_UTILS_MATH_SHA1_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include <cstdint>
#include <iostream>
#include <string>

namespace genesis {
namespace utils {

/**
 * @brief Calculate SHA1 hashes for strings and files.
 *
 * After creating an object of this type, call update() with your input data as argument, as often
 * as needed. Then, call final() to obtain the hash and reset the object for reuse.
 *
 * If you simply need the hash for a string or content of a file, use from_string() or
 * from_file(), which are static shortcuts for the above.
 *
 * The implementation is based on [https://github.com/vog/sha1](https://github.com/vog/sha1),
 * which is 100% Public Domain.
 */
class SHA1
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Constants
    // -------------------------------------------------------------------------

    // Number of 32bit integers per SHA1 block.
    static const size_t BlockInts  = 16;
    static const size_t BlockBytes = BlockInts * 4;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SHA1();
    ~SHA1() = default;

    SHA1( SHA1 const& ) = default;
    SHA1( SHA1&& )      = default;

    SHA1& operator= ( SHA1 const& ) = default;
    SHA1& operator= ( SHA1&& )      = default;

    // -------------------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------------------

    void update( std::string const& s );
    void update( std::istream& is );
    std::string final();

    static std::string from_file(   std::string const& filename );
    static std::string from_string( std::string const& input );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    uint32_t    digest_[5];
    std::string buffer_;
    uint64_t    transforms_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
