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

class NewickWriter
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using prepare_writing_function = std::function< void(
        Tree const& tree, NewickBroker& broker
    ) >;

    using finish_writing_function = std::function< void(
        Tree const& tree, NewickBroker& broker
    ) >;

    using node_to_element_function = std::function< void(
        TreeNode const& node, NewickBrokerElement& element
    ) >;

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

    void        to_file   (const Tree& tree, const std::string filename);
    void        to_string (const Tree& tree, std::string& ts);
    std::string to_string (const Tree& tree);

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    std::vector<prepare_writing_function> prepare_writing_plugins;
    std::vector<finish_writing_function>  finish_writing_plugins;

    std::vector<node_to_element_function> node_to_element_plugins;
    std::vector<edge_to_element_function> edge_to_element_plugins;

    // -------------------------------------------------------------------------
    //     Internal Member Functions
    // -------------------------------------------------------------------------

private:

    void tree_to_broker_ (const Tree& tree, NewickBroker& broker);

};

} // namespace tree
} // namespace genesis

#endif // include guard
