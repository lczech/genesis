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
        MD5 License
    =================================================================

    The implementation is based on http://www.zedwood.com/article/cpp-md5-function,
    which itself was converted to C++ class by Frank Thilo (thilo@unix-ag.org)
    for bzflag (http://www.bzflag.org), based on:

        md5.h and md5.c
        reference implemantion of RFC 1321

    Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
    rights reserved.

    License to copy and use this software is granted provided that it
    is identified as the "RSA Data Security, Inc. MD5 Message-Digest
    Algorithm" in all material mentioning or referencing this software
    or this function.

    License is also granted to make and use derivative works provided
    that such works are identified as "derived from the RSA Data
    Security, Inc. MD5 Message-Digest Algorithm" in all material
    mentioning or referencing the derived work.

    RSA Data Security, Inc. makes no representations concerning either
    the merchantability of this software or the suitability of this
    software for any particular purpose. It is provided "as is"
    without express or implied warranty of any kind.

    These notices must be retained in any copies of any part of this
    documentation and/or software.
*/

#include "genesis/utils/tools/md5.hpp"

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

/**
 * @brief Initialize the object for use.
 */
MD5::MD5()
{
    reset_();
}

// ================================================================================================
//     Member Functions
// ================================================================================================

/**
 * @brief Add the contents of a string to the hash digest.
 */
void MD5::update( std::string const& s )
{
    update( s.c_str(), s.size() );
}

void MD5::update(std::istream& is)
{
    char sbuf[MD5::BlockSize];
    while (true) {

        // Read a block and use it for an update.
        is.read( sbuf, MD5::BlockSize );
        size_t cnt = is.gcount();
        update( sbuf, cnt );

        // If we didn't get a full block, the stream is done.
        if( cnt != MD5::BlockSize ) {
            return;
        }
    }
}

void MD5::update( char const* input, MD5::size_type length )
{
    // Ugly conversion, but still better than the silent one used in the original code.
    auto const* in_uchar = reinterpret_cast<unsigned char const*>( input );
    update_( in_uchar, length);
}

/**
 * @brief Finish the calculation, prepare the object for next use, and return the hash.
 */
std::string MD5::final_hex()
{
    // Calculate digest, also reset for next use.
    return digest_to_hex( final_digest() );
}

/**
 * @brief Finish the calculation, prepare the object for next use, and return the digest.
 */
MD5::DigestType MD5::final_digest()
{
    static unsigned char padding[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    // Save number of bits
    unsigned char bits[8];
    encode_(bits, count_, 8);

    // pad out to 56 mod 64.
    size_type index = count_[0] / 8 % 64;
    size_type padLen = (index < 56) ? (56 - index) : (120 - index);
    update_(padding, padLen);

    // Append length (before padding)
    update_(bits, 8);

    // Store state in digest
    encode_( digest_.data(), state_, 16 );

    // Zeroize sensitive information.
    memset(buffer_, 0, sizeof buffer_);
    memset(count_, 0, sizeof count_);

    reset_();
    return digest_;
}

/**
 * @brief Calculate the checksum for the content of a file, given its path.
 */
std::string MD5::from_file_hex( std::string const& filename )
{
    std::ifstream stream( filename.c_str(), std::ios::binary );
    MD5 checksum;
    checksum.update(stream);
    return checksum.final_hex();
}

/**
 * @brief Calculate the hash digest for the content of a file, given its path.
 */
MD5::DigestType MD5::from_file_digest( std::string const& filename )
{
    std::ifstream stream( filename.c_str(), std::ios::binary );
    MD5 checksum;
    checksum.update(stream);
    return checksum.final_digest();
}

/**
 * @brief Calculate the checksum for the content of a string.
 */
std::string MD5::from_string_hex( std::string const& input )
{
    MD5 checksum;
    checksum.update( input );
    return checksum.final_hex();
}

/**
 * @brief Calculate the hash digest for the content of a string.
 */
MD5::DigestType MD5::from_string_digest( std::string const& input )
{
    MD5 checksum;
    checksum.update( input );
    return checksum.final_digest();
}

std::string MD5::digest_to_hex( MD5::DigestType const& digest )
{
    /* Hex std::string */
    std::ostringstream result;
    for (size_t i = 0; i < digest.size(); ++i) {
        result << std::hex << std::setfill('0') << std::setw(2);
        result << static_cast<int>( digest[i] );
    }

    return result.str();
}

MD5::DigestType MD5::hex_to_digest( std::string const& hex )
{
    // Safety first!
    bool const all_hex = std::all_of( hex.begin(), hex.end(), []( char c ){
        return std::isxdigit( c );
    });
    if( hex.size() != 32 || ! all_hex ) {
        throw std::runtime_error( "Invalid MD5 hex string." );
    }

    // The following test was introduced to check the scanf format "%2hhx",
    // which just is an "unsigned char", which is not a fixed size.
    // We now use the SCNxN typedefs that offer fixed with replacements, see
    // http://pubs.opengroup.org/onlinepubs/009604599/basedefs/inttypes.h.html

    // Make sure that the scan works!
    // static_assert(
    //     sizeof( unsigned char ) == 1,
    //     "Cannot compile MD5::hex_to_digest() with sizeof( unsigned char ) != 1"
    // );

    // Convert.
    MD5::DigestType result;
    for (size_t i = 0; i < result.size(); ++i) {
        // auto const n = sscanf( &hex[ 2 * i ], "%2hhx", &(result[i]) );
        auto const n = sscanf( &hex[ 2 * i ], "%2" SCNx8, &(result[i]) );
        if( n != 1 ) {
            throw std::runtime_error( "Invalid MD5 hex string." );
        }
    }

    return result;
}

// ================================================================================================
//     Internal Functions
// ================================================================================================

void MD5::reset_()
{
    count_[0] = 0;
    count_[1] = 0;

    // load magic initialization constants.
    state_[0] = 0x67452301;
    state_[1] = 0xefcdab89;
    state_[2] = 0x98badcfe;
    state_[3] = 0x10325476;
}

void MD5::update_( unsigned char const* input, size_type length )
{
    // compute number of bytes mod 64
    MD5::size_type index = count_[0] / 8 % MD5::BlockSize;

    // Update number of bits
    if ((count_[0] += (length << 3)) < (length << 3)) {
        count_[1]++;
    }
    count_[1] += (length >> 29);

    // number of bytes we need to fill in buffer
    MD5::size_type firstpart = 64 - index;
    MD5::size_type i;

    // transform as many times as possible.
    if (length >= firstpart) {
        // fill buffer first, transform
        memcpy( &buffer_[index], input, firstpart );
        transform_(buffer_);

        // transform chunks of MD5::BlockSize (64 bytes)
        for (i = firstpart; i + MD5::BlockSize <= length; i += MD5::BlockSize) {
            transform_( &input[i] );
        }
        index = 0;
    } else {
        i = 0;
    }

    // buffer remaining input
    memcpy( &buffer_[index], &input[i], length-i );
}

inline uint32_t MD5::F_(uint32_t x, uint32_t y, uint32_t z)
{
    return ( x & y ) | ( ~x & z );
}

inline uint32_t MD5::G_(uint32_t x, uint32_t y, uint32_t z)
{
    return ( x & z ) | ( y & ~z );
}

inline uint32_t MD5::H_(uint32_t x, uint32_t y, uint32_t z)
{
    return x ^ y ^ z;
}

inline uint32_t MD5::I_(uint32_t x, uint32_t y, uint32_t z)
{
    return y ^ ( x | ~z );
}

inline uint32_t MD5::rotate_left_(uint32_t x, int n)
{
    return (x << n) | (x >> (32-n));
}

inline void MD5::FF_(
    uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac
) {
    a = rotate_left_(a+ F_(b,c,d) + x + ac, s) + b;
}

inline void MD5::GG_(
    uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac
) {
    a = rotate_left_(a + G_(b,c,d) + x + ac, s) + b;
}

inline void MD5::HH_(
    uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac
) {
    a = rotate_left_(a + H_(b,c,d) + x + ac, s) + b;
}

inline void MD5::II_(
    uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac
) {
    a = rotate_left_(a + I_(b,c,d) + x + ac, s) + b;
}

void MD5::transform_( const uint8_t block[MD5::BlockSize] )
{
    uint32_t a = state_[0], b = state_[1], c = state_[2], d = state_[3], x[16];
    decode_( x, block, MD5::BlockSize );

    // Constants for MD5Transform routine.
    static uint32_t S11 = 7;
    static uint32_t S12 = 12;
    static uint32_t S13 = 17;
    static uint32_t S14 = 22;
    static uint32_t S21 = 5;
    static uint32_t S22 = 9;
    static uint32_t S23 = 14;
    static uint32_t S24 = 20;
    static uint32_t S31 = 4;
    static uint32_t S32 = 11;
    static uint32_t S33 = 16;
    static uint32_t S34 = 23;
    static uint32_t S41 = 6;
    static uint32_t S42 = 10;
    static uint32_t S43 = 15;
    static uint32_t S44 = 21;

    /* Round 1 */
    FF_ (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF_ (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF_ (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF_ (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF_ (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF_ (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF_ (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF_ (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF_ (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF_ (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF_ (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF_ (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF_ (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF_ (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF_ (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF_ (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    GG_ (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG_ (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG_ (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG_ (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG_ (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG_ (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG_ (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG_ (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG_ (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG_ (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG_ (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG_ (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG_ (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG_ (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG_ (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG_ (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH_ (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH_ (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH_ (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH_ (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH_ (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH_ (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH_ (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH_ (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH_ (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH_ (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH_ (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH_ (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    HH_ (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH_ (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH_ (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH_ (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II_ (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II_ (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II_ (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II_ (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II_ (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II_ (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II_ (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II_ (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II_ (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II_ (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II_ (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II_ (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II_ (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II_ (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II_ (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II_ (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;

    // Zeroize sensitive information.
    memset(x, 0, sizeof x);
}

void MD5::decode_( uint32_t output[], const uint8_t input[],  size_type len )
{
    for (unsigned int i = 0, j = 0; j < len; i++, j += 4) {
        output[i]
            = ((uint32_t)input[j])
            | (((uint32_t)input[j+1]) << 8)
            | (((uint32_t)input[j+2]) << 16)
            | (((uint32_t)input[j+3]) << 24)
        ;
    }
}

void MD5::encode_( uint8_t  output[], const uint32_t input[], size_type len )
{
    for (size_type i = 0, j = 0; j < len; i++, j += 4) {
        output[j]   = input[i] & 0xff;
        output[j+1] = (input[i] >> 8) & 0xff;
        output[j+2] = (input[i] >> 16) & 0xff;
        output[j+3] = (input[i] >> 24) & 0xff;
    }
}

} // namespace utils
} // namespace genesis
