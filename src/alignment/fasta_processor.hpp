#ifndef GNS_ALIGNMENT_FASTAPROCESSOR_H_
#define GNS_ALIGNMENT_FASTAPROCESSOR_H_

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
//     Fasta Lexer
// =============================================================================

class FastaLexer : public Lexer
{
public:
    FastaLexer() {
        // set all whitespaces except new lines (CR and LF) to some other token type.
        // TODO make space possible to appear in sequences, eg for GenBank format.
        SetCharType (LexerTokenType::kUnknown,  " ");
        SetCharType (LexerTokenType::kUnknown,  "\x09\x0B\x0C");

        // we use a tag for marking the label of a sequence
        SetCharType (LexerTokenType::kTag,      ">");

        // set the two special fasta symbols for gap and sequence end
        SetCharType (LexerTokenType::kSymbol,   "-*");

        // comments start with ; and continue until the end of the line
        SetCharType (LexerTokenType::kComment,  ";");

        // so far, we do not support digits in sequences.
        // TODO add this, in order to support eg GenBank format!
        SetCharType (LexerTokenType::kUnknown,   "0123456789");

        // furthermore, set all remaining graphic chars to unknown. thus, they cannot appear in
        // sequences (but tags, the are scanned differently).
        SetCharType (LexerTokenType::kUnknown,  "!\"#$%&'()+,./:<=?@[\\]^_`{|}~");

        // set the flags as needed
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

    static bool FromFile   (const std::string  fn, Alignment& aln);
    static bool FromString (const std::string& fs, Alignment& aln);

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    static bool ToFile   (const std::string fn, const Alignment& aln);
    static void ToString (std::string& fs,      const Alignment& aln);
    static std::string ToString (               const Alignment& aln);
};

} // namespace genesis

#endif // include guard
