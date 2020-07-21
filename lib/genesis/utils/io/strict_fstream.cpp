/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,  either version 3 of the License,  or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not,  see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab,  Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35,  D-69118 Heidelberg,  Germany
*/

/*
    The code in this file as well as the according header file lib/utils/io/strict_fstream.hpp are
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

#include "genesis/utils/io/strict_fstream.hpp"

#include "genesis/utils/core/exception.hpp"

#include <cassert>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// ================================================================================================
//     Error Reporting
// ================================================================================================

/**
 * @brief Overload of error-reporting function, to enable use with VS.
 *
 * Ref: http://stackoverflow.com/a/901316/717706
 */
static std::string strerror_()
{
    std::string buff(512, '\0');

    #ifdef _WIN32

        if( strerror_s( &buff[0], buff.size(), errno ) != 0 ) {
            buff = "Unknown error";
        }

    #elif ((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE) || defined(__APPLE__)

        // XSI-compliant strerror_r()
        if( strerror_r( errno, &buff[0], buff.size() ) != 0 ) {
            buff = "Unknown error";
        }

    #else

        // GNU-specific strerror_r()
        auto p = strerror_r(errno, &buff[0], buff.size());
        std::string tmp(p, std::strlen( p ));
        std::swap(buff, tmp);

    #endif

    return std::string( buff.c_str() );
}

// ================================================================================================
//     Internal Helper Functions
// ================================================================================================

static std::string mode_to_string_( std::ios_base::openmode mode )
{
    static const int n_modes = 6;
    static const std::ios_base::openmode mode_val_v[n_modes] =
    {
        std::ios_base::in,
        std::ios_base::out,
        std::ios_base::app,
        std::ios_base::ate,
        std::ios_base::trunc,
        std::ios_base::binary
    };

    static char const* mode_name_v[n_modes] =
    {
        "in",
        "out",
        "app",
        "ate",
        "trunc",
        "binary"
    };

    std::string res;
    for (int i = 0; i < n_modes; ++i) {
        if (mode & mode_val_v[i]) {
            res += (! res.empty()? "|" : "");
            res += mode_name_v[i];
        }
    }

    if (res.empty()) {
        res = "none";
    }
    return res;
}

static void check_mode_( std::string const& filename, std::ios_base::openmode mode )
{
    if ((mode & std::ios_base::trunc) && ! (mode & std::ios_base::out)) {
        throw except::IOError(
            std::string("Strict IO File Stream: open('") + filename + "'): mode error: trunc and not out",
            filename
        );
    } else if ((mode & std::ios_base::app) && ! (mode & std::ios_base::out)) {
        throw except::IOError(
            std::string("Strict IO File Stream: open('") + filename + "'): mode error: app and not out",
            filename
        );
    } else if ((mode & std::ios_base::trunc) && (mode & std::ios_base::app)) {
        throw except::IOError(
            std::string("Strict IO File Stream: open('") + filename + "'): mode error: trunc and app",
            filename
        );
    }
 }

static void check_open_(std::ios * s_p,  std::string const& filename, std::ios_base::openmode mode )
{
    if (s_p->fail()) {
        throw except::IOError(
            std::string("Strict IO File Stream: open('") + filename + "'," + mode_to_string_(mode) +
            "): open failed: " + strerror_(),
            filename
        );
    }
}

static void check_peek_(std::istream * is_p,  std::string const& filename, std::ios_base::openmode mode )
{
    bool peek_failed = true;
    try {
        is_p->peek();
        peek_failed = is_p->fail();
    } catch( std::ios_base::failure const& e ) {
        // Nothing to do.
    }
    if( peek_failed ) {
        throw except::IOError(
            std::string("Strict IO File Stream: open('") + filename + "'," + mode_to_string_(mode) +
            "): peek failed: " + strerror_(),
            filename
        );
    }
    is_p->clear();
}

// ================================================================================================
//     Strict File Streams
// ================================================================================================

void StrictIFStream::open( std::string const& filename, std::ios_base::openmode mode )
{
    mode |= std::ios_base::in;
    exceptions(std::ios_base::badbit);
    check_mode_(filename, mode);
    std::ifstream::open(filename, mode);
    check_open_(this, filename, mode);
    check_peek_(this, filename, mode);
}

void StrictOFStream::open( std::string const& filename, std::ios_base::openmode mode )
{
    mode |= std::ios_base::out;
    exceptions(std::ios_base::badbit);
    check_mode_(filename, mode);
    std::ofstream::open(filename, mode);
    check_open_(this, filename, mode);
}

void StrictFStream::open( std::string const& filename, std::ios_base::openmode mode )
{
    if (! (mode & std::ios_base::out)) {
        mode |= std::ios_base::in;
    }
    exceptions(std::ios_base::badbit);
    check_mode_(filename, mode);
    std::fstream::open(filename, mode);
    check_open_(this, filename, mode);
    check_peek_(this, filename, mode);
}

} // namespace utils
} // namespace genesis
