#ifndef GENESIS_POPULATION_FILTER_FILTER_STATUS_H_
#define GENESIS_POPULATION_FILTER_FILTER_STATUS_H_

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
 * @ingroup population
 */

#include <cstdint>
#include <stdexcept>
#include <string>

namespace genesis {
namespace population {

// =================================================================================================
//     Filter Status
// =================================================================================================

/**
 * @brief Tag class to assign a filter status to a Variant or SampleCounts.
 *
 * This class provides a 32bit uint as a status tag or indicator to keep track of whether a sample
 * or a position passed all filters, or, if not, which filter failed. It is initialized to 0,
 * meant to indicate that the data is good and to be used. Any value other than 0 shall be
 * interpreted to mean that this data does not meet certain criteria (it did not pass a filter),
 * and is hence to be ignored in any downstream computations and analyses. This is used in some of
 * the filters and processing.
 *
 * The status can be set() once, but has to be reset() afterwards if a different value is needed.
 * This is meant as a protection to accidentally overwrite the filter status when a filter has
 * already been considered to not pass. Hence, when testing and applying a filter, the status needs
 * to be checked before, so that new filters are only applied to data that are yet still passing.
 *
 * We currently offer the value to be general as far as the data is concerned, meaning that the
 * interpretation of any value other than 0 is left up to the user. However, for the filter
 * functions that offered here, we use the VariantFilterTag and SampleFilterTag enums, and
 * recommend to stick to this as well, unless extra filter types are needed.
 */
class FilterStatus
{
public:

    using IntType = uint32_t;

    // -------------------------------------------
    //     Getter
    // -------------------------------------------

    bool passing() const
    {
        return status_ == 0;
    }

    bool is( IntType value ) const
    {
        return status_ == value;
    }

    template<typename FilterTag>
    bool is( FilterTag value ) const
    {
        return is( static_cast<IntType>( value ));
    }

    IntType get() const
    {
        return status_;
    }

    // -------------------------------------------
    //     Setter
    // -------------------------------------------

    void set( IntType value )
    {
        if( status_ != 0 ) {
            throw std::domain_error(
                "Cannot set FilterStatus to " + std::to_string(value) + ", as the status has "
                "already been set to " + std::to_string(status_) + " before. Use reset() instead."
            );
        }
        status_ = value;
    }

    template<typename FilterTag>
    void set( FilterTag value )
    {
        set( static_cast<IntType>( value ));
    }

    void reset()
    {
        status_ = 0;
    }

    void reset( IntType value )
    {
        status_ = value;
    }

    template<typename FilterTag>
    void reset( FilterTag value )
    {
        reset( static_cast<IntType>( value ));
    }

    // -------------------------------------------
    //     Private Members
    // -------------------------------------------

private:

    IntType status_ = 0;
};

} // namespace population
} // namespace genesis

#endif // include guard
