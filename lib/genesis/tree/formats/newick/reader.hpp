#ifndef GENESIS_TREE_FORMATS_NEWICK_READER_H_
#define GENESIS_TREE_FORMATS_NEWICK_READER_H_

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
//     Newick Reader
// =================================================================================================

class NewickReader
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using prepare_reading_function = std::function< void(
        NewickBroker const& broker, Tree& tree
    ) >;

    using finish_reading_function = std::function< void(
        NewickBroker const& broker, Tree& tree
    ) >;

    using create_node_data_function = std::function< void( TreeNode& node ) >;

    using create_edge_data_function = std::function< void( TreeEdge& edge ) >;

    using element_to_node_function = std::function< void(
        NewickBrokerElement const& element, TreeNode& node
    ) >;

    using element_to_edge_function = std::function< void(
        NewickBrokerElement const& element, TreeEdge& edge
    ) >;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    NewickReader() = default;
    virtual ~NewickReader() = default;

    NewickReader(NewickReader const&) = default;
    NewickReader(NewickReader&&)      = default;

    NewickReader& operator= (NewickReader const&) = default;
    NewickReader& operator= (NewickReader&&)      = default;

    // -------------------------------------------------------------------------
    //     Reading
    // -------------------------------------------------------------------------

    bool from_file    (const std::string& filename,    Tree& tree) const;
    bool from_string  (const std::string& tree_string, Tree& tree) const;

    bool from_file    (
        const std::string& filename,
        TreeSet& tree_set
    ) const;

    bool from_string  (
        const std::string& tree_string,
        TreeSet& tree_set,
        const std::string& default_name = ""
    ) const;

    bool from_files   (
        const std::vector<std::string>& filenames,
        TreeSet& tree_set
    ) const;

    bool from_strings (
        const std::vector<std::string>& tree_strings,
        TreeSet& tree_set,
        const std::string& default_name = ""
    ) const;

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
    //     Internal Member Functions
    // -------------------------------------------------------------------------

private:

    void broker_to_tree_ (NewickBroker const& broker, Tree& tree) const;

};

} // namespace tree
} // namespace genesis

#endif // include guard
