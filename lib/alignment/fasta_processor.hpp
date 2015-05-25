#ifndef GENESIS_ALIGNMENT_FASTAPROCESSOR_H_
#define GENESIS_ALIGNMENT_FASTAPROCESSOR_H_

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
//     Fasta Lexer
// =============================================================================

class FastaLexer : public Lexer
{
public:
    FastaLexer() {
        // whitespaces except new lines (CR and LF) are not part of the fasta format.
        SetCharType (LexerTokenType::kUnknown,  "\x09\x0B\x0C");
        SetCharType (LexerTokenType::kUnknown,  " ");

        // we use a tag for marking the label of a sequence.
        SetCharType (LexerTokenType::kTag,      ">");

        // set the two special fasta symbols for gap and sequence end.
        SetCharType (LexerTokenType::kSymbol,   "-*");

        // comments start with ; and continue until the end of the line.
        SetCharType (LexerTokenType::kComment,  ";");

        // digits are not part of fasta sequences.
        SetCharType (LexerTokenType::kUnknown,   "0123456789");

        // set the flags as needed.
        include_whitespace        = false;
        include_comments          = false;
        glue_sign_to_number       = false;
        trim_quotation_marks      = false;
        use_string_escape         = false;
        use_string_doubled_quotes = false;
    }

protected:

    inline bool ScanComment()
    {
        // semicolon is the only char starting a comment. if found, skip it.
        if (GetChar() != ';') {
            return false;
        }
        NextChar();

        // comments end with a new line. new lines are the only white space, scan until one found.
        size_t start = GetPosition();
        while (!IsEnd() && GetCharType() != LexerTokenType::kWhite) {
            NextChar();
        }
        if (include_comments) {
            PushToken(LexerTokenType::kComment, start, GetPosition());
        }
        return true;
    }

    inline bool ScanTag()
    {
        // the greater sign is the only one used to start a tag. skip it.
        assert(GetChar() == '>');
        NextChar();

        if (IsEnd()) {
            // TODO error
        }

        // scan until we reach a new line (this is the only type of whitespace used in this lexer)
        size_t start = GetPosition();
        while (!IsEnd() && GetCharType() != LexerTokenType::kWhite) {
            NextChar();
        }
        PushToken(LexerTokenType::kTag, start, GetPosition());
        return true;
    }
};

// =============================================================================
//     Fasta Processor
// =============================================================================

/**
 * @brief Processes a FASTA file into an alignment object.
 *
 * Specification:
 *
 *  *  http://en.wikipedia.org/wiki/FASTA_format
 *  *  http://blast.ncbi.nlm.nih.gov/blastcgihelp.shtml
 *  *  http://zhanglab.ccmb.med.umich.edu/FASTA/
 * %
 */
class FastaProcessor
{
public:

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    static bool FromFile   (const std::string  fn, SequenceSet& aln);
    static bool FromString (const std::string& fs, SequenceSet& aln);

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    static size_t line_length;

    static bool ToFile   (const std::string fn, const SequenceSet& aln);
    static void ToString (std::string& fs,      const SequenceSet& aln);
    static std::string ToString (               const SequenceSet& aln);
};

} // namespace genesis

#endif // include guard
