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
 * @brief Implementation of functions for reading and writing Phyloxml files.
 *
 * For reasons of readability, in this implementation file, the template data types
 * NodeDataType and EdgeDataType are abbreviated using NDT and EDT, respectively.
 *
 * @file
 * @ingroup tree
 */

#include <assert.h>
#include <stdexcept>
#include <vector>

#include "tree/function/distances.hpp"
#include "tree/iterator/preorder.hpp"
#include "tree/tree.hpp"
#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"
#include "utils/io/xml/document.hpp"
#include "utils/io/xml/writer.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Parsing
// =================================================================================================

// -------------------------------------------------------------------------
//     Virtual Methods
// -------------------------------------------------------------------------

// template <typename TreeType>
// void PhyloxmlWriter<TreeType>::element_to_node( XmlElement const& element, NodeType& node )
// {
//     // Silence unused parameter warnings.
//     (void) element;
//     (void) noe;
// }
//
// template <typename TreeType>
// void PhyloxmlWriter<TreeType>::element_to_edge( XmlElement const& element, EdgeType& edge )
// {
//     // Silence unused parameter warnings.
//     (void) element;
//     (void) edge;
// }

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief Writes the tree to a file in Phyloxml format.
 *
 * If the file already exists, the function throws `std::runtime_error`.
 * The function uses utils::file_write. See there for other exceptions that can be thrown.
 */
template <typename TreeType>
void PhyloxmlWriter<TreeType>::to_file (const TreeType& tree, const std::string filename)
{
    if( utils::file_exists(filename) ) {
        throw std::runtime_error( "Phyloxml file '" + filename + "' already exist." );
    }
    std::string ts;
    to_string(tree, ts);
    utils::file_write(ts, filename);
}

/**
 * @brief Gives a Phyloxml string representation of the tree.
 *
 * In case the tree was read from a Phyloxml file, this function should produce the same
 * representation.
 */
template <typename TreeType>
void PhyloxmlWriter<TreeType>::to_string (const TreeType& tree, std::string& ts)
{
    ts = to_string(tree);
}

/**
 * @brief Returns a Phyloxml string representation of the tree.
 *
 * In case the tree was read from a Phyloxml file, this function should produce the same
 * representation.
 */
template <typename TreeType>
std::string PhyloxmlWriter<TreeType>::to_string (const TreeType& tree)
{
    utils::XmlDocument xml;
    to_document(tree, xml);
    return utils::XmlWriter().to_string(xml);
}

/**
 * @brief Stores the information of the tree into an Phyloxml-formatted XmlDocument.
 */
template <typename TreeType>
void PhyloxmlWriter<TreeType>::to_document (const TreeType& tree, utils::XmlDocument& xml)
{
    xml.clear();
    prepare_writing(tree, xml);

    // Set XML declaration.
    // xml.xml_tag = "xml";
    // xml.declarations.emplace("version",  "1.0");
    // xml.declarations.emplace("encoding", "UTF-8");

    // Set XML root element.
    xml.tag = "phyloxml";
    xml.attributes.emplace("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    xml.attributes.emplace(
        "xsi:schemaLocation",
        "http://www.phyloxml.org http://www.phyloxml.org/1.10/phyloxml.xsd"
    );
    xml.attributes.emplace("xmlns",     "http://www.phyloxml.org");

    // Add the (phylogeny) element.
    auto phylogeny = make_unique< utils::XmlElement >();
    phylogeny->tag = "phylogeny";
    phylogeny->attributes.emplace("rooted",     "true");
    //~ phylogeny.attributes.emplace("rerootable", "true");

    // Create a stack where we will push the tree elements to. Use the phylogeny element as root.
    std::vector< utils::XmlElement* > stack;
    stack.push_back(phylogeny.get());
    xml.content.push_back(std::move(phylogeny));
    int cur_d = 0;

    // Store the distance from each node to the root. Will be used to determine the position on the
    // stack that is used for adding clades to the phylogeny.
    std::vector<int> depths = node_depth_vector(tree);

    for( auto it : preorder(tree) ) {
        // Depth can never increase more than one between two nodes when doing a preoder traversal.
        assert(depths[it.node().index()] <= cur_d + 1);

        // Delete end of stack when moving up the tree, unless we are already at the root.
        while (cur_d >= depths[it.node().index()] && depths[it.node().index()] > 0) {
            assert(stack.size() > 0);
            stack.pop_back();
            --cur_d;
        }
        // Set current depth (explicitly needed in case we are moving further into the tree, which
        // means that the loop above is not executed).
        cur_d = depths[it.node().index()];

        // Create clade element, append it to the stack, so that all sub-elements will use it as
        // parent.
        auto clade = make_unique< utils::XmlElement >();
        clade->tag = "clade";

        node_to_element( it.node(), *clade.get() );
        edge_to_element( it.edge(), *clade.get() );

        // Append the clade to the current parent (end of the stack), then use it as the new parent
        // for the next iteration of the loop.
        auto clade_ptr = clade.get();
        stack.back()->content.push_back(std::move(clade));
        stack.push_back(clade_ptr);
    }

    finish_writing(tree, xml);
}

// -------------------------------------------------------------------------
//     Virtual Methods
// -------------------------------------------------------------------------

template <typename TreeType>
void PhyloxmlWriter<TreeType>::prepare_writing( TreeType const& tree, utils::XmlDocument& xml )
{
    // Silence unused parameter warnings.
    (void) tree;
    (void) xml;
}

template <typename TreeType>
void PhyloxmlWriter<TreeType>::node_to_element( NodeType const& node, utils::XmlElement& element )
{
    // Silence unused parameter warnings.
    (void) element;
    (void) node;
}

template <typename TreeType>
void PhyloxmlWriter<TreeType>::edge_to_element( EdgeType const& edge, utils::XmlElement& element )
{
    // Silence unused parameter warnings.
    (void) element;
    (void) edge;
}

template <typename TreeType>
void PhyloxmlWriter<TreeType>::finish_writing( TreeType const& tree, utils::XmlDocument& xml )
{
    // Silence unused parameter warnings.
    (void) tree;
    (void) xml;
}

} // namespace tree
} // namespace genesis
