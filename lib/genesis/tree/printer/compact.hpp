#ifndef GENESIS_TREE_PRINTER_COMPACT_H_
#define GENESIS_TREE_PRINTER_COMPACT_H_

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

#include <iosfwd>
#include <functional>
#include <string>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;
class TreeLink;

// =================================================================================================
//     Printer Compact
// =================================================================================================

/**
 * @brief Print a Tree in a compact form, i.e., each node and edge on one line.
 *
 * For example, given the Newick tree
 *
 *     (((A:0.2,B:0.3)C:0.3,(D:0.4,E:0.45,F:0.3,G:0.35)H:0.25)I:0.8,J:0.4,K:0.5)L;
 *
 * and using the print( Tree const& ) function for @link tree::DefaultTree DefaultTrees@endlink,
 * the output is
 *
 *     L: 0.8
 *     ├── I: 0.8
 *     │   ├── C: 0.3
 *     │   │   ├── A: 0.2
 *     │   │   └── B: 0.3
 *     │   └── H: 0.25
 *     │       ├── D: 0.4
 *     │       ├── E: 0.45
 *     │       ├── F: 0.3
 *     │       └── G: 0.35
 *     ├── J: 0.4
 *     └── K: 0.5
 *
 * Using the
 * @link print( Tree const&, std::function< std::string(TreeNode const &node, TreeEdge const &edge)> const ) print()@endlink
 * functions with function parameters, it is also possible to print custom data for each node and
 * edge.
 */
class PrinterCompact
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Default constructor.
     */
    PrinterCompact()  = default;

    /**
    * @brief Default destructor.
    */
    ~PrinterCompact() = default;

    /**
     * @brief Default copy constructor.
     */
    PrinterCompact( PrinterCompact const& ) = default;

    /**
     * @brief Default move constructor.
     */
    PrinterCompact( PrinterCompact&& )      = default;

    /**
     * @brief Deault copy assignment.
     */
    PrinterCompact& operator= ( PrinterCompact const& ) = default;

    /**
     * @brief Default move assignment.
     */
    PrinterCompact& operator= ( PrinterCompact&& )      = default;

    // -------------------------------------------------------------------------
    //     Print
    // -------------------------------------------------------------------------

    /**
     * @brief Print a compact representation of a Tree to an output stream, using a given
     * function for output of the Node and Edge data.
     *
     * The @p print_line function parameter is used to customize the output per printed line:
     * The function takes the current TreeNode and TreeEdge, and returns the desired string
     * representation for that line. See the implementation of print( Tree const& ) for an
     * example.
     */
    void print (
        std::ostream&   out,
        Tree const& tree,
        std::function<std::string (
            TreeNode const& node,
            TreeEdge const& edge
        )> const print_line
    );

    /**
     * @brief Print a compact representation of a Tree to a `std::string`, using a given
     * function for output of the Node and Edge data.
     *
     * See
     * @link print( Tree const&, std::function< std::string(TreeNode const &node, TreeEdge const &edge)> const ) print()@endlink
     * for details on the @p print_line function parameter.
     */
    std::string print (
        Tree const& tree,
        std::function<std::string (
            TreeNode const& node,
            TreeEdge const& edge
        )> const print_line
    );

    /**
     * @brief Print a compact representation of a Tree to a `std::string`, for
     * @link tree::DefaultTree DefaultTrees@endlink.
     *
     * The function casts the node and edge data to DefaultNodeData and DefaultEdgeData, and, if
     * successfull, prints the @ref genesis::tree::DefaultNodeData::name "node name" and
     * @ref genesis::tree::DefaultEdgeData::branch_length "branch length" for each line.
     *
     * If the data is not actually default data, the nodes are printed without any text, i.e.,
     * only the topology of the Tree via lines is shown.
     */
    std::string print ( Tree const& tree );

    // std::string operator() ( Tree const& tree );

};

} // namespace tree
} // namespace genesis

#endif // include guard
