#ifndef GENESIS_TREE_PRINTER_DETAILED_H_
#define GENESIS_TREE_PRINTER_DETAILED_H_

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
//     Printer Detailed
// =================================================================================================

/**
 * @brief Print a text representation of the Tree, showing all nodes, edges and links
 * with their indices.
 */
class PrinterDetailed
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    PrinterDetailed()  = default;
    ~PrinterDetailed() = default;

    PrinterDetailed( PrinterDetailed const& ) = default;
    PrinterDetailed( PrinterDetailed&& )      = default;

    PrinterDetailed& operator= ( PrinterDetailed const& ) = default;
    PrinterDetailed& operator= ( PrinterDetailed&& )      = default;

    // -------------------------------------------------------------------------
    //     Print
    // -------------------------------------------------------------------------

    void        print( std::ostream& out, Tree const& tree ) const;
    std::string print( Tree const& tree ) const;

    // std::string operator() ( Tree const& tree );

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    bool             use_color() const
    {
        return use_color_;
    }

    PrinterDetailed& use_color( bool value )
    {
        use_color_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Member Data
    // -------------------------------------------------------------------------

private:

    bool use_color_ = true;
};

} // namespace tree
} // namespace genesis

#endif // include guard
