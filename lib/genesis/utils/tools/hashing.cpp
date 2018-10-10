/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/tools/hashing.hpp"

#include "genesis/utils/tools/md5.hpp"
#include "genesis/utils/tools/sha1.hpp"
#include "genesis/utils/tools/sha256.hpp"

#include <iostream>
#include <stdexcept>

namespace genesis {
namespace utils {

// ================================================================================================
//     Hashing
// ================================================================================================

std::string hash_from_file_hex( std::string const& filename, HashingFunctions hash_fct )
{
    switch( hash_fct ) {
        case HashingFunctions::kMD5: {
            return MD5::from_file_hex( filename );
        }
        case HashingFunctions::kSHA1: {
            return SHA1::from_file_hex( filename );
        }
        case HashingFunctions::kSHA256: {
            return SHA256::from_file_hex( filename );
        }
    }

    throw std::invalid_argument( "Invalid hashing function selected." );
}

std::string hash_from_string_hex( std::string const& input, HashingFunctions hash_fct )
{
    switch( hash_fct ) {
        case HashingFunctions::kMD5: {
            return MD5::from_string_hex( input );
        }
        case HashingFunctions::kSHA1: {
            return SHA1::from_string_hex( input );
        }
        case HashingFunctions::kSHA256: {
            return SHA256::from_string_hex( input );
        }
    }

    throw std::invalid_argument( "Invalid hashing function selected." );
}

std::string hash_from_stream_hex( std::istream& is, HashingFunctions hash_fct )
{
    switch( hash_fct ) {
        case HashingFunctions::kMD5: {
            return MD5::from_stream_hex( is );
        }
        case HashingFunctions::kSHA1: {
            return SHA1::from_stream_hex( is );
        }
        case HashingFunctions::kSHA256: {
            return SHA256::from_stream_hex( is );
        }
    }

    throw std::invalid_argument( "Invalid hashing function selected." );
}

} // namespace utils
} // namespace genesis
