#ifndef GENESIS_POPULATION_GENOME_LOCUS_H_
#define GENESIS_POPULATION_GENOME_LOCUS_H_

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

#include <stdexcept>
#include <string>

namespace genesis {
namespace population {

// =================================================================================================
//     Genome Locus
// =================================================================================================

/**
 * @brief A single locus, that is, a position (or coordinate) on a chromosome.
 *
 * For our purposes here, we define a locus to be one position (1-based) on one chromosome exactly.
 *
 * When working with multiple loci, we here often expect their chromosome names to be sorted in
 * lexicographical order. Hence, for example, when comparing two loci on different chromosomes,
 * we also take the ordering of their chromosome names into account.
 *
 * There are some special cases:
 *
 *   * We use an empty chromosome with position 0 to denote an empty(), unspecified locus.
 *   * A non-empty chromosome with position 0 means "any position" on the chromosome;
 *     this is however not considered to be a specified() locus.
 *   * An empty chromosome with a position other than 0 is an invalid state, and will throw.
 *
 * @see GenomeLocusSet
 * @see GenomeRegion
 * @see GenomeRegionList
 */
struct GenomeLocus
{
    std::string chromosome;
    size_t      position = 0;

    GenomeLocus( std::string const& chr = "", size_t pos = 0 )
        : chromosome( chr )
        , position( pos )
    {
        throw_if_invalid_();
    }

    ~GenomeLocus() = default;

    GenomeLocus( GenomeLocus const& ) = default;
    GenomeLocus( GenomeLocus&& )      = default;

    GenomeLocus& operator= ( GenomeLocus const& ) = default;
    GenomeLocus& operator= ( GenomeLocus&& )      = default;

    bool empty() const
    {
        throw_if_invalid_();
        return chromosome == "" && position == 0;
    }

    bool specified() const
    {
        throw_if_invalid_();
        return chromosome != "" && position != 0;
    }

    void clear()
    {
        chromosome = "";
        position = 0;
    }

private:

    void throw_if_invalid_() const
    {
        if( chromosome == "" and position > 0 ) {
            throw std::runtime_error(
                "GenomeLocus with invalid state: Empty chromosome but non-zero position."
            );
        }
    }
};

// Alternative version that also has a data field.
//
// // =================================================================================================
// //     Genome Locus
// // =================================================================================================
//
// /**
//  * @brief A single locus, that is, a position on a chromosome.
//  *
//  * For our purposes, we define a locus to be one position on one chromosome exactly.
//  * We use 1-based positions, in order to comply with common chromosome annotation formats.
//  * Furthermore, we use an empty chromosome string and/or position 0 as indicators of an empty or
//  * default constructed locus.
//  *
//  * When working with multiple loci, we often expect their chromosome names to be sorted in
//  * lexicographical order. Hence, for example, when comparing two loci on different chromosomes,
//  * we also take the ordering of their chromosome names into account, see the `locus_...()`
//  * compare functions for example.
//  *
//  * The class takes an extra data type as template parameter, which can be used to store
//  * arbitrary data for this locus.
//
//  * @see GenomeRegion
//  * @see GenomeRegionList
//  */
// template<class DataType = EmptyGenomeData>
// class GenomeLocus
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
//      * @brief Default construct an empty locus.
//      */
//     GenomeLocus() = default;
//
//     /**
//      * @brief Construct a locus with a chromosome and position.
//      */
//     GenomeLocus( std::string const& chr, size_t pos )
//         : GenomeLocus( chr, pos, DataType{} )
//     {}
//
//     /**
//      * @brief Construct a locus with a chromosome and position, and copy the data.
//      */
//     GenomeLocus( std::string const& chr, size_t pos, DataType const& data )
//         : GenomeLocus( chr, pos, DataType{ data })
//     {}
//
//     /**
//      * @brief Construct a locus with a chromosome and position, and move the data.
//      */
//     GenomeLocus( std::string const& chr, size_t pos, DataType&& data )
//         : chromosome_( chr )
//         , position_( pos )
//         , data_( std::move( data ))
//     {
//         if( chr.empty() || pos == 0 ) {
//             throw std::invalid_argument(
//                 "Cannot construct GenomeLocus with empty chromosome or position zero."
//             );
//         }
//     }
//
//     ~GenomeLocus() = default;
//
//     GenomeLocus( GenomeLocus const& ) = default;
//     GenomeLocus( GenomeLocus&& )      = default;
//
//     GenomeLocus& operator= ( GenomeLocus const& ) = default;
//     GenomeLocus& operator= ( GenomeLocus&& )      = default;
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
//     size_t position() const
//     {
//         return position_;
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
//     bool empty() const
//     {
//         return chromosome_ == "" && position_ == 0;
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
//         return chromosome_ + ":" + std::to_string( position_ );
//     }
//
//     // -------------------------------------------------------------------------
//     //     Member Variables
//     // -------------------------------------------------------------------------
//
// private:
//
//     std::string chromosome_;
//     size_t      position_ = 0;
//
//     data_type   data_;
//
// };

} // namespace population
} // namespace genesis

#endif // include guard
