#ifndef GENESIS_UTILS_CONTAINERS_HASH_TUPLE_H_
#define GENESIS_UTILS_CONTAINERS_HASH_TUPLE_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

#include "genesis/utils/core/std.hpp"

#include <tuple>

namespace genesis {
namespace utils {

// =================================================================================================
//     Helper For Hashing Tuples
// =================================================================================================

// These helper functions and structs allow for each std::tuple that consists of hashable types
// to be hashed automatically as well.
//
// Code adapted from https://stackoverflow.com/a/7115547/4184258 and
// https://blog.adamfurmanek.pl/2019/09/28/caching-result-of-any-function-in-c/
// We replaced their hash_combine by our 64bit implementation.

namespace
{
    // Recursive template code derived from Matthieu M.
    template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
    struct HashValueImpl
    {
        static void apply( size_t& seed, Tuple const& tuple )
        {
            HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
            seed = hash_combine( seed, std::get<Index>( tuple ));
        }
    };

    template <class Tuple>
    struct HashValueImpl<Tuple,0>
    {
        static void apply( size_t& seed, Tuple const& tuple )
        {
            seed = hash_combine( seed, std::get<0>( tuple ));
        }
    };
}

template <typename TT>
struct hash
{
    size_t operator()( TT const& tt ) const
    {
        return std::hash<TT>()(tt);
    }
};

template <typename ... TT>
struct hash<std::tuple<TT...>>
{
    size_t operator()(std::tuple<TT...> const& tt) const
    {
        size_t seed = 0;
        HashValueImpl<std::tuple<TT...> >::apply( seed, tt );
        return seed;
    }
};


} // namespace utils
} // namespace genesis

#endif // include guard
