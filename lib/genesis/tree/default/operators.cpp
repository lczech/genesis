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
 * @ingroup tree
 */

#include "genesis/tree/default/operators.hpp"

#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/function/operators.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/math/common.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Comparison and Conversion
// =================================================================================================

bool equal_default_trees(
    Tree const& lhs,
    Tree const& rhs,
    bool compare_node_names,
    bool compare_branch_lengths
) {
    auto node_comparator = [&] (
        DefaultTreeNode const& node_l,
        DefaultTreeNode const& node_r
    ) {
        if( ! compare_node_names ) {
            return true;
        }

        auto l_ptr = dynamic_cast< DefaultNodeData const* >( node_l.data_ptr() );
        auto r_ptr = dynamic_cast< DefaultNodeData const* >( node_r.data_ptr() );
        if( l_ptr == nullptr || r_ptr == nullptr ) {
            return false;
        }
        // if( l_ptr->name != r_ptr->name ) {
        //     LOG_DBG << "Differing names: " << l_ptr->name << " and " << r_ptr->name;
        // }

        return l_ptr->name == r_ptr->name;
    };

    auto edge_comparator = [&] (
        DefaultTreeEdge const& edge_l,
        DefaultTreeEdge const& edge_r
    ) {
        if( ! compare_branch_lengths ) {
            return true;
        }

        auto l_ptr = dynamic_cast< DefaultEdgeData const* >( edge_l.data_ptr() );
        auto r_ptr = dynamic_cast< DefaultEdgeData const* >( edge_r.data_ptr() );
        if( l_ptr == nullptr || r_ptr == nullptr ) {
            return false;
        }
        // if( ! utils::almost_equal_relative( l_ptr->branch_length, r_ptr->branch_length ) ) {
        //     LOG_DBG << "Differing branch lengths: " << l_ptr->branch_length << " and " << r_ptr->branch_length;
        // }

        return utils::almost_equal_relative( l_ptr->branch_length, r_ptr->branch_length );
    };

    return tree::equal( lhs, rhs, node_comparator, edge_comparator );
}

DefaultTree convert_to_default_tree( Tree const& source_tree )
{
    // In both converter functions, we first cast to default data, in order to make sure that we
    // actually have data that is derived from default data. If not, those casts will throw.
    // Then, we explicitly call functions of the default data classes, i.e., we break the virtual
    // call on purpose, by calling `.DefaultNodeData::...`. This makes sure that we actually get
    // default data and not the most derived one.

    auto node_data_converter = [] ( tree::BaseNodeData const& source_node ) {
        return dynamic_cast< DefaultNodeData const& >( source_node ).DefaultNodeData::clone();
    };

    auto edge_data_converter = [] ( tree::BaseEdgeData const& source_edge ) {
        return dynamic_cast< DefaultEdgeData const& >( source_edge ).DefaultEdgeData::clone();
    };

    return tree::convert(
        source_tree,
        node_data_converter,
        edge_data_converter
    );
}

} // namespace tree
} // namespace genesis
