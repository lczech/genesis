#ifndef GENESIS_TREE_FORMATS_PHYLOXML_WRITER_H_
#define GENESIS_TREE_FORMATS_PHYLOXML_WRITER_H_

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
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <functional>
#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward declarations
// =================================================================================================

namespace utils {
    class XmlDocument;
    class XmlElement;
}

namespace tree {

class  Tree;
class  TreeNode;
class  TreeEdge;
class  TreeSet;

// =================================================================================================
//     Phyloxml Writer
// =================================================================================================

/**
 * @brief Write a Tree to Phyloxml format.
 *
 * This class supports to write a Tree into a Phyloxml format representation, using
 *
 *   * to_file()
 *   * to_string()
 *   * to_document()
 *
 * It understands the Phyloxml format, but is agnostic of the actual data representation of
 * TreeNode and TreeEdge data. This approach allows to store data in any wanted format.
 *
 * In order to translate data from the Tree into a Phyloxml format representation, a set of plugin
 * functions is used, that need to be set before writing a Tree. Those functions are a form of
 * intermediaries, which take tree data and turn them into the wanted Phyloxml representation.
 * It is possible to use lambdas for this, or any other function that can be stored in a
 * `std::function`.
 *
 * The following plugin points are provided:
 *
 *   * #prepare_writing_plugins
 *   * #node_to_element_plugins
 *   * #edge_to_element_plugins
 *   * #finish_writing_plugins
 *
 * For example, the DefaultTreePhyloxmlWriterPlugin is a convenience class that provides such plugin
 * functions. It translates from a #DefaultTree with TreeNode%s that contain names and TreeEdge%s
 * that contain branch lengths into the standard Phyloxml format. Using plugin classes like this
 * additionally allows to use state for the plugin functions - that is, to use some settings for
 * how to write data.
 *
 * Furthermore, as we use vectors of plugin functions, it is possible (and often necessary) to
 * register multiple such functions, which are then called one after another. This allows to e.g.,
 * first translate a branch length for an edge in one plugin function, and then translating
 * a bootstrap value or edge color in another plugin function.
 *
 * This whole approach is a bit tedious, but unfortunately the price for the flexibility of this
 * class. In order to keep the standard use cases simple, we also provide classes like
 * DefaultTreePhyloxmlWriter, which hides the whole plugin system and allows simple writing
 * of default trees to standard Phyloxml.
 */
class PhyloxmlWriter
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Function type that allows to do some preparatory work with the Tree and
     * @link utils::XmlDocument XmlDocument@endlink before the actual tree writing begins.
     *
     * This is for example useful if a certain kind of value for the nodes depends on other
     * nodes. Using this function, such data can be collected and then used when writing the nodes.
     */
    using prepare_writing_function = std::function< void(
        Tree const& tree, utils::XmlDocument& xml
    ) >;

    /**
     * @brief Function type that allows to do some finalizing work with the Tree and
     * @link utils::XmlDocument XmlDocument@endlink after the actual tree writing finished.
     *
     * This can for example be used for some cleanup.
     */
    using finish_writing_function = std::function< void(
        Tree const& tree, utils::XmlDocument& xml
    ) >;

    /**
     * @brief Function type that translates from a TreeNode to an
     * @link utils::XmlElement XmlElement@endlink.
     *
     * This is called for each TreeNode while writing the Tree to Phyloxml and is used to
     * transfer data from the node into a suitable representation in the Phyloxml format.
     */
    using node_to_element_function = std::function< void(
        TreeNode const& node, utils::XmlElement&  element
    ) >;

    /**
     * @brief Function type that translates from a TreeEdge to an
     * @link utils::XmlElement XmlElement@endlink.
     *
     * This is called for each TreeEdge while writing the Tree to Phyloxml and is used to
     * transfer data from the edge into a suitable representation in the Phyloxml format.
     */
    using edge_to_element_function = std::function< void(
        TreeEdge const& edge, utils::XmlElement&  element
    ) >;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    PhyloxmlWriter() = default;
    virtual ~PhyloxmlWriter() = default;

    PhyloxmlWriter(PhyloxmlWriter const&) = default;
    PhyloxmlWriter(PhyloxmlWriter&&)      = default;

    PhyloxmlWriter& operator= (PhyloxmlWriter const&) = default;
    PhyloxmlWriter& operator= (PhyloxmlWriter&&)      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    /**
     * @brief Writes the tree to a file in Phyloxml format.
     *
     * If the file cannot be written to, the function throws an exception. Also, by default, if the file
     * already exists, an exception is thrown.
     * See @link utils::Options::allow_file_overwriting( bool ) Options::allow_file_overwriting()@endlink
     * to change this behaviour.
     */
    void        to_file     (const Tree& tree, const std::string filename) const;

    /**
     * @brief Gives a Phyloxml string representation of the tree.
     */
    void        to_string   (const Tree& tree, std::string& ts) const;

    /**
     * @brief Return a Phyloxml string representation of the tree.
     */
    std::string to_string   (const Tree& tree) const;

    /**
     * @brief Store the information of the tree into an Phyloxml-formatted XmlDocument.
     */
    void        to_document (const Tree& tree, utils::XmlDocument& xml) const;

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Collect all functions to be called before starting the actual tree writing.
     */
    std::vector<prepare_writing_function> prepare_writing_plugins;

    /**
     * @brief Collect all functions to be called after finishing the actual tree writing.
     */
    std::vector<finish_writing_function>  finish_writing_plugins;

    /**
    * @brief Collect all functions to be called for each TreeNode in order to translate it to
    * a Phyloxml representation.
    */
    std::vector<node_to_element_function> node_to_element_plugins;

    /**
     * @brief Collect all functions to be called for each TreeEdge in order to translate it to
     * a Phyloxml representation.
     */
    std::vector<edge_to_element_function> edge_to_element_plugins;

};

} // namespace tree
} // namespace genesis

#endif // include guard
