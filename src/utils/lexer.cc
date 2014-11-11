 #include <string>
 #include <cstring>
 #include <vector>
 #include <stack>

 #include "lexer.hh"
 #include "log.hh"
 #include "utils.hh"

namespace genesis {
namespace utils {

/**
 * @brief Analyze a string and store the resulting tokens in this Lexer object.
 *
 * This function empties the token list stored for this object and fills it
 * with the results of analyzing the given string. For the types of tokens being
 * extracted, see LexerToken; for accessing the results, see Lexer.
 *
 * Returns true if successful. In case an error is encountered while analyzing
 * the text, this functions returns false and the last token will be of type
 * LexerTokenType::kError, with the value being an error message describing the
 * type of error.
 *
 * Common usage:
 *
 *     Lexer l;
 *     if (!l.Analyze("tree(some:0.5,items:0.3);")) {
 *         LexerToken b = l.back();
 *         std::cout
 *             << b.TypeToStr()
 *             << " at " << b.line() << ":" << b.column()
 *             << " with message " << b.value() << std::endl;
 *     }
 *
 * As stated in the description of this Lexer class, the class is meant to be
 * derived for concrete types of lexers. Thus, here are some comments about the
 * working of this function:
 *
 * For most types of text files, the first character of each token determines
 * the type of the token (for example, a digit almost always leads to a number
 * token). This is why we use a list telling us which char leads to which token
 * type. This list is a speedup, because using it, we do not need to try every
 * scanner (for numbers, symbols, strings, etc) at the beginning of each new
 * token, but simply do a lookup to find out "this char means we have to use
 * this scanner now". (Also see GetCharType and SetCharType for this.)
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
 * they are specially treaded in the Analyze function.
 *
 * So, in situations, where the type of the next token cannot be determined from
 * its first character (except comments), Analyze has to be overridden in the
 * derived class in order to do some other checking methods to determine the
 * correct scanner. In the new Analyze function, first call Init to reset all
 * internal variables. Also see ScanUnknown for some important information.
 */
bool Lexer::Analyze(const std::string& text)
{
    Init(text);

    while (!IsEnd()) {
        // scan arbitrary amount of interleaved whitespace and comments
        while (ScanWhitespace() || ScanComment()) continue;

        LexerTokenType t = GetCharType();
        if (t == kError) {
            PushToken(kError, GetPosition(), "Invalid character.");
            return false;
        }

        switch (t) {
            case kSymbol:
                ScanSymbol();
                break;
            case kNumber:
                ScanNumber();
                break;
            case kString:
                ScanString();
                break;
            case kBracket:
                ScanBracket();
                break;
            case kOperator:
                ScanOperator();
                break;
            case kTag:
                ScanTag();
                break;

            case kUnknown:
            default:
                ScanUnknown();
        }

        if (tokens_.empty()) {
            return true;
        } else if (tokens_.back().type() == kError) {
            return false;
        }
    }

    return true;
}

//~ void Lexer::StripComments ()
//~ {
//~
//~ }

/**
 * @brief Scans a range between two strings.
 *
 * If the current position in the text starts with the value of from, this
 * scanner continues in the text until the value of to is found (or the end of
 * the text). In case of success (both from and to were found), it returns true,
 * false otherwise.
 */
bool Lexer::ScanFromTo (const char* from, const char* to)
{
    // first check if the current position actually contains the "from" string
    if (IsEnd() || strncmp(from, text_+itr_, strlen(from)) != 0) {
        return false;
    }

    // if so, move as many chars forward. we have to split this from the
    // checking, because we do not want to change itr_ in case it is not a
    // match. also, calling NextChar here ensures integrity of the line
    // counting.
    for (size_t i = 0; i < strlen(from); ++i) {
        NextChar();
    }

    // now try to find the "to" string
    while (!IsEnd() && strncmp(to, text_+itr_, strlen(to)) != 0) {
        NextChar();
    }

    // if the "to" string was not found before he end of the text, we are done
    if (IsEnd()) {
        return false;
    }

    // "to" string was found. move as many chars forward.
    for (size_t i = 0; i < strlen(to); ++i) {
        NextChar();
    }
    return true;
}

/**
 * @brief Scans the text as long as the current char is of type kUnknown.
 *
 * It is possible that this function has to be overridden in case that Analyze
 * is overridden as well. This is because of the following:
 *
 * Overriding Analyze means that the approach of determining the correct scanner
 * for a token from its first character does not work. Thus, all characters that
 * do not indicate a scanner will usually be set to unknown in order to treat
 * them specially in Analyze. This means that scanning unknown chars should stop
 * after each char, return to Analyze, and check again if now it is time to
 * activate a special scanner. Thus, in this case, ScanUnknown should only scan
 * one character at a time.
 */
inline bool Lexer::ScanUnknown()
{
    size_t start = GetPosition();
    while (!IsEnd() && GetCharType() == kUnknown) {
        NextChar();
    }
    PushToken(kUnknown, start, GetPosition());
    return true;
}

/*
// version that only scans one char at a time. useful for allowing e.g.
// complex comment markers, that would otherwise be turned into unknown tokens.
inline bool Lexer::ScanUnknown()
{
    PushToken(LexerToken::kUnknown, GetPosition(), GetPosition() + 1);
    NextChar();
    return true;
}
*/

/**
 * @brief Scan for whitespace.
 *
 * Returns true iff whitespace was found.
 */
inline bool Lexer::ScanWhitespace()
{
    bool   found = false;
    size_t start = GetPosition();

    while (!IsEnd() && GetCharType() == kWhite) {
        NextChar();
        found = true;
    }
    if (include_whitespace && found) {
        PushToken(kWhite, start, itr_);
    }
    return found;
}

/**
 * @brief Scans for comments in different formats.
 *
 * In the base class, this functions simply returns false. In order to scan for
 * actual comments, it has to be overridden. A typical implementation might
 * look like this:
 *
 *     size_t start = GetPosition();
 *     bool   found = ScanFromTo("[", "]") || ScanFromTo("#", "\n");
 *     if (found && include_comments) {
 *         PushToken(kComment, start, GetPosition());
 *     }
 *     return found;
 *
 * This scans for comments in square brackets or those starting with a hash tag.
 */
inline bool Lexer::ScanComment()
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
inline bool Lexer::ScanSymbol()
{
    size_t start = GetPosition();
    while (!IsEnd() && GetCharType() == kSymbol) {
        NextChar();
    }
    PushToken(kSymbol, start, GetPosition());
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
inline bool Lexer::ScanNumber()
{
    size_t start   = GetPosition();
    bool   found_d = false; // found a dot
    bool   found_e = false; // found the letter e
    bool   err     = false; // encountered an error while scanning

    // scan
    while(!IsEnd()) {
        if(CharIsDigit(GetChar())) {
            // nothing to do
        } else if (GetChar() == '.') {
            // do not allow more than one dot, require a number after the dot
            if (
                found_d
                || IsEnd(+1) || !CharIsDigit(GetChar(+1))
            ) {
                err = true;
                break;
            }
            found_d = true;
        } else if (CharMatch(GetChar(), 'e')) {
            // do not allow more than one e (treat it as the end of the number)
            if (found_e) {
                break;
            }
            // require a number or sign after the first e
            if (
                IsEnd(+1)
                || (!CharIsDigit(GetChar(+1)) && !CharIsSign(GetChar(+1)))
            ) {
                NextChar();
                err = true;
                break;
            }
            found_e = true;
        } else if (CharIsSign(GetChar())) {
            // conditions for when a sign is valid:
            //   - it is at the beginning of the token and followed by digits
            //   - it comes immediately after the e and is followed by digits
            // --> produce error when neither is fullfilled
            if (
                !(
                    GetPosition() == start
                    && !IsEnd(+1) && CharIsDigit(GetChar(+1))
                ) &&
                !(
                    found_e && CharMatch(GetChar(-1), 'e')
                    && !IsEnd(+1) && CharIsDigit(GetChar(+1))
                )
            ) {
                err = true;
                break;
            }
        } else {
            break;
        }
        NextChar();
    }

    // create result
    if (err) {
        PushToken(kError, GetPosition(), "Malformed number.");
        return false;
    } else {
        PushToken(kNumber, start, GetPosition());
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
inline bool Lexer::ScanString()
{
    // skip the first quotation mark, save its value for later comparision
    // so that the string ends with the same type of mark
    char qmark = GetChar();
    NextChar();
    if (IsEnd()) {
        PushToken(kError, GetPosition()-1, "Malformed string.");
        return false;
    }

    size_t start = GetPosition();
    bool jump    = false; // marks when we jumped over an escape or quote mark
    bool found_e = false; // found an escape sequence
    bool found_q = false; // found an escape sequence

    // scan
    while (!IsEnd()) {
        // if we find a backslash and use escape characters, we skip the
        // backslash and the following char. they will then be de-escaped after
        // the end of the string is reached.
        if (GetChar() == '\\' && use_string_escape) {
            jump    = true;
            found_e = true;
            NextChar();
            NextChar();
            continue;
        }
        if (
            GetChar() == qmark && GetChar(+1) == qmark
            && use_string_doubled_quotes
        ) {
            jump    = true;
            found_q = true;
            NextChar();
            NextChar();
            continue;
        }
        // check if we reached the end of the string
        if (GetChar() == qmark) {
            NextChar();
            break;
        }
        jump = false;
        NextChar();
    }

    // reached end of text before ending quotation mark.
    // we need to check jump here, because an escape sequence or doubled
    // quote mark can look like the string ending, but actually isn't.
    if (jump || (IsEnd() && !(GetChar(-1) == qmark))) {
        PushToken(kError, start-1, "Malformed string.");
        return false;
    }

    // de-escape the string (transform backslash-escaped chars)
    std::string res = GetSubstr(start, GetPosition()-1);
    if (found_e && use_string_escape) {
        res = StringDeescape(res);
    }

    // transform doubled qmarks like "" into single ones like "
    if (found_q && use_string_doubled_quotes) {
        res = StringReplaceAll(
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
    PushToken(kString, start-1, res);
    return true;
}

/**
 * @brief Scans a sequence of operators.
 *
 * This scanner continues as long as chars of type kOperator are found. If this
 * is not helpful for a derived class, it should be overridden.
 *
 * If the operator is a sign and the next char in the text is a digit,
 * and glue_sign_to_number is set, we scan it as a number. For example, the
 * sequence
 *
 *     a+=-3;
 *
 * will result in three tokens: Symbol "a", Operator "+=" and Number "-3".
 */
inline bool Lexer::ScanOperator()
{
    size_t start = GetPosition();
    bool found_n = false; // we found the beginning of a number: (+-)[0-9]

    while (!IsEnd() && GetCharType() == kOperator) {
        // if the operator is a sign followed by a number, scan it as a number
        if (
            CharIsSign(GetChar()) && glue_sign_to_number &&
            !IsEnd(+1) && CharIsDigit(GetChar(+1))
        ) {
            found_n = true;
            break;
        }
        NextChar();
    }

    // if we found operator chars, push them
    if (start != GetPosition()) {
        PushToken(kOperator, start, GetPosition());
    }
    // if we found the beginning of a number, scan it
    if (found_n) {
        return ScanNumber();
    }
    return true;
}

/**
 * @brief Scans a single bracket.
 *
 * Returns true.
 */
inline bool Lexer::ScanBracket()
{
    PushToken(kBracket, GetPosition(), GetPosition()+1);
    NextChar();
    return true;
}

/**
 * @brief Scans a single tag char.
 *
 * Returns true.
 */
inline bool Lexer::ScanTag()
{
    PushToken(kTag, GetPosition(), GetPosition()+1);
    NextChar();
    return true;
}

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
bool Lexer::ValidateBrackets()
{
    std::stack<char> stk;
    for (LexerToken t : tokens_) {
        if (t.type() != kBracket) {
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
std::string Lexer::Dump()
{
    std::string res;
    for (size_t i = 0; i < tokens_.size(); i++) {
        LexerToken t = tokens_[i];
        char out[30];
        sprintf(out, "[%03d] @%03d:%03d %10s : ",
            static_cast<unsigned int>(i),
            static_cast<unsigned int>(t.line()),
            static_cast<unsigned int>(t.column()),
            t.TypeToStr().c_str()
        );
        res += out + t.value() + '\n';
    }
    return res;
}

} // namespace utils
} // namespace genesis
