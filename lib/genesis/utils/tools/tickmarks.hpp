#ifndef GENESIS_UTILS_TOOLS_TICKMARKS_H_
#define GENESIS_UTILS_TOOLS_TICKMARKS_H_

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

#include <cstddef>
#include <limits>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Tickmarks
// =================================================================================================

/**
 * @brief Helper class to find "nice" tickmark intervals for creating scales and axes.
 */
struct Tickmarks
{
public:

    // -------------------------------------------------------------------------
    //     Helper Structs
    // -------------------------------------------------------------------------

    struct LabeledTick
    {
        LabeledTick( double rel_pos, double lab )
            : relative_position( rel_pos )
            , label( lab )
        {}

        double relative_position;
        double label;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    Tickmarks() = default;
    ~Tickmarks() = default;

    Tickmarks(Tickmarks const&)     = default;
    Tickmarks(Tickmarks&&) noexcept = default;

    Tickmarks& operator= (Tickmarks const&)     = default;
    Tickmarks& operator= (Tickmarks&&) noexcept = default;

    // -------------------------------------------------------------------------
    //     Drudges
    // -------------------------------------------------------------------------

    /**
     * @brief Calculate a step size that fills the `interval_size` in approximately `target_steps`
     * many steps.
     *
     * The resulting step size is calculated to be a "nice" size, i.e. fits `[ 1 | 2 | 5 ] * 10^n`.
     * Thus, exemplary nice step sizes are 0.01, 20 and 500.
     */
    static double step_size( double interval_size, size_t target_steps );

    /**
     * @brief Calculate a set of ticks that linearily span from `min` to `max` in approximately
     * `target_steps` many steps, where each tick sits on a "nice" number.
     *
     * See step_size() for what is meant by nice numbers.
     */
    std::vector<double> linear_ticks( double min, double max, size_t target_steps );

    /**
     * @brief Return a set of labels with relative positions between `min` and `max`, where the
     * labels correspond to the linear_ticks() numbers.
     *
     * This function is a convenience wrapper for linear_ticks(), that turns the tick positions
     * into relative values, while keeping the actual tick value as a label. Relative position 0.0
     * corresponds to `min`, relative position 1.0 to `max`.
     */
    std::vector<LabeledTick> linear_labels( double min, double max, size_t target_steps );

    /**
     * @brief Return a set of labels with relative positions between `min` and `max`, where the
     * labels correspond to logarithmic ticks.
     *
     * This function is the equivalent of linear_labels(), but instead uses logarithmic scaling.
     * Also, it always starts at a minimum of 1 and inserts as many ticks as are needed to go up
     * to `max` in powers of ten.
     *
     * Example: Given `max == 350`, with `overshoot_at_max == false` and `include_max == true`,
     * the resulting vector contains entries with labels `1, 10, 100, 350`.
     *
     * Remark: There is no logarithmic equivalent for linear_ticks(), as those would simply be
     * powers of 10 up to `max`.
     */
    std::vector<LabeledTick> logarithmic_labels( double max );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Should the provided min value be included in the resulting list of ticks or not.
     */
    bool include_min = true;

    /**
     * @brief Should the provided max value be included in the resulting list of ticks or not.
     */
    bool include_max = true;

    /**
     * @brief Should the lowest value in the resulting list of ticks be below the provided min
     * value (`true`) or not (`false`).
     */
    bool undershoot_at_min = false;

    /**
     * @brief Should the highest value in the resulting list of ticks be above the provided max
     * value (`true`) or not (`false`).
     */
    bool overshoot_at_max = false;

    /**
     * @brief Relative epsilon used to exclude two tickmarks that are too close to each other.
     */
    double relative_epsilon = std::numeric_limits<double>::epsilon();

};

} // namespace utils
} // namespace genesis

#endif // include guard
