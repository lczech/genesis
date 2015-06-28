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

#include "tree/tree.hpp"
#include "utils/logging.hpp"
#include "utils/xml_document.hpp"
#include "utils/xml_processor.hpp"

namespace genesis {

// =================================================================================================
//     Parsing
// =================================================================================================

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief Writes the tree to a file in Phyloxml format.
 *
 * If the file already exists, the function does not overwrite it.
 */
template <class NDT, class EDT>
bool PhyloxmlProcessor::to_file (const std::string fn, const Tree<NDT, EDT>& tree)
{
    if (file_exists(fn)) {
        LOG_WARN << "Phyloxml file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string ts;
    to_string(ts, tree);
    return file_write(fn, ts);
}

/**
 * @brief Gives a Phyloxml string representation of the tree.
 *
 * In case the tree was read from a Phyloxml file, this function should produce the same
 * representation.
 */
template <class NDT, class EDT>
void PhyloxmlProcessor::to_string (std::string& ts, const Tree<NDT, EDT>& tree)
{
    ts = to_string(tree);
}

/**
 * @brief Returns a Phyloxml string representation of the tree.
 *
 * In case the tree was read from a Phyloxml file, this function should produce the same
 * representation.
 */
template <class NDT, class EDT>
std::string PhyloxmlProcessor::to_string (const Tree<NDT, EDT>& tree)
{
    XmlDocument xml;
    to_document(xml, tree);
    return XmlProcessor().to_string(xml);
}

/**
 * @brief Stores the information of the tree into an Phyloxml-formatted XmlDocument.
 */
template <class NDT, class EDT>
void PhyloxmlProcessor::to_document (XmlDocument& xml, const Tree<NDT, EDT>& tree)
{
    xml.clear();

    // set xml declaration
    xml.xml_tag = "xml";
    xml.declarations.emplace("version",  "1.0");
    xml.declarations.emplace("encoding", "UTF-8");

    // set xml root element
    xml.tag = "Phyloxml";
    xml.attributes.emplace("xmlns:xsi",          "http://www.w3.org/2001/XMLSchema-instance");
    xml.attributes.emplace("xsi:schemaLocation", "http://www.Phyloxml.org http://www.Phyloxml.org/1.10/Phyloxml.xsd");
    xml.attributes.emplace("xmlns",              "http://www.Phyloxml.org");

    // add the tree element
    XmlElement* pg = new XmlElement();
    xml.content.push_back(pg);
    pg->tag = "phylogeny";
    pg->attributes.emplace("rooted",     "true");
    //~ pg.attributes.emplace("rerootable", "true");

    // store the distance from each node to the root.
    std::vector<int> depths = tree.node_depth_vector();

    std::vector<XmlElement*> stack;
    stack.push_back(pg);
    int cur_d = 0;

    for (
        typename Tree<NDT, EDT>::ConstIteratorPreorder it = tree.begin_preorder();
        it != tree.end_preorder();
        ++it
    ) {
        // depth can never increase more than one between two nodes when doing a preoder traversal.
        assert(depths[it.node()->index()] <= cur_d + 1);

        // delete end of stack when moving up the tree, unless we are already at the root.
        while (cur_d >= depths[it.node()->index()] && depths[it.node()->index()] > 0) {
            assert(stack.size() > 0);
            stack.pop_back();
            --cur_d;
        }
        // set current depth (needed in case we are moving further into the tree, so that the loop
        // is not executed).
        cur_d = depths[it.node()->index()];

        // create clade element
        XmlElement* clade = new XmlElement();
        stack.back()->content.push_back(clade);
        stack.push_back(clade);
        clade->tag = "clade";

        // create name for clade.
        // TODO move to node.toPhyloxmlelement
        XmlElement* name_e = new XmlElement();
        clade->content.push_back(name_e);
        name_e->tag = "name";
        XmlMarkup* name_m = new XmlMarkup();
        name_e->content.push_back(name_m);
        name_m->content = it.node()->name;

        //~ it.node()->to_newick_broker_element(bn);
        // only write edge data to the broker element if it is not the last iteration.
        // the last iteration is the root, which usually does not have edge information in newick.
        // caveat: for the root node, the edge will point to an arbitrary edge away from the root.
        //~ if (!it.is_last_iteration()) {
            //~ it.edge()->to_newick_broker_element(bn);
        //~ }

        // filter out default names if needed
        //~ if (!use_default_names && bn->name != "" && (
            //~ bn->name == default_leaf_name ||
            //~ bn->name == default_internal_name ||
            //~ bn->name == default_root_name
        //~ )) {
            //~ bn->name = "";
        //~ }
    }
}

} // namespace genesis
