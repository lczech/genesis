#ifndef GENESIS_TREE_PRINTER_COMPACT_H_
#define GENESIS_TREE_PRINTER_COMPACT_H_

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
//     Printer Compact
// =================================================================================================

/**
 * @brief
 */
class PrinterCompact
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    PrinterCompact()  = default;
    ~PrinterCompact() = default;

    PrinterCompact( PrinterCompact const& ) = default;
    PrinterCompact( PrinterCompact&& )      = default;

    PrinterCompact& operator= ( PrinterCompact const& ) = default;
    PrinterCompact& operator= ( PrinterCompact&& )      = default;

    // -------------------------------------------------------------------------
    //     Print
    // -------------------------------------------------------------------------

    template <typename TreeType>
    void print (
        std::ostream&   out,
        TreeType const& tree,
        std::function<std::string (typename TreeType::ConstIteratorPreorder& it)> const print_line
    );

    template <typename TreeType>
    std::string print (
        TreeType const& tree,
        std::function<std::string (typename TreeType::ConstIteratorPreorder& it)> const print_line
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
#include "tree/printer/compact.tpp"

#endif // include guard
