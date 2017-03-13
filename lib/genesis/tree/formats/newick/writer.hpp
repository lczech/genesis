#ifndef GENESIS_TREE_FORMATS_NEWICK_WRITER_H_
#define GENESIS_TREE_FORMATS_NEWICK_WRITER_H_

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
namespace tree {

// =================================================================================================
//     Forward declarations
// =================================================================================================

class  Tree;
class  TreeNode;
class  TreeEdge;
class  TreeSet;

class  NewickBroker;
struct NewickBrokerElement;

// =================================================================================================
//     Newick Writer
// =================================================================================================

/**
 * @brief Write a Tree to Newick format.
 *
 * This class supports to write a Tree into a Newick format representation, using
 *
 *   * to_file()
 *   * to_string()
 *
 * It understands the Newick format, but is agnostic of the actual data representation of
 * TreeNode and TreeEdge data. This approach allows to store data in any wanted format.
 * For example, bootstrap values could be stored as either Newick comments ("[0.4]") or as a second
 * "branch length" value (":0.4"), depending on the user's needs.
 *
 * In order to translate data from the Tree into a Newick format representation, a set of plugin
 * functions is used, that need to be set before writing a Tree. Those functions are a form of
 * intermediaries, which take tree data and turn them into the wanted Newick representation.
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
 * For example, the DefaultTreeNewickWriterPlugin is a convenience class that provides such plugin
 * functions. It translates from a #DefaultTree with TreeNode%s that contain names and TreeEdge%s
 * that contain branch lengths into the standard Newick format. Using plugin classes like this
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
 * DefaultTreeNewickWriter, which hides the whole plugin system and allows simple writing
 * of default trees to standard Newick.
 */
class NewickWriter
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Function type that allows to do some preparatory work with the Tree and NewickBroker
     * before the actual tree writing begins.
     *
     * This is for example useful if a certain kind of value for the nodes depends on other
     * nodes. Using this function, such data can be collected and then used when writing the nodes.
     */
    using prepare_writing_function = std::function< void(
        Tree const& tree, NewickBroker& broker
    ) >;

    /**
     * @brief Function type that allows to do some finalizing work with the Tree and NewickBroker
     * after the actual tree writing finished.
     *
     * This can for example be used for some cleanup.
     */
    using finish_writing_function = std::function< void(
        Tree const& tree, NewickBroker& broker
    ) >;

    /**
     * @brief Function type that translates from a TreeNode to a NewickBrokerElement.
     *
     * This is called for each TreeNode while writing the Tree to Newick and is used to
     * transfer data from the node into a suitable representation in the Newick format.
     */
    using node_to_element_function = std::function< void(
        TreeNode const& node, NewickBrokerElement& element
    ) >;

    /**
     * @brief Function type that translates from a TreeEdge to a NewickBrokerElement.
     *
     * This is called for each TreeEdge while writing the Tree to Newick and is used to
     * transfer data from the edge into a suitable representation in the Newick format.
     */
    using edge_to_element_function = std::function< void(
        TreeEdge const& edge, NewickBrokerElement& element
    ) >;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    NewickWriter() = default;
    virtual ~NewickWriter() = default;

    NewickWriter(NewickWriter const&) = default;
    NewickWriter(NewickWriter&&)      = default;

    NewickWriter& operator= (NewickWriter const&) = default;
    NewickWriter& operator= (NewickWriter&&)      = default;

    // -------------------------------------------------------------------------
    //     Writing
    // -------------------------------------------------------------------------

    /**
     * @brief Writes the tree to a file in Newick format.
     *
     * If the file cannot be written to, the function throws an exception. Also, by default, if the file
     * already exists, an exception is thrown.
     * See @link utils::Options::allow_file_overwriting( bool ) Options::allow_file_overwriting()@endlink to
     * change this behaviour.
     */
    void        to_file   (const Tree& tree, const std::string filename) const;

    /**
     * @brief Gives a Newick string representation of the tree.
     */
    void        to_string (const Tree& tree, std::string& ts) const;

    /**
     * @brief Returns a Newick string representation of the tree.
     */
    std::string to_string (const Tree& tree) const;

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
     * a Newick representation.
     */
    std::vector<node_to_element_function> node_to_element_plugins;

    /**
     * @brief Collect all functions to be called for each TreeEdge in order to translate it to
     * a Newick representation.
     */
    std::vector<edge_to_element_function> edge_to_element_plugins;

    // -------------------------------------------------------------------------
    //     Internal Member Functions
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Stores the information of the tree into a NewickBroker object.
     */
    void tree_to_broker_ (const Tree& tree, NewickBroker& broker) const;

};

} // namespace tree
} // namespace genesis

#endif // include guard
