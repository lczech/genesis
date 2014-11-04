 #include <string>
 #include <vector>
 #include <stack>

 #include "lexer.hh"
 #include "log.hh"

namespace genesis {
namespace utils {

/**
 * @brief Analyze a string and store the resulting tokens in this Lexer object.
 *
 * This function empties the token list stored for this object and fills it
 * with the results of analyzing the given string.
 */
bool Lexer::Analyze(const std::string& text)
{
    text_ = text.c_str();
    itr_  = 0;
    len_  = text.size();
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
 * @brief General function to delegate to scanning of tokens to their
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
        LexerToken t(LexerToken::kError, itr_, GetSubstr(itr_, itr_+1));
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
        ++itr_;
        found = true;
    }
    if (include_whitespace && found) {
        LexerToken t(LexerToken::kWhite, start, GetSubstr(start, itr_));
        tokens_.push_back(t);
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
        ++itr_;
    }

    // here we are at either end of file or end of comment.
    // we treat both as valid.
    if (include_comments) {
        LexerToken t(LexerToken::kComment, start, GetSubstr(start, itr_));
        tokens_.push_back(t);
    }

    // set iterator to next char (mode also serves as lookup for how many
    // chars we have to skip according to the type of the comment)
    itr_ += mode;
    return true;
}

/**
 * @brief Scan a symbol.
 *
 * Returns true, as symbols cannot be malformatted.
 */
inline bool Lexer::ScanSymbol()
{
    size_t start = itr_;
    while (!IsEnd() && IsAlphanum(text_[itr_])) {
        ++itr_;
    }
    LexerToken t(LexerToken::kSymbol, start, GetSubstr(start, itr_));
    tokens_.push_back(t);
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
        LexerToken t(LexerToken::kError, itr_, GetSubstr(itr_, itr_+1));
        tokens_.push_back(t);
        return false;
    } else {
        LexerToken t(LexerToken::kNumber, start, GetSubstr(start, itr_));
        tokens_.push_back(t);
        return true;
    }
}

/**
 * @brief Scan a string.
 *
 * A string can be enclosed either in 'abc' or in "def". Within a string, any
 * character is allowed; the respective quotation mark can be escaped using a
 * backslash. Other valid escape sequences are \n, \t and \r, which will be
 * resolved to their respective white space charater.
 *
 * Returns true iff the string is finished with the correct quotation mark.
 */
inline bool Lexer::ScanString()
{
    // skip the first quotation mark, save it for later comparision
    char qmark = text_[itr_];
    ++itr_;
    if (IsEnd()) {
        LexerToken t(LexerToken::kError, itr_-1, "");
        tokens_.push_back(t);
        return false;
    }

    size_t start = itr_;
    bool esc     = false; // are we in escape mode?
    bool found_e = false; // found an escape sequence

    // scan
    while (!IsEnd()) {
        if (text_[itr_] == '\\') {
            esc = true;
            found_e = true;
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
        LexerToken t(LexerToken::kError, itr_-1, "");
        tokens_.push_back(t);
        return false;
    }

    // de-escape the string
    // TODO this is not fast. could be better by using char[] (save reallocs)
    std::string res = GetSubstr(start, itr_-1);
    if (found_e) {
        std::string tmp = "";
        for (size_t i = 0; i < res.size(); i++) {
            if (res[i] == '\\') {
                if (i+1 >= res.size()){
                    break;
                }
                switch (res[i+1]) {
                    case 'n' : tmp += '\n'; break;
                    case 't' : tmp += '\t'; break;
                    case 'r' : tmp += '\r'; break;
                    default  : tmp += res[i+1];
                }
                ++i;
            } else {
                tmp += res[i];
            }
        }
        res = tmp;
    }

    // create result
    LexerToken t(LexerToken::kString, start-1, res);
    tokens_.push_back(t);
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

    LexerToken t(LexerToken::kOperator, itr_, GetSubstr(itr_, itr_+1));
    ++itr_;
    tokens_.push_back(t);
    return true;
}

/**
 * @brief Scans a bracket.
 *
 * Returns true.
 */
inline bool Lexer::ScanBracket()
{
    LexerToken t(LexerToken::kBracket, itr_, GetSubstr(itr_, itr_+1));
    ++itr_;
    tokens_.push_back(t);
    return true;
}

/**
 * @brief Checkes whether the bracket tokes are valid.
 *
 * In order to be valid, every opening bracket must be matched with a
 * corresponding closing bracket, and their order has to be correct.
 */
bool Lexer::CheckBrackets()
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
 * @brief Returns a listing of the parse result.
 */
std::string Lexer::Dump()
{
    std::string res;
    for (size_t i = 0; i < tokens_.size(); i++) {
        LexerToken t = tokens_[i];
        char out[25];
        sprintf(out, "[%03d] @%03d %10s : ",
            static_cast<unsigned int>(i),
            static_cast<unsigned int>(t.position()),
            t.ToStr().c_str()
        );
        res += out + t.value() + '\n';
    }
    return res;
}

/**
 * @brief Clears all private values, as if the object was newly created.
 *
 * The options for including whitespace and comments are however not changed.
 */
void Lexer::Clear()
{
    text_ = "";
    itr_  = 0;
    len_  = 0;
    tokens_.clear();
}

} // namespace utils
} // namespace genesis
