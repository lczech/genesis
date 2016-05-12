#ifndef GENESIS_UTILS_FORMATS_JSON_LEXER_H_
#define GENESIS_UTILS_FORMATS_JSON_LEXER_H_

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
 * @ingroup utils
 */

#include "utils/io/lexer.hpp"

namespace genesis {
namespace utils {

// =============================================================================
//     Json Lexer
// =============================================================================

/**
 * @brief JSON lexer that takes a JSON document string and turns it into a sequence of tokens.
 *
 * See Lexer class for details on how the lexing process works.
 */
class JsonLexer : public Lexer
{
public:
    /** @brief Constructor that sets the basic JSON character types. */
    JsonLexer() {
        // set the special chars for json files
        set_char_type (LexerTokenType::kBracket,  "[]{}");
        set_char_type (LexerTokenType::kOperator, ",:");
        set_char_type (LexerTokenType::kString,   "\"");
        set_char_type (LexerTokenType::kNumber,   "+-");

        // set the flags as needed
        include_whitespace        = false;
        include_comments          = false;
        glue_sign_to_number       = true;
        trim_quotation_marks      = true;
        use_string_escape         = true;
        use_string_doubled_quotes = false;
    }

protected:
    /**
     * @brief Scans a JSON symbol.
     *
     * JSON only knows three different symbols: `null`, `true` and `false`. Every other sequence
     * of chars must be a string enclosed in double quotation marks, which is covered by the
     * ScanString() function. Thus, this function scans for non-enclosed char sequences and checks
     * if it is one of the three. If so, it is returned as a symbol, oterhwise an error token is
     * produced.
     */
    inline bool scan_symbol()
    {
        size_t start = get_position();
        while (!is_end() && get_char_type() == LexerTokenType::kSymbol) {
            next_char();
        }

        std::string res = get_substr(start, get_position());
        if (res.compare("null") && res.compare("true") && res.compare("false")) {
            push_token(LexerTokenType::kError, start, "Invalid symbols: \"" + res + "\"");
            return false;
        }
        push_token(LexerTokenType::kSymbol, start, res);
        return true;
    }
};

} // namespace utils
} // namespace genesis

#endif // include guard
