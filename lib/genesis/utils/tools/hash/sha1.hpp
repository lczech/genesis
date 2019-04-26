#ifndef GENESIS_UTILS_TOOLS_HASH_SHA1_H_
#define GENESIS_UTILS_TOOLS_HASH_SHA1_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/io/input_source.hpp"

#include <array>
#include <cstdint>
#include <iosfwd>
#include <string>

namespace genesis {
namespace utils {

// ================================================================================================
//     SHA1
// ================================================================================================

/**
 * @brief Calculate SHA1 hashes for strings and files.
 *
 * After creating an object of this type, call update() with your input data as argument, as often
 * as needed. Then, call final_hex() or final_digest() to obtain the hash and reset the object
 * for reuse.
 *
 * If you simply need the hash for a string or content of a file, use read_hex() or
 * read_digest(), which are static shortcuts for the above using an input source.
 * Use functions such as utils::from_file() and utils::from_string() to conveniently
 * get an input source that can be used here.
 *
 * The implementation is based on [https://github.com/vog/sha1](https://github.com/vog/sha1),
 * which is 100% Public Domain, see also
 * @link supplement_acknowledgements_code_reuse_sha Acknowledgements@endlink.
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

    /**
     * @brief Store a SHA1 digest.
     *
     * This type can be used for storing hashes instead of the hex representation. It only
     * needs 20 bytes instead of 40 bytes for the hex. As it is a `std::array`, this type can
     * be compared using normal `==`.
     */
    using DigestType = std::array< uint32_t, 5 >;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Initialize the object for use.
     */
    SHA1();
    ~SHA1() = default;

    SHA1( SHA1 const& ) = default;
    SHA1( SHA1&& )      = default;

    SHA1& operator= ( SHA1 const& ) = default;
    SHA1& operator= ( SHA1&& )      = default;

    // -------------------------------------------------------------------------
    //     Full Hashing
    // -------------------------------------------------------------------------

    /**
     * @brief Calculate the checksum for the content of an input source.
     */
    static std::string read_hex( std::shared_ptr<BaseInputSource> source );

    /**
     * @brief Calculate the hash digest for the content of an input source.
     */
    static DigestType  read_digest( std::shared_ptr<BaseInputSource> source );

    static std::string digest_to_hex( DigestType const& digest );
    static DigestType hex_to_digest( std::string const& hex );

    // -------------------------------------------------------------------------
    //     Iterative Hashing
    // -------------------------------------------------------------------------

    /**
     * @brief Reset to initial state, that is, delete any intermediate input from update() calls.
     */
    void clear();

    void update( std::shared_ptr<BaseInputSource> source );

    /**
     * @brief Add the contents of a string to the hash digest.
     */
    void update( std::string const& s );

    /**
     * @brief Add the contents of a stream to the hash digest.
     */
    void update( std::istream& is );

    /**
     * @brief Finish the calculation, prepare the object for next use, and return the hash.
     */
    std::string final_hex();

    /**
     * @brief Finish the calculation, prepare the object for next use, and return the digest.
     */
    DigestType  final_digest();

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    void reset_();

    uint32_t rol_(const uint32_t value, const size_t bits);
    uint32_t blk_(const uint32_t block[SHA1::BlockInts], const size_t i);

    void R0_(
        const uint32_t block[SHA1::BlockInts], const uint32_t v, uint32_t& w, const uint32_t x,
        const uint32_t y, uint32_t& z, const size_t i
    );
    void R1_(
        uint32_t block[SHA1::BlockInts], const uint32_t v, uint32_t& w, const uint32_t x,
        const uint32_t y, uint32_t& z, const size_t i
    );
    void R2_(
        uint32_t block[SHA1::BlockInts], const uint32_t v, uint32_t& w, const uint32_t x,
        const uint32_t y, uint32_t& z, const size_t i
    );
    void R3_(
        uint32_t block[SHA1::BlockInts], const uint32_t v, uint32_t& w, const uint32_t x,
        const uint32_t y, uint32_t& z, const size_t i
    );
    void R4_(
        uint32_t block[SHA1::BlockInts], const uint32_t v, uint32_t& w, const uint32_t x,
        const uint32_t y, uint32_t& z, const size_t i
    );

    /**
     * @brief Hash a single 512-bit block. This is the core of the algorithm.
     */
    void transform_( uint32_t block[SHA1::BlockInts] );

    /**
     * @brief Convert the std::string (byte buffer) to a uint32_t array (MSB)
     */
    void buffer_to_block_(const std::string& buffer, uint32_t block[SHA1::BlockInts]);

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    DigestType  digest_;
    std::string buffer_;
    size_t      transforms_;

};

} // namespace utils
} // namespace genesis

// ================================================================================================
//     Standard Hash Function
// ================================================================================================

namespace std
{
    /**
     * @brief Hash function for SHA1 digestes.
     *
     * Basically, we re-hash from 160 bit to 64 bit. This is ugly, but currently faster to implement
     * than a custom container that uses the full hash width. Might work on this in the future.
     */
    template<>
    struct hash<genesis::utils::SHA1::DigestType>
    {
        using argument_type = genesis::utils::SHA1::DigestType;
        using result_type   = std::size_t;

        result_type operator()( argument_type const& s ) const {
            result_type result = 0;
            result ^= s[0] ^ ( static_cast<result_type>( s[1] ) << 32 );
            result ^= s[2] ^ ( static_cast<result_type>( s[3] ) << 32 );
            result ^= s[4];
            return result;
        }
    };
}

#endif // include guard
