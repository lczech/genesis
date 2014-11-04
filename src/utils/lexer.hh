#ifndef GNS_UTILS_LEXER_H_
#define GNS_UTILS_LEXER_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

 #include <string>
 #include <vector>

namespace genesis {
namespace utils {

inline bool IsWhitespace(const char c)
{
    return (' '  == c) || ('\n' == c) || ('\r' == c) || ('\t' == c) ||
           ('\b' == c) || ('\v' == c) || ('\f' == c) ;
}

inline bool IsLetter(const char c)
{
    return (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'));
}

inline bool IsDigit(const char c)
{
    return ('0' <= c) && (c <= '9');
}

inline bool IsAlphanum(const char c)
{
    return IsLetter(c) || IsDigit(c) || c == '_';
}

inline bool IsLeftBracket(const char c)
{
    return ('(' == c) || ('[' == c) || ('{' == c);
}

inline bool IsRightBracket(const char c)
{
    return (')' == c) || (']' == c) || ('}' == c);
}

inline bool IsBracket(const char c)
{
    return IsLeftBracket(c) || IsRightBracket(c);
}

inline bool IsSign(const char c)
{
    return ('+' == c) || ('-' == c);
}

inline bool IsOperator(const char c)
{
    return
    ('+' == c) || ('-' == c) ||
    ('*' == c) || ('/' == c) ||
    ('<' == c) || ('>' == c) ||
    ('?' == c) || ('!' == c) ||
    ('^' == c) || ('=' == c) ||
    ('%' == c) || ('&' == c) ||
    ('|' == c) || (',' == c) ||
    (':' == c) || (';' == c);
}

inline bool IsQuotemark(const char c)
{
    return ('"' == c) || ('\'' == c);
}

/**
 * @brief Returns whether two chars are the same, case insensitive.
 */
inline bool CharMatch(const char c1, const char c2)
{
    return std::tolower(c1) == std::tolower(c2);
}

/**
 * @brief Represents a token that is outputted by the Lexer.
 *
 * The main types of tokens are:
 *
 * 1. **Symbol**: A named symbol, that starts with a letter, followed by any
 *    number of letters, digits or underscores.
 *
 * 2. **Number**: A number in the format
 *
 *        [+-]123[.456][eE[+-]789].
 *
 * 3. **String**: A literal string, enclosed in either 'abc' or "def". It can
 *    contain escape charaters using a backslash, where \\n, \\t and \\r are
 *    translated into their whitespace representation.
 *
 * 4. **Operator**: An operator out of the set
 *
 *        + - * / < > ? ! ^ = % & | , : ;
 *
 * 5. **Bracket**: Any of these brackets
 *
 *        ( ) [ ] { }
 *
 * Furthermore, there are token types marking whitespaces and comments, which
 * are included when the corresponding options are set in the Lexer class.
 *
 * In case of an error while processing the input, an error token is produced
 * which contains the location of the error.
 */
struct LexerToken
{
    enum TokenType {
        kNone       = 0,
        kError      = 1,
        kEOF        = 8,
        kWhite      = 9,
        kComment    = 10,

        kSymbol     = 20,
        kNumber     = 21,
        kString     = 22,

        kOpPlus     = '+',
        kOpMinus    = '-',
        kOpMult     = '*',
        kOpDiv      = '/',
        kOpLT       = '<',
        kOpGT       = '>',
        kOpQMark    = '?',
        kOpEMark    = '!',
        kOpPower    = '^',
        kOpEq       = '=',
        kOpMod      = '%',
        kOpAmp      = '&',
        kOpBar      = '|',
        kOpComma    = ',',
        kOpColon    = ':',
        kOpSemicolon= ';',

        kBracketPL  = '(', // parenthesis bracket left
        kBracketPR  = ')', // parenthesis bracket right
        kBracketSL  = '[', // square      bracket left
        kBracketSR  = ']', // squrare     bracket right
        kBracketCL  = '{', // curly       bracket left
        kBracketCR  = '}', // curly       bracket right
    };

    inline LexerToken(const TokenType t, const size_t p, const std::string v) :
        type(t), position(p), value(v) {};

    inline bool IsError()
    {
        return type == kError;
    }

    inline bool IsOperator()
    {
        switch(type) {
            case kOpPlus      :
            case kOpMinus     :
            case kOpMult      :
            case kOpDiv       :
            case kOpLT        :
            case kOpGT        :
            case kOpQMark     :
            case kOpEMark     :
            case kOpPower     :
            case kOpEq        :
            case kOpMod       :
            case kOpAmp       :
            case kOpBar       :
            case kOpComma     :
            case kOpColon     :
            case kOpSemicolon : return true;
            default           : return false;
        }
    }

    inline bool IsBracket()
    {
        switch(type) {
            case kBracketPL   :
            case kBracketPR   :
            case kBracketSL   :
            case kBracketSR   :
            case kBracketCL   :
            case kBracketCR   : return true;
            default           : return false;
        }
    }

    static inline std::string ToStr(const TokenType t)
    {
        switch(t) {
            case kNone        : return "None";
            case kError       : return "Error";
            case kEOF         : return "EOF";
            case kWhite       : return "Whitespace";

            case kComment     : return "Comment";
            case kSymbol      : return "Symbol";
            case kNumber      : return "Number";
            case kString      : return "String";

            case kOpPlus      :
            case kOpMinus     :
            case kOpMult      :
            case kOpDiv       :
            case kOpLT        :
            case kOpGT        :
            case kOpQMark     :
            case kOpEMark     :
            case kOpPower     :
            case kOpEq        :
            case kOpMod       :
            case kOpAmp       :
            case kOpBar       :
            case kOpComma     :
            case kOpColon     :
            case kOpSemicolon : return "Operator";

            case kBracketPL   :
            case kBracketPR   :
            case kBracketSL   :
            case kBracketSR   :
            case kBracketCL   :
            case kBracketCR   : return "Bracket";

            default           : return "Unknown";
        }
    }

    inline std::string ToStr()
    {
        return ToStr(type);
    }

    TokenType   type;
    size_t      position;
    std::string value;
};

/**
 * @brief %Lexer class that provides an easy way of tokenizing a string.
 *
 * The usage of this class is as follows:
 *
 *     Lexer l;
 *     l.Analyse("1+2=3");
 *
 * The tokens produced with the Analyse method are of type LexerToken and can
 * be accessed using an iterator over the Lexer object:
 *
 * TODO
 *
 * Te options include_whitespace and include_comments are disabled by default,
 * which helps separating the important content from whites and comments.
 * If needed, they can be enabled to include those tokens also.
 */
class Lexer
{
    public:
        Lexer() {};
        ~Lexer() {};

        bool Analyze(const std::string& text);
        bool CheckBrackets();

        std::string Dump();
        void Clear();

        /** @brief Determines whether whitespaces are included as tokens. */
        bool include_whitespace = false;

        /** @brief Determines whether comments are included as tokens. */
        bool include_comments   = false;

        /** @brief Determines whether to glue a sign followed by a number.
         *
         * If disabled, a term like `1+2=3` will be parsed into single tokes
         * for each character:
         *
         *     1 + 2 = 3
         *
         * If enabled, signs that preceed a number will be glued to that number,
         * so that a term like `items [1.0, -3.14]` will result in
         *
         *     [ 1.0 , -3.14 ]
         *
         * This is useful when the input is a list or similar data.
         */
         bool glue_sign_to_number = true;

    protected:
        bool ScanToken();
        bool ScanWhitespace();
        bool ScanComment();
        bool ScanSymbol();
        bool ScanNumber();
        bool ScanString();
        bool ScanOperator();
        bool ScanBracket();

    private:
        inline bool IsEnd()
        {
            return itr_ >= len_;
        }

        inline bool IsEnd(size_t pos)
        {
            return pos >= len_;
        }

        inline std::string GetSubstr (size_t start, size_t end)
        {
            return std::string(text_ + start, end-start);
        }

        const char* text_ = 0;
        size_t      itr_  = 0;
        size_t      len_  = 0;
        std::vector<LexerToken> tokens_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
