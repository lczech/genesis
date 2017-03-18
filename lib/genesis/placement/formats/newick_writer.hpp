#ifndef GENESIS_PLACEMENT_FORMATS_NEWICK_WRITER_H_
#define GENESIS_PLACEMENT_FORMATS_NEWICK_WRITER_H_

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

#include <assert.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/placement_tree.hpp"
#include "genesis/placement/sample.hpp"
#include "genesis/tree/default/newick_writer.hpp"
#include "genesis/tree/formats/newick/writer.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Tree Newick Writer Plugin
// =================================================================================================

/**
 * @brief
 */
class PlacementTreeNewickWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    PlacementTreeNewickWriterPlugin() = default;
    virtual ~PlacementTreeNewickWriterPlugin() = default;

    PlacementTreeNewickWriterPlugin(PlacementTreeNewickWriterPlugin const&) = default;
    PlacementTreeNewickWriterPlugin(PlacementTreeNewickWriterPlugin&&)      = default;

    PlacementTreeNewickWriterPlugin& operator= (PlacementTreeNewickWriterPlugin const&) = default;
    PlacementTreeNewickWriterPlugin& operator= (PlacementTreeNewickWriterPlugin&&)      = default;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

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

        for( size_t edge_i = 0; edge_i < place_map.size(); ++edge_i ) {
            placement_counts_[ edge_i ] = place_map[ edge_i ].size();
        }
    }

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    void edge_to_element( tree::TreeEdge const& edge, tree::NewickBrokerElement& element ) const
    {
        if (enable_edge_nums_) {
            element.tags.push_back( std::to_string(
                edge.data<PlacementEdgeData>().edge_num()
            ));
        }
        if (enable_placement_counts_) {
            element.comments.push_back(std::to_string( placement_counts_[ edge.index() ]));
        }
    }

    void register_with( tree::NewickWriter& writer ) const
    {
        // Set edge functions.
        writer.edge_to_element_plugins.push_back(
            [&]( tree::TreeEdge const& edge, tree::NewickBrokerElement& element ) {
                PlacementTreeNewickWriterPlugin::edge_to_element( edge, element );
            }
        );
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

class PlacementTreeNewickWriter
    : public tree::NewickWriter
    , public tree::DefaultTreeNewickWriterPlugin
    , public PlacementTreeNewickWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    PlacementTreeNewickWriter()
    {
        DefaultTreeNewickWriterPlugin::register_with( *this );
        PlacementTreeNewickWriterPlugin::register_with( *this );
    }
};

} // namespace placement
} // namespace genesis

#endif // include guard
