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
 * @brief Testing Default Tree methods.
 *
 * @file
 * @ingroup test
 */

#include "src/common.hpp"

#include "genesis/tree/attribute_tree/tree.hpp"
#include "genesis/tree/attribute_tree/indexed_newick_reader.hpp"
// #include "genesis/tree/attribute_tree/keyed_newick_reader.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/tree.hpp"

#include <algorithm>
#include <string>

using namespace genesis;
using namespace tree;

TEST( AttributeTree, IndexedNewickReaderIndex )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    std::string infile = environment->data_dir + "tree/indexed_attributes_0.newick";

    IndexedAttributeTreeNewickReader reader;
    reader.add_indexed_attribute(
        IndexedAttributeTreeNewickReader::Source::kComment, 0,
        IndexedAttributeTreeNewickReader::Target::kEdge,    "bootstrap"
    );

    auto tree = reader.from_file( infile );

    size_t node_attr_cnt = 0;
    for( auto const& node : tree.nodes() ) {
        auto const& data = node->data<AttributeTreeNodeData>();
        node_attr_cnt += data.attributes.size();
    }

    size_t edge_attr_cnt = 0;
    for( auto const& edge : tree.edges() ) {
        auto const& data = edge->data<AttributeTreeEdgeData>();
        edge_attr_cnt += data.attributes.size();
    }

    EXPECT_EQ( 0, node_attr_cnt );
    EXPECT_EQ( 3, edge_attr_cnt );

    // LOG_DBG << "Nodes";
    // for( auto const& node : tree.nodes() ) {
    //     auto const& data = node->data<AttributeTreeNodeData>();
    //     LOG_DBG1 << "node " << data.name;
    //     for( auto const& m : data.attributes ) {
    //         LOG_DBG1 << m.first << " --> " << m.second;
    //     }
    // }
    //
    // LOG_DBG << "Edges";
    // for( auto const& edge : tree.edges() ) {
    //     auto const& data = edge->data<AttributeTreeEdgeData>();
    //     for( auto const& m : data.attributes ) {
    //         LOG_DBG1 << m.first << " --> " << m.second;
    //     }
    // }
}

TEST( AttributeTree, IndexedNewickReaderCatchAll )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    std::string infile = environment->data_dir + "tree/indexed_attributes_1.newick";

    IndexedAttributeTreeNewickReader reader;
    reader.add_catch_all_attribute(
        IndexedAttributeTreeNewickReader::Source::kComment,
        IndexedAttributeTreeNewickReader::Target::kEdge,    "comment_"
    );

    auto tree = reader.from_file( infile );

    size_t node_attr_cnt = 0;
    for( auto const& node : tree.nodes() ) {
        auto const& data = node->data<AttributeTreeNodeData>();
        node_attr_cnt += data.attributes.size();
    }

    size_t edge_attr_cnt = 0;
    for( auto const& edge : tree.edges() ) {
        auto const& data = edge->data<AttributeTreeEdgeData>();
        edge_attr_cnt += data.attributes.size();
    }

    EXPECT_EQ(  0, node_attr_cnt );
    EXPECT_EQ( 12, edge_attr_cnt );
}
