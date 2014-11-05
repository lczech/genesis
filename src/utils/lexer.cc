 #include <string>
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
 * In case an error is encountered while analyzing the string, this functions
 * returns false and the last token will be of type
 * LexerToken::TokenType::kError, with the value being an error message
 * describing the type of error.
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
 * %
 */
bool Lexer::Analyze(const std::string& text)
{
    text_ = text.c_str();
    itr_  = 0;
    len_  = text.size();
    line_ = 1;
    tokens_.clear();

    while (!IsEnd()) {
        // scan arbitrary amount of interleaved whitespace and comments
        while (ScanWhitespace() || ScanComment());

        ScanToken();

        if (tokens_.empty()) {
            return true;
        } else if (tokens_.back().IsError()) {
            return false;
        }
    }

    return true;
}

/**
 * @brief General function to delegate the scanning of tokens to their
 * special implementations.
 */
inline bool Lexer::ScanToken()
{
    if (IsEnd()) {
        return false;
    } else if (IsLetter(text_[itr_])) {
        ScanSymbol();
    } else if (IsDigit(text_[itr_]) || text_[itr_] == '.') {
        ScanNumber();
    } else if (IsQuotemark(text_[itr_])) {
        ScanString();
    } else if (IsOperator(text_[itr_])) {
        ScanOperator();
    } else if (IsBracket(text_[itr_])) {
        ScanBracket();
    } else {
        // TODO this will make an extra token for each consecutive unknown
        // element. maybe those should better go into one unknown token.
        PushToken(LexerToken::kUnknown, itr_, itr_+1);
        ++itr_;
        return false;
    }

    return true;
}

/**
 * @brief Scan for whitespace.
 *
 * Returns true iff whitespace was found.
 */
inline bool Lexer::ScanWhitespace()
{
    bool   found = false;
    size_t start = itr_;

    while (!IsEnd() && IsWhitespace(text_[itr_])) {
        if (text_[itr_] == '\n') {
            ++line_;
        }
        ++itr_;
        found = true;
    }
    if (include_whitespace && found) {
        PushToken(LexerToken::kWhite, start, itr_);
    }
    return found;
}

/**
 * @brief Scans for comments in different formats.
 *
 * Supported formats are:
 *
 * 1.     #  ... \\n
 * 2.     // ... \\n
 * 3.     / * ... * /
 *
 * Returns true iff comment was found.
 */
inline bool Lexer::ScanComment()
{
    // check if still within text boundaries.
    // slightly wrong: if # is the last char in text_, it will not be found.
    if (IsEnd() || IsEnd(itr_+1)) {
        return false;
    }

    // check if comment begins.
    // mode states which type of comment it is, particularly what the
    // end of the comment looks like (1: \n, 2: */)
    int mode = 0;
    if (text_[itr_] == '#') {
        mode = 1;
        ++itr_;
    } else if (text_[itr_] == '/' && text_[itr_+1] == '/') {
        mode = 1;
        itr_ += 2;
    } else if (text_[itr_] == '/' && text_[itr_+1] == '*') {
        mode = 2;
        itr_ += 2;
    }
    if (mode == 0) {
        return false;
    }

    // if comment found, find its end.
    size_t start = itr_;
    while (
        (!IsEnd()       && mode == 1 && text_[itr_] != '\n') ||
        (!IsEnd(itr_+1) && mode == 2 && text_[itr_] != '*'
        && text_[itr_+1] != '/')
    ) {
        if (text_[itr_] == '\n') {
            ++line_;
        }
        ++itr_;
    }

    // TODO trim comments
    // here we are at either end of file or end of comment.
    // we treat both as valid.
    if (include_comments) {
        PushToken(LexerToken::kComment, start, itr_);
    }

    // set iterator to next char (mode also serves as lookup for how many
    // chars we have to skip according to the type of the comment)
    itr_ += mode;
    return true;
}

/**
 * @brief Scan a symbol.
 *
 * A symbol starts with a letter or underscore and goes on as long as only
 * letters, underscores and digits are encountered.
 * Returns true, as symbols cannot be malformatted.
 */
inline bool Lexer::ScanSymbol()
{
    size_t start = itr_;
    while (!IsEnd() && IsAlphanum(text_[itr_])) {
        ++itr_;
    }
    PushToken(LexerToken::kSymbol, start, itr_);
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
    size_t start   = itr_;
    bool   found_d = false; // found a dot
    bool   found_e = false; // found the letter e
    bool   err     = false; // encountered an error while scanning

    // scan
    while(!IsEnd()) {
        if(IsDigit(text_[itr_])) {
            // nothing to do
        } else if (text_[itr_] == '.') {
            // do not allow more than one dot
            if (found_d) {
                err = true;
                break;
            }
            found_d = true;
        } else if (CharMatch(text_[itr_], 'e')) {
            // do not allow more than one e, require a number after the e
            if (found_e
                || IsEnd(itr_+1)
                || (!IsDigit(text_[itr_+1]) && !IsSign(text_[itr_+1]))
            ) {
                err = true;
                break;
            }
            found_e = true;
        } else if (IsSign(text_[itr_])) {
            // conditions for which a sign is valid:
            //   - it is at the beginning of the token
            //   - it comes immediately after the e and is follow by digits
            // --> produce error when neither is fullfilled
            if (!(itr_ == start) &&
                !(
                    found_e && CharMatch(text_[itr_-1], 'e')
                    && !IsEnd(itr_+1) && IsDigit(text_[itr_+1])
                )
            ) {
                err = true;
                break;
            }
        } else {
            break;
        }
        ++itr_;
    }

    // create result
    if (err) {
        PushToken(LexerToken::kError, start, "Malformed number.");
        return false;
    } else {
        PushToken(LexerToken::kNumber, start, itr_);
        return true;
    }
}

/**
 * @brief Scan a string.
 *
 * A string can be enclosed either in 'abc' or in "def". Within a string, any
 * character is allowed; the respective quotation mark can be escaped using a
 * backslash. Other valid escape sequences are \n, \t and \r, which will be
 * resolved to their respective white space character.
 *
 * Returns true iff the string is finished with the correct quotation mark.
 */
inline bool Lexer::ScanString()
{
    // skip the first quotation mark, save its value for later comparision
    // so that the string ends with the same type of mark
    char qmark = text_[itr_];
    ++itr_;
    if (IsEnd()) {
        PushToken(LexerToken::kError, itr_-1, "Malformed string.");
        return false;
    }

    size_t start = itr_;
    bool esc     = false; // are we in escape mode?
    bool found_e = false; // found an escape sequence

    // scan
    while (!IsEnd()) {
        if (text_[itr_] == '\n') {
            ++line_;
        }
        if (text_[itr_] == '\\') {
            esc = true;
            found_e = true;
            if (text_[itr_+1] == '\n') {
                ++line_;
            }
            itr_ += 2;
            continue;
        }
        if(text_[itr_] == qmark) {
            ++itr_;
            break;
        }
        esc = false;
        ++itr_;
    }

    // reached end of text before ending quotation mark
    if (esc || (IsEnd() && !(text_[itr_-1] == qmark))) {
        PushToken(LexerToken::kError, start-1, "Malformed string.");
        return false;
    }

    // de-escape the string (transform backslash-escaped chars)
    std::string res = GetSubstr(start, itr_-1);
    if (found_e && deescape_strings) {
        res = StringDeescape(res);
    }

    // if needed, add qmarks again
    if (!trim_quotation_marks) {
        res = qmark + res + qmark;
    }

    // create result
    PushToken(LexerToken::kString, start-1, res);
    return true;
}

// TODO joining of operators like += or >> might be useful in the future.
/**
 * @brief Scans an operator.
 *
 * If the operator is a sign and the next char in the text is a digit,
 * we jump to scan_number and return its value. Otherwise return true.
 */
inline bool Lexer::ScanOperator()
{
    // if the operator is a sign followed by a number, scan it as a number
    if (IsSign(text_[itr_]) && glue_sign_to_number &&
        !IsEnd(itr_+1) && IsDigit(text_[itr_+1])
    ) {
        return ScanNumber();
    }

    PushToken(LexerToken::kOperator, itr_, itr_+1);
    ++itr_;
    return true;
}

/**
 * @brief Scans a bracket.
 *
 * Returns true.
 */
inline bool Lexer::ScanBracket()
{
    PushToken(LexerToken::kBracket, itr_, itr_+1);
    ++itr_;
    return true;
}

/**
 * @brief Checkes whether the bracket tokes are valid.
 *
 * In order to be valid, every opening bracket must be matched with a
 * corresponding closing bracket, and their order has to be correct.
 */
bool Lexer::ValidateBrackets()
{
    std::stack<char> stk;
    for (LexerToken t : tokens_) {
        if (!t.IsBracket()) {
            continue;
        }

        char c = t.value()[0];
        if (c == '(') stk.push(')');
        if (c == '[') stk.push(']');
        if (c == '{') stk.push('}');

        if (IsRightBracket(c)) {
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
