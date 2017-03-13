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
 * @brief Implementation of functions for reading and writing Phyloxml files.
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/formats/phyloxml/writer.hpp"

#include "genesis/tree/tree.hpp"

#include "genesis/tree/function/distances.hpp"
#include "genesis/tree/iterator/preorder.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/logging.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/formats/xml/document.hpp"
#include "genesis/utils/formats/xml/writer.hpp"
#include "genesis/utils/io/output_stream.hpp"

#include <assert.h>
#include <stdexcept>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Printing
// =================================================================================================

void PhyloxmlWriter::to_file (const Tree& tree, const std::string filename) const
{
    std::string ts;
    to_string(tree, ts);
    utils::file_write(ts, filename);
}

void PhyloxmlWriter::to_string (const Tree& tree, std::string& ts) const
{
    ts = to_string(tree);
}

std::string PhyloxmlWriter::to_string (const Tree& tree) const
{
    utils::XmlDocument xml;
    to_document(tree, xml);
    return utils::XmlWriter().to_string(xml);
}

void PhyloxmlWriter::to_document (const Tree& tree, utils::XmlDocument& xml) const
{
    xml.clear();

    // Call all preparatory plugins.
    for( auto const& prepare_plugin : prepare_writing_plugins ) {
        prepare_plugin( tree, xml );
    }

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

        // Call all plugins to translate node and edge data to xml.
        for( auto const& node_plugin : node_to_element_plugins ) {
            node_plugin( it.node(), *clade.get() );
        }
        for( auto const& edge_plugin : edge_to_element_plugins ) {
            edge_plugin( it.edge(), *clade.get() );
        }

        // Append the clade to the current parent (end of the stack), then use it as the new parent
        // for the next iteration of the loop.
        auto clade_ptr = clade.get();
        stack.back()->content.push_back(std::move(clade));
        stack.push_back(clade_ptr);
    }

    // Call all finalizing plugins.
    for( auto const& finish_plugin : finish_writing_plugins ) {
        finish_plugin( tree, xml );
    }
}

} // namespace tree
} // namespace genesis
