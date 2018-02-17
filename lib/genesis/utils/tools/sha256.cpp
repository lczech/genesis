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

/*
    =================================================================
        SHA256 License
    =================================================================

    Updated to C++, zedwood.com 2012
    Based on Olivier Gay's version
    See Modified BSD License below:

    FIPS 180-2 SHA-224/256/384/512 implementation
    Issue date:  04/30/2005
    http://www.ouah.org/ogay/sha2/

    Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the project nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

#include "genesis/utils/tools/sha256.hpp"

#include <algorithm>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace utils {

// ================================================================================================
//     Constructors and Rule of Five
// ================================================================================================

const unsigned int SHA256::sha256_k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/**
 * @brief Initialize the object for use.
 */
SHA256::SHA256()
{
    reset_();
}

// ================================================================================================
//     Member Functions
// ================================================================================================

/**
 * @brief Add the contents of a string to the hash digest.
 */
void SHA256::update( std::string const& s )
{
    update( s.c_str(), s.size() );
}

void SHA256::update(std::istream& is)
{
    char sbuf[SHA256::BlockSize];
    while (true) {

        // Read a block and use it for an update.
        is.read( sbuf, SHA256::BlockSize );
        size_t cnt = is.gcount();
        update( sbuf, cnt );

        // If we didn't get a full block, the stream is done.
        if( cnt != SHA256::BlockSize ) {
            return;
        }
    }
}

void SHA256::update( char const* input, size_t length )
{
    // Ugly conversion, but still better than the silent one used in the original code.
    auto const* in_uchar = reinterpret_cast<unsigned char const*>( input );
    update_( in_uchar, length);
}

/**
 * @brief Finish the calculation, prepare the object for next use, and return the hash.
 */
std::string SHA256::final_hex()
{
    // Calculate digest, also reset for next use.
    return digest_to_hex( final_digest() );
}

/**
 * @brief Finish the calculation, prepare the object for next use, and return the digest.
 */
SHA256::DigestType SHA256::final_digest()
{
    unsigned int block_nb;
    unsigned int pm_len;
    unsigned int len_b;
    block_nb = (1 + ((SHA256::BlockSize - 9) < (len_ % SHA256::BlockSize)));
    len_b = (tot_len_ + len_) << 3;
    pm_len = block_nb << 6;
    memset(block_ + len_, 0, pm_len - len_);
    block_[len_] = 0x80;
    SHA2_UNPACK32(len_b, block_ + pm_len - 4);
    transform_(block_, block_nb);

    // Turn into byte array. Might be useful later.
    // unsigned char digest[SHA256::DigestSize];
    // memset(digest,0,SHA256::DigestSize);
    // for (size_t i = 0 ; i < 8; i++) {
    //     SHA2_UNPACK32(digest_[i], &digest[i << 2]);
    // }

    auto const result = digest_;
    reset_();
    return result;
}

/**
 * @brief Calculate the checksum for the content of a file, given its path.
 */
std::string SHA256::from_file_hex( std::string const& filename )
{
    std::ifstream stream( filename.c_str(), std::ios::binary );
    SHA256 checksum;
    checksum.update(stream);
    return checksum.final_hex();
}

/**
 * @brief Calculate the hash digest for the content of a file, given its path.
 */
SHA256::DigestType SHA256::from_file_digest( std::string const& filename )
{
    std::ifstream stream( filename.c_str(), std::ios::binary );
    SHA256 checksum;
    checksum.update(stream);
    return checksum.final_digest();
}

/**
 * @brief Calculate the checksum for the content of a string.
 */
std::string SHA256::from_string_hex( std::string const& input )
{
    SHA256 checksum;
    checksum.update( input );
    return checksum.final_hex();
}

/**
 * @brief Calculate the hash digest for the content of a string.
 */
SHA256::DigestType SHA256::from_string_digest( std::string const& input )
{
    SHA256 checksum;
    checksum.update( input );
    return checksum.final_digest();
}

std::string SHA256::digest_to_hex( SHA256::DigestType const& digest )
{
    // Simple version. Equally as fast as the printf one below.
    std::ostringstream result;
    for (size_t i = 0; i < digest.size(); ++i) {
        result << std::hex << std::setfill('0') << std::setw(8);
        result << static_cast<int>( digest[i] );
    }
    return result.str();

    // Print bytes to string.
    // char buf[ 2 * DigestSize + 1 ];
    // buf[ 2 * DigestSize ] = '\0';
    // for( size_t i = 0; i < 8; i++ ) {
    //     sprintf( buf + i * 8, "%08x", digest[i] );
    // }
    // return std::string(buf);
}

SHA256::DigestType SHA256::hex_to_digest( std::string const& hex )
{
    // Safety first!
    bool const all_hex = std::all_of( hex.begin(), hex.end(), []( char c ){
        return std::isxdigit( c );
    });
    if( hex.size() != 64 || ! all_hex ) {
        throw std::runtime_error( "Invalid SHA256 hex string." );
    }

    // The following test was introduced to check the scanf format "%8x",
    // which just is an "unsigned int", which is not a fixed size.
    // We now use the SCNxN typedefs that offer fixed with replacements, see
    // http://pubs.opengroup.org/onlinepubs/009604599/basedefs/inttypes.h.html

    // Make sure that the scan works! Need to have 32 bit type.
    // static_assert(
    //     sizeof( unsigned int ) == 4,
    //     "Cannot compile SHA256::hex_to_digest() with sizeof( unsigned int ) != 4"
    // );

    // Convert.
    SHA256::DigestType result;
    for (size_t i = 0; i < result.size(); ++i) {
        // auto const n = sscanf( &hex[ 8 * i ], "%8x", &(result[i]) );
        auto const n = sscanf( &hex[ 8 * i ], "%8" SCNx32, &(result[i]) );
        if( n != 1 ) {
            throw std::runtime_error( "Invalid SHA256 hex string." );
        }
    }

    return result;
}

// ================================================================================================
//     Internal Functions
// ================================================================================================

void SHA256::reset_()
{
    digest_[0] = 0x6a09e667;
    digest_[1] = 0xbb67ae85;
    digest_[2] = 0x3c6ef372;
    digest_[3] = 0xa54ff53a;
    digest_[4] = 0x510e527f;
    digest_[5] = 0x9b05688c;
    digest_[6] = 0x1f83d9ab;
    digest_[7] = 0x5be0cd19;
    len_ = 0;
    tot_len_ = 0;
}

void SHA256::update_( unsigned char const* message, size_t len )
{
    unsigned int block_nb;
    unsigned int new_len, rem_len, tmp_len;
    const unsigned char *shifted_message;
    tmp_len = SHA256::BlockSize - len_;
    rem_len = len < tmp_len ? len : tmp_len;
    memcpy( &block_[len_], message, rem_len );
    if (len_ + len < SHA256::BlockSize) {
        len_ += len;
        return;
    }
    new_len = len - rem_len;
    block_nb = new_len / SHA256::BlockSize;
    shifted_message = message + rem_len;
    transform_( block_, 1);
    transform_(shifted_message, block_nb);
    rem_len = new_len % SHA256::BlockSize;
    memcpy( block_, &shifted_message[block_nb << 6], rem_len);
    len_ = rem_len;
    tot_len_ += (block_nb + 1) << 6;
}

void SHA256::transform_( unsigned char const* message, unsigned int block_nb )
{
    uint32_t w[64];
    uint32_t wv[8];
    uint32_t t1, t2;
    const unsigned char *sub_block;
    int i;
    int j;
    for (i = 0; i < (int) block_nb; i++) {
        sub_block = message + (i << 6);
        for (j = 0; j < 16; j++) {
            w[j] = SHA2_PACK32(&sub_block[j << 2]);
        }
        for (j = 16; j < 64; j++) {
            w[j] =  SHA256_F4(w[j -  2]) + w[j -  7] + SHA256_F3(w[j - 15]) + w[j - 16];
        }
        for (j = 0; j < 8; j++) {
            wv[j] = digest_[j];
        }
        for (j = 0; j < 64; j++) {
            t1 = wv[7] + SHA256_F2(wv[4]) + SHA2_CH(wv[4], wv[5], wv[6])
                + sha256_k[j] + w[j];
            t2 = SHA256_F1(wv[0]) + SHA2_MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }
        for (j = 0; j < 8; j++) {
            digest_[j] += wv[j];
        }
    }
}

} // namespace utils
} // namespace genesis
