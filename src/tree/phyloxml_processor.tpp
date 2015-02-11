/**
 * @brief Implementation of functions for reading and writing PhyloXML files.
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

// =============================================================================
//     Parsing
// =============================================================================

// =============================================================================
//     Printing
// =============================================================================

/**
 * @brief Writes the tree to a file in PhyloXML format.
 *
 * If the file already exists, the function does not overwrite it.
 */
template <class NDT, class EDT>
bool PhyloXmlProcessor::ToFile (const std::string fn, Tree<NDT, EDT>& tree)
{
    if (FileExists(fn)) {
        LOG_WARN << "PhyloXML file '" << fn << "' already exist. Will not overwrite it.";
        return false;
    }
    std::string ts;
    ToString(ts, tree);
    return FileWrite(fn, ts);
}

/**
 * @brief Gives a PhyloXML string representation of the tree.
 *
 * In case the tree was read from a PhyloXML file, this function should produce the same
 * representation.
 */
template <class NDT, class EDT>
void PhyloXmlProcessor::ToString (std::string& ts, Tree<NDT, EDT>& tree)
{
    ts = ToString(tree);
}

/**
 * @brief Returns a PhyloXML string representation of the tree.
 *
 * In case the tree was read from a PhyloXML file, this function should produce the same
 * representation.
 */
template <class NDT, class EDT>
std::string PhyloXmlProcessor::ToString (Tree<NDT, EDT>& tree)
{
    XmlDocument xml;
    ToDocument(xml, tree);
    return XmlProcessor::ToString(xml);
}

/**
 * @brief Stores the information of the tree into an PhyloXML-formatted XmlDocument.
 */
template <class NDT, class EDT>
void PhyloXmlProcessor::ToDocument (XmlDocument& xml, Tree<NDT, EDT>& tree)
{
    xml.clear();

    // set xml declaration
    xml.xml_tag = "xml";
    xml.declarations.emplace("version",  "1.0");
    xml.declarations.emplace("encoding", "UTF-8");

    // set xml root element
    xml.tag = "phyloxml";
    xml.attributes.emplace("xmlns:xsi",          "http://www.w3.org/2001/XMLSchema-instance");
    xml.attributes.emplace("xsi:schemaLocation", "http://www.phyloxml.org http://www.phyloxml.org/1.10/phyloxml.xsd");
    xml.attributes.emplace("xmlns",              "http://www.phyloxml.org");

    // add the tree element
    XmlElement* pg = new XmlElement();
    xml.content.push_back(pg);
    pg->tag = "phylogeny";
    pg->attributes.emplace("rooted",     "true");
    //~ pg.attributes.emplace("rerootable", "true");

    // store the distance from each node to the root.
    std::vector<int> depths = tree.NodeDepthVector();

    std::vector<XmlElement*> stack;
    stack.push_back(pg);
    int cur_d = 0;

    for (
        typename Tree<NDT, EDT>::IteratorPreorder it = tree.BeginPreorder();
        it != tree.EndPreorder();
        ++it
    ) {
        // depth can never increase more than one between two nodes when doing a preoder traversal.
        assert(depths[it.Node()->Index()] <= cur_d + 1);

        // delete end of stack when moving up the tree, unless we are already at the root.
        while (cur_d >= depths[it.Node()->Index()] && depths[it.Node()->Index()] > 0) {
            assert(stack.size() > 0);
            stack.pop_back();
            --cur_d;
        }
        // set current depth (needed in case we are moving further into the tree, so that the loop
        // is not executed).
        cur_d = depths[it.Node()->Index()];

        // create clade element
        XmlElement* clade = new XmlElement();
        stack.back()->content.push_back(clade);
        stack.push_back(clade);
        clade->tag = "clade";

        // create name for clade.
        // TODO move to node.tophyloxmlelement
        XmlElement* name_e = new XmlElement();
        clade->content.push_back(name_e);
        name_e->tag = "name";
        XmlMarkup* name_m = new XmlMarkup();
        name_e->content.push_back(name_m);
        name_m->content = it.Node()->name;

        //~ it.Node()->ToNewickBrokerElement(bn);
        // only write edge data to the broker element if it is not the last iteration.
        // the last iteration is the root, which usually does not have edge information in newick.
        // caveat: for the root node, the edge will point to an arbitrary edge away from the root.
        //~ if (!it.IsLastIteration()) {
            //~ it.Edge()->ToNewickBrokerElement(bn);
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
