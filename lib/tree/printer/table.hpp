#ifndef GENESIS_TREE_PRINTER_TABLE_H_
#define GENESIS_TREE_PRINTER_TABLE_H_

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
//     Printer Table
// =================================================================================================

/**
 * @brief Print lists of all nodes, edges and links including their indices and connections
 * with each other.
 */
class PrinterTable
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    PrinterTable()  = default;
    ~PrinterTable() = default;

    PrinterTable( PrinterTable const& ) = default;
    PrinterTable( PrinterTable&& )      = default;

    PrinterTable& operator= ( PrinterTable const& ) = default;
    PrinterTable& operator= ( PrinterTable&& )      = default;

    // -------------------------------------------------------------------------
    //     Print
    // -------------------------------------------------------------------------

    void        print( std::ostream& out, Tree const& tree );
    std::string print( Tree const& tree );

    // std::string operator() ( Tree const& tree );

};

} // namespace tree
} // namespace genesis

#endif // include guard
