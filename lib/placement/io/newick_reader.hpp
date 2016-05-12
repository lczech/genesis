#ifndef GENESIS_PLACEMENT_IO_NEWICK_READER_H_
#define GENESIS_PLACEMENT_IO_NEWICK_READER_H_

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

#include "placement/placement_tree.hpp"
#include "tree/default/newick_reader.hpp"
#include "tree/formats/newick/reader.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Tree Newick Reader Mixin
// =================================================================================================

/**
 * @brief
 */
template <typename Base>
class PlacementTreeNewickReaderMixin : public Base
{
    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    typedef typename Base::TreeType TreeType;
    typedef typename Base::NodeType NodeType;
    typedef typename Base::EdgeType EdgeType;
    typedef typename Base::LinkType LinkType;

    // -------------------------------------------------------------------------
    //     Overridden Member Functions
    // -------------------------------------------------------------------------

protected:

    virtual void element_to_edge( tree::NewickBrokerElement const& element, EdgeType& edge ) override
    {
        Base::element_to_edge(element, edge);

        // Process the edge num.
        edge.data.reset_edge_num(-1);
        if (element.tags.size() == 0) {
            throw std::invalid_argument(
                "Edge at node '" + element.name + "' does not contain a tag value like '{42}'" +
                " for the placement edge_num of this edge."
            );
        }
        if (element.tags.size() > 1) {
            throw std::invalid_argument(
                "Edge at node '" + element.name + "' contains more than one tag value like " +
                "'{xyz}'. Expecting only one for the placement edge_num of this edge."
            );
        }
        assert(element.tags.size() == 1);
        edge.data.reset_edge_num( std::stoi( element.tags[0] ));
    }

};

// =================================================================================================
//     Placement Tree Newick Reader
// =================================================================================================

typedef PlacementTreeNewickReaderMixin<
        tree::DefaultTreeNewickReaderMixin< tree::NewickReader< PlacementTree > >
    >
    PlacementTreeNewickReader;

} // namespace placement
} // namespace genesis

#endif // include guard
