#ifndef GENESIS_UTILS_CONTAINERS_DATAFRAME_OPERATORS_H_
#define GENESIS_UTILS_CONTAINERS_DATAFRAME_OPERATORS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/containers/dataframe.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Conversion Functions
// =================================================================================================

bool is_convertible_to_bool( Dataframe const& df, size_t col_index );
bool is_convertible_to_bool( Dataframe const& df, std::string const& col_name );

bool is_convertible_to_double( Dataframe const& df, size_t col_index );
bool is_convertible_to_double( Dataframe const& df, std::string const& col_name );

void convert_to_bool( Dataframe& df, size_t col_index );
void convert_to_bool( Dataframe& df, std::string const& col_name );

void convert_to_double( Dataframe& df, size_t col_index );
void convert_to_double( Dataframe& df, std::string const& col_name );

std::string summarize_column( Dataframe const& df, size_t col_index );
std::string summarize_column( Dataframe const& df, std::string const& col_name );

std::string summarize_columns( Dataframe const& df );

// =================================================================================================
//     Merging and Combining
// =================================================================================================

// enum class DataframeMergeMode
// {
//     kExpectEqualRowNames,
//     kUseRowNameIntersection,
//     kUseRowNameUnion
// };
//
// void append(
//     Dataframe& target,
//     Dataframe const& appendix,
//     size_t col_index,
//     DataframeMergeMode mode = DataframeMergeMode::kExpectEqualRowNames
// );
//
// void append(
//     Dataframe& target,
//     Dataframe const& appendix,
//     std::string const& col_name,
//     DataframeMergeMode mode = DataframeMergeMode::kExpectEqualRowNames
// );
//
// void append(
//     Dataframe& target,
//     Dataframe const& appendix,
//     DataframeMergeMode mode = DataframeMergeMode::kExpectEqualRowNames
// );

// =================================================================================================
//     Helpful Functions
// =================================================================================================

bool validate( Dataframe const& df );

} // namespace utils
} // namespace genesis

#endif // include guard
