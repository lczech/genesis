#ifndef GNS_ALIGNMENT_PHYLIPPROCESSOR_H_
#define GNS_ALIGNMENT_PHYLIPPROCESSOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup alignment
 */

#include <assert.h>
#include <string>

#include "utils/lexer.hpp"

namespace genesis {

// =============================================================================
//     Forward declarations
// =============================================================================

class Alignment;

// =============================================================================
//     Phylip Lexer
// =============================================================================

// =============================================================================
//     Phylip Processor
// =============================================================================

/**
 * @brief Processes a Phylip file.
 */
class PhylipProcessor
{
public:

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    static bool ToFile   (const std::string fn, const Alignment& aln);
    static void ToString (std::string& fs,      const Alignment& aln);
    static std::string ToString (               const Alignment& aln);
};

} // namespace genesis

#endif // include guard
