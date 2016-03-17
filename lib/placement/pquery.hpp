#ifndef GENESIS_PLACEMENT_PQUERY_H_
#define GENESIS_PLACEMENT_PQUERY_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <memory>
#include <string>
#include <vector>

#include "placement/pquery/name.hpp"
#include "placement/pquery/placement.hpp"

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace tree {
    template<class NodeDataType, class EdgeDataType>
    class TreeEdge;
}

namespace placement {
    class PlacementTreeEdgeData;
    class PlacementTreeNodeData;

    typedef tree::TreeEdge< PlacementTreeNodeData, PlacementTreeEdgeData > PlacementTreeEdge;
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
 * supports to store multiple PquereName%s, each of them containing an identifying name string and
 * a so called `multiplicity`, which can be used as e.g., an abundance count for the associated
 * name.
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

    // -------------------------------------------------------------------
    //     Placement Accessors and Modifiers
    // -------------------------------------------------------------------

    PqueryPlacement& add_placement( PlacementTreeEdge& edge );
    PqueryPlacement& add_placement( PqueryPlacement const& val );

    PqueryPlacement& add_placement(
        PlacementTreeEdge    & edge,
        PqueryPlacement const& val
    );

    PqueryPlacements const& placements() const;
    size_t placement_size() const;

    PqueryPlacement      & placement_at( size_t index );
    PqueryPlacement const& placement_at( size_t index ) const;

    void clear_placements();

    // -------------------------------------------------------------------
    //     Name Iterators
    // -------------------------------------------------------------------

    iterator_names       begin_names();
    const_iterator_names begin_names() const;

    iterator_names       end_names();
    const_iterator_names end_names() const;

    // -------------------------------------------------------------------
    //     Name Accessors and Modifiers
    // -------------------------------------------------------------------

    PqueryName& add_name( std::string name = "", double multiplicity = 0.0 );
    PqueryName& add_name( PqueryName const& other );

    PqueryNames const& names() const;
    size_t name_size() const;

    PqueryName      & name_at( size_t index );
    PqueryName const& name_at( size_t index ) const;

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
