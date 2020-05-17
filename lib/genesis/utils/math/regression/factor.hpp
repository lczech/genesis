#ifndef GENESIS_UTILS_MATH_REGRESSION_FACTOR_H_
#define GENESIS_UTILS_MATH_REGRESSION_FACTOR_H_

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

#include <algorithm>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Factors and Categorical Variables
// =================================================================================================

template<class T>
struct GlmFactor
{
    /**
     * @brief Set of unique values of the factor. The indices in this vector are the indices
     * that are used in `values` to encode each original value.
     */
    std::vector<T> levels;

    /**
     * @brief List of factor indices for the original values.
     *
     * The values in this list are indices of the corresponding level, that is,
     * `levels[values[i]]` yields the level of the `i`th value. Note that excluded levels
     * (see glm_factor()) get assigned a value of `max size_t`, and hence cannot be looked up
     * this way.
     */
    std::vector<size_t> values;
};

/**
 * @brief Reduce a list of values in the given range to a set of unique factors.
 *
 * The function takes a range (@p first to @p last, with last being the past-the-end element)
 * and finds all unique values (called `levels`) in the range. These are stored in the
 * GlmFactor::levels set of the result. The provided values in the given range are then encoded
 * in GlmFactor::values using the indices of the unique values in the levels.
 *
 * If the parameter @p levels is not provided, the found unique levels are sorted.
 * If however @p levels are provided, these are used instead, and their ordering is used for the
 * encoding of the values. Any value that is not found in the provided @p levels is assigned
 * `max size_t` as an indicator of an unused/missing level.
 *
 * The paramter @p exclude works similary: All levels in the @p exlcude list are removed from the
 * result (no matter whether @p levels were provided or the found ones are used), and all
 * corresponding values are encoded as `max size_t`.
 */
template <class ForwardIterator>
GlmFactor<typename ForwardIterator::value_type> glm_factor(
    ForwardIterator first,
    ForwardIterator last,
    std::vector<typename ForwardIterator::value_type> const& levels,
    std::vector<typename ForwardIterator::value_type> const& exclude
) {
    // Prepare result.
    using T = typename ForwardIterator::value_type;
    GlmFactor<T> result;

    // If no levels are provided, add all first.
    if( levels.empty() ) {
        auto it = first;
        while( it != last ) {
            result.levels.push_back( *it );
            ++it;
        }

        // Sort and make levels uniq.
        // We don't do this if levels are given as param, because then we just want to keep
        // they order they are already in.
        std::sort( result.levels.begin(), result.levels.end() );
        auto uit = std::unique( result.levels.begin(), result.levels.end() );
        result.levels.erase( uit, result.levels.end() );
    } else {

        // Test if the provided levels are uniq.
        result.levels = levels;
        std::sort( result.levels.begin(), result.levels.end() );
        auto uit = std::unique( result.levels.begin(), result.levels.end() );
        if( uit != result.levels.end() ) {
            throw std::invalid_argument( "Provided levels are not unique." );
        }

        // Now set again to the actual level order. Ineffcient, but better we do the check above.
        result.levels = levels;
    }

    // Remove the excluded ones again.
    for( auto const& e : exclude ) {
        auto fit = std::find( result.levels.begin(), result.levels.end(), e );
        if( fit != result.levels.end() ) {
            result.levels.erase( fit );
        }
    }

    // Fill the values. This is inefficient because of the repeated lookup of levels,
    // but for now, good enough, because the set of levels is usually small.
    auto it = first;
    while( it != last ) {
        auto fit = std::find( result.levels.begin(), result.levels.end(), *it );
        if( fit != result.levels.end() ) {
            result.values.push_back( std::distance( result.levels.begin(), fit ));
        } else {
            // Value not found.
            result.values.push_back( std::numeric_limits<std::size_t>::max() );
        }
        ++it;
    }

    return result;
}

template <class ForwardIterator>
GlmFactor<typename ForwardIterator::value_type> glm_factor(
    ForwardIterator first,
    ForwardIterator last,
    std::vector<typename ForwardIterator::value_type> const& levels
) {
    return glm_factor(
        first,
        last,
        levels,
        std::vector<typename ForwardIterator::value_type>()
    );
}

template <class ForwardIterator>
GlmFactor<typename ForwardIterator::value_type> glm_factor(
    ForwardIterator first,
    ForwardIterator last
) {
    return glm_factor(
        first,
        last,
        std::vector<typename ForwardIterator::value_type>(),
        std::vector<typename ForwardIterator::value_type>()
    );
}

/**
 * @brief Get the number of occurrences of each level in a GlmFactor.
 *
 * The resulting vector indicates how often each level of the factor occurs in its values,
 * using the same indices as the levels:
 *
 *     // List the number of occurrences for each factor.
 *     auto const smry = glm_factor_summary( factor );
 *     for( size_t i = 0; i < factor.levels.size(); ++i ) {
 *      std::cout << "Level " << factor.levels[i] << ": " << smry[i];
 *     }
 *
 * This is for example useful for user output.
 */
template<class T>
std::vector<size_t> glm_factor_summary( GlmFactor<T> const& factor )
{
    auto result = std::vector<size_t>( factor.levels.size(), 0 );
    for( auto val : factor.values ) {
        if( val < factor.levels.size() ) {
            ++result[val];
        }
    }
    return result;
}

/**
 * @brief Turn a GlmFactor into a set of (dummy) indicator variables to be used in regression.
 *
 * The function takes a @p factor object, and for its `k` levels creates `k - 1` dummy indicator
 * variables that encode the factors for regression. The given @p reference_level is excluded,
 * and for all other levels, a column is returned that contains `1.0` wherever the `values`
 * of the @p factor are equal to that level, and `0.0` otherwise.
 * Missing/exluded levels are encoded as `NaN`.
 */
template<class T>
Dataframe glm_indicator_variables(
    GlmFactor<T> const& factor,
    T const& reference_level,
    std::vector<std::string> const& row_names = std::vector<std::string>{}
) {
    // Error checks.
    if( factor.levels.empty() ) {
        throw std::runtime_error( "Cannot create indicator variable from empty factor." );
    }
    if( ! row_names.empty() && row_names.size() != factor.values.size() ) {
        throw std::runtime_error(
            "Row names for indicator variable do not have the same size as the values of the factor."
        );
    }

    // We need to find the ref level in the factor levels.
    // This is a bit wasteful, and we could instead use its index as parameter,
    // but this way, the api is nicer. It might be possible to offer both versions,
    // but that could get tricky if T is some integer type, because then we get identical signatures.
    // So, for now, we opt for usability instead of efficiency here.
    // Level lists are usually small, so that should not be a big issue.
    auto const rit = std::find( factor.levels.begin(), factor.levels.end(), reference_level );
    if( rit == factor.levels.end() ) {
        throw std::runtime_error(
            "Cannot create indicator variable. "
            "Provided reference level is not part of the factor levels."
        );
    }
    auto const ref_idx = std::distance( factor.levels.begin(), rit );
    assert( ref_idx >= 0 );

    // Prepare result, add all needed rows.
    Dataframe result;
    if( row_names.empty() ) {
        for( size_t i = 0; i < factor.values.size(); ++i ) {
            result.add_unnamed_row();
        }
    } else {
        assert( row_names.size() == factor.values.size() );
        for( size_t i = 0; i < factor.values.size(); ++i ) {
            result.add_row( row_names[i] );
        }
    }
    assert( result.rows() == factor.values.size() );

    // Make indicator variables for each but the reference level.
    for( size_t lvl_idx = 0; lvl_idx < factor.levels.size(); ++lvl_idx ) {
        if( lvl_idx == static_cast<size_t>( ref_idx )) {
            continue;
        }

        // Make column name by concatenating the ref level and current level.
        auto make_name = []( T const& level ){
            std::stringstream ss;
            ss << level;
            auto ret = ss.str();
            if( ret.empty() ) {
                ret = "[empty]";
            }
            return ret;
        };
        auto col_name = make_name( factor.levels[ ref_idx ] ) + "." + make_name( factor.levels[ lvl_idx ] );
        auto& col = result.add_col<double>( col_name );

        // Add the column content.
        for( size_t val_idx = 0; val_idx < factor.values.size(); ++val_idx ) {
            if( factor.values[val_idx] >= factor.levels.size() ) {
                col[val_idx] = std::numeric_limits<double>::quiet_NaN();
            } else if( factor.values[val_idx] == lvl_idx ) {
                col[val_idx] = 1.0;
            } else {
                col[val_idx] = 0.0;
            }
        }
    }
    assert( factor.levels.size() > 0 );
    assert( result.cols() == factor.levels.size() - 1 );

    return result;
}

/**
 * @brief Turn a GlmFactor into a set of (dummy) indicator variables to be used in regression.
 *
 * The function uses the most common level as reference level, and otherwise behaves the same as
 * @link glm_indicator_variables( GlmFactor<T> const&, T const&, std::vector<std::string> const& ) glm_indicator_variables()@endlink.
 */
template<class T>
Dataframe glm_indicator_variables(
    GlmFactor<T> const& factor,
    std::vector<std::string> const& row_names = std::vector<std::string>{}
) {
    if( factor.levels.empty() ) {
        throw std::runtime_error( "Cannot create indicator variable from empty factor." );
    }

    // Find the most common value.
    auto const smry = glm_factor_summary( factor );
    auto const maxit = std::max_element( smry.begin(), smry.end() );
    size_t const max_level = std::distance( smry.begin(), maxit );
    assert( max_level < factor.levels.size() );

    return glm_indicator_variables( factor, factor.levels[max_level], row_names );
}

} // namespace utils
} // namespace genesis

#endif // include guard
