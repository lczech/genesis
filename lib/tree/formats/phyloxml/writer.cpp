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
 * @file
 * @ingroup tree
 */

#include "tree/formats/phyloxml/writer.hpp"

#include "tree/tree.hpp"

#include "tree/function/distances.hpp"
#include "tree/iterator/preorder.hpp"

#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"
#include "utils/formats/xml/document.hpp"
#include "utils/formats/xml/writer.hpp"

#include <assert.h>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Parsing
// =================================================================================================

// -------------------------------------------------------------------------
//     Virtual Methods
// -------------------------------------------------------------------------

// // void PhyloxmlWriter::element_to_node( XmlElement const& element, NodeType& node )
// {
//     // Silence unused parameter warnings.
//     (void) element;
//     (void) noe;
// }
//
// // void PhyloxmlWriter::element_to_edge( XmlElement const& element, EdgeType& edge )
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
void PhyloxmlWriter::to_file (const Tree& tree, const std::string filename)
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
void PhyloxmlWriter::to_string (const Tree& tree, std::string& ts)
{
    ts = to_string(tree);
}

/**
 * @brief Returns a Phyloxml string representation of the tree.
 *
 * In case the tree was read from a Phyloxml file, this function should produce the same
 * representation.
 */
std::string PhyloxmlWriter::to_string (const Tree& tree)
{
    utils::XmlDocument xml;
    to_document(tree, xml);
    return utils::XmlWriter().to_string(xml);
}

/**
 * @brief Stores the information of the tree into an Phyloxml-formatted XmlDocument.
 */
void PhyloxmlWriter::to_document (const Tree& tree, utils::XmlDocument& xml)
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
    auto phylogeny = utils::make_unique< utils::XmlElement >();
    phylogeny->tag = "phylogeny";
    phylogeny->attributes.emplace("rooted",     "true");
    //~ phylogeny.attributes.emplace("rerootable", "true");

    // Create a stack where we will push the tree elements to. Use the phylogeny element as root.
    std::vector< utils::XmlElement* > stack;
    stack.push_back(phylogeny.get());
    xml.content.push_back(std::move(phylogeny));
    size_t cur_d = 0;

    // Store the distance from each node to the root. Will be used to determine the position on the
    // stack that is used for adding clades to the phylogeny.
    auto depths = node_path_length_vector(tree);

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
        auto clade = utils::make_unique< utils::XmlElement >();
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

void PhyloxmlWriter::prepare_writing( Tree const& tree, utils::XmlDocument& xml )
{
    // Silence unused parameter warnings.
    (void) tree;
    (void) xml;
}

void PhyloxmlWriter::node_to_element( TreeNode const& node, utils::XmlElement& element )
{
    // Silence unused parameter warnings.
    (void) element;
    (void) node;
}

void PhyloxmlWriter::edge_to_element( TreeEdge const& edge, utils::XmlElement& element )
{
    // Silence unused parameter warnings.
    (void) element;
    (void) edge;
}

void PhyloxmlWriter::finish_writing( Tree const& tree, utils::XmlDocument& xml )
{
    // Silence unused parameter warnings.
    (void) tree;
    (void) xml;
}

} // namespace tree
} // namespace genesis
