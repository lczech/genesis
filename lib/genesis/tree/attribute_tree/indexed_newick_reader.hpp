#ifndef GENESIS_TREE_ATTRIBUTE_TREE_INDEXED_NEWICK_READER_H_
#define GENESIS_TREE_ATTRIBUTE_TREE_INDEXED_NEWICK_READER_H_

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

#include "genesis/tree/attribute_tree/tree.hpp"
#include "genesis/tree/default/newick_reader.hpp"
#include "genesis/tree/formats/newick/element.hpp"
#include "genesis/tree/formats/newick/reader.hpp"

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Indexed Attribute Tree Newick Reader Plugin
// =================================================================================================

/**
 * @brief Provide a set of plugin functions for NewickReader to read ordered attributes of the
 * Nodes and Edges into an #AttributeTree.
 *
 * This class is a plugin that adds functionality to a NewickReader. The easiest way to use it is to
 * instanciate a IndexedAttributeTreeNewickReader, which combines a NewickReader with this plugin.
 *
 * The class is useful for Newick trees that contain a fixed, ordered set of additional data stored
 * for the nodes and edges of the tree, e.g.,
 *
 *     ((C,D)[0.1],(A,(B,X)[0.3])[0.2],E);
 *
 * This represents a Newick tree that contains bootstrap support values at the inner branches, where
 * these data occur always as the first Newick comment for a given node.
 *
 * The plugin allows to process the following types of Newick data:
 *
 *   * Comments, of the form `[0.1]`, e.g., bootstrap values.
 *   * Values, of the form `:3.14`, e.g., branch lengths.
 *   * Tags, of the form `{42}`, e.g., edge numbers using in the `jplace` format.
 *
 * See NewickBrokerElement for more details on those Newick data types.
 *
 * The class offers two ways to process and store these data:
 *
 *   * add_attribute()
 *   * add_catch_all()
 *
 * See there for details.
 *
 * Both ways use Source to select which Newick data to take (comments, values or tags), and use
 * Target to select where to store it in the Tree (Nodes or Edges).
 *
 * Be aware that the latter is an important distinction, as the semantics of the data are not given
 * by Newick. For example, storing bootstrap support values at Nodes insteda of Edges can lead to
 * unexpected and wrong results. You are supposed to know what kind of data the Newick file you are
 * processing contains. See also https://academic.oup.com/mbe/article-lookup/doi/10.1093/molbev/msx055
 *
 * This class is limited to the simple case where the Newick data is always stored in a certain
 * order, i.e., the bootstrap values are always the first comment for a Newick node.
 * Another common Newick data type are key-value-pairs, for example the New Hampshire eXtended (NHX)
 * format. For such Newick trees, see KeyedAttributeTreeNewickReaderPlugin.
 * If you have even more complex Newick data, you need to add your own NewickReader plugin functions.
 */
class IndexedAttributeTreeNewickReaderPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = IndexedAttributeTreeNewickReaderPlugin;

    /**
     * @brief Select which kind of Newick data to take, i.e., either comments, values, or tags.
     *
     * See NewickBrokerElement for more details.
     */
    enum class Source
    {
        /**
         * @brief Take data from Newick values, i.e., `:3.14`.
         */
        kValue,

        /**
         * @brief Take data from Newick comments, i.e., `[something]`.
         */
        kComment,

        /**
         * @brief Take data from Newick tags, i.e., `{42}`.
         */
        kTag
    };

    /**
     * @brief Select where to store the data, i.e., at Nodes or Edges of the Tree.
     */
    enum class Target
    {
        /**
         * @brief Store data at the @link AttributeTreeNodeData::attributes attributes@endlink map
         * of an #AttributeTreeNode.
         */
        kNode,

        /**
        * @brief Store data at the @link AttributeTreeEdgeData::attributes attributes@endlink map
        * of an #AttributeTreeEdge.
        */
        kEdge
    };

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    IndexedAttributeTreeNewickReaderPlugin() = default;
    virtual ~IndexedAttributeTreeNewickReaderPlugin() = default;

    IndexedAttributeTreeNewickReaderPlugin(IndexedAttributeTreeNewickReaderPlugin const&) = default;
    IndexedAttributeTreeNewickReaderPlugin(IndexedAttributeTreeNewickReaderPlugin&&)      = default;

    self_type& operator= (IndexedAttributeTreeNewickReaderPlugin const&) = default;
    self_type& operator= (IndexedAttributeTreeNewickReaderPlugin&&)      = default;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Store the Newick data at a given @p index in an #AttributeTreeMap.
     *
     * Consider the exemplary Newick tree
     *
     *     ((C,D)[0.1],(A,(B,X)[0.3])[0.2],E);
     *
     * In this tree, Newick comments are used to store bootstrap support values. The values are
     * always the first Newick comment of a node (if they occur), i.e. the comment with index `0`.
     *
     * To store these values in the @link AttributeTreeEdgeData::attributes attributes@endlink map
     * of the corresponding AttributeTreeEdgeData, we can use:
     *
     *  ~~~{.cpp}
     * IndexedAttributeTreeNewickReader reader;
     * reader.add_attribute(
     *     IndexedAttributeTreeNewickReader::Source::kComment, 0,
     *     IndexedAttributeTreeNewickReader::Target::kEdge,    "bootstrap"
     * );
     * auto tree = reader.from_file( "path/to/tree.newick" );
     * ~~~
     *
     * This stores the Newick comment (such as "[0.1]" in the example) with index `0` (the first
     * one) at the corresponding Edge, using the key "bootstrap".
     *
     * The same can be done for Newick values and tags (see NewickBrokerElement for their
     * description), and we can store them on the Node instead of the Edge.
     *
     * In cases where there is no corresponding index in the Newick element data, no data is added.
     * For example, the Newick tree above does not contain any bootstrap support values for the leaf
     * nodes. That means, the @link NewickBrokerElement::comments comments@endlink of the
     * NewickBrokerElement is empty, so there is no index `0` in them.
     * If you want to use a default value in such cases, use
     * add_attribute( Source, size_t, Target, std::string const&, std::string const& )
     * instead.
     *
     * @param source     Input Newick element to take the data from. One of `kComment`, `kValue` or
     *                   `kTag`.
     * @param index      Input index of the element, e.g., `0`, to select the first comment.
     * @param target     Target Tree element to store the data at. Either `kNode` or `kEdge`.
     * @param target_key Target key to use for storing the data in the #AttributeTreeMap of
     *                   the Node or Edge, e.g., "bootstrap".
     */
    self_type& add_attribute(
        Source             source,
        size_t             index,
        Target             target,
        std::string const& target_key
    );

    /**
     * @brief Store the Newick data at a given @p index in an #AttributeTreeMap, using a default
     * value if there is no data at that index.
     *
     * The function behaves the same as
     * add_attribute( Source, size_t, Target, std::string const& ) in cases where the given
     * index is found at a Newick element.
     *
     * However, in cases where there is no corresponding index in the Newick element data, this
     * function uses a default value in the attribute map, insted of ingoring it.
     *
     * For example, the Newick tree
     *
     *     ((C,D)[0.1],(A,(B,X)[0.3])[0.2],E);
     *
     * does not contain any bootstrap support values for the leaf nodes. That means, the
     * @link NewickBrokerElement::comments comments@endlink of the NewickBrokerElement is empty, so
     * there is no index `0` in them. In this case, the specified default value is used.
     *
     * @param source        Input Newick element to take the data from. One of `kComment`, `kValue`
     *                      or `kTag`.
     * @param index         Input index of the element, e.g., `0`, to select the first comment.
     * @param target        Target Tree element to store the data at. Either `kNode` or `kEdge`.
     * @param target_key    Target key to use for storing the data in the #AttributeTreeMap of
     *                      the Node or Edge, e.g., "bootstrap".
     * @param default_value Default value for Newick elements where there is no corresponding index.
     */
    self_type& add_attribute(
        Source             source,
        size_t             index,
        Target             target,
        std::string const& target_key,
        std::string const& default_value
    );

    /**
     * @brief Store all Newick data of a given source in an #AttributeTreeMap.
     *
     * This function allows to capture all Newick data of a given type (comment, value or tag)
     * and store it at a Tree element (Node or Edge), using a given prefix and a sequential number.
     *
     * For example, given the Newick tree
     *
     *     ((C,D)[inner_A],(A,(B,X)[inner_B])[inner_C],E);
     *
     * we can store all comments at the tree Nodes using
     *
     * ~~~{.cpp}
     * IndexedAttributeTreeNewickReader reader;
     * reader.add_catch_all(
     *     IndexedAttributeTreeNewickReader::Source::kComment,
     *     IndexedAttributeTreeNewickReader::Target::kNode,    "comment_"
     * );
     * auto tree = reader.from_file( "path/to/tree.newick" );
     * ~~~
     *
     * The resulting tree has attributes at the inner nodes, with the key `comment_0` and the values
     * `inner_A`, `inner_B` and `inner_C`, respectively.
     *
     * Remark: This will store all data at either the Nodes or Edges of the Tree. This can lead
     * to problems if some of the data actually belongs to the other element (Edges or Nodes).
     * In these cases, better set the capturing explicitly, using add_attribute().
     *
     * @param source            Input Newick element to take the data from. One of `kComment`,
     *                          `kValue` or `kTag`.
     * @param target            Target Tree element to store the data at. Either `kNode` or `kEdge`.
     * @param target_key_prefix Prefix for the target key, to which a sequential number according
     *                          to the index of the data is appended.
     */
    self_type& add_catch_all(
        Source             source,
        Target             target,
        std::string const& target_key_prefix
    );

    /**
     * @brief Store all Newick data in an #AttributeTreeMap.
     *
     * This is a simplification of add_catch_all(), which adds three catch-alls for
     * the Newick comments, values and tags at once. It uses `comment_`, `value_` and `tag_` as
     * key prefix, respectively. The function is meant for the most general use case, where we want
     * to capture all Newick data and store it in an #AttributeTree.
     *
     * @param target Optional parameter that can be used to specify where to store the data; default
     *               is to store it at the Tree Nodes.
     */
    self_type& add_catch_all( Target target = Target::kNode );

    /**
     * @brief Reset all settings to the default, i.e., delete all attribute settings.
     */
    void clear();

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    void element_to_node( NewickBrokerElement const& element, TreeNode& node ) const;

    void element_to_edge( NewickBrokerElement const& element, TreeEdge& edge ) const;

    void register_with( NewickReader& reader ) const;

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    std::vector<std::string> const& get_attribute_source_(
        NewickBrokerElement const& element,
        Source                     source
    ) const;

    void process_indexed_attributes_(
        NewickBrokerElement const& element,
        AttributeTreeMap& attributes,
        Target target
    ) const;

    void process_catch_all_attributes_(
        NewickBrokerElement const& element,
        AttributeTreeMap& attributes,
        Target target
    ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    struct AttributeDescriptor
    {
        Source      source;
        size_t      index;
        Target      target;
        std::string target_key;
        std::string default_value;
        bool        use_default;
    };

    std::vector<AttributeDescriptor> indexed_attributes_;
    std::vector<AttributeDescriptor> catch_all_attributes_;

};

// =================================================================================================
//     Attribute Tree Newick Reader
// =================================================================================================

/**
 * @brief Read Newick trees with ordered attributes for the Nodes and Edges.
 *
 * This class is a convenience wrapper that combines a NewickReader with an
 * IndexedAttributeTreeNewickReaderPlugin. It is intended to be used for standard use cases, and
 * produces a Tree with AttributeTreeNodeData and AttributeTreeEdgeData at its nodes and edges.
 *
 * It is also possible to register additional plugins on top of this class.
 */
class IndexedAttributeTreeNewickReader
    : public NewickReader
    , public DefaultTreeNewickReaderPlugin
    , public IndexedAttributeTreeNewickReaderPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    IndexedAttributeTreeNewickReader()
    {
        // We first register the default reader, then the placement reader, because the latter
        // overwrites the data creation functions.
        DefaultTreeNewickReaderPlugin::register_with( *this );
        IndexedAttributeTreeNewickReaderPlugin::register_with( *this );
    }
};

} // namespace tree
} // namespace genesis

#endif // include guard
