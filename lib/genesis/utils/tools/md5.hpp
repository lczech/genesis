#ifndef GENESIS_UTILS_TOOLS_MD5_H_
#define GENESIS_UTILS_TOOLS_MD5_H_

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
//     MD5
// ================================================================================================

/**
 * @brief Calculate MD5 hashes for strings and files.
 *
 * After creating an object of this type, call update() with your input data as argument, as often
 * as needed. Then, call final_hex() or final_digest() to obtain the hash and reset the object
 * for reuse.
 *
 * If you simply need the hash for a string or content of a file, use from_string() or
 * from_file(), which are static shortcuts for the above.
 *
 * The implementation is based on http://www.zedwood.com/article/cpp-md5-function,
 * which itself was converted to C++ class by Frank Thilo (thilo@unix-ag.org) for bzflag
 * (http://www.bzflag.org), and is based on the reference implementation of `RFC 1321`:
 *
 * > Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.
 * >
 * > License to copy and use this software is granted provided that it
 * > is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * > Algorithm" in all material mentioning or referencing this software
 * > or this function.
 * >
 * > License is also granted to make and use derivative works provided
 * > that such works are identified as "derived from the RSA Data
 * > Security, Inc. MD5 Message-Digest Algorithm" in all material
 * > mentioning or referencing the derived work.
 * >
 * > RSA Data Security, Inc. makes no representations concerning either
 * > the merchantability of this software or the suitability of this
 * > software for any particular purpose. It is provided "as is"
 * > without express or implied warranty of any kind.
 * >
 * > These notices must be retained in any copies of any part of this
 * > documentation and/or software.
 *
 * See also @link supplement_acknowledgements_code_reuse_md Acknowledgements@endlink.
 */
class MD5
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Constants
    // -------------------------------------------------------------------------

    using size_type = uint32_t;

    static const size_t BlockSize = 64;

    /**
     * @brief Store an MD5 digest.
     *
     * This type can be used for storing hashes instead of the hex representation. It only
     * needs 16 bytes instead of 32 bytes for the hex. As it is a `std::array`, this type can
     * be compared using normal `==`.
     */
    using DigestType = std::array< uint8_t, 16 >;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    MD5();
    ~MD5() = default;

    MD5( MD5 const& ) = default;
    MD5( MD5&& )      = default;

    MD5& operator= ( MD5 const& ) = default;
    MD5& operator= ( MD5&& )      = default;

    // -------------------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------------------

    void update( std::string const& s );
    void update( std::istream& is );
    void update( char const* input, size_type length );

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

    // MD5 block update operation. Continues an MD5 message-digest
    // operation, processing another message block
    void update_( unsigned char const* input, size_type length );

    // F, G, H and I are basic MD5 functions.
    static inline uint32_t F_(uint32_t x, uint32_t y, uint32_t z);
    static inline uint32_t G_(uint32_t x, uint32_t y, uint32_t z);
    static inline uint32_t H_(uint32_t x, uint32_t y, uint32_t z);
    static inline uint32_t I_(uint32_t x, uint32_t y, uint32_t z);

    // rotates x left n bits.
    static inline uint32_t rotate_left_(uint32_t x, int n);

    // FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
    // Rotation is separate from addition to prevent recomputation.
    static inline void FF_(
        uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac
    );
    static inline void GG_(
        uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac
    );
    static inline void HH_(
        uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac
    );
    static inline void II_(
        uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac
    );

    // apply MD5 algo on a block
    void transform_( const uint8_t block[MD5::BlockSize] );

    // decodes input (unsigned char) into output (uint32_t). Assumes len is a multiple of 4.
    static void decode_( uint32_t output[], const uint8_t input[],  size_type len );

    // encodes input (uint32_t) into output (unsigned char). Assumes len is a multiple of 4.
    static void encode_( uint8_t  output[], const uint32_t input[], size_type len );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    uint8_t buffer_[MD5::BlockSize]; // bytes that didn't fit in last 64 byte chunk
    uint32_t count_[2];   // 64bit counter for number of bits (lo, hi)
    uint32_t state_[4];   // digest so far

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
     * @brief Hash function for MD5 digestes.
     *
     * Basically, we re-hash from 128 bit to 64 bit. This is ugly, but currently faster to implement
     * than a custom container that uses the full hash width. Might work on this in the future.
     */
    template<>
    struct hash<genesis::utils::MD5::DigestType>
    {
        using argument_type = genesis::utils::MD5::DigestType;
        using result_type   = std::size_t;

        // We use two intermediate hashes to allow better optimization.
        result_type operator()( argument_type const& s ) const {
            result_type hash1 = 0;
            result_type hash2 = 0;
            hash1 ^= ( static_cast<result_type>( s[0]  ) <<  0 );
            hash1 ^= ( static_cast<result_type>( s[1]  ) <<  8 );
            hash1 ^= ( static_cast<result_type>( s[2]  ) << 16 );
            hash1 ^= ( static_cast<result_type>( s[3]  ) << 24 );
            hash1 ^= ( static_cast<result_type>( s[4]  ) << 32 );
            hash1 ^= ( static_cast<result_type>( s[5]  ) << 40 );
            hash1 ^= ( static_cast<result_type>( s[6]  ) << 48 );
            hash1 ^= ( static_cast<result_type>( s[7]  ) << 56 );
            hash2 ^= ( static_cast<result_type>( s[8]  ) <<  0 );
            hash2 ^= ( static_cast<result_type>( s[9]  ) <<  8 );
            hash2 ^= ( static_cast<result_type>( s[10] ) << 16 );
            hash2 ^= ( static_cast<result_type>( s[11] ) << 24 );
            hash2 ^= ( static_cast<result_type>( s[12] ) << 32 );
            hash2 ^= ( static_cast<result_type>( s[13] ) << 40 );
            hash2 ^= ( static_cast<result_type>( s[14] ) << 48 );
            hash2 ^= ( static_cast<result_type>( s[15] ) << 56 );
            return hash1 ^ hash2;
        }
    };
}

#endif // include guard
