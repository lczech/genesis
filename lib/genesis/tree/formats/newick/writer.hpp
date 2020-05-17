#ifndef GENESIS_TREE_FORMATS_NEWICK_WRITER_H_
#define GENESIS_TREE_FORMATS_NEWICK_WRITER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/io/output_target.hpp"

#include <iosfwd>
#include <functional>
#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
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
 * This class supports to write a Tree into a Newick format representation, for example
 *
 *     Tree tree;
 *     CommonTreeNewickWriter().write( tree, utils::to_file( "path/to/file.newick" ));
 *
 * It understands the Newick format, but is agnostic of the actual data representation of
 * TreeNode and TreeEdge data. This approach allows to store data in any wanted format.
 * For example, bootstrap values could be stored as either Newick comments (`[0.4]`) or as a second
 * "branch length"-like value (`:0.4`), depending on the user's needs.
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
 * For example, the CommonTreeNewickWriterPlugin is a convenience class that provides such plugin
 * functions. It translates from a #CommonTree with TreeNode%s that contain names and TreeEdge%s
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
 * CommonTreeNewickWriter, which hides the whole plugin system and allows simple writing
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
     * @brief Write a Tree to an output target, using the Newick format.
     *
     * See the output target convenience functions utils::to_file(), utils::to_stream(), and
     * utils::to_string() for examples of how to obtain a suitable output target.
     */
    void write( Tree const& tree, std::shared_ptr<utils::BaseOutputTarget> target ) const;

    /**
     * @brief Write all Tree%s in a TreeSet to an output target, using the Newick format.
     *
     * Trees are separated from each other by semicolons and new line characters. If @p with_names
     * is set to `true`, each tree is preprended by its name as stored in the TreeSet, in the format
     *
     *     name = (<newick tree>);
     *
     * See the output target convenience functions utils::to_file(), utils::to_stream(), and
     * utils::to_string() for examples of how to obtain a suitable output target.
     */
    void write( TreeSet const& tree_set, std::shared_ptr<utils::BaseOutputTarget> target, bool with_names = false ) const;

    /**
     * @brief Shorthand to write a Tree to Newick format and return it is a string.
     */
    std::string to_string( Tree const& tree ) const;

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
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Set the type of quotation marks used for node names that contain special characters.
     *
     * According to [http://evolution.genetics.washington.edu/phylip/newicktree.html]
     * (http://evolution.genetics.washington.edu/phylip/newicktree.html),
     *
     *  > "A name can be any string of printable characters except blanks, colons, semicolons,
     *  > parentheses, and square brackets."
     *
     * They forgot to mention commas in that list. One more reason to be suspicious of the Newick
     * format.
     * Anyway, whenever one of these characters (including commas) occurs in the name of a node
     * (see NewickBrokerElement::name), the writer wraps the whole name in quotation markes.
     * This is not officially in the standard, but common practice.
     *
     * Remark: When using a CommonTreeNewickWriterPlugin (or a CommonTreeNewickWriter, as it
     * internally uses the plugin), you can use its setting
     * @link CommonTreeNewickWriterPlugin::replace_name_spaces( bool ) replace_name_spaces()@endlink
     * in order to replace any spaces in node nams into underscores.
     *
     * This function sets the kind of quotation marks used for wrapping such names. Common are
     * double quotation marks ('"'), which seem to be understood by many other programs that work
     * with Newick trees.
     *
     * We currently do not support a function to deactivate quotation marks - they are used whenever
     * necessary, for safety reasons. Otherwise, we'd end up with invalid trees anyway. Thus, in
     * order to get a Newick tree without any quotation marks, make sure that your node names do not
     * contain any of the listed characters.
     *
     * Lastly, if write_tags() is `true`, names with curly braces in them ('{}') are also wrapped
     * in quotation marks, as those are used for tags.
     */
    NewickWriter& quotation_mark( char value )
    {
        quotation_mark_ = value;
        return *this;
    }

    /**
     * @brief Get the currently set type of quotation marks used for node names.
     *
     * See quotation_mark( char ) for details.
     */
    char quotation_mark() const
    {
        return quotation_mark_;
    }

    /**
     * @brief If set to `true`, all names are wrapped in quotation marks, regardless of whether
     * the name contains any characters that need to be wrapped
     *
     * Default is `false`. This setting can be used to ensure that all names have quotation marks,
     * which is a requirement for certain other parsers. See also quotation_mark( char ) to set
     * the type of quotation mark.
     */
    NewickWriter& force_quotation_marks( bool value )
    {
        force_quot_marks_ = value;
        return *this;
    }

    /**
     * @brief Get whether all names are wrapped in quotation marks.
     *
     * @see force_quotation_marks( bool )
     */
    bool force_quotation_marks() const
    {
        return force_quot_marks_;
    }

    /**
     * @brief Set the approximate maximal line length to use when writing Newick trees.
     *
     * Some large trees become hard to inspect in the resulting Newick file if the whole tree is
     * written in just one long line. Also, some tools might not be able to handle such long lines
     * properly. Hence, setting this option to a value other than 0 (default, which means, all
     * is written in one line) leads to the writer inserting line breaks at the next possible
     * character after the @p value line length has been reached. Hence, lines can be longer than
     * the given value. Also, be aware that some other tools might not be able to read trees
     * that are spread across several lines. Newick is messy.
     */
    NewickWriter& line_length( size_t value )
    {
        line_length_ = value;
        return *this;
    }

    /**
     * @brief Get the currently set approximate maximal line length.
     *
     * Default is 0, which means, all is written in one long line.
     *
     * @see line_length( size_t )
     */
    size_t line_length() const
    {
        return line_length_;
    }

    /**
     * @brief Set whether to write a new line char `\n` at the end of each tree.
     *
     * By default, this is set to `true`, meaning that each tree is finished with a `\n` char,
     * which is usually desirable when writing to Newick files or the like. However, in cases
     * where the Newick string itself is used in some other format (e.g., jplace or nexus), this
     * might not be wanted. Hence, we allow to disable the new line.
     */
    NewickWriter& trailing_new_line( bool value )
    {
        trailing_new_line_ = value;
        return *this;
    }

    /**
     * @brief Get the current setting for whether a new line char `\n` is written to the end
     * of each tree.
     *
     * @see trailing_new_line( bool )
     */
    bool trailing_new_line() const
    {
        return trailing_new_line_;
    }

    /**
     * @brief Set whether to write Newick node names.
     *
     * Default is `true`. This setting can be used to override any names that might be set by a
     * plugin.
     */
    NewickWriter& write_names( bool value )
    {
        write_names_ = value;
        return *this;
    }

    /**
     * @brief Get whether Newick node names are written.
     *
     * @see write_names( bool )
     */
    bool write_names() const
    {
        return write_names_;
    }

    /**
     * @brief Set whether to write Newick values (e.g., branch lengths).
     *
     * Default is `true`. This setting can be used to override any values that might be set by a
     * plugin.
     */
    NewickWriter& write_values( bool value )
    {
        write_values_ = value;
        return *this;
    }

    /**
     * @brief Get whether Newick values (e.g., branch lengths) are written.
     *
     * @see write_values( bool )
     */
    bool write_values() const
    {
        return write_values_;
    }

    /**
     * @brief Set whether to write Newick comments (e.g., some forms of bootstrap values).
     *
     * Default is `true`. This setting can be used to override any comments that might be set by a
     * plugin.
     */
    NewickWriter& write_comments( bool value )
    {
        write_comments_ = value;
        return *this;
    }

    /**
     * @brief Get whether Newick comments (e.g., some forms of bootstrap values) are written.
     *
     * @see write_comments( bool )
     */
    bool write_comments() const
    {
        return write_comments_;
    }

    /**
     * @brief Set whether to write Newick tags (e.g., for `jplace` files).
     *
     * Default is `true`. This setting can be used to override any tags that might be set by a
     * plugin. See NewickReader::enable_tags( bool ) for details on the inofficial Newick tags
     * extension.
     */
    NewickWriter& write_tags( bool value )
    {
        write_tags_ = value;
        return *this;
    }

    /**
     * @brief Get whether Newick tags (e.g., for `jplace` files) are written.
     *
     * @see write_tags( bool )
     */
    bool write_tags() const
    {
        return write_tags_;
    }

    // -------------------------------------------------------------------------
    //      Intermediate Functions
    // -------------------------------------------------------------------------

    /**
    * @brief Transform the information of the tree into a NewickBroker object.
    */
    NewickBroker tree_to_broker( Tree const& tree ) const;

    /**
     * @brief Write a NewickBroker to an output target, in Newick format.
     */
    void write( NewickBroker const& broker, std::shared_ptr<utils::BaseOutputTarget> target ) const;

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Write the Newick text string representation of a NewickBrokerElement to an output target.
     *
     * Returns the number of characters that have been written.
     */
    size_t write_( NewickBrokerElement const& bn, std::ostream& os ) const;

    /**
     * @brief Recursive function that returns the string representation of a clade of a tree.
     */
    // std::string to_string_rec_( NewickBroker const& broker, size_t pos ) const;

    // -------------------------------------------------------------------------
    //     Member Data
    // -------------------------------------------------------------------------

    bool force_quot_marks_ = false;
    char quotation_mark_  = '\"';
    size_t line_length_ = 0;
    bool trailing_new_line_ = true;

    bool write_names_    = true;
    bool write_values_   = true;
    bool write_comments_ = true;
    bool write_tags_     = true;

};

} // namespace tree
} // namespace genesis

#endif // include guard
