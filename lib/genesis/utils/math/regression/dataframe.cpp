/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.        See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.        If not, see <http://www.gnu.org/licenses/>.

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

#include "genesis/utils/math/regression/dataframe.hpp"

#include "genesis/utils/containers/dataframe/operators.hpp"
#include "genesis/utils/math/regression/factor.hpp"
#include "genesis/utils/math/statistics.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Dataframe Helper Functions
// =================================================================================================

Dataframe glm_prepare_dataframe( Dataframe const& df, std::string& report )
{
    // Prepare rows of the resulting dataframe.
    Dataframe result;
    for( size_t i = 0; i < df.rows(); ++i ) {
        result.add_row( df.row_name(i) );
    }

    // While iterating the dataframe, we also produce some user info.
    std::stringstream ss;
    ss << "Data contains " << result.rows() << " rows, and the following columns:\n";

    // Do the conversions.
    for( size_t i = 0; i < df.cols(); ++i ) {
        if( df[i].is<double>() ) {

            // Simple case: column is already double.
            auto const& dbl_col = df[i].as<double>();
            result.add_col<double>( df.col_name(i), dbl_col );

            // Get the min and max, excluding nan entries.
            // Then, count the number of valid and total entries,
            // and use this to determine the number of unused entries.
            auto const mm = finite_minimum_maximum( dbl_col.begin(), dbl_col.end() );
            auto const ip = count_finite_elements( dbl_col.begin(), dbl_col.end() );
            assert( ip.first <= ip.second );
            assert( ip.second == df.rows() );
            auto const iv = ip.second - ip.first;

            // User output.
            ss << i << ": \"" << df[i].name() << "\" (numerical, min: " << ( mm.min );
            ss << ", max: " << ( mm.max ) << ", unused entries: " << iv << ")\n";

        } else if( df[i].is<std::string>() ) {
            auto const& df_col = df[i].as<std::string>();

            if( is_convertible_to_bool_double( df_col.begin(), df_col.end() )) {

                // Convert to bool, but as doubles. This ensures that empty cells
                // are converted to nan instead of false/0.
                auto const bool_col = convert_to_bool_double(
                    df_col.begin(), df_col.end(), df_col.size()
                );
                result.add_col<double>( df.col_name(i), bool_col );

                // Count the number of entries for user output.
                size_t true_cnt  = 0;
                size_t false_cnt = 0;
                for( size_t j = 0; j < bool_col.size(); ++j ) {
                    true_cnt  += ( bool_col[j] == 1.0 ? 1 : 0 );
                    false_cnt += ( bool_col[j] == 0.0 ? 1 : 0 );
                }
                assert( bool_col.size() >= true_cnt );
                assert( bool_col.size() >= false_cnt );
                auto const ip = count_finite_elements( bool_col.begin(), bool_col.end() );
                assert( ip.first <= ip.second );
                assert( ip.second == df.rows() );
                auto const iv = ip.second - ip.first;

                // User output.
                ss << i << ": \"" << df[i].name() << "\" (binary, true: " << ( true_cnt );
                ss << ", false: " << ( false_cnt ) << ", unused entries: " << iv << ")\n";

            } else if( is_convertible_to_double( df_col.begin(), df_col.end() )) {

                auto const dbl_col = convert_to_double( df_col.begin(), df_col.end(), df_col.size() );
                result.add_col<double>( df.col_name(i), dbl_col );

                // Get the min and max, excluding nan entries.
                // Then, count the number of valid and total entries,
                // and use this to determine the number of unused entries.
                auto const mm = finite_minimum_maximum( dbl_col.begin(), dbl_col.end() );
                auto const ip = count_finite_elements( dbl_col.begin(), dbl_col.end() );
                assert( ip.first <= ip.second );
                assert( ip.second == df.rows() );
                auto const iv = ip.second - ip.first;

                // User output.
                ss << i << ": \"" << df[i].name() << "\" (numerical, min: " << ( mm.min );
                ss << ", max: " << ( mm.max ) << ", unused entries: " << iv << ")\n";

            } else {

                // No conversion possible. Make it a factor. We exclude empty entries,
                // as they do not contain any valid information, and hence would add random signal.
                auto const fact = glm_factor(
                    df_col.begin(),
                    df_col.end(),
                    std::vector<std::string>{},
                    std::vector<std::string>{""}
                );
                auto const fact_df = glm_indicator_variables( fact, df.row_names() );

                // Add factor cols to result. They are named using the format:
                // <original column name>.<reference level>.<factor level>,
                // where the level names come from the glm_indicator_variables() function.
                for( size_t j = 0; j < fact_df.cols(); ++j ) {
                    assert( fact_df[j].Dataframe::ColumnBase::is<double>() );
                    auto const& fact_col = fact_df[j].Dataframe::ColumnBase::as<double>();
                    result.add_col<double>( df_col.name() + "." + fact_col.name(), fact_col );
                }

                // Count number of empty entries that were excluded.
                size_t empty_cnt = 0;
                for( auto const& c : df_col ) {
                    if( c.empty() ) {
                        ++empty_cnt;
                    }
                }
                assert( empty_cnt <= df_col.size() );

                // User output.
                ss << i << ": \"" << df[i].name() << "\" (categorical, levels: ";
                ss << fact.levels.size() << ", unused entries: " << empty_cnt << ")\n";
            }

        } else {

            // We might add other types in the future. For now, we throw instead.
            throw std::invalid_argument(
                "Can only use Dataframe Columns of types double or std::string for preparing "
                "a GLM Matrix."
            );
        }
    }

    report = ss.str();
    return result;
}

Dataframe glm_prepare_dataframe( Dataframe const& df )
{
    std::string report;
    return glm_prepare_dataframe( df, report );
}

Matrix<double> glm_convert_dataframe(
    Dataframe const& df,
    std::vector<std::string> row_order
) {
    auto result = Matrix<double>( df.rows(), df.cols() );

    // Prepare row name order.
    if( ! row_order.empty() && row_order.size() != df.rows() ) {
        throw std::runtime_error( "Row order has to be empty or of same size as Dataframe rows." );
    }
    auto const row_names = ( row_order.empty() ? df.row_names() : row_order );
    assert( row_names.size() == df.rows() );
    assert( result.rows() == df.rows() );
    assert( result.cols() == df.cols() );

    // Iterate columns of the dataframe.
    for( size_t c = 0; c < df.cols(); ++c ) {
        if( ! df[c].Dataframe::ColumnBase::is<double>() ) {
            throw std::runtime_error( "GLM Dataframe conversion expects Columns of type double." );
        }
        auto const& col = df[c].Dataframe::ColumnBase::as<double>();

        // Add row content in the provided order.
        for( size_t r = 0; r < row_names.size(); ++r ) {
            auto const& row_name = row_names[r];
            if( ! df.has_row_name( row_name )) {
                throw std::runtime_error( "Invalid row name in Dataframe for GLM conversion." );
            }

            result( r, c ) = col[ row_name ];
        }
    }

    return result;
}

Matrix<double> glm_convert_dataframe(
    Dataframe const& df
) {
    return glm_convert_dataframe( df, std::vector<std::string>() );
}

} // namespace utils
} // namespace genesis
