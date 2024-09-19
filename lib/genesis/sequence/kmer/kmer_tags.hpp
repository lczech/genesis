#ifndef GENESIS_SEQUENCE_KMER_KMER_TAGS_H_
#define GENESIS_SEQUENCE_KMER_KMER_TAGS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "genesis/sequence/kmer/kmer.hpp"
#include "genesis/utils/core/std.hpp"

// The functionality provided in this file is only available with C++20 and later.
#if GENESIS_CPP_STD >= GENESIS_CPP_STD_20

#include <tuple>
#include <type_traits>
#include <utility>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Kmer Tag Helper Functions
// =================================================================================================

/**
 * @brief Tag class template to use for Kmer%s for a set of values of `k`.
 *
 * The Kmer class is templated via a parameter `Tag` that can be used to create distinct
 * Kmer classes that differ in their value for `k`, without having to store `k` in every instance.
 * This feature can however be a bit tricky to work with, as then all Kmer classes and all helper
 * classes such as KmerExtractor need to be templated on the Tag.
 *
 * Here, we provide such a tag class for a generic use case where the Tag itself also stores the
 * value of `k`, such that multiple instanciations of this class template can be created automatically.
 */
template<int K>
struct KmerTagNumeric
{
    static constexpr int k = K;
};

/**
 * @brief Helper to create a tuple of tagged classes for custom tag values (custom integers case)
 *
 * See make_numeric_tagged_kmer_classes_from_custom_set() and
 * make_numeric_tagged_kmer_classes_from_range()
 */
template<template<typename> class ClassTemplate, int... Ints>
auto make_numeric_tagged_kmer_classes( std::integer_sequence<int, Ints...> )
{
    // Call Kmer<Tag>::set_k for each Tag so that the correct k is being set.
    // This is idempotent if the k for the Tag is already set,
    // and throws an exception should the value of k mismatch somehow.
    ( Kmer<KmerTagNumeric<Ints>>::set_k(Ints), ... );

    // Create ClassTemplate<Tag<int>> for each int in the sequence
    return std::make_tuple(ClassTemplate<KmerTagNumeric<Ints>>()...);
}

/**
 * @brief Create a std::tuple containing instances of Kmer classes for a given list of values of `k`.
 *
 * The function can be used to create Kmer instances and related classes that use a Tag
 * to determine the value of `k`, for some custom values of `k` (e.g., 10, 20, 30).
 *
 * Usage:
 *
 *     auto kmer_extractors = make_numeric_tagged_kmer_classes_from_custom_set<
 *         KmerExtractor, 10, 20, 30
 *     >();
 *
 * yields a `std::tuple` containing three instances of KmerExtractor, tagged with
 * KmerTagNumeric according to the int values, and using the specified values of `k`.
 */
template<template<typename> class ClassTemplate, int... Ints>
auto make_numeric_tagged_kmer_classes_from_custom_set()
{
    return make_numeric_tagged_kmer_classes<ClassTemplate>( std::integer_sequence<int, Ints...>{} );
}

/**
 * @brief Create a std::tuple containing instances of Kmer classes for a given range of values of `k`.
 *
 * The function can be used to create Kmer instances and related classes that use a Tag
 * to determine the value of `k`, for a range of values of `k` (e.g., 10-15, inclusive).
 *
 * Usage:
 *
 *     auto kmer_extractors = make_numeric_tagged_kmer_classes_from_range<
 *         KmerExtractor, 10, 15
 *     >();
 *
 * yields a `std::tuple` containing six instances of KmerExtractor, tagged with
 * KmerTagNumeric according to the int values, one for each value of `k` between 10 and 15.
 */
template<template<typename> class ClassTemplate, int Start, int End>
auto make_numeric_tagged_kmer_classes_from_range()
{
    constexpr std::size_t N = End - Start + 1;
    return []<std::size_t... Is>( std::index_sequence<Is...> )
    {
        return make_numeric_tagged_kmer_classes<ClassTemplate>(
            std::integer_sequence<int, (Start + Is)...>{}
        );
    }( std::make_index_sequence<N>{} );
}

/**
 * @brief Helper function to iterate over tuple and call the provided function on each class instance.
 *
 * This can be used with the results of make_numeric_tagged_kmer_classes_from_custom_set()
 * and make_numeric_tagged_kmer_classes_from_range() to call a function on all instances in the tuple.
 */
template<std::size_t I = 0, typename Tuple, typename Func>
void call_function_on_numeric_tagged_kmer_tuple( std::size_t index, Tuple& tuple, Func&& func )
{
    if( I == index ) {
        // Call function on I-th element
        func( std::get<I>(tuple) );
    } else if constexpr ( I + 1 < std::tuple_size<Tuple>::value ) {
        // Recurse to the next index
        call_function_on_numeric_tagged_kmer_tuple<I + 1>(
            index, tuple, std::forward<Func>(func)
        );
    }
}

/**
 * @brief Helper function to iterate over a set of tuples of numerical tagged classes,
 * and call the provided function for a zipped set of instances for each tag.
 */
template<std::size_t I = 0, typename Func, typename Tuple1, typename Tuple2, typename... Tuples>
void call_function_on_numeric_tagged_kmer_tuple(
    std::size_t index, Func&& func, Tuple1& tuple1, Tuple2& tuple2, Tuples&... tuples
) {
    if (I == index) {
        // Call function on I-th element of all tuples, zipped
        func(std::get<I>(tuple1), std::get<I>(tuple2), std::get<I>(tuples)...);
    } else if constexpr (I + 1 < std::tuple_size<Tuple1>::value) {
        // Recurse to the next index
        call_function_on_numeric_tagged_kmer_tuple<I + 1>(
            index, std::forward<Func>(func), tuple1, tuple2, tuples...
        );
    }
}

} // namespace sequence
} // namespace genesis

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_20
#endif // include guard
