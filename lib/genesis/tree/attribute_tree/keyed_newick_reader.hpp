#ifndef GENESIS_TREE_ATTRIBUTE_TREE_KEYED_NEWICK_READER_H_
#define GENESIS_TREE_ATTRIBUTE_TREE_KEYED_NEWICK_READER_H_

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
//     Keyed Attribute Tree Newick Reader Plugin
// =================================================================================================

/**
 * @brief Provide a set of plugin functions for NewickReader to read key-value-pair data attributes
 * into an #AttributeTree.
 *
 * This class is a plugin that adds functionality to a NewickReader. The easiest way to use it is to
 * instanciate a KeyedAttributeTreeNewickReader, which combines a NewickReader with this plugin.
 *
 * The class can be used to read Newick trees that contain additional data in Newick comments, if
 * this data is structured into key-value-pairs, e.g.,
 *
 *     ((C,D)[&!color=#009966],(A,(B,X)[&bs=82,!color=#137693])[&bs=70],E);
 *
 * This Newick tree contains a few values that represent branch colors (`color`) and bootstrap
 * support values (`bs`).
 *
 * The class offers two ways to process and store these data:
 *
 *   * add_attribute()
 *   * add_catch_all()
 *
 * Furthermore, it offers simple support for the New Hampshire eXtended (NHX) format, see
 * set_nhx_delimiters() and add_nhx_attributes().
 *
 * By default, the classes uses `&` as the comment prefix, `,` as the separator between
 * key-value-pairs, and `=` as the symbol between the key and the value. This is what the example
 * above uses. In order to change those, use set_delimiters().
 *
 * Remark: This kind of key-value data is only useful in comments. Thus, different from the
 * IndexedAttributeTreeNewickReaderPlugin, this plugin does not support Newick values or tags.
 */
class KeyedAttributeTreeNewickReaderPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = KeyedAttributeTreeNewickReaderPlugin;

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

    KeyedAttributeTreeNewickReaderPlugin() = default;
    virtual ~KeyedAttributeTreeNewickReaderPlugin() = default;

    KeyedAttributeTreeNewickReaderPlugin(KeyedAttributeTreeNewickReaderPlugin const&) = default;
    KeyedAttributeTreeNewickReaderPlugin(KeyedAttributeTreeNewickReaderPlugin&&)      = default;

    self_type& operator= (KeyedAttributeTreeNewickReaderPlugin const&) = default;
    self_type& operator= (KeyedAttributeTreeNewickReaderPlugin&&)      = default;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Store values of a @p key at a @p target (i.e., Node or Edge).
     *
     * For example, using the Newick tree
     *
     *     ((C,D)[&!color=#009966],(A,(B,X)[&bs=82,!color=#137693])[&bs=70],E);
     *
     * we can read the bootstrap support values (`bs`) and store them at the Edges like this:
     *
     * ~~~{.cpp}
     * KeyedAttributeTreeNewickReader reader;
     * reader.add_attribute( "bs", KeyedAttributeTreeNewickReader::Target::kEdge );
     * auto tree = reader.from_file( "path/to/tree.newick" );
     * ~~~
     *
     * This is a simple form which uses the same key for the source (Newick comment) and target
     * (Node or Edge), and does not use a default value, i.e., if the key is not found, no value
     * is added to the target.
     *
     * See @link add_attribute( std::string const&, Target, std::string const&, std::string const& )
     * add_attribute( source_key, target, target_key )@endlink for more details.
     *
     * @param key    Key of a key-value-pair. If found in the source Newick comment, the value is
     *               added to the target Node or Edge, using the same key.
     * @param target Target Tree element to store the data at. Either `kNode` or `kEdge`.
     */
    self_type& add_attribute(
        std::string const& key,
        Target             target
    );

    /**
     * @brief Store values of a @p source_key at a @p target (i.e., Node or Edge), using the
     * @p target_key.
     *
     * Using this form, the source and target key can differ. For example, using the Newick tree
     *
     *     ((C,D)[&!color=#009966],(A,(B,X)[&bs=82,!color=#137693])[&bs=70],E);
     *
     * we can read the color values (`!color`) like this:
     *
     *  ~~~{.cpp}
     * KeyedAttributeTreeNewickReader reader;
     * reader.add_attribute( "!color", KeyedAttributeTreeNewickReader::Target::kEdge, "color" );
     * auto tree = reader.from_file( "path/to/tree.newick" );
     * ~~~
     *
     * which stores the values under the key `color` (without the leading exclamation mark).
     *
     * The function does not use default values. I.e., if the key is not found in the source comment,
     * no value is added to the target.
     *
     * Also, see
     * @link add_attribute( std::string const&, Target, std::string const&, std::string const& )
     * add_attribute( source_key, target, target_key, default_value )@endlink for details.
     *
     * @param source_key Source key of a key-value-pair. If found in the source Newick comment, the
     *                   value is added to the target Node or Edge, using the @p target_key.
     * @param target     Target Tree element to store the data at. Either `kNode` or `kEdge`.
     * @param target_key Target key, used to store a value, if the @p source_key is found in the
     *                   Newick comment.
     */
    self_type& add_attribute(
        std::string const& source_key,
        Target             target,
        std::string const& target_key
    );

    /**
     * @brief Store values of a @p source_key at a @p target (i.e., Node or Edge), using the
     * @p target_key, and a @p default_value, if the key is not found in the source.
     *
     * This is the most flexible form of this function. It is similar to
     * @link add_attribute( std::string const&, Target, std::string const& )
     * add_attribute( source_key, target, target_key )@endlink, but additionally
     * used a default value, if the key is not found in the source Newick comment.
     *
     * @param source_key Source key of a key-value-pair. If found in the source Newick comment, the
     *                   value is added to the target Node or Edge, using the @p target_key.
     * @param target     Target Tree element to store the data at. Either `kNode` or `kEdge`.
     * @param target_key Target key, used to store a value at the target Node or Edge.
     * @param default_value Default value to be used if the source Newick comment does not contain
     *                      the @p source_key.
     */
    self_type& add_attribute(
        std::string const& source_key,
        Target             target,
        std::string const& target_key,
        std::string const& default_value
    );

    /**
     * @brief Store all key-value-pairs of the Newick data in an #AttributeTree.
     *
     * This function allows to capture all Newick comment data in key-value-form and store it
     * at a Tree element (Node or Edge).
     *
     * For example, given the Newick tree
     *
     *     ((C,D)[&!color=#009966],(A,(B,X)[&bs=82,!color=#137693])[&bs=70],E);
     *
     * we can store all data at the tree Edges using
     *
     *  ~~~{.cpp}
     * KeyedAttributeTreeNewickReader reader;
     * reader.add_catch_all( KeyedAttributeTreeNewickReader::Target::kEdge );
     * auto tree = reader.from_file( "path/to/tree.newick" );
     * ~~~
     *
     * Remark: This will store all data at either the Nodes or Edges of the Tree. This can lead
     * to problems if some of the data actually belongs to the other element (Edges or Nodes).
     * In these cases, better set the captureing explicitly, using add_attribute().
     */
    self_type& add_catch_all( Target target = Target::kNode );

    /**
     * @brief Add typical attributes of the NHX format, and set the appropriate delimiters.
     *
     * The New Hampshire eXtended (NHX) format is an extension of the Newick format that uses
     * specially formatted Newick comments to store additional data for the nodes and edges of a
     * tree. See for example https://sites.google.com/site/cmzmasek/home/software/forester/nhx
     * and https://home.cc.umanitoba.ca/~psgendb/doc/atv/NHX.pdf for details.
     *
     * This function adds capturing for the following keys:
     *
     * Key | Target | Description
     * ----|--------|-------------------------------------------------------------------------------
     * AC  | Node   | sequence accession
     * B   | Edge   | confidence value for parent branch
     * Co  | Node   | collapse this node when drawing the tree (default is not to collapse)
     * D   | Node   | duplication event
     * E   | Node   | EC number
     * GN  | Node   | gene name
     * L   | Edge   | log likelihood value on parent branch
     * O   | Node   | orthologous to this external node
     * S   | Node   | species name
     * SO  | Node   | "super orthologous" (no duplications on paths)
     * Sw  | Edge   | placing a subtree on the parent branch of this node makes the tree significantly worse according to Kishino/Hasegawa test (or similar)
     * T   | Node   | taxonomy ID
     *
     * If you need other NHX keys, want to use different keys for the target, or want to use default
     * values for keys that are not present in the Newick data, please use the normal
     * add_attribute() functions or add_catch_all() instead.
     * This is only meant to be a very basic simplification for supporting NHX.
     */
    self_type& add_nhx_attributes();

    /**
     * @brief Set the delimiters to the format used by NHX.
     *
     * These are
     *
     *   * Prefix: `&&NHX`
     *   * Separator: `:`
     *   * Assigner: `=`
     *
     * See add_nhx_attributes() for details on the NHX format.
     */
    self_type& set_nhx_delimiters();

    /**
     * @brief Set the delimiters for key-value-pairs.
     *
     * This function is a shortcut to set the prefix(), separator() and assigner() at once.
     */
    self_type& set_delimiters(
        std::string const& prefix,
        std::string const& separator,
        std::string const& assigner = "="
    ) {
        prefix_    = prefix;
        separator_ = separator;
        assigner_  = assigner;
        return *this;
    }

    /**
     * @brief Set the prefix to look for in Newick comments.
     *
     * Default is `&`, as used in the examples of this class.
     */
    self_type& prefix( std::string const& value )
    {
        prefix_ = value;
        return *this;
    }

    /**
     * @brief Get the currently set prefix to look for in Newick comments.
     */
    std::string prefix() const
    {
        return prefix_;
    }

    /**
     * @brief Set the separator between key-value-pairs.
     *
     * Default is `,`, as used in the examples of this class.
     */
    self_type& separator( std::string const& value )
    {
        separator_ = value;
        return *this;
    }

    /**
     * @brief Get the currently set separator between key-value-pairs.
     */
    std::string separator() const
    {
        return separator_;
    }

    /**
     * @brief Set the assign symbol between a key and its value.
     *
     * Default is `=`, as used in the examples of this class.
     */
    self_type& assigner( std::string const& value )
    {
        assigner_ = value;
        return *this;
    }

    /**
     * @brief Get the currently set assign symbol between a key and its value.
     */
    std::string assigner() const
    {
        return assigner_;
    }

    /**
     * @brief Set whether to trim keys and values before storing them in the Tree.
     *
     * Default is `true`.
     */
    self_type& trim( bool value )
    {
        trim_ = value;
        return *this;
    }

    /**
     * @brief Get whether to trim keys and values before storing them in the Tree.
     */
    bool trim() const
    {
        return trim_;
    }

    /**
     * @brief Reset all settings to the default and delete all attribute settings.
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

    using KeyValuePair = std::pair<std::string, std::string>;
    using PairList     = std::vector<KeyValuePair>;

    /**
     * @brief Helper function that returns whether the given target is currently used.
     *
     * The function checks whether any of `keyed_attributes_` and `catch_all_attributes_` has a
     * target equal to the given @p target. That is, it checks whether we want to place any
     * data on the Nodes or Edges of the tree at all. Used for speedup.
     */
    bool has_attributes_for_target_( Target target ) const;

    /**
     * @brief Helper function that splits the data of a NewickBrokerElement into keys and values.
     */
    PairList get_data_( NewickBrokerElement const& element ) const;

    void process_keyed_attributes_(
        PairList const& data,
        AttributeTreeMap& attributes,
        Target target
    ) const;

    void process_catch_all_attributes_(
        PairList const& data,
        AttributeTreeMap& attributes,
        Target target
    ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    struct AttributeDescriptor
    {
        std::string source_key;
        Target      target;
        std::string target_key;
        std::string default_value;
        bool        use_default;
    };

    std::string prefix_    = "&";
    std::string separator_ = ",";
    std::string assigner_  = "=";

    bool trim_ = true;

    std::vector<AttributeDescriptor> keyed_attributes_;
    std::vector<AttributeDescriptor> catch_all_attributes_;

    static std::vector<AttributeDescriptor> nhx_attributes_;

};

// =================================================================================================
//     Attribute Tree Newick Reader
// =================================================================================================

/**
 * @brief Read default Newick trees, i.e., trees with names and branch lengths.
 *
 * This class is a convenience wrapper that combines a NewickReader with a
 * KeyedAttributeTreeNewickReaderPlugin. It is intended to be used for standard use cases, and
 * produces a Tree with AttributeTreeNodeData and AttributeTreeEdgeData at its nodes and edges.
 *
 * It is also possible to register additional plugins on top of this class.
 */
class KeyedAttributeTreeNewickReader
    : public NewickReader
    , public DefaultTreeNewickReaderPlugin
    , public KeyedAttributeTreeNewickReaderPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    KeyedAttributeTreeNewickReader()
    {
        // We first register the default reader, then the placement reader, because the latter
        // overwrites the data creation functions.
        DefaultTreeNewickReaderPlugin::register_with( *this );
        KeyedAttributeTreeNewickReaderPlugin::register_with( *this );
    }
};

} // namespace tree
} // namespace genesis

#endif // include guard
