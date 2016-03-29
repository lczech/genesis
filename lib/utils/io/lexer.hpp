#ifndef GENESIS_UTILS_IO_LEXER_H_
#define GENESIS_UTILS_IO_LEXER_H_

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
 * @brief Provides a basic lexer to process a string and split it into tokens.
 *
 * @file
 * @ingroup utils
 */

#include <string>
#include <vector>

#include "utils/io/lexer_token.hpp"

namespace genesis {
namespace utils {

// =============================================================================
//     Forward Declaration
// =============================================================================

class LexerIterator;

// =============================================================================
//     Lexer
// =============================================================================

/**
 * @brief Basic %Lexer class that provides an easy way of splitting a string into LexerTokens.
 *
 * This class is intended to be a base class that concrete lexers can inherit
 * from in order to get the basic functioning. An instance of this base class is
 * possible, but will only be able to find numbers as well as symbols consisting
 * of consecutive letters. In order to make use of other semantics like
 * comments, strings, operators etc, it has to be derived.
 *
 * When doing so, have a look at GetToken() to learn about how this class works.
 * Also, see set_char_type() for more information on how to change which characters
 * are interpreted as which type of token.
 */
class Lexer
{
public:
    // -------------------------------------------------------------------------
    //     Main Functions
    // -------------------------------------------------------------------------

    virtual ~Lexer() = default;

    // bool from_file   (const std::string& fn);
    bool from_string (const std::string& in);

    bool process_step();
    bool process_all();

    bool validate_brackets() const;
    std::string dump() const;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    /**
     * @brief Determines whether whitespaces are included as tokens.
     */
    bool include_whitespace = false;

    /**
     * @brief Determines whether comments are included as tokens. *
     */
    bool include_comments = false;

    /*
     * @brief If set, comments are stripped from the text before starting
     * the analysis process.
     *
     * This is useful when the specification of the text allows comments to
     * appear anywhere (e.g. Newick trees). If it is not set, comments are
     * only found at the borders between tokens, but not within them
     * (for example, within a number).
     */
    // TODO make the option strip_comments available
    // one way to do so is to scan including comments, and after the scanning,
    // loop over the result, and merge each comment (and adjacent comments also)
    // with its surrounding tokens (one to the left and one to the right) into
    // one new token (probably mostly with the same type as the left token).
    // e.g.:     some_token comment_a comment_b other_token
    // becomes:   some_tokenother_token
    //~ bool strip_comments = false;

    /**
     * @brief Determines whether to glue a sign to a number following it.
     *
     * If disabled, a term like `1+2=3` will be parsed into single tokes
     * for each character:
     *
     *     "1" "+" "2" "=" "3"
     *
     * If enabled, signs that preceed a number will be glued to that number,
     * so that a term like `items [1.0, -3.14]` will result in
     *
     *     "items" "[" "1.0" "," "-3.14" "]"
     *
     * This is useful when the input is a list or similar data. As this case
     * is more common in bioinformatics, this is the default.
     */
    bool glue_sign_to_number = true;

    /**
     * @brief Determines whether the quotation marks shall be included
     * when a literal string is found.
     *
     * Strings are usually enclosed in 'abc' or "def", see scan_string() for more
     * details on that. The value of trim_quotation_marks determines
     * whether those marks are included in the final token or not.
     * Default is to not include them, which makes preprocessing of the
     * string easier.
     */
    bool trim_quotation_marks = true;

    /**
     * @brief Determines whether to use escape sequences for literal strings.
     *
     * If set to true, and escape sequence starting with a backslash will be
     * treated specially within a literal string: The character after the
     * backslash will be de-escaped using StringDeescape(), meaning that for
     * example an escaped quotation mark will not end the string but be included
     * literally in the result. Also, sequences like \\n will turn into a new
     * line and so on.
     *
     * This only affects literal strings, typically enclosed in 'abc' or
     * "def". See scan_string() for more.
     */
    bool use_string_escape = false;

    /**
     * @brief If set, doubled quotation marks in a string are considered
     * as normal quotation marks without ending the string.
     *
     * For example, the character sequence (including all quotation marks)
     *
     *     "For learning C++, ""Hello World"" is a good start."
     *
     * will be interpreted as a string containing normal quotation marks
     * around `"Hello World"`.
     *
     * The type of quotation marks used here depends on which chars are set
     * to LexerTokenType kString using set_char_type(). See scan_string() for more.
     */
    bool use_string_doubled_quotes = false;

    // -------------------------------------------------------------------------
    //     Accessors and Iterator
    // -------------------------------------------------------------------------

    friend LexerIterator;

    typedef LexerIterator iterator;

    virtual iterator begin();
    virtual iterator end();

    /**
     * @brief Returns a reference to the last token.
     *
     * Calling this function on an empty() lexer causes undefined behavior.
     */
    inline LexerToken back() const
    {
        return tokens_.back();
    }

    /**
     * @brief Returns whether the list of tokens is empty.
     *
     * This is usually the case before ProcessString() was run.
     */
    inline bool empty() const
    {
        return tokens_.empty();
    }

    /**
     * @brief Returns the number of tokens produced during the analysis process.
     */
    inline size_t size() const
    {
        return tokens_.size();
    }

    /**
     * @brief Clears all tokens, as if the object was newly created.
     *
     * The options of the lexer are however not changed.
     */
    inline void clear()
    {
        std::vector<LexerToken>().swap(tokens_);
    }

    /** @brief Returns whether there appeared an error while lexing. */
    inline bool has_error() const
    {
        return !tokens_.empty() && tokens_.back().is_error();
    }

protected:
    // -------------------------------------------------------------------------
    //     Internal functions
    // -------------------------------------------------------------------------

    bool evaluate_from_to (const char* from, const char* to);

    virtual bool scan_unknown();
    virtual bool scan_whitespace();
    virtual bool scan_comment();
    virtual bool scan_symbol();
    virtual bool scan_number();
    virtual bool scan_string();
    virtual bool scan_operator();
    virtual bool scan_bracket();
    virtual bool scan_tag();

    /** @brief Returns the current iterator position while lexing. */
    inline size_t get_position() const
    {
        return itr_;
    }

    /**
     * @brief Returns the char at the iterator position plus an offset.
     *
     * If the position is not inside the text, a null char is returned.
     *
     * It can be used for a save version to get the current char by using
     * `get_char(0);` instead of `get_char();`.
     */
    inline char get_char(const int offset) const
    {
        int pos = itr_ + offset;
        if (pos < 0 || static_cast<size_t>(pos) >= len_) {
            return '\0';
        } else {
            return text_[pos];
        }
    }

    /**
     * @brief Returns the char at the current iterator position.
     *
     * Caveat: The function does no boundary check for speed reasons. Thus,
     * it should be used in combination with is_end only.
     */
    inline char get_char() const
    {
        return text_[itr_];
    }

    /**
     * @brief Returns the LexerTokenType of a char.
     *
     * This does not mean that any char of a given type can only appear in
     * tokens of that type. For example, typically a symbol can start with
     * letters, but then contain numbers later, too. Thus, although a digit
     * is of type kNumber, it can end up in a symbol token, depending on the
     * context.
     *
     * For more information on how this char type is used, see GetToken().
     */
    inline LexerTokenType get_char_type(const char c) const
    {
        // we use char [-128,127] here.
        if (c < 0) {
            return LexerTokenType::kError;
        } else {
            return start_char_table_[static_cast<unsigned char>(c)];
        }
    }

    /**
     * @brief Returns the token type of the current char.
     *
     * Caveat: The function does not do a boundary check to see if the
     * current char in inside the text. Thus, the function should only be
     * used in combination with is_end.
     */
    inline LexerTokenType get_char_type() const
    {
        return get_char_type(get_char());
    }

    /** @brief Sets the token type for a set of characters.
     *
     * This function takes a token type and a list of characters in form of a
     * string and sets the char type for each of them to the given type.
     * This type will be used by the standard implementation of GetToken() to
     * determine the correct scanner for a token (see GetToken() for more on that).
     *
     * If this class is derived, the derived constructor will typically this
     * function in order to set the particular chars needed for the concrete
     * lexer to process its text. For example, it might set "[]" as comment
     * chars and so on.
     *
     * Following chars are particularly interesting to consider:
     *
     *     ! " # $ % & ' ( ) * + , - . / : ; < = > ? @ [ \ ] ^ _ ` { | } ~
     *
     * This is a superset of the C graphical characters and contains all ASCII
     * chars that are on a standard keyboard layout. See start_char_table_
     * for their ASCII representation.
     */
    inline void set_char_type (const LexerTokenType type, const std::string& chars)
    {
        for (char c : chars) {
            start_char_table_[static_cast<unsigned char>(c)] = type;
        }
    }

    /**
     * @brief Moves the internal iterator to the next char.
     *
     * While doing so, it checks whether also to increase the line counter.
     * This induces some overhead for most parts of the
     * scanning process, because usually it is sufficient to check for
     * the new line conditions only during whitespace scanning, comments
     * or quotes. However, doing it here has two advantages:
     *   * it takes away the responsibility from the scanners,
     *     making them simpler,
     *   * and thus errors in line counting are completely avoided,
     *     making it more reliable
     * %
     */
    inline void next_char()
    {
        ++itr_;
        ++col_;

        // we use the save version of get_char here, which is equivalant to
        // check for is_end. first, CR, then LF. the second condition ensures
        // not to count a CR+LF as two line increases.
        if ((get_char(0) == '\n' && get_char(-1) != '\r') || (get_char(0) == '\r')) {
            ++line_;
            col_ = 0;
        }
    }

    /**
     * @brief True if the internal iterator is at the end of the text.
     */
    inline bool is_end() const
    {
        return itr_ >= len_;
    }

    /**
     * @brief True if the internal iterator plus some offset is at the
     * end of the text.
     */
    inline bool is_end(int offset) const
    {
        return itr_ + offset >= len_;
    }

    /**
     * @brief Extracts a substring of the text betweeen two positions, end
     * excluded.
     */
    inline std::string get_substr (size_t start, size_t end) const
    {
        if (start<end) {
            return std::string(text_+start, end-start);
        } else {
            return std::string("");
        }
    }

    /**
     * @brief Create a token and push it to the list.
     */
    inline void push_token (const LexerTokenType t, const size_t start, const std::string& value)
    {
        // the column is the one where the token started. start gives this position as absolute position
        // in the string, so sutract it from itr_ to get how many chars we need to go back as compared
        // to the current col_.
        tokens_.emplace_back(t, line_, col_ - (itr_ - start), value);
    }

    /**
     * @brief Create a token and push it to the list.
     */
    inline void push_token (const LexerTokenType t, const size_t start, const size_t end)
    {
        push_token(t, start, get_substr(start, end));
    }

    /**
     * @brief Create a token and push it to the list.
     */
    inline void push_token (const LexerTokenType t)
    {
        push_token(t, itr_, "");
    }

private:
    // -------------------------------------------------------------------------
    //     Member Variables (make up the state of the object)
    // -------------------------------------------------------------------------

    /**
     * @brief This array contains the token types for all chars, in order to
     * determine the correct scanner for the char.
     *
     * See process_step() for more on this.
     *
     * These are the printable special characters in this list:
     *
     *     !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
     *
     * They should be taken care of when deriving a concrete lexer.
     */
    LexerTokenType start_char_table_[128] = {
        /*      */  LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,
        /*      */  LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,
        /*      */  LexerTokenType::kError,     LexerTokenType::kWhite,     LexerTokenType::kWhite,     LexerTokenType::kWhite,
        /*      */  LexerTokenType::kWhite,     LexerTokenType::kWhite,     LexerTokenType::kError,     LexerTokenType::kError,
        /*      */  LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,
        /*      */  LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,
        /*      */  LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,
        /*      */  LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,     LexerTokenType::kError,
        /*  !"# */  LexerTokenType::kWhite,     LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,
        /* $%&' */  LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,
        /* ()*+ */  LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,
        /* ,-./ */  LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,
        /* 0123 */  LexerTokenType::kNumber,    LexerTokenType::kNumber,    LexerTokenType::kNumber,    LexerTokenType::kNumber,
        /* 4567 */  LexerTokenType::kNumber,    LexerTokenType::kNumber,    LexerTokenType::kNumber,    LexerTokenType::kNumber,
        /* 89:; */  LexerTokenType::kNumber,    LexerTokenType::kNumber,    LexerTokenType::kUnknown,   LexerTokenType::kUnknown,
        /* <=>? */  LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,
        /* @ABC */  LexerTokenType::kUnknown,   LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* DEFG */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* HIJK */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* LMNO */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* PQRS */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* TUVW */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* XYZ[ */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kUnknown,
        /* \]^_ */  LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,
        /* `abc */  LexerTokenType::kUnknown,   LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* defg */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* hijk */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* lmno */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* pqrs */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* tuvw */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,
        /* xyz{ */  LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kSymbol,    LexerTokenType::kUnknown,
        /* |}~  */  LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kUnknown,   LexerTokenType::kError
    };

    // Caveat: the following variables are heavily interweaved during a run
    // of the Lexer! They have to stay consistent, otherwise the resulting
    // tokens will contain wrong information.

    /** @brief The text that is being processed. */
    const char* text_ = "\0";

    /** @brief The current position in the text while processing. */
    size_t      itr_  = 0;

    /** @brief The length of the text being processed. */
    size_t      len_  = 0;

    /** @brief The current line in the text while processing. */
    int         line_ = 1;

    /** @brief The current column in the text while processing. */
    int         col_  = 0;

    /**
     * @brief The list of tokens resulting from the analysis process.
     */
    std::vector<LexerToken> tokens_;
};

} // namespace utils
} // namespace genesis

// =============================================================================
//     Lexer Iterator
// =============================================================================

#include "utils/io/lexer_iterator.hpp"

#endif // include guard
