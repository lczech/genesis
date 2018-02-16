#ifndef GENESIS_PLACEMENT_PQUERY_H_
#define GENESIS_PLACEMENT_PQUERY_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

    /**
     * @brief Clear all data stored in this Pquery, i.e., clear all PqueryName%s and all
     * PqueryPlacement%s.
     */
    void clear();

    // -------------------------------------------------------------------
    //     Placement Iterators
    // -------------------------------------------------------------------

    iterator_placements       begin_placements();
    const_iterator_placements begin_placements() const;

    iterator_placements       end_placements();
    const_iterator_placements end_placements() const;

    /**
     * @brief Return a Range iterator to the PqueryPlacement%s.
     *
     * This makes iterating placements via a range based for loop easy.
     */
    utils::Range<iterator_placements>       placements();

    /**
    * @brief Return a const Range iterator to the PqueryPlacement%s.
    *
    * This makes iterating placements via a range based for loop easy.
    */
    utils::Range<const_iterator_placements> placements() const;

    std::vector<PqueryPlacement>& expose_placements();

    // -------------------------------------------------------------------
    //     Placement Accessors and Modifiers
    // -------------------------------------------------------------------

    /**
     * @brief Create a new PqueryPlacement at a given PlacementTreeEdge, add it to the Pquery and
     * return it.
     *
     * The values of the placement can then be adjusted using the returned object reference.
     *
     * It is important that the provided edge belongs to the same PlacementTree as the Pquery and
     * its containing Sample do. This is up to the user and not checked.
     *
     * As this function might reallocate the memory where placements are stored, all iterators
     * and pointer to PqueryPlacement%s are invalidated.
     */
    PqueryPlacement& add_placement( PlacementTreeEdge& edge );

    /**
     * @brief Create a new PqueryPlacement as a copy of the provided one, add it to the Pquery and
     * return it.
     *
     * As this function might reallocate the memory where placements are stored, all iterators
     * and pointer to PqueryPlacement%s are considered to be invalidated.
     */
    PqueryPlacement& add_placement( PqueryPlacement const& val );

    /**
     * @brief Create a new PqueryPlacement at a given PlacementTreeEdge, add it to the Pquery and
     * return it. The property values of the provided PqueryPlacement are copied.
     *
     * It is important that the provided edge belongs to the same PlacementTree as the Pquery and
     * its containing Sample do. This is up to the user and not checked.
     *
     * As this function might reallocate the memory where placements are stored, all iterators
     * and pointer to PqueryPlacement%s are considered to be invalidated.
     */
    PqueryPlacement& add_placement(
        PlacementTreeEdge    & edge,
        PqueryPlacement const& val
    );

    /**
     * @brief Return the number of PqueryPlacement%s stored in this Pquery.
     */
    size_t placement_size() const;

    /**
     * @brief Return the PqueryPlacement at a certain index.
     *
     * The index must be smaller than placement_size(), otherwise this functions throws an exception.
     */
    PqueryPlacement      & placement_at( size_t index );

    /**
     * @brief Return the PqueryPlacement at a certain index.
     *
     * The index must be smaller than placement_size(), otherwise this functions throws an exception.
     */
    PqueryPlacement const& placement_at( size_t index ) const;

    /**
     * @brief Remove the PqueryPlacement at a certain index position within this Pquery.
     *
     * As this function moves placements in the container, all iterators and pointer to
     * PqueryPlacement%s are considered to be invalidated.
     */
    void remove_placement_at( size_t index );

    /**
     * @brief Delete all PqueryPlacement%s of this Pquery.
     */
    void clear_placements();

    // -------------------------------------------------------------------
    //     Name Iterators
    // -------------------------------------------------------------------

    iterator_names       begin_names();
    const_iterator_names begin_names() const;

    iterator_names       end_names();
    const_iterator_names end_names() const;

    /**
    * @brief Return a Range iterator to the PqueryName%s.
    *
    * This makes iterating placements via a range based for loop easy.
    */
    utils::Range<iterator_names>       names();

    /**
    * @brief Return a const Range iterator to the PqueryName%s.
    *
    * This makes iterating placements via a range based for loop easy.
    */
    utils::Range<const_iterator_names> names() const;

    std::vector<PqueryName>& expose_names();

    // -------------------------------------------------------------------
    //     Name Accessors and Modifiers
    // -------------------------------------------------------------------

    /**
     * @brief Create a new PqueryName using the provided parameters, add it to the Pquery and return it.
     *
     * As this function might reallocate the memory where names are stored, all iterators
     * and pointer to PqueryNames%s are considered to be invalidated.
     */
    PqueryName& add_name( std::string name = "", double multiplicity = 1.0 );

    /**
     * @brief Create a new PqueryName as a copy of the provided one, add it to the Pquery and return it.
     *
     * As this function might reallocate the memory where names are stored, all iterators
     * and pointer to PqueryNames%s are considered to be invalidated.
     */
    PqueryName& add_name( PqueryName const& other );

    /**
     * @brief Return the number of PqueryName%s stored in this Pquery.
     */
    size_t name_size() const;

    /**
     * @brief Return the PqueryName at a certain index.
     *
     * The index must be smaller than name_size(), otherwise this functions throws an exception.
     */
    PqueryName      & name_at( size_t index );

    /**
     * @brief Return the PqueryName at a certain index.
     *
     * The index must be smaller than name_size(), otherwise this functions throws an exception.
     */
    PqueryName const& name_at( size_t index ) const;

    /**
     * @brief Remove the PqueryName at a certain index position within this Pquery.
     *
     * As this function moves names in the container, all iterators and pointer to PqueryNames%s
     * are considered to be invalidated.
     */
    void remove_name_at( size_t index );

    /**
     * @brief Delete all PqueryName%s of this Pquery.
     */
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
