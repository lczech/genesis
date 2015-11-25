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
#include <vector>

#include "tree/distances.hpp"
#include "tree/iterators/preorder.hpp"
#include "tree/tree.hpp"
#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/std.hpp"
#include "utils/io/xml_document.hpp"
#include "utils/io/xml_processor.hpp"

namespace genesis {

// =================================================================================================
//     Parsing
// =================================================================================================

// template <typename TreeType>
// void PhyloxmlProcessor<TreeType>::element_to_tree_node( XmlElement const& element, EdgeType& edge )
// {
//     // Silence unused parameter warnings.
//     (void) element;
//     (void) edge;
// }
//
// template <typename TreeType>
// void PhyloxmlProcessor<TreeType>::element_to_tree_edge( XmlElement const& element, NodeType& node )
// {
//     // Silence unused parameter warnings.
//     (void) element;
//     (void) node;
// }

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief Writes the tree to a file in Phyloxml format.
 *
 * If the file already exists, the function does not overwrite it.
 */
template <typename TreeType>
bool PhyloxmlProcessor<TreeType>::to_file (const TreeType& tree, const std::string fn)
{
    if (file_exists(fn)) {
        LOG_WARN << "Phyloxml file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string ts;
    to_string(tree, ts);
    return file_write(fn, ts);
}

/**
 * @brief Gives a Phyloxml string representation of the tree.
 *
 * In case the tree was read from a Phyloxml file, this function should produce the same
 * representation.
 */
template <typename TreeType>
void PhyloxmlProcessor<TreeType>::to_string (const TreeType& tree, std::string& ts)
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
std::string PhyloxmlProcessor<TreeType>::to_string (const TreeType& tree)
{
    XmlDocument xml;
    to_document(tree, xml);
    return XmlProcessor().to_string(xml);
}

/**
 * @brief Stores the information of the tree into an Phyloxml-formatted XmlDocument.
 */
template <typename TreeType>
void PhyloxmlProcessor<TreeType>::to_document (const TreeType& tree, XmlDocument& xml)
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
    auto phylogeny = make_unique<XmlElement>();
    phylogeny->tag = "phylogeny";
    phylogeny->attributes.emplace("rooted",     "true");
    //~ phylogeny.attributes.emplace("rerootable", "true");

    // Create a stack where we will push the tree elements to. Use the phylogeny element as root.
    std::vector<XmlElement*> stack;
    stack.push_back(phylogeny.get());
    xml.content.push_back(std::move(phylogeny));
    int cur_d = 0;

    // Store the distance from each node to the root. Will be used to determine the position on the
    // stack that is used for adding clades to the phylogeny.
    std::vector<int> depths = node_depth_vector(tree);

    for (
        auto it = tree.begin_preorder();
        it != tree.end_preorder();
        ++it
    ) {
        // Depth can never increase more than one between two nodes when doing a preoder traversal.
        assert(depths[it.node()->index()] <= cur_d + 1);

        // Delete end of stack when moving up the tree, unless we are already at the root.
        while (cur_d >= depths[it.node()->index()] && depths[it.node()->index()] > 0) {
            assert(stack.size() > 0);
            stack.pop_back();
            --cur_d;
        }
        // Set current depth (explicitly needed in case we are moving further into the tree, which
        // means that the loop above is not executed).
        cur_d = depths[it.node()->index()];

        // Create clade element, append it to the stack, so that all sub-elements will use it as
        // parent.
        auto clade = make_unique<XmlElement>();
        clade->tag = "clade";

        tree_node_to_element(*it.node(), *clade.get());
        tree_edge_to_element(*it.edge(), *clade.get());

        // Append the clade to the current parent (end of the stack), then use it as the new parent
        // for the next iteration of the loop.
        auto clade_ptr = clade.get();
        stack.back()->content.push_back(std::move(clade));
        stack.push_back(clade_ptr);
    }

    finish_writing(tree, xml);
}

template <typename TreeType>
void PhyloxmlProcessor<TreeType>::prepare_writing( TreeType const& tree, XmlDocument& xml )
{
    // Silence unused parameter warnings.
    (void) tree;
    (void) xml;
}

template <typename TreeType>
void PhyloxmlProcessor<TreeType>::tree_node_to_element( NodeType const& node, XmlElement& element )
{
    // Silence unused parameter warnings.
    (void) element;
    (void) node;
}

template <typename TreeType>
void PhyloxmlProcessor<TreeType>::tree_edge_to_element( EdgeType const& edge, XmlElement& element )
{
    // Silence unused parameter warnings.
    (void) element;
    (void) edge;
}

template <typename TreeType>
void PhyloxmlProcessor<TreeType>::finish_writing( TreeType const& tree, XmlDocument& xml )
{
    // Silence unused parameter warnings.
    (void) tree;
    (void) xml;
}

} // namespace genesis
