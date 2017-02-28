/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/utils/math/range_minimum_query.hpp"

#include <assert.h>
#include <limits>
#include <stdexcept>
#include <string>

namespace genesis {
namespace utils {

// ================================================================================================
//     Lookup Tables
// ================================================================================================

/**
 * @brief Precomputed catalan_numbers_ triangle (17 is enough for 64bit computing):
 */
const size_t RangeMinimumQuery::catalan_numbers_[17][17] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 },
    { 0, 0, 2, 5, 9, 14, 20, 27, 35, 44, 54, 65, 77, 90, 104, 119, 135 },
    { 0, 0, 0, 5, 14, 28, 48, 75, 110, 154, 208, 273, 350, 440, 544, 663, 798 },
    { 0, 0, 0, 0, 14, 42, 90, 165, 275, 429, 637, 910, 1260, 1700, 2244, 2907, 3705 },
    { 0, 0, 0, 0, 0, 42, 132, 297, 572, 1001, 1638, 2548, 3808, 5508, 7752, 10659, 14364 },
    { 0, 0, 0, 0, 0, 0, 132, 429, 1001, 2002, 3640, 6188, 9996, 15504, 23256, 33915, 48279 },
    { 0, 0, 0, 0, 0, 0, 0, 429, 1430, 3432, 7072, 13260, 23256, 38760, 62016, 95931, 144210 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1430, 4862, 11934, 25194, 48450, 87210, 149226, 245157, 389367 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 4862, 16796, 41990, 90440, 177650, 326876, 572033, 961400 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16796, 58786, 149226, 326876, 653752, 1225785, 2187185 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 58786, 208012, 534888, 1188640, 2414425, 4601610 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 208012, 742900, 1931540, 4345965, 8947575 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 742900, 2674440, 7020405, 15967980 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2674440, 9694845, 25662825 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9694845, 35357670 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35357670 }
};

/**
 * @brief The following stuff is for fast base 2 logarithms:
 * (currently only implemented for 32 bit numbers)
 */
const unsigned char RangeMinimumQuery::log_table_256_[256] =
{
    0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
};

/**
 * @brief Least Significant Bits for 8-bit-numbers.
 */
const unsigned char RangeMinimumQuery::lsb_table_256_[256] =
{
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

const RangeMinimumQuery::SuccinctType RangeMinimumQuery::highest_bits_set_[8] =
{
    // ~1, ~3, ~7, ~15, ~31, ~63, ~127
    static_cast< RangeMinimumQuery::SuccinctType >( ~0   ),
    static_cast< RangeMinimumQuery::SuccinctType >( ~1   ),
    static_cast< RangeMinimumQuery::SuccinctType >( ~3   ),
    static_cast< RangeMinimumQuery::SuccinctType >( ~7   ),
    static_cast< RangeMinimumQuery::SuccinctType >( ~15  ),
    static_cast< RangeMinimumQuery::SuccinctType >( ~31  ),
    static_cast< RangeMinimumQuery::SuccinctType >( ~63  ),
    static_cast< RangeMinimumQuery::SuccinctType >( ~127 )
};

// ================================================================================================
//     Constructor and Rule of Five
// ================================================================================================

/**
 * @brief Constructor that takes a vector of integer values by copying it.
 */
RangeMinimumQuery::RangeMinimumQuery( std::vector<RangeMinimumQuery::IntType> const& array )
    : array_( array )
{
    init_();
}

/**
* @brief Constructor that takes a vector of integer values by moving it.
*/
RangeMinimumQuery::RangeMinimumQuery( std::vector<RangeMinimumQuery::IntType>&& array )
    : array_( std::move( array ))
{
    init_();
}

// ================================================================================================
//     Member Functions
// ================================================================================================

/**
 * @brief Main function for the Range Minimum Query.
 *
 * The function returns the index of the minimum element in the given vector of integers
 * (see RangeMinimumQuery()) that lies between the indices `i` and `j`.
 *
 * Caveat: Both given indices are inclusive, i.e., the interval is `[ i, j ]`.
 * If `j < i`, the function throws an `std::invalid_argument`.
 */
size_t RangeMinimumQuery::query( size_t i, size_t j ) const
{
    // Range check.
    if( j < i ) {
        throw std::invalid_argument(
            "Invalid range minimum query with indices i==" +
            std::to_string(i) + " and j==" + std::to_string(j) + "."
        );
    }

    // If the array is too small, we do not precompute the internal data structures,
    // but want to use the naive approach.
    if( naive_ ) {
        size_t naive_min = j;
        for( size_t x = i; x < j; x++ ) {
            if( array_[x] < array_[naive_min] ) {
                naive_min = x;
            }
        }
        return naive_min;
    }

    size_t mb_i = microblock_(i);      // i's microblock
    size_t mb_j = microblock_(j);      // j's microblock
    size_t min, min_i, min_j;         // min: to be returned
    size_t s_mi = mb_i * micro_size_; // start of i's microblock
    size_t i_pos = i - s_mi;          // pos. of i in its microblock

    if( mb_i == mb_j ) { // only one microblock-query
        min_i = clearbits_(precomputed_queries_(block_types_[mb_i], j-s_mi), i_pos);
        min = min_i == 0 ? j : s_mi + lsb_(min_i);
    } else {
        size_t b_i   = block_(i);              // i's block
        size_t b_j   = block_(j);              // j's block
        size_t s_mj  = mb_j * micro_size_;    // start of j's microblock
        size_t j_pos = j - s_mj;              // position of j in its microblock
        min_i = clearbits_(precomputed_queries_(block_types_[mb_i], micro_size_-1), i_pos);
        min = min_i == 0 ? s_mi + micro_size_ - 1 : s_mi + lsb_(min_i); // left in-microblock-query

        // right in-microblock-query
        if( precomputed_queries_(block_types_[mb_j], j_pos) == 0 ) {
            min_j = j;
        } else {
            min_j = s_mj + lsb_(precomputed_queries_(block_types_[mb_j], j_pos));
        }
        if( array_[min_j] < array_[min] ) {
            min = min_j;
        }

        if( mb_j > mb_i + 1 ) { // otherwise we're done!
            size_t s_bi = b_i * block_size_;      // start of block i
            size_t s_bj = b_j * block_size_;      // start of block j
            if( s_bi+micro_size_ > i ) { // do another microblock-query!
                mb_i++; // go one microblock to the right

                // right in-block-query
                if( precomputed_queries_(block_types_[mb_i], micro_size_-1) == 0 ) {
                    min_i = s_bi + block_size_ - 1;
                } else {
                    min_i = s_mi + micro_size_ + lsb_(
                        precomputed_queries_(block_types_[mb_i], micro_size_-1)
                    );
                }
                if( array_[min_i] < array_[min] ) {
                    min = min_i;
                }
            }
            if( j >= s_bj+micro_size_ ) { // and yet another microblock-query!
                mb_j--; // go one microblock to the left

                // right in-block-query
                if( precomputed_queries_(block_types_[mb_j], micro_size_-1) == 0 ) {
                    min_j = s_mj - 1;
                } else {
                    min_j = s_bj + lsb_(precomputed_queries_(block_types_[mb_j], micro_size_-1));
                }
                if( array_[min_j] < array_[min] ) {
                    min = min_j;
                }
            }

            size_t block_difference = b_j - b_i;
            if( block_difference > 1 ) { // otherwise we're done!
                size_t k, twotothek, block_tmp;  // for index calculations in M and M'
                b_i++; // block where out-of-block-query starts
                if( s_bj - s_bi - block_size_ <= super_size_ ) { // just one out-of-block-query
                    k = log2fast_(block_difference - 2);
                    twotothek = 1 << k; // 2^k
                    i = m_(k, b_i);
                    j = m_(k, b_j-twotothek);
                    min_i = array_[i] <= array_[j] ? i : j;
                }
                else { // here we have to answer a superblock-query:
                     size_t sb_i = superblock_(i); // i's superblock
                     size_t sb_j = superblock_(j); // j's superblock

                     block_tmp = block_((sb_i+1)*super_size_); // end of left out-of-block-query
                     k = log2fast_(block_tmp - b_i);
                     twotothek = 1 << k; // 2^k
                     i = m_(k, b_i);
                     j = m_(k, block_tmp+1-twotothek);
                     min_i = array_[i] <= array_[j] ? i : j;

                    block_tmp = block_(sb_j*super_size_); // start of right out-of-block-query
                    k = log2fast_(b_j - block_tmp);
                    twotothek = 1 << k; // 2^k
                    block_tmp--; // going one block to the left doesn't harm and saves some tests
                    i = m_(k, block_tmp);
                    j = m_(k, b_j-twotothek);
                    min_j = array_[i] <= array_[j] ? i : j;

                    if( array_[min_j] < array_[min_i] ) {
                        min_i = min_j;
                    }

                    if( sb_j > sb_i + 1 ) { // finally, the superblock-query:
                        k = log2fast_(sb_j - sb_i - 2);
                        twotothek = 1 << k;
                        i = m_prime_(k, sb_i+1);
                        j = m_prime_(k, sb_j-twotothek);
                        min_j = array_[i] <= array_[j] ? i : j;
                        if (array_[min_j] < array_[min_i]) {
                            min_i = min_j; // does NOT always return leftmost min!!!
                        }
                    }

                }
                if( array_[min_i] < array_[min] ) {
                    min = min_i; // does NOT always return leftmost min!!!
                }
            }
        }
    }

    return min;
}

// ================================================================================================
//     Internal Functions
// ================================================================================================

/**
 * @brief Initialization that populates all internal data, called from the constructors.
 */
void RangeMinimumQuery::init_()
{
    micro_size_ = 1 << 3; // microblock-size
    block_size_ = 1 << 4; // block-size
    super_size_ = 1 << 8; // superblock-size

    auto const n = array_.size();

    // number of microblocks = array_.size() / micro_size_
    // number of blocks      = array_.size() / block_size_
    // number of superblocks = array_.size() / super_size_
    auto const micro_count = microblock_(n-1)+1;
    auto const block_count = block_(n-1)+1;
    auto const super_count = superblock_(n-1)+1;

    // The following is necessary because we've fixed s, s' and s'' according to the computer's
    // word size and NOT according to the input size. This may cause the (super-)block-size
    // to be too big, or, in other words, the array too small. If this code is compiled on
    // a 32-bit computer, this happens iff n < 113. For such small instances it isn't
    // advisable anyway to use the whole data structure, because simpler methods are faster and
    // less space consuming.
    // Thus, in those cases, we default to a naive implementation.
    // We do not need the precomputation. Instead, query() will simply use the array directly.
    if( block_count < super_size_ / (2*block_size_) ) {
        naive_ = true;
        return;
    }

    // Type-calculation for the microblocks and pre-computation of in-microblock-queries:
    block_types_ = std::vector<BlockTypeType>(micro_count);

    precomputed_queries_ = Matrix<SuccinctType>(
        catalan_numbers_[micro_size_][micro_size_], micro_size_
    );
    for( size_t i = 0; i < catalan_numbers_[micro_size_][micro_size_]; i++ ) {
        precomputed_queries_(i, 0) = 1; // init with impossible value
    }

    std::vector<IntType> rp(micro_size_+1); // rp: rightmost path in Cart. tree
    size_t z = 0;            // index in array a
    size_t start;            // start of current block
    size_t end;              // end of current block
    size_t q;                // position in catalan_numbers_ triangle
    size_t p;                // --------- " ----------------
    rp[0] = std::numeric_limits<IntType>::min(); // stopper (minus infinity)

    // prec[i]: the jth bit is 1 iff j is 1. pos. to the left of i where array_[j] < array_[i]
    std::vector<size_t> gstack( micro_size_ );
    size_t gstacksize;
    size_t g; // first position to the left of i where array_[g[i]] < array_[i]

    for( size_t i = 0; i < micro_count; i++ ) { // step through microblocks
        start = z;                      // init start
        end = start + micro_size_;      // end of block (not inclusive!)
        if( end > n ) {
            end = n; // last block could be smaller than s!
        }

        // compute block type as in Fischer/Heun CPM'06:
        q = micro_size_;        // init q
        p = micro_size_-1;      // init p
        block_types_[i] = 0;    // init type (will be increased!)
        rp[1] = array_[z];      // init rightmost path

        while( ++z < end ) {   // step through current block:
            p--;
            while( rp[q-p-1] > array_[z] ) {
                block_types_[i] += catalan_numbers_[p][q]; // update type
                q--;
            }
            rp[q-p] = array_[z]; // add last element to rightmost path
        }

        // precompute in-block-queries for this microblock (if necessary)
        // as in Alstrup et al. SPAA'02:
        if( precomputed_queries_(block_types_[i], 0) == 1 ) {
            precomputed_queries_(block_types_[i], 0) = 0;
            gstacksize = 0;
            for( size_t j = start; j < end; j++ ) {
                while( gstacksize > 0 && (array_[j] < array_[gstack[gstacksize-1]]) ) {
                    gstacksize--;
                }
                if( gstacksize > 0 ) {
                    g = gstack[gstacksize-1];
                    precomputed_queries_(block_types_[i], j-start)
                        = precomputed_queries_(block_types_[i], g-start) | (1 << (g % micro_size_));
                } else {
                    precomputed_queries_(block_types_[i], j-start) = 0;
                }
                gstack[gstacksize++] = j;
            }
        }
    }

    // space for out-of-block- and out-of-superblock-queries:
    auto M_depth = static_cast<size_t>(
        floor( log2( static_cast<double>( super_size_ ) / static_cast<double>( block_size_ ) ))
    );
    m_matrix_ = Matrix<SuccinctType>(M_depth, block_count);

    auto Mprime_depth = static_cast<size_t>( floor(log2(super_count)) ) + 1;
    m_prime_ = Matrix<size_t>(Mprime_depth, super_count);

    // fill 0'th rows of M and Mprime:
    z = 0; // minimum in current block
    q = 0; // pos. of min in current superblock
    g = 0; // number of current superblock
    for( size_t i = 0; i < block_count; i++ ) { // step through blocks
        start = z;                   // init start
        p = start;                   // init minimum
        end = start + block_size_;   // end of block (not inclusive!)
        if( end > n ) {
            end = n;   // last block could be smaller than block_size_!
        }
        if( array_[z] < array_[q] ) {
            q = z; // update minimum in superblock
        }

        while( ++z < end ) { // step through current block:
            if( array_[z] < array_[p] ) {
                p = z; // update minimum in block
            }
            if( array_[z] < array_[q] ) {
                q = z; // update minimum in superblock
            }
        }
        m_matrix_(0, i) = p-start;                     // store index of block-minimum (offset!)
        if( z % super_size_ == 0 || z == n ) {  // reached end of superblock?
            m_prime_(0, g++) = q;               // store index of superblock-minimum
            q = z;
        }
    }

    // fill M:
    size_t dist = 1; // always 2^(j-1)
    for( size_t j = 1; j < M_depth; j++ ) {
        for( size_t i = 0; i < block_count - dist; i++ ) { // be careful: loop may go too far
            // add 'skipped' elements in a
            if( array_[m_(j-1, i)] <= array_[m_(j-1,i+dist)] ) {
                m_matrix_(j, i) = m_matrix_(j-1, i);
            } else {
                m_matrix_(j, i) = m_matrix_(j-1, i+dist) + (dist*block_size_);
            }
        }
        for (size_t i = block_count - dist; i < block_count; i++) {
            m_matrix_(j, i) = m_matrix_(j-1, i); // fill overhang
        }
        dist *= 2;
    }

    // fill M':
    dist = 1; // always 2^(j-1)
    for( size_t j = 1; j < Mprime_depth; j++ ) {
        for( size_t i = 0; i < super_count - dist; i++ ) {
            // overhang
            if( array_[m_prime_(j-1, i)] <= array_[m_prime_(j-1, i+dist)] ) {
                m_prime_(j, i) = m_prime_(j-1, i);
            } else {
                m_prime_(j, i) = m_prime_(j-1, i+dist);
            }
        }
        for (size_t i = super_count - dist; i < super_count; i++) {
            m_prime_(j, i) = m_prime_(j-1, i);
        }
        dist *= 2;
    }
}

/**
 * @brief Helper function for efficiently calculating log_2 for 32-bit integer numbers.
 */
size_t RangeMinimumQuery::log2fast_( size_t v ) const
{
    size_t c = 0; // c will be lg(v)
    size_t t, tt; // temporaries

    if(( tt = v >> 16 )) {
        c = (t = v >> 24) ? 24 + log_table_256_[t] : 16 + log_table_256_[tt & 0xFF];
    } else {
        c = (t = v >> 8) ? 8 + log_table_256_[t] : log_table_256_[v];
    }
    return c;
}

/**
 * @brief Clear the least significant x bits (change for 64-bit computing).
 */
RangeMinimumQuery::SuccinctType RangeMinimumQuery::clearbits_( SuccinctType n, size_t x ) const
{
    return n & highest_bits_set_[x];
}

} // namespace utils
} // namespace genesis
