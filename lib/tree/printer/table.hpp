#ifndef GENESIS_TREE_PRINTER_TABLE_H_
#define GENESIS_TREE_PRINTER_TABLE_H_

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

    template <typename TreeType>
    void print (
        std::ostream&   out,
        TreeType const& tree
    );

    template <typename TreeType>
    std::string print ( TreeType const& tree );

    // template <typename TreeType>
    // std::string operator() ( TreeType const& tree );

};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This class contains function templates, so do the inclusion here.
#include "tree/printer/table.tpp"

#endif // include guard
