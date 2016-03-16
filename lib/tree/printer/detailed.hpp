#ifndef GENESIS_TREE_PRINTER_DETAILED_H_
#define GENESIS_TREE_PRINTER_DETAILED_H_

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

    template <typename TreeType>
    void print (
        std::ostream&   out,
        TreeType const& tree
    );

    template <typename TreeType>
    std::string print (
        TreeType const& tree
    );

    // template <typename TreeType>
    // std::string operator() ( TreeType const& tree );

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    bool             use_color() const;
    PrinterDetailed& use_color( bool value );

    // -------------------------------------------------------------------------
    //     Member Data
    // -------------------------------------------------------------------------

private:

    bool use_color_ = true;
};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This class contains function templates, so do the inclusion here.
#include "tree/printer/detailed.tpp"

#endif // include guard
