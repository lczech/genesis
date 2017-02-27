#ifndef GENESIS_PLACEMENT_PQUERY_H_
#define GENESIS_PLACEMENT_PQUERY_H_

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
 * @ingroup placement
 */

#include <memory>
#include <string>
#include <vector>

#include "genesis/placement/pquery/name.hpp"
#include "genesis/placement/pquery/placement.hpp"

#include "genesis/utils/core/range.hpp"

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree {

    class TreeEdge;

}

namespace placement {

    class PlacementEdgeData;
    class PlacementNodeData;

}

// =================================================================================================
//     Pquery
// =================================================================================================

namespace placement {

/**
 * @brief A pquery holds a set of PqueryPlacement%s and a set of PqueryName%s.
 *
 * According to the `jplace` standard, a pquery is a container object that represents the possible
 * phylogenetic placement positions of a sequence (or set of sequences). Each such position is
 * stored as a PqueryPlacement and can be accessed via the functions of this class.
 *
 * Furthermore, it might be useful to combine the placement positions of several sequences into
 * one object. This is for example the case if there are replicate sequences. Thus, a Pquery
 * supports to store multiple PqueryName%s, each of them containing an identifying name string and
 * a so called @link PqueryName::multiplicity multiplicity@endlink, which can be used as e.g.,
 * an abundance count for the associated name, and is used as a factor for the weights of the
 * placements in some calculations.
 */
class Pquery
{
public:

    // -------------------------------------------------------------------
    //     Typedefs
    // -------------------------------------------------------------------

    typedef std::vector<PqueryPlacement>                 PqueryPlacements;
    typedef std::vector<PqueryName>                      PqueryNames;

    typedef std::vector<PqueryPlacement>::iterator       iterator_placements;
    typedef std::vector<PqueryPlacement>::const_iterator const_iterator_placements;

    typedef std::vector<PqueryName>::iterator            iterator_names;
    typedef std::vector<PqueryName>::const_iterator      const_iterator_names;

    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

    Pquery () = default;
    ~Pquery() = default;

    Pquery( Pquery const& ) = default;
    Pquery( Pquery&& )      = default;

    Pquery& operator= ( Pquery const& ) = default;
    Pquery& operator= ( Pquery&& )      = default;

    // -------------------------------------------------------------------
    //     General Modifiers
    // -------------------------------------------------------------------

    void clear();

    // -------------------------------------------------------------------
    //     Placement Iterators
    // -------------------------------------------------------------------

    iterator_placements       begin_placements();
    const_iterator_placements begin_placements() const;

    iterator_placements       end_placements();
    const_iterator_placements end_placements() const;

    utils::Range<iterator_placements>       placements();
    utils::Range<const_iterator_placements> placements() const;

    // -------------------------------------------------------------------
    //     Placement Accessors and Modifiers
    // -------------------------------------------------------------------

    PqueryPlacement& add_placement( PlacementTreeEdge& edge );
    PqueryPlacement& add_placement( PqueryPlacement const& val );

    PqueryPlacement& add_placement(
        PlacementTreeEdge    & edge,
        PqueryPlacement const& val
    );

    size_t placement_size() const;

    PqueryPlacement      & placement_at( size_t index );
    PqueryPlacement const& placement_at( size_t index ) const;

    void remove_placement_at( size_t index );
    void clear_placements();

    // -------------------------------------------------------------------
    //     Name Iterators
    // -------------------------------------------------------------------

    iterator_names       begin_names();
    const_iterator_names begin_names() const;

    iterator_names       end_names();
    const_iterator_names end_names() const;

    utils::Range<iterator_names>       names();
    utils::Range<const_iterator_names> names() const;

    // -------------------------------------------------------------------
    //     Name Accessors and Modifiers
    // -------------------------------------------------------------------

    PqueryName& add_name( std::string name = "", double multiplicity = 1.0 );
    PqueryName& add_name( PqueryName const& other );

    size_t name_size() const;

    PqueryName      & name_at( size_t index );
    PqueryName const& name_at( size_t index ) const;

    void remove_name_at( size_t index );
    void clear_names();

    // -------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------

private:

    std::vector<PqueryPlacement> placements_;
    std::vector<PqueryName>      names_;

};

} // namespace placement
} // namespace genesis

#endif // include guard
