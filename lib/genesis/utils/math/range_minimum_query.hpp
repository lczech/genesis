#ifndef GENESIS_UTILS_MATH_RANGE_MINIMUM_QUERY_H_
#define GENESIS_UTILS_MATH_RANGE_MINIMUM_QUERY_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/utils/math/matrix.hpp"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <vector>

namespace genesis {
namespace utils {

/**
 * @brief Class that allows to efficiently find the index of the minimum element within an interval
 * of an array of integer values.
 *
 * The implementation is based on the Succinct RMQ implementation
 * (https://www.bio.ifi.lmu.de/forschung/succinct/#software) by Johannes Fischer, with the author's
 * explicit permission to use this code here. See also
 * @link supplement_acknowledgements_code_reuse_succinct_rmq Acknowledgements@endlink. Most of the
 * original implementation is used as-is.
 *
 * We added some convenience by introducing the ability to use data with only a few elements.
 * The original code expected input data with > 100 elements.
 * Furthermore, we adapted the code to C++11, by replacing manually allocated data (`new ...`) with
 * `std::vector` and utils::Matrix, and by replacing C-style casts with proper C++-casts.
 * The functions, variables and typedefs were renamed and the code reformatted in order to fit our
 * code guidelines.
 */
class RangeMinimumQuery
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Data type of the array for which we want to run RMQ queries.
     *
     * Currently, this is fixed to a signed 32-bit integer. If a wider range is needed,
     * many internal functions need to be adapted first.
     */
    using IntType       = int32_t;

    using SuccinctType  = unsigned char;
    using BlockTypeType = unsigned short;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    RangeMinimumQuery( std::vector<IntType> const& array );
    RangeMinimumQuery( std::vector<IntType>&&      array );

    ~RangeMinimumQuery() = default;

    RangeMinimumQuery( RangeMinimumQuery const& ) = default;
    RangeMinimumQuery( RangeMinimumQuery&& )      = default;

    RangeMinimumQuery& operator= ( RangeMinimumQuery const& ) = default;
    RangeMinimumQuery& operator= ( RangeMinimumQuery&& )      = default;

    // -------------------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------------------

    size_t query( size_t i, size_t j ) const;

    // -------------------------------------------------------------------------
    //     Internal Inline Functions
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Return the microblock-number of entry i.
     */
    inline size_t microblock_( size_t i ) const
    {
        return i / micro_size_;
    }

    /**
     * @brief Return the block-number of entry i.
     */
    inline size_t block_( size_t i ) const
    {
        return i / block_size_;
    }

    /**
     * @brief Return the superblock-number of entry i.
     */
    inline size_t superblock_( size_t i ) const
    {
        return i / super_size_;
    }

    /**
     * @brief Return least signigicant bit in constant time (change for 64bit version).
     */
    inline size_t lsb_( SuccinctType v ) const
    {
        return lsb_table_256_[v];
    }

    /**
     * @brief Because M just stores offsets (rel. to start of block),
     * this method re-calculates the true index:
     */
    inline size_t m_(size_t k, size_t block) const
    {
        return m_matrix_(k, block)+(block*block_size_);
    }

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

    void init_();

    size_t log2fast_( size_t v ) const;
    SuccinctType clearbits_( SuccinctType n, size_t x ) const;

    // -------------------------------------------------------------------------
    //     Lookup Tables
    // -------------------------------------------------------------------------

private:

    static const size_t        catalan_numbers_[17][17];
    static const unsigned char log_table_256_[256];
    static const unsigned char lsb_table_256_[256];
    static const SuccinctType  highest_bits_set_[8];

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    // data
    std::vector<IntType> array_;

    // table M for the out-of-block queries (contains indices of block-minima)
    Matrix<SuccinctType> m_matrix_;

    // table M' for superblock-queries (contains indices of block-minima)
    Matrix<size_t> m_prime_;

    // type of blocks
    std::vector<BlockTypeType> block_types_;

    // precomputed in-block queries
    Matrix<SuccinctType> precomputed_queries_;

    // microblock size
    size_t micro_size_;

    // block size
    size_t block_size_;

    // superblock size
    size_t super_size_;

    // If the data array is too small, we use the naive approach instead.
    bool naive_ = false;

};

} // namespace utils
} // namespace genesis

#endif // include guard
