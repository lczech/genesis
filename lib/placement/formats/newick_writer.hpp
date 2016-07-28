#ifndef GENESIS_PLACEMENT_FORMATS_NEWICK_WRITER_H_
#define GENESIS_PLACEMENT_FORMATS_NEWICK_WRITER_H_

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

#include <assert.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "placement/function/helper.hpp"
#include "placement/placement_tree.hpp"
#include "placement/sample.hpp"
#include "tree/default/newick_writer.hpp"
#include "tree/formats/newick/writer.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Tree Newick Writer Mixin
// =================================================================================================

/**
 * @brief
 */
template <typename Base>
class PlacementTreeNewickWriterMixin : public Base
{
    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

public:

    bool enable_edge_nums() const
    {
        return enable_edge_nums_;
    }

    void enable_edge_nums(bool value)
    {
        enable_edge_nums_ = value;
    }

    bool enable_placement_counts() const
    {
        return enable_placement_counts_;
    }

    void enable_placement_counts(bool value)
    {
        enable_placement_counts_ = value;
    }

    void prepare_sample( Sample const& smp )
    {
        auto place_map = placements_per_edge( smp );
        placement_counts_.resize( place_map.size(), 0 );

        for( auto const p : place_map ) {
            placement_counts_[ p.first ] = p.second.size();
        }
    }

    // -------------------------------------------------------------------------
    //     Overridden Member Functions
    // -------------------------------------------------------------------------

protected:

    virtual void edge_to_element( tree::TreeEdge const& edge, tree::NewickBrokerElement& element ) override
    {
        Base::edge_to_element(edge, element);

        if (enable_edge_nums_) {
            element.tags.push_back( std::to_string(
                edge.data<PlacementEdgeData>().edge_num()
            ));
        }
        if (enable_placement_counts_) {
            element.comments.push_back(std::to_string( placement_counts_[ edge.index() ]));
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    bool enable_edge_nums_        = true;
    bool enable_placement_counts_ = false;

    std::vector<size_t> placement_counts_;
};

// =================================================================================================
//     Placement Tree Newick Writer
// =================================================================================================

typedef PlacementTreeNewickWriterMixin<
        tree::DefaultTreeNewickWriterMixin< tree::NewickWriter >
    >
    PlacementTreeNewickWriter;

} // namespace placement
} // namespace genesis

#endif // include guard
