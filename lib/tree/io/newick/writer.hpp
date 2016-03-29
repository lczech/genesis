#ifndef GENESIS_TREE_IO_NEWICK_WRITER_H_
#define GENESIS_TREE_IO_NEWICK_WRITER_H_

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

template <class TreeType>
class  TreeSet;

class  NewickBroker;
struct NewickBrokerElement;

// =================================================================================================
//     Newick Writer
// =================================================================================================

template <typename TreeType_>
class NewickWriter
{

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    typedef TreeType_ TreeType;
    typedef typename TreeType::NodeType NodeType;
    typedef typename TreeType::EdgeType EdgeType;
    typedef typename TreeType::LinkType LinkType;

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

    void        to_file   (const TreeType& tree, const std::string filename);
    void        to_string (const TreeType& tree, std::string& ts);
    std::string to_string (const TreeType& tree);

    // -------------------------------------------------------------------------
    //     Virtual Printing Helpers
    // -------------------------------------------------------------------------

protected:

    virtual void prepare_writing( TreeType const& tree, NewickBroker& broker );
    virtual void node_to_element( NodeType const& node, NewickBrokerElement& element );
    virtual void edge_to_element( EdgeType const& edge, NewickBrokerElement& element );
    virtual void finish_writing( TreeType const& tree, NewickBroker& broker );

    // -------------------------------------------------------------------------
    //     Internal Member Functions
    // -------------------------------------------------------------------------

private:

    void tree_to_broker (const TreeType& tree, NewickBroker& broker);

};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/io/newick/writer.tpp"

#endif // include guard
