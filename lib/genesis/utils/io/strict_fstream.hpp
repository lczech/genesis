#ifndef GENESIS_UTILS_IO_STRICT_FSTREAM_H_
#define GENESIS_UTILS_IO_STRICT_FSTREAM_H_

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

/*
    The code in this file as well as the according source file lib/utils/io/strict_fstream.cpp are
    adapted from the excellent zstr library (C++ header-only ZLib wrapper" classes) by Matei David,
    see https://github.com/mateidavid/zstr

    We adapted the original code by renaming all classes and variables to our standards,
    moving much of the implementation into a source file, and refining some functionality.

    For this and the according source file, we need to include the following original license:

    The MIT License (MIT)

    Copyright (c) 2015 Matei David, Ontario Institute for Cancer Research

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <fstream>
#include <string>

namespace genesis {
namespace utils {

// ================================================================================================
//     Strict Input File Stream
// ================================================================================================

/**
 * @brief Stream that defines a strict wrapper around std::ifstream
 *
 * The wrapper performs the following steps:
 *
 *  - Check that the open modes make sense
 *  - Check that the call to open() is successful
 *  - Check that the opened file is peek-able
 *  - Turn on the badbit in the exception mask
 *
 * This is useful to ensure proper file handling.
 *
 * The class is based on the strict_fstream::ifstream class of the excellent
 * [zstr library](https://github.com/mateidavid/zstr) by Matei David; see also our
 * @link supplement_acknowledgements_code_reuse_gzip_streams Acknowledgements@endlink.
 *
 * @see StrictOFStream
 * @see StrictFStream
 */
class StrictIFStream
    : public std::ifstream
{
public:

    StrictIFStream() = default;

    StrictIFStream( std::string const& filename, std::ios_base::openmode mode = std::ios_base::in )
    {
        open(filename, mode);
    }

    void open( std::string const& filename, std::ios_base::openmode mode = std::ios_base::in) ;
};

// ================================================================================================
//     Strict Output File Stream
// ================================================================================================

/**
 * @brief Stream that defines a strict wrapper around std::ofstream
 *
 * The wrapper performs the following steps:
 *
 *  - Check that the open modes make sense
 *  - Check that the call to open() is successful
 *  - Turn on the badbit in the exception mask
 *
 * This is useful to ensure proper file handling.
 *
 * The class is based on the strict_fstream::ofstream class of the excellent
 * [zstr library](https://github.com/mateidavid/zstr) by Matei David; see also our
 * @link supplement_acknowledgements_code_reuse_gzip_streams Acknowledgements@endlink.
 *
 * @see StrictIFStream
 * @see StrictFStream
 */
class StrictOFStream
    : public std::ofstream
{
public:

    StrictOFStream() = default;

    StrictOFStream( std::string const& filename, std::ios_base::openmode mode = std::ios_base::out )
    {
        open(filename, mode);
    }

    void open( std::string const& filename, std::ios_base::openmode mode = std::ios_base::out );
};

// ================================================================================================
//     Strict File Stream
// ================================================================================================

/**
 * @brief Stream that defines a strict wrapper around std::fstream
 *
 * The wrapper performs the following steps:
 *
 *  - Check that the open modes make sense
 *  - Check that the call to open() is successful
 *  - Check that the opened file is peek-able
 *  - Turn on the badbit in the exception mask
 *
 * This is useful to ensure proper file handling.
 *
 * The class is based on the strict_fstream::fstream class of the excellent
 * [zstr library](https://github.com/mateidavid/zstr) by Matei David; see also our
 * @link supplement_acknowledgements_code_reuse_gzip_streams Acknowledgements@endlink.
 *
 * @see StrictIFStream
 * @see StrictOFStream
 */
class StrictFStream
    : public std::fstream
{
public:

    StrictFStream() = default;

    StrictFStream( std::string const& filename, std::ios_base::openmode mode = std::ios_base::in )
    {
        open(filename, mode);
    }

    void open( std::string const& filename, std::ios_base::openmode mode = std::ios_base::in );
};

} // namespace utils
} // namespace genesis

#endif // include guard
