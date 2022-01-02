#ifndef GENESIS_UTILS_CONTAINERS_INTERVAL_TREE_FUNCTIONS_H_
#define GENESIS_UTILS_CONTAINERS_INTERVAL_TREE_FUNCTIONS_H_

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

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/containers/interval_tree/interval.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Make Safe Interval
// =================================================================================================

/**
 * Creates a safe Interval that puts the lower bound left automatically.
 *
 * The data of the newly created interval is default constructed.
 */
template <
    typename DataType = EmptyIntervalData,
    typename NumericalType = DefaultIntervalNumericalType,
    typename IntervalKind = IntervalClosed
>
#if __cplusplus >= 201703L
    constexpr
#endif
Interval<DataType, NumericalType, IntervalKind> make_safe_interval(
    NumericalType lhs,
    NumericalType rhs
) {
    return Interval<DataType, NumericalType, IntervalKind>{
        std::min(lhs, rhs), std::max(lhs, rhs)
    };
}

/**
 *  Creates a safe Interval that puts the lower bound left automatically.
 *
 * The provided @p data is copied to the new Interval.
 */
template <
    typename DataType = EmptyIntervalData,
    typename NumericalType = DefaultIntervalNumericalType,
    typename IntervalKind = IntervalClosed
>
#if __cplusplus >= 201703L
    constexpr
#endif
Interval<DataType, NumericalType, IntervalKind> make_safe_interval(
    NumericalType lhs,
    NumericalType rhs,
    DataType const& data
) {
    return Interval<DataType, NumericalType, IntervalKind>{
        std::min(lhs, rhs), std::max(lhs, rhs), data
    };
}

/**
 *  Creates a safe Interval that puts the lower bound left automatically.
 *
 * The provided @p data is moved to the new Interval.
 */
template <
    typename DataType = EmptyIntervalData,
    typename NumericalType = DefaultIntervalNumericalType,
    typename IntervalKind = IntervalClosed
>
#if __cplusplus >= 201703L
    constexpr
#endif
Interval<DataType, NumericalType, IntervalKind> make_safe_interval(
    NumericalType lhs,
    NumericalType rhs,
    DataType&& data
) {
    return Interval<DataType, NumericalType, IntervalKind>{
        std::min(lhs, rhs), std::max(lhs, rhs), std::move(data)
    };
}

// =================================================================================================
//     Join
// =================================================================================================

/**
 * @brief Creates a new Interval that contains both intervals and whatever is between.
 *
 * The data of the newly created interval is default constructed.
 */
template <
    typename DataType = EmptyIntervalData,
    typename NumericalType = DefaultIntervalNumericalType,
    typename IntervalKind = IntervalClosed
>
Interval<DataType, NumericalType, IntervalKind> join(
    Interval<DataType, NumericalType, IntervalKind> const& a,
    Interval<DataType, NumericalType, IntervalKind> const& b
) {
    return { std::min(a.low(), b.low()), std::max(a.high(), b.high()) };
}

/**
 * @brief Creates a new Interval that contains both intervals and whatever is between.
 *
 * The provided @p data is copied to the new Interval.
 */
template <
    typename DataType = EmptyIntervalData,
    typename NumericalType = DefaultIntervalNumericalType,
    typename IntervalKind = IntervalClosed
>
Interval<DataType, NumericalType, IntervalKind> join(
    Interval<DataType, NumericalType, IntervalKind> const& a,
    Interval<DataType, NumericalType, IntervalKind> const& b,
    DataType const& data
) {
    return { std::min(a.low(), b.low()), std::max(a.high(), b.high()), data };
}

/**
 * @brief Creates a new Interval that contains both intervals and whatever is between.
 *
 * The provided @p data is moved to the new Interval.
 */
template <
    typename DataType = EmptyIntervalData,
    typename NumericalType = DefaultIntervalNumericalType,
    typename IntervalKind = IntervalClosed
>
Interval<DataType, NumericalType, IntervalKind> join(
    Interval<DataType, NumericalType, IntervalKind> const& a,
    Interval<DataType, NumericalType, IntervalKind> const& b,
    DataType&& data
) {
    return { std::min(a.low(), b.low()), std::max(a.high(), b.high()), std::move(data) };
}

} // namespace utils
} // namespace genesis

#endif // include guard
