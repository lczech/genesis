#ifndef GENESIS_TREE_FORMATS_NEWICK_WRITER_H_
#define GENESIS_TREE_FORMATS_NEWICK_WRITER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include <string>

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
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

public:

    NewickWriter() = default;
    virtual ~NewickWriter() {}

    NewickWriter(NewickWriter const&) = default;
    NewickWriter(NewickWriter&&)      = default;

    NewickWriter& operator= (NewickWriter const&) = default;
    NewickWriter& operator= (NewickWriter&&)      = default;

    // -------------------------------------------------------------------------
    //     Writing
    // -------------------------------------------------------------------------

public:

    void        to_file   (const Tree& tree, const std::string filename);
    void        to_string (const Tree& tree, std::string& ts);
    std::string to_string (const Tree& tree);

    // -------------------------------------------------------------------------
    //     Virtual Printing Helpers
    // -------------------------------------------------------------------------

protected:

    virtual void prepare_writing( Tree const& tree, NewickBroker& broker );
    virtual void node_to_element( TreeNode const& node, NewickBrokerElement& element );
    virtual void edge_to_element( TreeEdge const& edge, NewickBrokerElement& element );
    virtual void finish_writing( Tree const& tree, NewickBroker& broker );

    // -------------------------------------------------------------------------
    //     Internal Member Functions
    // -------------------------------------------------------------------------

private:

    void tree_to_broker (const Tree& tree, NewickBroker& broker);

};

} // namespace tree
} // namespace genesis

#endif // include guard
