#ifndef GENESIS_UTILS_CONTAINERS_INTERVAL_TREE_INTERVAL_H_
#define GENESIS_UTILS_CONTAINERS_INTERVAL_TREE_INTERVAL_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/*
    The code below is adapted from https://github.com/5cript/interval-tree
    which was published under the CC0-1.0 License (Creative Commons Zero v1.0 Universal).
    We modified the code to fit our needs and formatting, but the functionality is the same.
 */

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <cassert>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "genesis/utils/containers/interval_tree/fwd.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Interval Types
// =================================================================================================

/**
 * @brief Helper type to define a left open `(]` Interval.
 */
struct IntervalLeftOpen
{
    template <typename NumericalType>
    static inline bool within(NumericalType b, NumericalType e, NumericalType p)
    {
        return (b < p) && (p <= e);
    }

    template <typename NumericalType>
    static inline std::string to_string(NumericalType b, NumericalType e, bool narrow = false)
    {
        if( narrow ) {
            return "("  + std::to_string(b) + ","  + std::to_string(e) + "]";
        } else {
            return "( " + std::to_string(b) + ", " + std::to_string(e) + " ]";
        }
    }
};

/**
 * @brief Helper type to define a right open `[)` Interval.
 */
struct IntervalRightOpen
{
    template <typename NumericalType>
    static inline bool within(NumericalType b, NumericalType e, NumericalType p)
    {
        return (b <= p) && (p < e);
    }

    template <typename NumericalType>
    static inline std::string to_string(NumericalType b, NumericalType e, bool narrow = false)
    {
        if( narrow ) {
            return "["  + std::to_string(b) + ","  + std::to_string(e) + ")";
        } else {
            return "[ " + std::to_string(b) + ", " + std::to_string(e) + " )";
        }
    }
};

/**
 * @brief Helper type to define a open `()` Interval.
 */
struct IntervalOpen
{
    template <typename NumericalType>
    static inline bool within(NumericalType b, NumericalType e, NumericalType p)
    {
        return (b < p) && (p < e);
    }

    template <typename NumericalType>
    static inline std::string to_string(NumericalType b, NumericalType e, bool narrow = false)
    {
        if( narrow ) {
            return "("  + std::to_string(b) + ","  + std::to_string(e) + ")";
        } else {
            return "( " + std::to_string(b) + ", " + std::to_string(e) + " )";
        }
    }
};

/**
 * @brief Helper type to define a closed `[]` Interval.
 */
struct IntervalClosed
{
    template <typename NumericalType>
    static inline bool within(NumericalType b, NumericalType e, NumericalType p)
    {
        return (b <= p) && (p <= e);
    }

    template <typename NumericalType>
    static inline std::string to_string(NumericalType b, NumericalType e, bool narrow = false)
    {
        if( narrow ) {
            return "["  + std::to_string(b) + ","  + std::to_string(e) + "]";
        } else {
            return "[ " + std::to_string(b) + ", " + std::to_string(e) + " ]";
        }
    }
};

// =================================================================================================
//     Helper Types
// =================================================================================================

/**
 * @brief Default numerical type to use in an Interval.
 */
using DefaultIntervalNumericalType = int;

/**
 * @brief Empty class used as default for Interval data.
 *
 * An Interval offers to store some data with with it, which is for example useful when working
 * with annotated genome regions. By default however, we do not want to store any data,
 * and use this class as a dummy,
 */
struct EmptyIntervalData
{};

// =================================================================================================
//     Interval
// =================================================================================================

/**
 * @brief Type to store an interval (range) between two numbers, as used in the IntervalTree.
 *
 * The implementation is based on [interval-tree](https://github.com/5cript/interval-tree),
 * which is published under the CC0-1.0 License (Creative Commons Zero v1.0 Universal);
 * see our @link supplement_acknowledgements_code_reuse_interval_tree Acknowledgements@endlink
 * for further details, and see IntervalTreeIterator, IntervalTreeNode, and IntervalTree for the
 * other classes based on the original code.
 */
template <
    typename DataType = EmptyIntervalData,
    typename NumericalType = DefaultIntervalNumericalType,
    typename IntervalKind = IntervalClosed
>
struct Interval
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using data_type      = DataType;
    using numerical_type = NumericalType;
    using value_type     = NumericalType;
    using interval_kind  = IntervalKind;

    static_assert(
        std::is_arithmetic<numerical_type>::value,
        "Interval can only be constructued with an arithmetic numerical type."
    );

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Construct an interval.
     *
     * The value @p low must be smaller than or equal to @p high, otherwise an exception is thrown.
     */
    #if __cplusplus >= 201703L
        constexpr
    #endif
    Interval( numerical_type low, numerical_type high )
        : Interval{ low, high, data_type{} }
    {}

    // Save version. See make_safe_interval() for a function to achieve this.
    // Interval(numerical_type low, numerical_type high)
    //     : low_{std::min(low, high)}
    //     , high_{std::max(low, high)}
    // {}

    /**
     * @brief Construct an interval.
     *
     * The value @p low must be smaller than or equal to @p high, otherwise an exception is thrown.
     * The @p data is copied over.
     */
    Interval( numerical_type low, numerical_type high, data_type const& data )
        : Interval{ low, high, data_type{ data }}
    {}

    /**
     * @brief Construct an interval.
     *
     * The value @p low must be smaller than or equal to @p high, otherwise an exception is thrown.
     * The @p data is moved over.
     */
    Interval( numerical_type low, numerical_type high, data_type&& data )
        : low_{low}
        , high_{high}
        , data_{std::move(data)}
    {
        if( low > high ){
            throw std::invalid_argument( "Cannot construct Interval with low > high." );
        }
    }

    ~Interval() = default;

    Interval( Interval const& ) = default;
    Interval( Interval&& )      = default;

    Interval& operator= ( Interval const& ) = default;
    Interval& operator= ( Interval&& )      = default;

    // -------------------------------------------------------------------------
    //     Data Accessors
    // -------------------------------------------------------------------------

    data_type const& data() const
    {
        return data;
    }

    data_type& data()
    {
        return data;
    }

    // -------------------------------------------------------------------------
    //     Comparators
    // -------------------------------------------------------------------------

    /**
     * @brief Return if both intervals equal.
     */
    friend bool operator==(Interval const& lhs, Interval const& other)
    {
        return lhs.low_ == other.low_ && lhs.high_ == other.high_;
    }

    /**
     * @brief Return if both intervals are different.
     */
    friend bool operator!=(Interval const& lhs, Interval const& other)
    {
        return lhs.low_ != other.low_ || lhs.high_ != other.high_;
    }

    /**
     * @brief Return whether the intervals overlap.
     *  For when both intervals are closed.
     */
    bool overlaps(numerical_type l, numerical_type h) const
    {
        return low_ <= h && l <= high_;
    }

    /**
     * @brief Return whether the intervals overlap, excluding border.
     *  For when at least one interval is open (l&r).
     */
    bool overlaps_exclusive(numerical_type l, numerical_type h) const
    {
        return low_ < h && l < high_;
    }

    /**
     * @brief Return whether the intervals overlap
     */
    bool overlaps(Interval const& other) const
    {
        return overlaps(other.low_, other.high_);
    }

    /**
     * @brief Return whether the intervals overlap, excluding border.
     */
    bool overlaps_exclusive(Interval const& other) const
    {
        return overlaps_exclusive(other.low_, other.high_);
    }

    /**
     * @brief Return whether the given value is in this.
     */
    bool within(numerical_type value) const
    {
        return interval_kind::within(low_, high_, value);
    }

    /**
     * @brief Return whether the given interval is in this.
     */
    bool within(Interval const& other) const
    {
        return low_ <= other.low_ && high_ >= other.high_;
    }

    // -------------------------------------------------------------------------
    //     Operators
    // -------------------------------------------------------------------------

    /**
     * @brief Calculates the distance between the two intervals.
     *  Overlapping intervals have 0 distance.
     */
    numerical_type operator-( Interval const& other ) const
    {
        if( overlaps(other) ) {
            return 0;
        }
        if( high_ < other.low_ ) {
            return other.low_ - high_;
        } else {
            return low_ - other.high_;
        }
    }

    /**
     * @brief Return the lower bound of the interval
     */
    numerical_type low() const
    {
        return low_;
    }

    /**
     * @brief Return the upper bound of the interval
     */
    numerical_type high() const
    {
        return high_;
    }

    /**
     * @brief Return the size of the interval, that is, the distance between low and high.
     */
    numerical_type size() const
    {
        return high_ - low_;
    }

    std::string to_string( bool narrow = false ) const
    {
        return interval_kind::to_string( low_, high_, narrow );
    }

    operator std::string() const
    {
        return to_string();
    }

    // -------------------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------------------

private:

    numerical_type low_;
    numerical_type high_;
    data_type  data_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
