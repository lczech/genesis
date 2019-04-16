#ifndef GENESIS_TREE_FORMATS_NEWICK_READER_H_
#define GENESIS_TREE_FORMATS_NEWICK_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/io/input_source.hpp"

#include <iosfwd>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward declarations
// =================================================================================================

namespace utils {
    class InputStream;
}

namespace tree {

class  Tree;
class  TreeNode;
class  TreeEdge;
class  TreeLink;
class  TreeSet;

class  NewickBroker;
struct NewickBrokerElement;

// =================================================================================================
//     Newick Reader
// =================================================================================================

class NewickReader
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Function type that allows to do some preparatory work with the NewickBroker and Tree
     * before the actual tree reading begins.
     *
     * This is for example useful if a certain kind of value for the nodes depends on other nodes.
     * Using this function, such data can be collected and then used when filling the nodes with data.
     */
    using prepare_reading_function = std::function< void(
        NewickBroker const& broker, Tree& tree
    ) >;

    /**
     * @brief Function type that allows to do some finalizing work with the Tree
     * after the actual tree reading finished.
     *
     * This can for example be used for some cleanup.
     */
    using finish_reading_function = std::function< void(
        Tree& tree
    ) >;

    /**
     * @brief Function type used to create the data pointer for each TreeNode.
     *
     * This function is called for each TreeNode in order to create a data pointer.
     * The type of this pointer is usually the most derived data class that is needed to store
     * the data of the tree. For example, see CommonNodeData for such a data type, and
     * CommonTreeNewickReaderPlugin for a class that uses a fitting function to create this data
     * type.
     */
    using create_node_data_function = std::function< void( TreeNode& node ) >;

    /**
     * @brief Function type used to create the data pointer for each TreeEdge.
     *
     * This function is called for each TreeEdge in order to create a data pointer.
     * The type of this pointer is usually the most derived data class that is needed to store
     * the data of the tree. For example, see CommonEdgeData for such a data type, and
     * CommonTreeNewickReaderPlugin for a class that uses a fitting function to create this data
     * type.
     */
    using create_edge_data_function = std::function< void( TreeEdge& edge ) >;

    /**
     * @brief Function type that translates from a NewickBrokerElement to a TreeNode.
     *
     * This is called for each TreeNode while reading the Tree and is used to
     * transfer data from a representation in the Newick format into the TreeNode.
     */
    using element_to_node_function = std::function< void(
        NewickBrokerElement const& element, TreeNode& node
    ) >;

    /**
     * @brief Function type that translates from a NewickBrokerElement to a TreeEdge.
     *
     * This is called for each TreeEdge while reading the Tree and is used to
     * transfer data from a representation in the Newick format into the TreeEdge.
     */
    using element_to_edge_function = std::function< void(
        NewickBrokerElement const& element, TreeEdge& edge
    ) >;

private:

    enum class TokenType
    {
        kUnknown,
        kOpeningParenthesis,
        kClosingParenthesis,
        kComma,
        kSemicolon,
        kEquals,
        kComment,
        kValue,
        kTag,
        kString,
        kEnd
    };

    struct Token
    {
        TokenType   type = TokenType::kEnd;
        std::string text;
        size_t      line;
        size_t      column;

        std::string at() const
        {
            return std::to_string( line ) + ":" + std::to_string( column );
        }
    };

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

public:

    NewickReader() = default;
    virtual ~NewickReader() = default;

    NewickReader(NewickReader const&) = default;
    NewickReader(NewickReader&&)      = default;

    NewickReader& operator= (NewickReader const&) = default;
    NewickReader& operator= (NewickReader&&)      = default;

    // -------------------------------------------------------------------------
    //     Reading
    // -------------------------------------------------------------------------

    /**
     * @brief Read a single Tree from an input source containing a Newick tree.
     *
     * Use functions such as utils::from_file() and utils::from_string() to conveniently
     * get an input source that can be used here.
     */
    Tree read( std::shared_ptr<utils::BaseInputSource> source ) const;

    /**
     * @brief Add Tree%s to a TreeSet from an input source containing a list of Newick trees.
     *
     * These trees can either be named or unnamed, using this syntax:
     *
     *     Tree_A = (...);
     *     'Tree B'=(...);
     *     (...);
     *
     * where the first two lines are named trees and the third line is an unnamed tree.
     * The trees do not have to be on distinct lines of the input, as whitespaces are completely
     * stripped anyway. However, they are required to end with a semicolon `;`.
     *
     * In case of unnamed trees, a `default_name` can be provided, which will be appended by a counter
     * that counts up all unnamed trees. For example, `default_name == "tree_"` results in trees
     * named "tree_0", "tree_1" etc. If no default name is given, the trees will simply be named
     * using the counter itself.
     *
     * The Trees are added to the TreeSet. That means, Trees that already exist in the
     * TreeSet are kept. Thus, this function can be used to add additional Trees to the set.
     *
     * Use functions such as utils::from_file() and utils::from_string() to conveniently
     * get an input source that can be used here.
     */
    void read(
        std::shared_ptr<utils::BaseInputSource> source,
        TreeSet& target,
        std::string const& default_name = ""
    ) const;

    /**
     * @brief Fill a TreeSet from a list of input sources containing Newick trees.
     *
     * See @link read( std::shared_ptr<utils::BaseInputSource>, TreeSet&, std::string const& ) const read()@endlink
     * for details.
     */
    void read(
        std::vector<std::shared_ptr<utils::BaseInputSource>> sources,
        TreeSet& target,
        std::string const& default_name = ""
    ) const;

    /**
     * @brief Return a TreeSet from a list of input sources containing Newick trees.
     *
     * See @link read( std::shared_ptr<utils::BaseInputSource>, TreeSet&, std::string const& ) const read()@endlink
     * for details.
     */
    TreeSet read(
        std::vector<std::shared_ptr<utils::BaseInputSource>> sources,
        std::string const& default_name = ""
    ) const;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Set whether Newick tags are enabled for reading.
     *
     * Newick tags are an inofficial extension to the Newick format. They consist of values in curly
     * braces, for example `{value}` and can occur where node labels are allowed in Newick.
     *
     * For example:
     *
     *     (( A{0}, B{1} )D{3}, C{4} )R{5};
     *
     * Thus, they are used similarly to the way Newick comments are often (mis-)used to annotate a
     * tree with additional information about the nodes and edges.
     *
     * They are for example used in the `jplace` format, see placement::JplaceReader for details.
     *
     * If this option is set to `true`, such tags are stored in NewickBrokerElement::tags. If it is
     * `false` (default), any string that has the form of Newick tags will simply be treated as part
     * of the node name.
     */
    NewickReader& enable_tags( bool value );

    /**
     * @brief Return whether currently Newick tags are enabled.
     *
     * See enable_tags( bool ) for details.
     */
    bool enable_tags() const;

    /**
     * @brief Set whether reading a single tree stops after the semicolon that finishes a Newick
     * tree.
     *
     * When reading a single Newick tree, it is possible that there is input after the semicolon.
     * If this input is just Newick comments, this is allowed. However, other input might indicate
     * an error in the tree. Using this function, the behaviour of the reading can be controlled.
     *
     * If set to `false` (default), reading continues until the end of the input is reached. This
     * is mostly wanted when reading e.g., a complete file. If then input contains invalid data
     * (non-comments) after the semicolon, an execption is thrown.
     *
     * If set to `true`, reading stops after the semicolon. This is useful if the Newick tree is
     * part of some other file, e.g., Nexus. In this case, we simply want to stop and continue
     * parsing the rest of the input as Nexus data.
     */
    NewickReader& stop_after_semicolon( bool value );

    /**
     * @brief Return whether currently reading stops after the  semicolon that finishes a Newick
     * tree.
     *
     * See stop_after_semicolon( bool ) for details.
     */
    bool stop_after_semicolon() const;

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    std::vector<prepare_reading_function> prepare_reading_plugins;
    std::vector<finish_reading_function>  finish_reading_plugins;

    create_node_data_function create_node_data_plugin;
    create_edge_data_function create_edge_data_plugin;

    std::vector<element_to_node_function> element_to_node_plugins;
    std::vector<element_to_edge_function> element_to_edge_plugins;

    // -------------------------------------------------------------------------
    //     Parsing Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Parse a single tree. Depending on stop_after_semicolon(), stop after the semicolon
     * or continue until the end of the input, checking if there are only comments.
     */
    Tree parse_single_tree( utils::InputStream& input_stream ) const;

    /**
     * @brief Parse until the end of the stream and add all Tree%s to the TreeSet.
     */
    void parse_multiple_trees(
        utils::InputStream& input_stream,
        TreeSet&            tree_set,
        std::string const&  default_name
    ) const;

    /**
     * @brief Parse one named tree, i.e., a tree as described
     * @link read( std::shared_ptr<utils::BaseInputSource>, TreeSet&, std::string const& ) const here@endlink.
     */
    std::pair< std::string, Tree > parse_named_tree( utils::InputStream& input_stream ) const;

    /**
     * @brief Build a Tree from a NewickBroker.
     *
     * This is mainly an internal function, but can be used by other functions that have
     * a NewickBroker and want to convert it into an actual Tree object.
     */
    Tree broker_to_tree( NewickBroker const& broker ) const;

    /**
     * @brief Build a Tree from a NewickBroker.
     *
     * Same as broker_to_tree(), but destroys the NewickBroker while building the Tree.
     * This saves memory (~50%), and is mainly intended as an internal function for reading.
     */
    Tree broker_to_tree_destructive( NewickBroker& broker ) const;

    // -------------------------------------------------------------------------
    //     Internal Member Functions
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Internal function to prepare a Tree for filling it with data from a NewickBroker.
     *
     * Used by broker_to_tree() and broker_to_tree_destructive_().
     */
    void broker_to_tree_prepare_(
        NewickBroker const& broker,
        Tree& tree
    ) const;

    /**
     * @brief Internal function to fill a Tree with data from a NewickBroker.
     *
     * Used by broker_to_tree() and broker_to_tree_destructive_().
     */
    void broker_to_tree_element_(
        NewickBrokerElement const& broker_node,
        std::vector<TreeLink*>& link_stack,
        Tree& tree
    ) const;

    /**
     * @brief Internal function to finish a Tree after filling it with data from a NewickBroker.
     *
     * Used by broker_to_tree() and broker_to_tree_destructive_().
     */
    void broker_to_tree_finish_(
        Tree& tree
    ) const;

    /**
     * @brief Check for input after a semicolon and throw if it is not a comment.
     */
    void parse_trailing_input_( utils::InputStream& input_stream ) const;

    /**
     * @brief Get the next Newick token from the stream. Used by the parsers.
     */
    Token get_next_token_( utils::InputStream& input_stream ) const;

    /**
     * @brief Parse input and build a broker. Stop after the semicolon.
     */
    NewickBroker parse_tree_to_broker_( utils::InputStream& input_stream ) const;

    // -------------------------------------------------------------------------
    //     Member Data
    // -------------------------------------------------------------------------

    bool enable_tags_       = false;
    bool stop_after_semicolon_ = false;

};

} // namespace tree
} // namespace genesis

#endif // include guard
