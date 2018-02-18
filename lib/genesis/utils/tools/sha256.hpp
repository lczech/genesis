#ifndef GENESIS_UTILS_TOOLS_SHA256_H_
#define GENESIS_UTILS_TOOLS_SHA256_H_

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

#include <array>
#include <cstdint>
#include <iosfwd>
#include <string>

namespace genesis {
namespace utils {

// ================================================================================================
//     SHA256
// ================================================================================================

/**
 * @brief Calculate SHA256 hashes for strings and files.
 *
 * After creating an object of this type, call update() with your input data as argument, as often
 * as needed. Then, call final_hex() or final_digest() to obtain the hash and reset the object
 * for reuse.
 *
 * If you simply need the hash for a string or content of a file, use from_string() or
 * from_file(), which are static shortcuts for the above.
 *
 * The implementation is based on http://www.zedwood.com/article/cpp-sha256-function,
 * which itself is based on Olivier Gay's version, and was published with a Modified BSD License:
 *
 * > Updated to C++, zedwood.com 2012
 * > Based on Olivier Gay's version
 * > See Modified BSD License below:
 * >
 * > FIPS 180-2 SHA-224/256/384/512 implementation
 * > Issue date:  04/30/2005
 * > http://www.ouah.org/ogay/sha2/
 * >
 * > Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
 * > All rights reserved.
 * >
 * > Redistribution and use in source and binary forms, with or without
 * > modification, are permitted provided that the following conditions
 * > are met:
 * > 1. Redistributions of source code must retain the above copyright
 * >    notice, this list of conditions and the following disclaimer.
 * > 2. Redistributions in binary form must reproduce the above copyright
 * >    notice, this list of conditions and the following disclaimer in the
 * >    documentation and/or other materials provided with the distribution.
 * > 3. Neither the name of the project nor the names of its contributors
 * >    may be used to endorse or promote products derived from this software
 * >    without specific prior written permission.
 * >
 * > THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * > ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * > IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * > ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * > FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * > DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * > OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * > HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * > LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * > OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * > SUCH DAMAGE.
 *
 * See also @link supplement_acknowledgements_code_reuse_sha256 Acknowledgements@endlink.
 */
class SHA256
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Constants
    // -------------------------------------------------------------------------

    static const size_t BlockSize = ( 512 / 8 );
    static const size_t DigestSize = ( 256 / 8);

    /**
     * @brief Store a SHA256 digest.
     *
     * This type can be used for storing hashes instead of the hex representation. It only
     * needs 32 bytes instead of 64 bytes for the hex. As it is a `std::array`, this type can
     * be compared using normal `==`.
     */
    using DigestType = std::array< uint32_t, 8 >;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SHA256();
    ~SHA256() = default;

    SHA256( SHA256 const& ) = default;
    SHA256( SHA256&& )      = default;

    SHA256& operator= ( SHA256 const& ) = default;
    SHA256& operator= ( SHA256&& )      = default;

    // -------------------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------------------

    void update( std::string const& s );
    void update( std::istream& is );
    void update( char const* input, size_t length );

    std::string final_hex();
    DigestType  final_digest();

    static std::string from_file_hex(      std::string const& filename );
    static DigestType  from_file_digest(   std::string const& filename );

    static std::string from_string_hex(    std::string const& input );
    static DigestType  from_string_digest( std::string const& input );

    static std::string digest_to_hex( DigestType const& digest );
    static DigestType hex_to_digest( std::string const& hex );

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    void reset_();

    // SHA256 block update operation. Continues an SHA256 message-digest
    // operation, processing another message block
    void update_( unsigned char const* message, size_t len );

    void transform_( unsigned char const* message, unsigned int block_nb );

    static inline uint32_t SHA2_SHFR( uint32_t x, uint32_t n )
    {
        return x >> n;
    }

    static inline uint32_t SHA2_ROTR( uint32_t x, uint32_t n )
    {
        return ((x >> n) | (x << ((sizeof(x) << 3) - n)));
    }

    static inline uint32_t SHA2_ROTL( uint32_t x, uint32_t n )
    {
        return ((x << n) | (x >> ((sizeof(x) << 3) - n)));
    }

    static inline uint32_t SHA2_CH( uint32_t x, uint32_t y, uint32_t z )
    {
        return ((x & y) ^ (~x & z));
    }

    static inline uint32_t SHA2_MAJ( uint32_t x, uint32_t y, uint32_t z )
    {
        return ((x & y) ^ (x & z) ^ (y & z));
    }

    static inline uint32_t SHA256_F1( uint32_t x )
    {
        return (SHA2_ROTR(x,  2) ^ SHA2_ROTR(x, 13) ^ SHA2_ROTR(x, 22));
    }

    static inline uint32_t SHA256_F2( uint32_t x )
    {
        return (SHA2_ROTR(x,  6) ^ SHA2_ROTR(x, 11) ^ SHA2_ROTR(x, 25));
    }

    static inline uint32_t SHA256_F3( uint32_t x )
    {
        return (SHA2_ROTR(x,  7) ^ SHA2_ROTR(x, 18) ^ SHA2_SHFR(x,  3));
    }

    static inline uint32_t SHA256_F4( uint32_t x )
    {
        return (SHA2_ROTR(x, 17) ^ SHA2_ROTR(x, 19) ^ SHA2_SHFR(x, 10));
    }

    static inline uint32_t SHA2_PACK32( unsigned char const* str )
    {
        return   ((uint32_t) *((str) + 3)      )
               | ((uint32_t) *((str) + 2) <<  8)
               | ((uint32_t) *((str) + 1) << 16)
               | ((uint32_t) *((str) + 0) << 24)
        ;
    }

    static inline void SHA2_UNPACK32( uint32_t x, unsigned char* str)
    {
        *((str) + 3) = (uint8_t) ((x)      );
        *((str) + 2) = (uint8_t) ((x) >>  8);
        *((str) + 1) = (uint8_t) ((x) >> 16);
        *((str) + 0) = (uint8_t) ((x) >> 24);
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    const static uint32_t sha256_k[];

    unsigned int  tot_len_;
    unsigned int  len_;
    unsigned char block_[ 2 * BlockSize ];

    DigestType digest_;

};

} // namespace utils
} // namespace genesis

// ================================================================================================
//     Standard Hash Function
// ================================================================================================

namespace std
{
    /**
     * @brief Hash function for SHA256 digestes.
     *
     * Basically, we re-hash from 256 bit to 64 bit. This is ugly, but currently faster to implement
     * than a custom container that uses the full hash width. Might work on this in the future.
     */
    template<>
    struct hash<genesis::utils::SHA256::DigestType>
    {
        using argument_type = genesis::utils::SHA256::DigestType;
        using result_type   = std::size_t;

        result_type operator()( argument_type const& s ) const {
            result_type hash = 0;
            hash ^= s[0] ^ ( static_cast<result_type>( s[1] ) << 32 );
            hash ^= s[2] ^ ( static_cast<result_type>( s[3] ) << 32 );
            hash ^= s[4] ^ ( static_cast<result_type>( s[5] ) << 32 );
            hash ^= s[6] ^ ( static_cast<result_type>( s[7] ) << 32 );
            return hash;
        }
    };
}

#endif // include guard
