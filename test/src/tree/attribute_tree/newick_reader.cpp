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
#include "genesis/tree/attribute_tree/keyed_newick_reader.hpp"
#include "genesis/tree/function/functions.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/tree.hpp"

#include <algorithm>
#include <string>
#include <utility>

using namespace genesis;
using namespace tree;

std::pair<size_t, size_t> count_attribute_tree_data( AttributeTree const& tree )
{
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

    return { node_attr_cnt, edge_attr_cnt };
}

void print_attribute_tree_data( AttributeTree const& tree )
{
    LOG_DBG << "Nodes";
    for( auto const& node : tree.nodes() ) {
        auto const& data = node->data<AttributeTreeNodeData>();
        LOG_DBG1 << "node " << data.name;
        for( auto const& m : data.attributes ) {
            LOG_DBG2 << m.first << " --> " << m.second;
        }
    }

    LOG_DBG << "Edges";
    for( auto const& edge : tree.edges() ) {
        auto const& data = edge->data<AttributeTreeEdgeData>();
        LOG_DBG1 << "edge";
        for( auto const& m : data.attributes ) {
            LOG_DBG2 << m.first << " --> " << m.second;
        }
    }
}

TEST( AttributeTree, IndexedNewickReaderIndex )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    std::string infile = environment->data_dir + "tree/indexed_attributes_0.newick";

    IndexedAttributeTreeNewickReader reader;
    reader.add_attribute(
        IndexedAttributeTreeNewickReader::Source::kComment, 0,
        IndexedAttributeTreeNewickReader::Target::kEdge,    "bootstrap"
    );

    auto tree = reader.from_file( infile );

    auto counts = count_attribute_tree_data( tree );
    EXPECT_EQ( 0, counts.first );
    EXPECT_EQ( 3, counts.second );

    // print_attribute_tree_data( tree );
}

TEST( AttributeTree, IndexedNewickReaderCatchAll )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    std::string infile = environment->data_dir + "tree/indexed_attributes_1.newick";

    IndexedAttributeTreeNewickReader reader;
    reader.add_catch_all(
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

    auto counts = count_attribute_tree_data( tree );
    EXPECT_EQ(  0, counts.first );
    EXPECT_EQ( 12, counts.second );
}

TEST( AttributeTree, KeyedNewickReaderKeys )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    std::string infile = environment->data_dir + "tree/keyed_attributes_0.newick";

    KeyedAttributeTreeNewickReader reader;
    reader.add_attribute( "bs",     KeyedAttributeTreeNewickReader::Target::kEdge );
    reader.add_attribute( "!color", KeyedAttributeTreeNewickReader::Target::kEdge, "color" );

    auto tree = reader.from_file( infile );

    auto counts = count_attribute_tree_data( tree );
    EXPECT_EQ( 0, counts.first );
    EXPECT_EQ( 4, counts.second );
}

TEST( AttributeTree, KeyedNewickReaderCatchAll )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    std::string infile = environment->data_dir + "tree/keyed_attributes_0.newick";

    KeyedAttributeTreeNewickReader reader;
    reader.add_catch_all( KeyedAttributeTreeNewickReader::Target::kEdge );

    auto tree = reader.from_file( infile );

    auto counts = count_attribute_tree_data( tree );
    EXPECT_EQ( 0, counts.first );
    EXPECT_EQ( 4, counts.second );

    // print_attribute_tree_data( tree );
}

TEST( AttributeTree, KeyedNewickReaderNHX )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    // Read and process tree.
    std::string infile = environment->data_dir + "tree/keyed_attributes_1.newick";

    KeyedAttributeTreeNewickReader reader;
    reader.add_nhx_attributes();
    // reader.add_catch_all( KeyedAttributeTreeNewickReader::Target::kNode );

    auto tree = reader.from_file( infile );

    auto counts = count_attribute_tree_data( tree );
    EXPECT_EQ( 25, counts.first );
    EXPECT_EQ(  1, counts.second );

    // print_attribute_tree_data( tree );
}
