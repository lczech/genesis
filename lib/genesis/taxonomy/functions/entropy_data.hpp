#ifndef GENESIS_TAXONOMY_FUNCTIONS_ENTROPY_DATA_H_
#define GENESIS_TAXONOMY_FUNCTIONS_ENTROPY_DATA_H_

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
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/taxon_data.hpp"
#include "genesis/sequence/counts.hpp"

#include <string>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Forwad Declarations
// =================================================================================================

class Taxon;
class Taxonomy;

// =================================================================================================
//     Entropy Data
// =================================================================================================

class EntropyTaxonData : public BaseTaxonData
{
public:

    // -------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------

    enum class PruneStatus
    {
        kInside,
        kBorder,
        kOutside
    };

    static std::string status_text( PruneStatus stat )
    {
        switch( stat ) {
            case PruneStatus::kInside:  return "Inside";
            case PruneStatus::kBorder:  return "Border";
            case PruneStatus::kOutside: return "Outside";
            default:                    return "Unknown";
        }
    }

    static std::string status_abbreviation( PruneStatus stat )
    {
        switch( stat ) {
            case PruneStatus::kInside:  return "I";
            case PruneStatus::kBorder:  return "B";
            case PruneStatus::kOutside: return "O";
            default:                    return "U";
        }
    }

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    virtual ~EntropyTaxonData() = default;

    // Move ctor and assignment.
    EntropyTaxonData( EntropyTaxonData&& )             = delete;
    EntropyTaxonData& operator= ( EntropyTaxonData&& ) = delete;

protected:

    EntropyTaxonData() = default;

    // Copy ctor and assignment.
    EntropyTaxonData( EntropyTaxonData const& )             = default;
    EntropyTaxonData& operator= ( EntropyTaxonData const& ) = default;

public:

    static std::unique_ptr< EntropyTaxonData > create()
    {
        return std::unique_ptr< EntropyTaxonData >( new EntropyTaxonData() );
    }

    virtual std::unique_ptr< BaseTaxonData > clone() const override
    {
        return std::unique_ptr< EntropyTaxonData >( new EntropyTaxonData( *this ));
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

    sequence::SequenceCounts counts;
    double                   entropy = 0.0;
    PruneStatus              status  = PruneStatus::kInside;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
