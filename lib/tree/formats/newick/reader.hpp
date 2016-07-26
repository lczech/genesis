#ifndef GENESIS_TREE_FORMATS_NEWICK_READER_H_
#define GENESIS_TREE_FORMATS_NEWICK_READER_H_

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

    bool from_file    (const std::string& filename,    Tree& tree);
    bool from_string  (const std::string& tree_string, Tree& tree);

    bool from_file    (
        const std::string& filename,
        TreeSet& tree_set
    );
    bool from_string  (
        const std::string& tree_string,
        TreeSet& tree_set,
        const std::string& default_name = ""
    );

    bool from_files   (
        const std::vector<std::string>& filenames,
        TreeSet& tree_set
    );

    bool from_strings (
        const std::vector<std::string>& tree_strings,
        TreeSet& tree_set,
        const std::string& default_name = ""
    );

    // -------------------------------------------------------------------------
    //     Virtual Parsing Helpers
    // -------------------------------------------------------------------------

protected:

    virtual void prepare_reading( NewickBroker const& broker, Tree& tree );
    virtual void element_to_node( NewickBrokerElement const& element, TreeNode& node );
    virtual void element_to_edge( NewickBrokerElement const& element, TreeEdge& edge );
    virtual void finish_reading( NewickBroker const& broker, Tree& tree );

    // -------------------------------------------------------------------------
    //     Internal Member Functions
    // -------------------------------------------------------------------------

private:

    void broker_to_tree (NewickBroker const& broker, Tree& tree);

};

} // namespace tree
} // namespace genesis

#endif // include guard
