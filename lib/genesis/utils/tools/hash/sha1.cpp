/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

/*
    The implementation is based on [https://github.com/vog/sha1](https://github.com/vog/sha1),
    which is 100% Public Domain, see also the Acknowledgements section of the documentation.
*/

#include "genesis/utils/tools/hash/sha1.hpp"

#include "genesis/utils/io/input_buffer.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstdio>
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

SHA1::SHA1()
{
    reset_();
}

// ================================================================================================
//     Full Hashing
// ================================================================================================

std::string SHA1::read_hex( std::shared_ptr<BaseInputSource> source )
{
    SHA1 checksum;
    checksum.update( source );
    return checksum.final_hex();
}

SHA1::DigestType SHA1::read_digest( std::shared_ptr<BaseInputSource> source )
{
    SHA1 checksum;
    checksum.update( source );
    return checksum.final_digest();
}

std::string SHA1::digest_to_hex( SHA1::DigestType const& digest )
{
    /* Hex std::string */
    std::ostringstream result;
    for (size_t i = 0; i < digest.size(); ++i) {
        result << std::hex << std::setfill('0') << std::setw(8);
        result << digest[i];
    }

    return result.str();
}

SHA1::DigestType SHA1::hex_to_digest( std::string const& hex )
{
    // Safety first!
    bool const all_hex = std::all_of( hex.begin(), hex.end(), []( char c ){
        return std::isxdigit( c );
    });
    if( hex.size() != 40 || ! all_hex ) {
        throw std::runtime_error( "Invalid SHA1 hex string." );
    }

    // Convert.
    SHA1::DigestType result;
    for (size_t i = 0; i < result.size(); ++i) {

        // Read the symbols into the digest. We tried this before using sscanf(), but that got quite
        // messy, as the int widths of the hex format macros are not consistent across compilers...
        // So now we copy the individual fragements to a string. Bit expensive, but a digest is short.
        std::string const sub = hex.substr( 8*i, 8 );
        char* endptr;
        auto const  res = strtoul( sub.c_str(), &endptr, 16 );
        if( *endptr != 0 ) {
            throw std::runtime_error( "Invalid SHA1 hex string: \"" + hex + "\"" );
        }
        result[i] = static_cast<uint32_t>(res);
    }

    return result;
}

// ================================================================================================
//     Member Functions
// ================================================================================================

void SHA1::clear()
{
    reset_();
}

void SHA1::update( std::shared_ptr<BaseInputSource> source )
{
    auto ib = InputBuffer( source );
    char sbuf[SHA1::BlockBytes];

    while (true) {

        // Read a block and use it for an update.
        auto count = ib.read( sbuf, SHA1::BlockBytes - buffer_.size() );
        buffer_.append( sbuf, count );

        // If we didn't get a full block, the input is done.
        if (buffer_.size() != SHA1::BlockBytes) {
            return;
        }

        // If the buffer is full, transform it.
        uint32_t block[SHA1::BlockInts];
        buffer_to_block_(buffer_, block);
        transform_( block );
        buffer_.clear();
    }
}

void SHA1::update( std::string const& s )
{
    std::istringstream is(s);
    update(is);
}

void SHA1::update( std::istream& is )
{
    char sbuf[SHA1::BlockBytes];

    while (true) {
        is.read(sbuf, SHA1::BlockBytes - buffer_.size());
        buffer_.append(sbuf, is.gcount());
        if (buffer_.size() != SHA1::BlockBytes) {
            return;
        }
        uint32_t block[SHA1::BlockInts];
        buffer_to_block_(buffer_, block);
        transform_( block );
        buffer_.clear();
    }
}

std::string SHA1::final_hex()
{
    // Calculate digest, also reset for next use.
    return digest_to_hex( final_digest() );
}

SHA1::DigestType SHA1::final_digest()
{
    /* Total number of hashed bits */
    uint64_t total_bits = (transforms_*SHA1::BlockBytes + buffer_.size()) * 8;

    /* Padding */
    buffer_ += static_cast<char>( 0x80 );
    size_t orig_size = buffer_.size();
    while (buffer_.size() < SHA1::BlockBytes) {
        buffer_ += static_cast<char>( 0x00 );
    }

    uint32_t block[SHA1::BlockInts];
    buffer_to_block_(buffer_, block);

    if (orig_size > SHA1::BlockBytes - 8) {
        transform_( block );
        for (size_t i = 0; i < SHA1::BlockInts - 2; i++) {
            block[i] = 0;
        }
    }

    /* Append total_bits, split this uint64_t into two uint32_t */
    block[SHA1::BlockInts - 1] = total_bits;
    block[SHA1::BlockInts - 2] = (total_bits >> 32);
    transform_( block );

    auto result = digest_;

    /* Reset for next run */
    reset_();

    return result;
}

// ================================================================================================
//     Internal Functions
// ================================================================================================

void SHA1::reset_()
{
    /* SHA1 initialization constants */
    digest_[0] = 0x67452301;
    digest_[1] = 0xefcdab89;
    digest_[2] = 0x98badcfe;
    digest_[3] = 0x10325476;
    digest_[4] = 0xc3d2e1f0;

    /* Reset counters */
    buffer_     = "";
    transforms_ = 0;
}

uint32_t SHA1::rol_(const uint32_t value, const size_t bits)
{
    return (value << bits) | (value >> (32 - bits));
}

uint32_t SHA1::blk_(const uint32_t block[SHA1::BlockInts], const size_t i)
{
    return rol_(block[(i+13)&15] ^ block[(i+8)&15] ^ block[(i+2)&15] ^ block[i], 1);
}

void SHA1::R0_(
    const uint32_t block[SHA1::BlockInts], const uint32_t v, uint32_t& w, const uint32_t x,
    const uint32_t y, uint32_t& z, const size_t i
) {
    z += ((w&(x^y))^y) + block[i] + 0x5a827999 + rol_(v, 5);
    w = rol_(w, 30);
}

void SHA1::R1_(
    uint32_t block[SHA1::BlockInts], const uint32_t v, uint32_t& w, const uint32_t x,
    const uint32_t y, uint32_t& z, const size_t i
) {
    block[i] = blk_(block, i);
    z += ((w&(x^y))^y) + block[i] + 0x5a827999 + rol_(v, 5);
    w = rol_(w, 30);
}

void SHA1::R2_(
    uint32_t block[SHA1::BlockInts], const uint32_t v, uint32_t& w, const uint32_t x,
    const uint32_t y, uint32_t& z, const size_t i
) {
    block[i] = blk_(block, i);
    z += (w^x^y) + block[i] + 0x6ed9eba1 + rol_(v, 5);
    w = rol_(w, 30);
}

void SHA1::R3_(
    uint32_t block[SHA1::BlockInts], const uint32_t v, uint32_t& w, const uint32_t x,
    const uint32_t y, uint32_t& z, const size_t i
) {
    block[i] = blk_(block, i);
    z += (((w|x)&y)|(w&x)) + block[i] + 0x8f1bbcdc + rol_(v, 5);
    w = rol_(w, 30);
}


void SHA1::R4_(
    uint32_t block[SHA1::BlockInts], const uint32_t v, uint32_t& w, const uint32_t x,
    const uint32_t y, uint32_t& z, const size_t i
) {
    block[i] = blk_(block, i);
    z += (w^x^y) + block[i] + 0xca62c1d6 + rol_(v, 5);
    w = rol_(w, 30);
}

void SHA1::transform_( uint32_t block[SHA1::BlockInts] )
{
    // Hash a single 512-bit block. This is the core of the algorithm.

    // Copy digest[] to working vars
    uint32_t a = digest_[0];
    uint32_t b = digest_[1];
    uint32_t c = digest_[2];
    uint32_t d = digest_[3];
    uint32_t e = digest_[4];

    // 4 rounds of 20 operations each. Loop unrolled.
    R0_(block, a, b, c, d, e,  0);
    R0_(block, e, a, b, c, d,  1);
    R0_(block, d, e, a, b, c,  2);
    R0_(block, c, d, e, a, b,  3);
    R0_(block, b, c, d, e, a,  4);
    R0_(block, a, b, c, d, e,  5);
    R0_(block, e, a, b, c, d,  6);
    R0_(block, d, e, a, b, c,  7);
    R0_(block, c, d, e, a, b,  8);
    R0_(block, b, c, d, e, a,  9);
    R0_(block, a, b, c, d, e, 10);
    R0_(block, e, a, b, c, d, 11);
    R0_(block, d, e, a, b, c, 12);
    R0_(block, c, d, e, a, b, 13);
    R0_(block, b, c, d, e, a, 14);
    R0_(block, a, b, c, d, e, 15);
    R1_(block, e, a, b, c, d,  0);
    R1_(block, d, e, a, b, c,  1);
    R1_(block, c, d, e, a, b,  2);
    R1_(block, b, c, d, e, a,  3);
    R2_(block, a, b, c, d, e,  4);
    R2_(block, e, a, b, c, d,  5);
    R2_(block, d, e, a, b, c,  6);
    R2_(block, c, d, e, a, b,  7);
    R2_(block, b, c, d, e, a,  8);
    R2_(block, a, b, c, d, e,  9);
    R2_(block, e, a, b, c, d, 10);
    R2_(block, d, e, a, b, c, 11);
    R2_(block, c, d, e, a, b, 12);
    R2_(block, b, c, d, e, a, 13);
    R2_(block, a, b, c, d, e, 14);
    R2_(block, e, a, b, c, d, 15);
    R2_(block, d, e, a, b, c,  0);
    R2_(block, c, d, e, a, b,  1);
    R2_(block, b, c, d, e, a,  2);
    R2_(block, a, b, c, d, e,  3);
    R2_(block, e, a, b, c, d,  4);
    R2_(block, d, e, a, b, c,  5);
    R2_(block, c, d, e, a, b,  6);
    R2_(block, b, c, d, e, a,  7);
    R3_(block, a, b, c, d, e,  8);
    R3_(block, e, a, b, c, d,  9);
    R3_(block, d, e, a, b, c, 10);
    R3_(block, c, d, e, a, b, 11);
    R3_(block, b, c, d, e, a, 12);
    R3_(block, a, b, c, d, e, 13);
    R3_(block, e, a, b, c, d, 14);
    R3_(block, d, e, a, b, c, 15);
    R3_(block, c, d, e, a, b,  0);
    R3_(block, b, c, d, e, a,  1);
    R3_(block, a, b, c, d, e,  2);
    R3_(block, e, a, b, c, d,  3);
    R3_(block, d, e, a, b, c,  4);
    R3_(block, c, d, e, a, b,  5);
    R3_(block, b, c, d, e, a,  6);
    R3_(block, a, b, c, d, e,  7);
    R3_(block, e, a, b, c, d,  8);
    R3_(block, d, e, a, b, c,  9);
    R3_(block, c, d, e, a, b, 10);
    R3_(block, b, c, d, e, a, 11);
    R4_(block, a, b, c, d, e, 12);
    R4_(block, e, a, b, c, d, 13);
    R4_(block, d, e, a, b, c, 14);
    R4_(block, c, d, e, a, b, 15);
    R4_(block, b, c, d, e, a,  0);
    R4_(block, a, b, c, d, e,  1);
    R4_(block, e, a, b, c, d,  2);
    R4_(block, d, e, a, b, c,  3);
    R4_(block, c, d, e, a, b,  4);
    R4_(block, b, c, d, e, a,  5);
    R4_(block, a, b, c, d, e,  6);
    R4_(block, e, a, b, c, d,  7);
    R4_(block, d, e, a, b, c,  8);
    R4_(block, c, d, e, a, b,  9);
    R4_(block, b, c, d, e, a, 10);
    R4_(block, a, b, c, d, e, 11);
    R4_(block, e, a, b, c, d, 12);
    R4_(block, d, e, a, b, c, 13);
    R4_(block, c, d, e, a, b, 14);
    R4_(block, b, c, d, e, a, 15);

    /* Add the working vars back into digest[] */
    digest_[0] += a;
    digest_[1] += b;
    digest_[2] += c;
    digest_[3] += d;
    digest_[4] += e;

    /* Count the number of transformations */
    ++transforms_;
}

void SHA1::buffer_to_block_(const std::string& buffer, uint32_t block[SHA1::BlockInts])
{
    // Convert the std::string (byte buffer) to a uint32_t array (MSB)

    for (size_t i = 0; i < SHA1::BlockInts; i++) {
        block[i] = ( buffer[4*i+3] & 0xff )
                 | ( buffer[4*i+2] & 0xff ) << 8
                 | ( buffer[4*i+1] & 0xff ) << 16
                 | ( buffer[4*i+0] & 0xff ) << 24;
    }
}

} // namespace utils
} // namespace genesis
