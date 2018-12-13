#ifndef GENESIS_UTILS_CONTAINERS_DATAFRAME_OPERATORS_H_
#define GENESIS_UTILS_CONTAINERS_DATAFRAME_OPERATORS_H_

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

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

#include "genesis/utils/containers/dataframe.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Helpful Functions
// =================================================================================================

// /* *
//  * @brief Merge two Dataframe%s by appending the columns of the second one (@p appendix) to the
//  * first one (@p target).
//  *
//  * The @p appendix needs to contain at least all the rows (by name) of the @p target.
//  * This necessiates that all rows in @p target need to be named (no empty names).
//  * Also, the column names need to be distinct.
//  */
// template< typename T >
// void append( Dataframe<T>& target, Dataframe<T> const& appendix )
// {
//     // TODO this could also simply be an overload of df.add_col() ...
//
//     for( size_t c = 0; c < appendix.cols(); ++c ) {
//         auto const& col_name = appendix.col_name(c);
//         auto& new_col = target.add_col( col_name );
//
//         for( size_t r = 0; r < target.rows(); ++r ) {
//             auto const& row_name = target.row_name(r);
//
//             // Throws if not present in appendix
//             auto const ari = appendix.row_index( row_name );
//
//             new_col[r] = appendix[c][ari];
//         }
//     }
// }
//
// template< typename T >
// Dataframe<T> combine( Dataframe<T> const& lhs, Dataframe<T> const& rhs )
// {
//     // First copy lhs and add all columns of rhs to it.
//     auto result = lhs;
//     append( result, rhs );
//
//     // Now add all additional rows of rhs.
//     for( size_t r = 0; r < rhs.rows(); ++r ) {
//         auto const& row_name = rhs.row_name(r);
//         if( ! result.has_row_name( row_name )) {
//
//             // Add the missing row and copy over all data for the columns of rhs.
//             result.add_row( row_name );
//             for( auto const& col : rhs ) {
//                 result[ col.name() ][ row_name ] = col[ row_name ];
//             }
//         }
//     }
//
//     return result;
// }

bool validate( Dataframe const& df )
{
    if( df.col_names_.size() != df.columns_.size() ) {
        return false;
    }

    for( size_t i = 0; i < df.columns_.size(); ++i ) {
        if( df.columns_[i]->size() != df.row_names_.size() ) {
            return false;
        }
        if( df.columns_[i]->index() != i ) {
            return false;
        }
        if( &df.columns_[i]->dataframe() != &df ) {
            return false;
        }
    }

    for( auto const& rl : df.row_lookup_ ) {
        if( rl.second >= df.row_names_.size() ) {
            return false;
        }
        if( rl.first != df.row_names_[ rl.second ] ) {
            return false;
        }
    }

    for( auto const& cl : df.col_lookup_ ) {
        if( cl.second >= df.col_names_.size() ) {
            return false;
        }
        if( cl.first != df.col_names_[ cl.second ] ) {
            return false;
        }
    }

    return true;
}

} // namespace utils
} // namespace genesis

#endif // include guard
