#ifndef GENESIS_PLACEMENT_PQUERY_NAME_H_
#define GENESIS_PLACEMENT_PQUERY_NAME_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @ingroup placement
 */

#include <string>

namespace genesis {
namespace placement {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Pquery;

// =================================================================================================
//     Pquery Name
// =================================================================================================

/**
 * @brief A name of a Pquery and its multiplicity.
 *
 * This class is modeled after the `jplace` standard, which allows for multiple names for a Pquery.
 * This is useful if there are identical sequences in the original data for which the phylogenetic
 * placement was carried out. The placements of those sequences can then be treated as one entity,
 * i.e., one Pquery, while still maintaining all their identifiers (names).
 *
 * Furthermore, each such #name can have a #multiplicity, which can be used to store e.g., the
 * number of replicates of the original sequence. It is used as a factor for the weights of
 * PqueryPlacement%s in some calculations.
 */
class PqueryName
{
public:

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

    /**
     * @brief Default constructor. Initializes the #name to an empty string and the #multiplicity
     * to 1.0.
     */
    PqueryName () = default;

    /**
     * @brief Constructor that takes a #name and optionally a #multiplicity.
     */
    PqueryName(std::string name, double multiplicity = 1.0)
        : name(name)
        , multiplicity(multiplicity)
    {}

    ~PqueryName() = default;

    PqueryName( PqueryName const& ) = default;
    PqueryName( PqueryName&& )      = default;

    PqueryName& operator= ( PqueryName const& ) = default;
    PqueryName& operator= ( PqueryName&& )      = default;

    // -------------------------------------------------------------------
    //     Public Property Data Members
    // -------------------------------------------------------------------

    // Yes, the following members are public data members. It's neither nice nor consistent,
    // but makes life so much easier for the moment. Maybe we'll change that in the future...

    /**
     * @brief Name for a Pquery.
     *
     * This property is defined by the `jplace` standard. It does not need to be unique. However,
     * using unique names certainly makes iditifying @link Pquery Pqueries @endlink easier.
     */
    std::string name;

    /**
     * @brief Multiplicity of the #name.
     *
     * This property is defined by the `jplace` standard. It is used as a count for e.g., the
     * abundance of this Pquery (respectively this name). For some calculations, this value is used
     * as a factor for the placment weights (see PqueryPlacement::like_weight_ratio). Thus, by
     * default, the value is initialized to 1.0.
     *
     * If a Pquery has multiple names, all their multiplicities are added when being used as a
     * weight factor.
     */
    double multiplicity = 1.0;

};

} // namespace placement
} // namespace genesis

#endif // include guard
