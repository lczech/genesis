#ifndef GENESIS_UTILS_IO_LEXER_TOKEN_H_
#define GENESIS_UTILS_IO_LEXER_TOKEN_H_

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

#include <string>

namespace genesis {
namespace utils {

// =============================================================================
//     General Definitions
// =============================================================================

/**
 * @brief Enum for the different types of LexerToken.
 */
enum class LexerTokenType {
    kError,
    kUnknown,
    kWhite,
    kComment,
    kSymbol,
    kNumber,
    kString,
    kBracket,
    kOperator,
    kTag,
    kEOF
};

/**
 * @brief Converts a LexerTokenType into its string representation.
 */
inline std::string lexer_token_type_to_string (const LexerTokenType t)
{
    switch (t) {
        case LexerTokenType::kError    : return "Error";
        case LexerTokenType::kUnknown  : return "Unknown";

        case LexerTokenType::kWhite    : return "Whitespace";
        case LexerTokenType::kComment  : return "Comment";
        case LexerTokenType::kSymbol   : return "Symbol";
        case LexerTokenType::kNumber   : return "Number";
        case LexerTokenType::kString   : return "String";
        case LexerTokenType::kBracket  : return "Bracket";
        case LexerTokenType::kOperator : return "Operator";
        case LexerTokenType::kTag      : return "Tag";

        case LexerTokenType::kEOF      : return "EOF";
        default                        : return "Unknown";
    }
}

// =============================================================================
//     Lexer Token
// =============================================================================

/**
 * @brief POD struct that represents a token that is outputted by the Lexer.
 *
 * The main types of tokens are:
 *
 * 1. **Symbol**: A named symbol, usually starts with a letter or underscore,
 *    followed by any number of letters, digits or underscores.
 *
 * 2. **Number**: A number in the format
 *
 *        [+-]123[.456][eE[+-]789].
 *
 * 3. **String**: A literal string, usually enclosed in either 'abc' or "def".
 *
 *    It can contain escape charaters using a backslash, where \\n, \\t and \\r
 *    are translated into their whitespace representation using
 *    StringDeescape() when the option Lexer::use_string_escape is set.
 *
 *    Also, see Lexer::trim_quotation_marks and Lexer::use_string_doubled_quotes
 *
 * 4. **Bracket**: Typically one of these brackets
 *
 *        ( ) [ ] { } < >
 *
 *    This is always a single bracket per token.
 *
 * 5. **Operator**: An operator or sequence of them, typically out of the set
 *
 *        + - * / < > ? ! ^ = % & | , : ;
 *
 * 6. **Tag**: A token denoting a tag, e.g. `<>` for xml. In such cases, it
 *    might be neccessary to to an extra lexing step for the inner part,
 *    using a different lexer class specialized for that purpose.
 *
 * Furthermore, there are token types marking whitespaces and comments, which
 * are included when the corresponding options are set in the Lexer class.
 *
 * The distinction between the types is arbitrary. It simply is the naming for
 * for most typical situations encountered in lexing. The distinction is meant
 * to help writing better parsers with higher semantic readability. Any deriving
 * class can assign completely different meaning to the names by implementing
 * a different behaviour (although that is not recommended).
 *
 * Most importantly, as the distinction between types is only on a semantic
 * level, it does not affect the type of the stored data. All token values are
 * stored as strings -- upstream analysis like parsers then have to convert it
 * to a proper type for further use (e.g. in case of numbers).
 *
 * If there is need for more types in the future, the enum, the default
 * implementations of the Lexer functions and some other places have to be adapted
 * accordingly.
 *
 * In case of an error while processing the input, an error token is produced
 * which contains the location of the error.
 */
struct LexerToken
{
public:
    // -------------------------------------------------------------------------
    //     Construction and Member Values
    // -------------------------------------------------------------------------

    /**
     * @brief Constructor that sets the values for this token.
     */
    inline LexerToken
    (
        const LexerTokenType type,   const int         line,
        const int            column, const std::string& value
    ) :
        type_(type), line_(line), column_(column), value_(value)
    {};

    /**
     * @brief Getter for the LexerTokenType of this token.
     */
    inline LexerTokenType type() const
    {
        return type_;
    }

    /**
     * @brief Getter for the line where this token occured.
     */
    inline size_t line() const
    {
        return line_;
    }

    /**
     * @brief Getter for the column where this token occured.
     */
    inline size_t column() const
    {
        return column_;
    }

    /**
     * @brief Getter for the string value of this token.
     */
    inline std::string value() const
    {
        return value_;
    }

    /**
     *  @brief Shortcut that returns "line:column" (e.g., for logging).
     */
    inline std::string at() const
    {
        return std::to_string(line_) + ":" + std::to_string(column_);
    }

    // -------------------------------------------------------------------------
    //     Shortcut Functions
    // -------------------------------------------------------------------------

    /**
     * @brief Shortcut to check if this is an error token.
     */
    inline bool is_error() const
    {
        return type_ == LexerTokenType::kError;
    }

    /**
     * @brief Shortcut to check if this is an unknown token.
     */
    inline bool is_unknown() const
    {
        return type_ == LexerTokenType::kUnknown;
    }

    /**
     * @brief Shortcut to check if this is a whitespace token.
     */
    inline bool is_white() const
    {
        return type_ == LexerTokenType::kWhite;
    }

    /**
     * @brief Returns the number of new line characters, if this token is a whitespace.
     */
    inline size_t has_new_lines() const
    {
        // check if this is actually a whitespace token.
        if (type_ != LexerTokenType::kWhite) {
            return 0;
        }

        // count occurences of CR or LF, while not counting a CR+LF twice.
        size_t cnt = 0;
        for (size_t i = 0; i < value_.size(); ++i) {
            char c = value_[i];
            if ((c == '\r') || (c == '\n' && i == 0) || (c == '\n' && value_[i-1] != '\r')) {
                ++cnt;
            }
        }
        return cnt;
    }

    /**
     * @brief Shortcut to check if this is a comment token.
     */
    inline bool is_comment() const
    {
        return type_ == LexerTokenType::kComment;
    }

    /**
     * @brief Shortcut to check if this is a symbol token.
     */
    inline bool is_symbol() const
    {
        return type_ == LexerTokenType::kSymbol;
    }

    /**
     * @brief Shortcut to check if this is a number token.
     */
    inline bool is_number() const
    {
        return type_ == LexerTokenType::kNumber;
    }

    /**
     * @brief Shortcut to check if this is a string token.
     */
    inline bool is_string() const
    {
        return type_ == LexerTokenType::kString;
    }

    /**
     * @brief Shortcut to check if this is a bracket token.
     *
     * There is an overload of this function that also checks if the token is a specific type of
     * bracket.
     */
    inline bool is_bracket() const
    {
        return type_ == LexerTokenType::kBracket;
    }

    /**
     * @brief Returns whether this token is a given type of bracket.
     *
     * Usage: `token.is_bracket(")")` will return true if this token is
     * of LexerTokenType kBracket and if it is the closing parenthesis.
     * This is a shortcut for testing type and value at the same time.
     */
    inline bool is_bracket(const std::string& br) const
    {
        return (type_ == LexerTokenType::kBracket) && (value_.compare(br) == 0);
    }

    /**
     * @brief Shortcut to check if this is an operator token.
     *
     * There is an overload of this function that also checks if the token is a specific type of
     * operator.
     */
    inline bool is_operator() const
    {
        return type_ == LexerTokenType::kOperator;
    }

    /**
     * @brief Returns whether this token is a given type of operator.
     *
     * Usage: `token.is_operator("%")` will return true if this token is
     * of LexerTokenType kOperator and if it is the modulo operator.
     * This is a shortcut for testing type and value at the same time.
     */
    inline bool is_operator(const std::string& op) const
    {
        return (type_ == LexerTokenType::kOperator) && (value_.compare(op) == 0);
    }

    /**
     * @brief Shortcut to check if this is a tag token.
     */
    inline bool is_tag() const
    {
        return type_ == LexerTokenType::kTag;
    }

    // -------------------------------------------------------------------------
    //     Others
    // -------------------------------------------------------------------------

    /**
     * @brief Returns the string representation for the LexerTokenType of this token.
     */
    inline std::string type_to_string() const
    {
        return lexer_token_type_to_string(type_);
    }

private:
    LexerTokenType type_;
    int line_;
    int column_;
    std::string value_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
