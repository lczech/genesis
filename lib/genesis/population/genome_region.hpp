#ifndef GENESIS_POPULATION_GENOME_REGION_H_
#define GENESIS_POPULATION_GENOME_REGION_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2022 Lucas Czech

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
 * @ingroup population
 */

#include <iosfwd>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Genome Region
// =================================================================================================

/**
 * @brief A region (between two positions) on a chromosome.
 *
 * This can be used to represent a gene, a feature, or just generally a region of interest.
 * We use a simple form with a chromosome name, and a start and end position, both inclusive.
 */
struct GenomeRegion
{
public:

    std::string chromosome;
    size_t start = 0;
    size_t end   = 0;

    GenomeRegion( std::string const& chr = "", size_t s = 0, size_t e = 0 )
        : chromosome( chr )
        , start(s)
        , end(e)
    {}
};

// Alternative version that also has a data field.
//
// // =================================================================================================
// //     Genome Region
// // =================================================================================================
//
// /**
//  * @brief A region (between a start and and end position) on a chromosome.
//  *
//  * This can be used to represent a gene, a feature, or just generally a region of interest.
//  * We use a simple form with a chromosome name, and a start and end position, both inclusive,
//  * that is, a closed interval. Both start and end can also be identical, in which case they
//  * denote a single position; see also GenomeLocus for a class representing this.
//  *
//  * We use 1-based positions, in order to comply with common chromosome annotation formats.
//  * Furthermore, we use an empty chromosome string and/or positions 0 as indicators of an empty or
//  * default constructed locus.
//  *
//  * The class takes an extra data type as template parameter, which can be used to store
//  * arbitrary data for this locus.
//  *
//  * @see GenomeLocus
//  * @see GenomeRegionList
//  */
// template<class DataType = EmptyGenomeData>
// class GenomeRegion
// {
// public:
//
//     // -------------------------------------------------------------------------
//     //     Typedefs
//     // -------------------------------------------------------------------------
//
//     using data_type = DataType;
//
//     // -------------------------------------------------------------------------
//     //     Constructors and Rule of Five
//     // -------------------------------------------------------------------------
//
//     /**
//      * @brief Default construct an empty region.
//      */
//     GenomeRegion() = default;
//
//     /**
//      * @brief Construct a region with a chromosome and a start and end position.
//      */
//     GenomeRegion( std::string const& chr, size_t start, size_t end )
//         : chromosome_( chr )
//         , start_(start)
//         , end_(end)
//         : GenomeRegion( chr, start, end, DataType{} )
//     {
//         if( chr.empty() || start == 0 || end == 0 ) {
//             throw std::invalid_argument(
//                 "Cannot construct GenomeRegion with empty chromosome or position zero."
//             );
//         }
//         if( start > end ) {
//             throw std::invalid_argument(
//                 "Cannot construct GenomeRegion with start > end."
//             );
//         }
//     }
//
//     /**
//      * @brief Construct a region with a chromosome and a start and end position,
//      * and copy the data.
//      */
//     GenomeRegion( std::string const& chr, size_t start, size_t end, DataType const& data )
//         : GenomeRegion( chr, start, end, DataType{ data })
//     {}
//
//     /**
//      * @brief Construct a region with a chromosome and a start and end position,
//      * and move the data.
//      */
//     GenomeRegion( std::string const& chr, size_t start, size_t end, DataType&& data )
//         : chromosome_( chr )
//         , start_(start)
//         , end_(end)
//         , data_( std::move( data ))
//     {
//         if( chr.empty() || start == 0 || end == 0 ) {
//             throw std::invalid_argument(
//                 "Cannot construct GenomeRegion with empty chromosome or position zero."
//             );
//         }
//     }
//
//     /**
//      * @brief Construct a region from a GenomeLocus of the same data type.
//      *
//      * This uses the GenomeLocus::position() for both start and end, and copies the data.
//      */
//     template<
//         class OtherDataType,
//         typename std::enable_if< std::is_same<DataType, OtherDataType>::value >::type = 0
//     >
//     GenomeRegion( GenomeLocus<OtherDataType> const& locus )
//         : GenomeRegion(
//             locus.chromosome(), locus.position(), locus.position(), locus.data()
//         )
//     {}
//
//     /**
//      * @brief Construct a region from a GenomeLocus of the same data type.
//      *
//      * This uses the GenomeLocus::position() for both start and end, and moves the data.
//      */
//     template<
//         class OtherDataType,
//         typename std::enable_if< std::is_same<DataType, OtherDataType>::value >::type = 0
//     >
//     GenomeRegion( GenomeLocus<OtherDataType>&& locus )
//         : GenomeRegion(
//             locus.chromosome(), locus.position(), locus.position(), std::move( locus.data() )
//         )
//     {}
//
//     /**
//      * @brief Construct a region from a GenomeLocus of a different data type.
//      *
//      * This uses the GenomeLocus::position() for both start and end,
//      * and default constructs the data.
//      */
//     template<
//         class OtherDataType,
//         typename std::enable_if< ! std::is_same<DataType, OtherDataType>::value >::type = 0
//     >
//     GenomeRegion( GenomeLocus<OtherDataType> const& locus )
//         : GenomeRegion( locus.chromosome(), locus.position(), locus.position() )
//     {}
//
//     ~GenomeRegion() = default;
//
//     GenomeRegion( GenomeRegion const& ) = default;
//     GenomeRegion( GenomeRegion&& )      = default;
//
//     GenomeRegion& operator= ( GenomeRegion const& ) = default;
//     GenomeRegion& operator= ( GenomeRegion&& )      = default;
//
//     // -------------------------------------------------------------------------
//     //     Accessors
//     // -------------------------------------------------------------------------
//
//     std::string const& chromosome() const
//     {
//         return chromosome_;
//     }
//
//     size_t start() const
//     {
//         return start_;
//     }
//
//     size_t end() const
//     {
//         return end_;
//     }
//
//     data_type& data()
//     {
//         return data_;
//     }
//
//     data_type const& data() const
//     {
//         return data_;
//     }
//
//     size_t length() const
//     {
//         // Closed interval, so we need to add 1.
//         return end_ - start_ + 1;
//     }
//
//     bool empty() const
//     {
//         return chromosome_ == "" && start_ == 0 && end_ == 0;
//     }
//
//     // -------------------------------------------------------------------------
//     //     Operators
//     // -------------------------------------------------------------------------
//
//     operator std::string() const
//     {
//         return to_string();
//     }
//
//     std::string to_string() const
//     {
//         if( start_ == 0 && end_ == 0 ) {
//             return chromosome_;
//         } else if( start_ == end_ ) {
//             return chromosome_ + ":" + std::to_string( start_ );
//         } else {
//             return
//                 chromosome_ + ":" +
//                 std::to_string( start_ ) + "-" +
//                 std::to_string( end_ )
//             ;
//         }
//     }
//
//     // -------------------------------------------------------------------------
//     //     Member Variables
//     // -------------------------------------------------------------------------
//
//     std::string chromosome_;
//     size_t      start_ = 0;
//     size_t      end_   = 0;
//
//     data_type   data_;
//
// };

} // namespace population
} // namespace genesis

#endif // include guard
