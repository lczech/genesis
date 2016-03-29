#ifndef GENESIS_TREE_IO_NEWICK_READER_H_
#define GENESIS_TREE_IO_NEWICK_READER_H_

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
#include <vector>

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
//     Newick Reader
// =================================================================================================

template <typename TreeType_>
class NewickReader
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

    NewickReader() = default;
    virtual ~NewickReader() {}

    NewickReader(NewickReader const&) = default;
    NewickReader(NewickReader&&)      = default;

    NewickReader& operator= (NewickReader const&) = default;
    NewickReader& operator= (NewickReader&&)      = default;

    // -------------------------------------------------------------------------
    //     Reading
    // -------------------------------------------------------------------------

public:

    bool from_file    (const std::string& filename,    TreeType& tree);
    bool from_string  (const std::string& tree_string, TreeType& tree);

    bool from_file    (
        const std::string& filename,
        TreeSet<TreeType>& tree_set
    );
    bool from_string  (
        const std::string& tree_string,
        TreeSet<TreeType>& tree_set,
        const std::string& default_name = ""
    );

    bool from_files   (
        const std::vector<std::string>& filenames,
        TreeSet<TreeType>& tree_set
    );

    bool from_strings (
        const std::vector<std::string>& tree_strings,
        TreeSet<TreeType>& tree_set,
        const std::string& default_name = ""
    );

    // -------------------------------------------------------------------------
    //     Virtual Parsing Helpers
    // -------------------------------------------------------------------------

protected:

    virtual void prepare_reading( NewickBroker const& broker, TreeType& tree );
    virtual void element_to_node( NewickBrokerElement const& element, NodeType& node );
    virtual void element_to_edge( NewickBrokerElement const& element, EdgeType& edge );
    virtual void finish_reading( NewickBroker const& broker, TreeType& tree );

    // -------------------------------------------------------------------------
    //     Internal Member Functions
    // -------------------------------------------------------------------------

private:

    void broker_to_tree (NewickBroker const& broker, TreeType& tree);

};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/io/newick/reader.tpp"

#endif // include guard
