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
 * @brief Implementation of Lexer class.
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/lexer.hpp"

#include <assert.h>
#include <cstring>
#include <stack>
#include <string>

#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/text/char.hpp"
#include "utils/text/string.hpp"

namespace genesis {
namespace utils {

// =============================================================================
//     Process
// =============================================================================

/**
 * @brief Shortcut function that reads the contents of a file and then calls from_string().
 *
 * If the file does not exist, a warning is triggered and false returned. Otherwise, the result
 * of from_string() is returned.
 */
/*
bool Lexer::from_file   (const std::string& fn)
{
    // TODO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! this is highly dangerous! the string will go
    // TODO out of scope at the end of this function, however, we just did one step!

    // if (!file_exists(fn)) {
    //     LOG_WARN << "File '" << fn << "' does not exist.";
    //     return false;
    // }
    // return from_string(file_read(fn));
}
*/

/**
 * @brief Process a string and store the resulting tokens in this Lexer object.
 *
 * This process analyzes and splits the string into different tokens. For the types of tokens being
 * extracted, see LexerToken; for accessing the results, see Lexer.
 *
 * Returns true iff successful. In case an error is encountered while analyzing
 * the text, this functions returns false and the last token will be of type
 * LexerTokenType::kError, with the value being an error message describing the
 * type of error.
 */
bool Lexer::from_string (const std::string& in)
{
    text_ = in.c_str();
    itr_  = 0;
    len_  = in.size();
    line_ = 1;
    col_  = 0;
    tokens_.clear();

    return process_step();
}

/**
 * @brief Processes one step of the lexing.
 *
 * As stated in the description of this Lexer class, the class is meant to be
 * derived for concrete types of lexers. Thus, here are some comments about the
 * working of this function:
 *
 * For most types of structured text, the first character of each token determines
 * the type of the token (for example, a digit almost always leads to a number
 * token). This is why we use a list telling us which char leads to which token
 * type. This list is a speedup, because using it, we do not need to try every
 * scanner (for numbers, symbols, strings, etc) at the beginning of each new
 * token, but simply do a lookup to find out "this char means we have to use
 * this scanner now". (Also see get_char_type and SetCharType for this.)
 *
 * This does not mean that any char of a given type can only appear in
 * tokens of that type. For example, typically a symbol can start with
 * letters, but then contain numbers later, too. Thus, although a digit
 * is of type kNumber, it can end up in a symbol token, depending on the
 * context.
 *
 * Instead, the char type here is meant as a speedup for finding the right
 * scanner when lexing the text: whenever one scanner finishes, the next
 * char is inspected and depending on its type, a fitting scanner is
 * activated (for digits the number scanner and so on).
 *
 * This technique will not work if finding the correct scanner depends on
 * more than the first character of the token. For example, comments usually
 * start with a more complex sequence ("//" or even "<!--"), which is why
 * they are specially treaded in this function.
 *
 * So, in situations, where the type of the next token cannot be determined from
 * its first character (except comments), this function has to be overridden in the
 * derived class in order to do some other checking methods to determine the
 * correct scanner.
 */
bool Lexer::process_step()
{
    if (is_end()) {
        return false;
    }

    // scan arbitrary amount of interleaved whitespace and comments
    while (scan_whitespace() || scan_comment()) continue;

    // check if whitespace or comment scanner yielded an error
    if (!tokens_.empty() && tokens_.back().is_error()) {
        return false;
    }

    // check if still not at end after whitespaces and comments
    if (is_end()) {
        return false;
    }

    // check if current char is an error char
    LexerTokenType t = get_char_type();
    if (t == LexerTokenType::kError) {
        push_token(LexerTokenType::kError, get_position(), "Invalid character.");
        return false;
    }

    // start the actual scanners depending on the first char
    switch (t) {
        case LexerTokenType::kSymbol:
            scan_symbol();
            break;
        case LexerTokenType::kNumber:
            scan_number();
            break;
        case LexerTokenType::kString:
            scan_string();
            break;
        case LexerTokenType::kBracket:
            scan_bracket();
            break;
        case LexerTokenType::kOperator:
            scan_operator();
            break;
        case LexerTokenType::kTag:
            scan_tag();
            break;
        case LexerTokenType::kUnknown:
            scan_unknown();
            break;

        case LexerTokenType::kWhite:
        case LexerTokenType::kComment:
            // this case happens if scan_whitespace or scan_comment are wrongly
            // implemented in a derived class, so that they return false
            // although they should scan something or create an error token.
            // this is particularly the case when scan_comment does not create
            // an error token when a close-comment-char appears without a
            // open-comment-char first, e.g. "]".
            assert(false);
        case LexerTokenType::kError:
            // this cannot happen, as we already covered the case before the switch
            assert(false);
        default:
            // this case only happens if someone added a new element to the enum.
            // make sure that we do not forget to include it in this switch!
            assert(false);
    }

    // check if the scanners produced an error
    if (tokens_.empty() || tokens_.back().is_error()) {
        return false;
    }
    return true;
}

/**
 * @brief
 */
bool Lexer::process_all()
{
    while (!is_end()) {
        if (!process_step()) {
            return tokens_.empty() || !tokens_.back().is_error();
        }
    }

    return true;
}

// =============================================================================
//     Scanners
// =============================================================================

/**
 * @brief Evaluates (scans) a range between two strings.
 *
 * If the current position in the text starts with the value of `from`, this
 * function continues in the text until the value of `to` is found (or the end of
 * the text). In case of success (both `from` and `to` were found), it returns true,
 * false otherwise.
 *
 * If successfull, the internal position of the Lexer is moved to the char after
 * the `to` string. Thus, the start position has to be stored before calling this function.
 *
 * Example:
 *
 *     size_t start = get_position();
 *     bool   found = evaluate_from_to("{", "}");
 *     if (found) {
 *         push_token(LexerTokenType::kTag, start+1, get_position()-1);
 *     }
 *
 * This scans between two curly brackets and if found, produces a token (that does not contain
 * the brackets themselves).
 */
bool Lexer::evaluate_from_to (const char* from, const char* to)
{
    // first check if the current position actually contains the "from" string
    if (is_end() || strncmp(from, text_+itr_, strlen(from)) != 0) {
        return false;
    }

    // if so, move as many chars forward. we have to split this from the
    // checking, because we do not want to change itr_ in case it is not a
    // match. also, calling next_char here ensures integrity of the line
    // counting.
    for (size_t i = 0; i < strlen(from); ++i) {
        next_char();
    }

    // now try to find the "to" string
    while (!is_end() && strncmp(to, text_+itr_, strlen(to)) != 0) {
        next_char();
    }

    // if the "to" string was not found before the end of the text, we are done
    if (is_end()) {
        return false;
    }

    // "to" string was found. move as many chars forward.
    for (size_t i = 0; i < strlen(to); ++i) {
        next_char();
    }
    return true;
}

/**
 * @brief Scans the text as long as the current char is of type kUnknown.
 *
 * It is possible that this function has to be overridden in case that ProcessString()
 * is overridden as well. This is because of the following:
 *
 * Overriding ProcessString means that the approach of determining the correct scanner
 * for a token from its first character does not work. Thus, all characters that
 * do not indicate a scanner will usually be set to unknown in order to treat
 * them specially in ProcessString. This means that scanning unknown chars should stop
 * after each char, return to ProcessString(), and check again if now it is time to
 * activate a special scanner. Thus, in this case, scan_unknown should only scan
 * one character at a time.
 */
bool Lexer::scan_unknown()
{
    size_t start = get_position();
    while (!is_end() && get_char_type() == LexerTokenType::kUnknown) {
        next_char();
    }
    push_token(LexerTokenType::kUnknown, start, get_position());
    return true;
}

/*
// version that only scans one char at a time. useful for allowing e.g.
// complex comment markers, that would otherwise be turned into unknown tokens.
bool Lexer::scan_unknown()
{
    push_token(LexerTokenType::kUnknown, get_position(), get_position() + 1);
    next_char();
    return true;
}
*/

/**
 * @brief Scan for whitespace.
 *
 * Returns true iff whitespace was found.
 */
bool Lexer::scan_whitespace()
{
    bool   found = false;
    size_t start = get_position();

    while (!is_end() && get_char_type() == LexerTokenType::kWhite) {
        next_char();
        found = true;
    }
    if (found && include_whitespace) {
        push_token(LexerTokenType::kWhite, start, get_position());
    }
    return found;
}

/**
 * @brief Scans for comments in different formats.
 *
 * In the base class, this functions simply returns false. In order to scan for
 * actual comments, it has to be overridden. A typical implementation can be
 * found in NewickLexer::scan_comment().
 */
bool Lexer::scan_comment()
{
    return false;
}

/**
 * @brief Scan a symbol.
 *
 * In the base class, the functions scans as long as the LexerToken type of the
 * current char is kSymbol. If it is necessary to continue scanning for other
 * types of chars once a symbol has started, this function has to be overridden.
 * This is particularly the case for digits: typically, symbols cannot start
 * with a digit, but once a symbol has started, digits can occur.
 *
 * Returns true, as symbols cannot be malformatted.
 */
bool Lexer::scan_symbol()
{
    size_t start = get_position();
    while (!is_end() && get_char_type() == LexerTokenType::kSymbol) {
        next_char();
    }
    push_token(LexerTokenType::kSymbol, start, get_position());
    return true;
}

/**
 * @brief Scans a number.
 *
 * Following formats are supported:
 *
 *     [+-]123[.456][eE[+-]789]
 *
 * Returns true iff valid number was found.
 */
bool Lexer::scan_number()
{
    size_t start   = get_position();
    bool   found_d = false; // found a dot
    bool   found_e = false; // found the letter e

    // encountered an error while scanning. this happens if we reach the end
    // of the number before seeing any number content (digit, dot, etc)
    bool   err     = false;

    // scan
    while(!is_end()) {
        if(utils::char_is_digit(get_char())) {
            // nothing to do
        } else if (get_char() == '.') {
            // do not allow more than one dot, require a number after the dot
            // (if not, treat it as the end of the number, stop scanning)
            if (
                found_d
                || is_end(+1) || !utils::char_is_digit(get_char(+1))
            ) {
                break;
            }
            found_d = true;
        } else if (utils::char_match_ci(get_char(), 'e')) {
            // do not allow more than one e (treat the second one as the end of
            // the number and stop scannning).
            // also, require a number or sign before and after the first e. if not,
            // treat it also as the end of the number and stop scanning
            if (
                found_e
                || get_position() == 0 || !utils::char_is_digit(get_char(-1))
                || is_end(+1)
                || (!utils::char_is_digit(get_char(+1)) && !utils::char_is_sign(get_char(+1)))
            ) {
                err = (get_position() == start);
                break;
            }
            found_e = true;
        } else if (utils::char_is_sign(get_char())) {
            // conditions for when a sign is valid:
            //   - it is at the beginning of the token and followed by digits
            //   - it comes immediately after the e and is followed by digits
            // --> when neither is fullfilled, treat it as the end of the
            //     number, stop scanning
            if (
                !(
                    get_position() == start
                    && !is_end(+1) && utils::char_is_digit(get_char(+1))
                ) &&
                !(
                    found_e && utils::char_match_ci(get_char(-1), 'e')
                    && !is_end(+1) && utils::char_is_digit(get_char(+1))
                )
            ) {
                err = (get_position() == start);
                break;
            }
        } else {
            err = (get_position() == start);
            break;
        }
        next_char();
    }

    // create result
    if (err) {
        push_token(LexerTokenType::kError, get_position(), "Malformed number.");
        return false;
    } else {
        push_token(LexerTokenType::kNumber, start, get_position());
        return true;
    }
}

/**
 * @brief Scan a string.
 *
 * A string is usually enclosed either in 'abc' or in "def" (this depends on
 * which chars have been set to LexerTokenType kString in the derived class).
 * Within a string, any character is allowed. See use_string_escape and
 * use_string_doubled_quotes for options modifying the behaviour of this
 * function.
 *
 * Returns true iff the string is finished with the correct quotation mark.
 */
bool Lexer::scan_string()
{
    // skip the first quotation mark, save its value for later comparision
    // so that the string ends with the same type of mark
    char qmark = get_char();
    next_char();
    if (is_end()) {
        push_token(LexerTokenType::kError, get_position()-1, "Malformed string.");
        return false;
    }

    size_t start = get_position();
    bool jump    = false; // marks when we jumped over an escape or quote mark
    bool found_e = false; // found an escape sequence
    bool found_q = false; // found a doubled qutation mark ""

    // scan
    while (!is_end()) {
        // if we find a backslash and use escape characters, we skip the
        // backslash and the following char. they will then be de-escaped after
        // the end of the string is reached.
        if (get_char() == '\\' && use_string_escape) {
            jump    = true;
            found_e = true;
            next_char();
            next_char();
            continue;
        }
        if (
            get_char() == qmark && get_char(+1) == qmark
            && use_string_doubled_quotes
        ) {
            jump    = true;
            found_q = true;
            next_char();
            next_char();
            continue;
        }
        // check if we reached the end of the string
        if (get_char() == qmark) {
            next_char();
            break;
        }
        jump = false;
        next_char();
    }

    // reached end of text before ending quotation mark.
    // we need to check jump here, because an escape sequence or doubled quote mark
    // (those are the situations when jump=true) can look like the string ending,
    // but actually isn't. this means, if jump is true, we left the loop right after such a case
    // (escape sequence or doubled qmark), and are now at the end of the text, without having
    // seen the end qmark. e.g.: "hello world\"
    if (jump || (is_end() && !(get_char(-1) == qmark))) {
        push_token(LexerTokenType::kError, start-1, "Malformed string.");
        return false;
    }

    // de-escape the string (transform backslash-escaped chars)
    std::string res = get_substr(start, get_position()-1);
    if (found_e && use_string_escape) {
        res = utils::deescape(res);
    }

    // transform doubled qmarks like "" into single ones like "
    if (found_q && use_string_doubled_quotes) {
        res = utils::replace_all(
            res,
            std::string(2, qmark),
            std::string(1, qmark)
        );
    }

    // if needed, add qmarks again
    if (!trim_quotation_marks) {
        res = qmark + res + qmark;
    }

    // create result
    push_token(LexerTokenType::kString, start-1, res);
    return true;
}

/**
 * @brief Scans a single operators.
 *
 * If the operator is a sign and the next char in the text is a digit,
 * and glue_sign_to_number is set, we scan it as a number. For example, the
 * sequence
 *
 *     a+=-3;
 *
 * will result in four tokens: Symbol "a", Operators "+" and "=" and Number "-3",
 * instead of five tokens with three consequtive operators.
 *
 * This allows to use signs (+-) as operator chars in cases where they are not
 * followed by a number.
 */
bool Lexer::scan_operator()
{
    // if the operator is a sign followed by a number, scan it as a number
    if (
        utils::char_is_sign(get_char()) && glue_sign_to_number &&
        !is_end(+1) && utils::char_is_digit(get_char(+1))
    ) {
        return scan_number();
    }

    push_token(LexerTokenType::kOperator, get_position(), get_position()+1);
    next_char();
    return true;
}

/**
 * @brief Scans a single bracket.
 *
 * Returns true.
 */
bool Lexer::scan_bracket()
{
    push_token(LexerTokenType::kBracket, get_position(), get_position()+1);
    next_char();
    return true;
}

/**
 * @brief Scans a tag.
 *
 * Tags can be anything from a single char to a complete string that has to
 * be processed further. It might be neccessary to run a second lexer step using
 * another lexer class on the inner part of the tag, for example when scanning
 * xml text:
 *
 *     <tag key1="x" key2-="y">
 *
 * may result as one tag (with or without the enclosing <>, depending on the
 * specific implementation and needs). Then, a second lexer can decompose this into
 *
 *     tag key1 = x key2 = y
 *
 * In the base class, this functions simply returns an unknown token. In order to scan for
 * actual comments, it has to be overridden. A typical implementation can be
 * found in NewickLexer::scan_comment().
 */
bool Lexer::scan_tag()
{
    return false;
}

// =============================================================================
//     Iterator
// =============================================================================

/**
 * @brief Returns an iterator to the beginning of the token list.
 *
 * This is used for the iterator and also allows to use range based
 * looping over the tokens:
 *
 *     Lexer l;
 *     for (LexerToken& t : l) {
 *         std::cout << t.value() << std::endl;
 *     }
 * %
 */
Lexer::iterator Lexer::begin()
{
    return iterator(this, 0);
}

/**
 * @brief Returns an iterator to the end of the token list.
 */
Lexer::iterator Lexer::end()
{
    return iterator(this, -1);
}

// =============================================================================
//     Helper functions
// =============================================================================

/**
 * @brief Checkes whether the bracket tokes are validly nested.
 *
 * In order to be valid, every opening bracket must be matched with a
 * corresponding closing bracket, and their order has to be correct.
 *
 * This function checks the following types of brackets:
 *
 *     () [] {} <>
 *
 * It is not particularly useful for xml, as there it is also important to use
 * closing tags like `<xml> ... </xml>`.
 */
bool Lexer::validate_brackets() const
{
    std::stack<char> stk;
    for (LexerToken t : tokens_) {
        if (!t.is_bracket()) {
            continue;
        }

        char c = t.value()[0];
        if (c == '(') stk.push(')');
        if (c == '[') stk.push(']');
        if (c == '{') stk.push('}');
        if (c == '<') stk.push('>');

        if (c == ')' || c == ']' || c == '}' || c == '>') {
            if (stk.empty() || c != stk.top()) {
                return false;
            } else {
                stk.pop();
            }
        }
    }
    return true;
}

/**
 * @brief Returns a listing of the parse result in readable form.
 */
std::string Lexer::dump() const
{
    std::string res;
    for (size_t i = 0; i < tokens_.size(); i++) {
        LexerToken t = tokens_[i];
        char out[30];
        sprintf(out, "[%03d] @%03d:%03d %10s : ",
            static_cast<unsigned int>(i),
            static_cast<unsigned int>(t.line()),
            static_cast<unsigned int>(t.column()),
            t.type_to_string().c_str()
        );
        res += out + t.value() + '\n';
    }
    return res;
}

} // namespace utils
} // namespace genesis
