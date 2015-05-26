#ifndef GENESIS_ALIGNMENT_PHYLIP_PROCESSOR_H_
#define GENESIS_ALIGNMENT_PHYLIP_PROCESSOR_H_

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

class SequenceSet;

// =============================================================================
//     Phylip Lexer
// =============================================================================

class PhylipLexer : public Lexer
{
public:
    PhylipLexer() {
        // we want to include all special chars, in order to allow them in labels.
        // those which do not belong into a sequence are filtered out later.
        SetCharType (LexerTokenType::kSymbol, "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");

        // set the flags as needed.
        include_whitespace        = true;
        include_comments          = false;
        glue_sign_to_number       = false;
        trim_quotation_marks      = false;
        use_string_escape         = false;
        use_string_doubled_quotes = false;
    }
};

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

    static size_t label_length;

    static bool FromFile   (const std::string  fn, SequenceSet& aln);
    static bool FromString (const std::string& fs, SequenceSet& aln);

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    static bool ToFile   (const std::string fn, const SequenceSet& aln);
    static void ToString (std::string& fs,      const SequenceSet& aln);
    static std::string ToString (               const SequenceSet& aln);
};

} // namespace genesis

#endif // include guard
