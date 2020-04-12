/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/containers/dataframe/operators.hpp"

#include "genesis/utils/math/statistics.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <unordered_set>

namespace genesis {
namespace utils {

// ================================================================================================
//     Check Conversion Functions
// ================================================================================================

static bool is_numerical_type_( Dataframe const& df, size_t col_index )
{
    // Try all supported types. Probably, some template meta-programming could help here...
    return df[col_index].is<float>() || df[col_index].is<double>()
        || df[col_index].is<int8_t>() || df[col_index].is<int16_t>()
        || df[col_index].is<int32_t>() || df[col_index].is<int64_t>()
        || df[col_index].is<uint8_t>() || df[col_index].is<uint16_t>()
        || df[col_index].is<uint32_t>() || df[col_index].is<uint64_t>();
}

bool is_convertible_to_bool( Dataframe const& df, size_t col_index )
{
    // Basic checks.
    if( col_index >= df.cols() ) {
        throw std::invalid_argument( "Dataframe column index is out of range." );
    }

    // Certain strings ("yes", "no" etc) can be converted to bool.
    if( df[col_index].is<std::string>() ) {
        auto const& df_cast = df[col_index].as<std::string>();
        return is_convertible_to_bool( df_cast.begin(), df_cast.end() );
    }

    // If its not a string, we can only convert numerical types.
    return is_numerical_type_( df, col_index );
}

bool is_convertible_to_bool( Dataframe const& df, std::string const& col_name )
{
    // Throws if column name is not found.
    return is_convertible_to_bool( df, df.col_index( col_name ));
}

bool is_convertible_to_double( Dataframe const& df, size_t col_index )
{
    if( col_index >= df.cols() ) {
        throw std::invalid_argument( "Dataframe column index is out of range." );
    }

    // Certain strings can be converted to double.
    if( df[col_index].is<std::string>() ) {
        auto const& df_cast = df[col_index].as<std::string>();
        return is_convertible_to_double( df_cast.begin(), df_cast.end() );
    }

    // If its not a string, we can only convert numerical types.
    return is_numerical_type_( df, col_index );
}

bool is_convertible_to_double( Dataframe const& df, std::string const& col_name )
{
    // Throws if column name is not found.
    return is_convertible_to_double( df, df.col_index( col_name ));
}

// ================================================================================================
//     Conversion Functions
// ================================================================================================

template<typename S, typename T>
void convert_to_type_( Dataframe& df, size_t col_index )
{
    auto const& df_cast = df[col_index].as<S>();
    std::vector<T> conv_col;
    conv_col.reserve( df_cast.size() );
    for( size_t i = 0; i < df_cast.size(); ++i ) {
        conv_col[i] = static_cast<T>( df_cast[i] );
    }
    df.replace_col<T>( col_index, conv_col );
}

template<typename T>
void convert_to_type_( Dataframe& df, size_t col_index )
{
    // Try all supported types. Probably, some template meta-programming could help here...
    // if( df[col_index].is<bool>() ) {
    //     convert_to_type_<bool, T>( df, col_index );
    // } else
    if( df[col_index].is<float>() ) {
        convert_to_type_<float, T>( df, col_index );
    } else if( df[col_index].is<double>() ) {
        convert_to_type_<double, T>( df, col_index );
    } else if( df[col_index].is<int8_t>() ) {
        convert_to_type_<int8_t, T>( df, col_index );
    } else if( df[col_index].is<int16_t>() ) {
        convert_to_type_<int16_t, T>( df, col_index );
    } else if( df[col_index].is<int32_t>() ) {
        convert_to_type_<int32_t, T>( df, col_index );
    } else if( df[col_index].is<int64_t>() ) {
        convert_to_type_<int64_t, T>( df, col_index );
    } else if( df[col_index].is<uint8_t>() ) {
        convert_to_type_<uint8_t, T>( df, col_index );
    } else if( df[col_index].is<uint16_t>() ) {
        convert_to_type_<uint16_t, T>( df, col_index );
    } else if( df[col_index].is<uint32_t>() ) {
        convert_to_type_<uint32_t, T>( df, col_index );
    } else if( df[col_index].is<uint64_t>() ) {
        convert_to_type_<uint64_t, T>( df, col_index );
    } else {
        throw std::invalid_argument(
            "Dataframe column is not of a type that be converted to the target type."
        );
    }
}

void convert_to_bool( Dataframe& df, size_t col_index )
{
    if( col_index >= df.cols() ) {
        throw std::invalid_argument( "Dataframe column index is out of range." );
    }
    if( df[col_index].is<std::string>() ) {

        // Convert the strings, resolving things like "yes" or "off"
        auto const& df_cast = df[col_index].as<std::string>();
        auto const bool_col = convert_to_bool( df_cast.begin(), df_cast.end(), df_cast.size() );

        // Convert to signed char, because std::vector<bool> is not a container...
        auto char_col = std::vector<signed char>( bool_col.size() );
        for( size_t i = 0; i < bool_col.size(); ++i ) {
            char_col[i] = bool_col[i];
        }

        df.replace_col<signed char>( col_index, char_col );
    } else {
        // Currently, we do not need number to bool conversion,
        // and with the given helper functions, it does not work the way we want it to.
        // So, out of lazyness, we forbid those conversions. For now.

        // convert_to_type_<char>( df, col_index );
        throw std::invalid_argument(
            "Dataframe column conversion to bool is only implemented for strings."
        );
    }
}

void convert_to_bool( Dataframe& df, std::string const& col_name )
{
    // Throws if column name is not found.
    return convert_to_bool( df, df.col_index( col_name ));
}

void convert_to_double( Dataframe& df, size_t col_index )
{
    if( col_index >= df.cols() ) {
        throw std::invalid_argument( "Dataframe column index is out of range." );
    }
    if( df[col_index].is<std::string>() ) {
        auto const& df_cast = df[col_index].as<std::string>();
        auto const double_col = convert_to_double( df_cast.begin(), df_cast.end(), df_cast.size() );
        df.replace_col<double>( col_index, double_col );
    } else {
        convert_to_type_<double>( df, col_index );
    }
}

void convert_to_double( Dataframe& df, std::string const& col_name )
{
    // Throws if column name is not found.
    return convert_to_double( df, df.col_index( col_name ));
}

// =================================================================================================
//     Summarize Columns
// =================================================================================================

std::string summarize_column_common_( Dataframe const& df, size_t col_index, std::string const& description )
{
    return std::to_string( col_index ) + ": \"" + df[col_index].name() + "\" " + description + "\n";
}

template<typename T>
std::string summarize_column_double_( Dataframe const& df, size_t col_index )
{
    auto const& col_cast = df[col_index].as<T>();

    // Get the min and max, excluding nan entries.
    // Then, count the number of valid and total entries,
    // and use this to determine the number of unused entries.
    auto const mm = finite_minimum_maximum( col_cast.begin(), col_cast.end() );
    auto const ip = count_finite_elements( col_cast.begin(), col_cast.end() );
    assert( ip.first <= ip.second );
    assert( ip.second == df.rows() );
    auto const iv = ip.second - ip.first;

    return summarize_column_common_(
        df, col_index,
        "(numerical, min: " + std::to_string( mm.min ) + ", max: " + std::to_string( mm.max ) +
        ", unused entries: " + std::to_string(iv) + ")"
    );
}

template<typename T>
std::string summarize_column_int_( Dataframe const& df, size_t col_index )
{
    auto const& col_cast = df[col_index].as<T>();
    auto const mm = std::minmax_element( col_cast.begin(), col_cast.end() );
    return summarize_column_common_(
        df, col_index,
        "(numerical, min: " + std::to_string( *mm.first ) + ", max: " +
        std::to_string( *mm.second ) + ")"
    );
}

std::string summarize_column_string_( Dataframe const& df, size_t col_index )
{
    // Make copies to get number of unique entries.
    auto const& str_cast = df[col_index].as<std::string>();
    std::unordered_set<std::string> uniq( str_cast.begin(), str_cast.end() );

    return summarize_column_common_(
        df, col_index,
        "(string, unique elements: " + std::to_string( uniq.size() ) + ")"
    );
}

std::string summarize_column( Dataframe const& df, size_t col_index )
{
    if( df[col_index].is<float>() ) {
        return summarize_column_double_<float>( df, col_index );
    } else if( df[col_index].is<double>() ) {
        return summarize_column_double_<double>( df, col_index );
    } else if( df[col_index].is<int8_t>() ) {
        return summarize_column_int_<int8_t>( df, col_index );
    } else if( df[col_index].is<int16_t>() ) {
        return summarize_column_int_<int16_t>( df, col_index );
    } else if( df[col_index].is<int32_t>() ) {
        return summarize_column_int_<int32_t>( df, col_index );
    } else if( df[col_index].is<int64_t>() ) {
        return summarize_column_int_<int64_t>( df, col_index );
    } else if( df[col_index].is<uint8_t>() ) {
        return summarize_column_int_<uint8_t>( df, col_index );
    } else if( df[col_index].is<uint16_t>() ) {
        return summarize_column_int_<uint16_t>( df, col_index );
    } else if( df[col_index].is<uint32_t>() ) {
        return summarize_column_int_<uint32_t>( df, col_index );
    } else if( df[col_index].is<uint64_t>() ) {
        return summarize_column_int_<uint64_t>( df, col_index );
    } else if( df[col_index].is<std::string>() ) {
        return summarize_column_string_( df, col_index );
    }

    return summarize_column_common_(
        df, col_index,
        "(unknown data type)"
    );
}

std::string summarize_column( Dataframe const& df, std::string const& col_name )
{
    // Throws if column name is not found.
    return summarize_column( df, df.col_index( col_name ));
}

std::string summarize_columns( Dataframe const& df )
{
    std::string result = "Data contains " + std::to_string( df.rows() ) + " rows, and the following columns:\n";
    for( size_t i = 0; i < df.cols(); ++i ) {
        result += summarize_column( df, i );
    }
    return result;
}

// =================================================================================================
//     Merging and Combining
// =================================================================================================

// void append(
//     Dataframe& target,
//     Dataframe const& appendix,
//     size_t col_index,
//     DataframeMergeMode mode
// ) {
//     auto const& col_name = appendix.col_name(col_index);
//     if( col_name.empty() ) {
//         throw std::runtime_error( "Cannot append dataframe with empty column names." );
//     }
//     if( target.has_col_name( col_name ) ) {
//         throw std::runtime_error( "Column '" + col_name + "' already exists in target dataframe." );
//     }
//     auto& new_col = target.add_col( col_name );
//
//     for( size_t r = 0; r < target.rows(); ++r ) {
//         auto const& row_name = target.row_name(r);
//
//         // Throws if not present in appendix
//         auto const ari = appendix.row_index( row_name );
//
//         new_col[r] = appendix[col_index][ari];
//     }
// }
//
// void append(
//     Dataframe& target,
//     Dataframe const& appendix,
//     std::string const& col_name,
//     DataframeMergeMode mode
// ) {
//
// }
//
// void append(
//     Dataframe& target,
//     Dataframe const& appendix,
//     DataframeMergeMode mode
// ) {
//     for( size_t c = 0; c < appendix.cols(); ++c ) {
//         append( target, appendix, c, mode );
//     }
// }
//
// /**
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
//
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

// ================================================================================================
//     Helpful Functions
// ================================================================================================

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
