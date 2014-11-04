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

inline bool is_whitespace(const char c)
{
    return (' '  == c) || ('\n' == c) || ('\r' == c) || ('\t' == c) ||
           ('\b' == c) || ('\v' == c) || ('\f' == c) ;
}

inline bool is_letter(const char c)
{
    return (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'));
}

inline bool is_digit(const char c)
{
    return ('0' <= c) && (c <= '9');
}

inline bool is_alphanum(const char c)
{
    return is_letter(c) || is_digit(c) || c == '_';
}

inline bool is_left_bracket(const char c)
{
    return ('(' == c) || ('[' == c) || ('{' == c);
}

inline bool is_right_bracket(const char c)
{
    return (')' == c) || (']' == c) || ('}' == c);
}

inline bool is_bracket(const char c)
{
    return is_left_bracket(c) || is_right_bracket(c);
}

inline bool is_sign(const char c)
{
    return ('+' == c) || ('-' == c);
}

inline bool is_operator(const char c)
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

inline bool is_quotemark(const char c)
{
    return ('"' == c) || ('\'' == c);
}

inline bool char_match(const char c1, const char c2)
{
    return std::tolower(c1) == std::tolower(c2);
}

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

    inline bool is_error()
    {
        return type == kError;
    }

    inline bool is_operator()
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

    inline bool is_bracket()
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

    static inline std::string to_str(const TokenType t)
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

    inline std::string to_str()
    {
        return to_str(type);
    }

    TokenType   type;
    size_t      position;
    std::string value;
};

class Lexer
{
    public:
        Lexer() {};
        ~Lexer() {};

        bool Analyze(const std::string& text);

        bool CheckBrackets();

        std::string Dump();

        bool include_whitespace = false;

        bool include_comments   = false;

    private:

        inline bool is_end()
        {
            return itr_ >= len_;
        }

        inline bool is_end(size_t pos)
        {
            return pos >= len_;
        }

        inline char char_()
        {
            return text_[itr_];
        }

        inline char char_(size_t pos)
        {
            return text_[pos];
        }

        inline std::string get_substr (size_t start, size_t end)
        {
            return std::string(text_ + start, end-start);
        }

        bool scan_token();
        bool scan_whitespace();
        bool scan_comment();
        bool scan_symbol();
        bool scan_number();
        bool scan_string();
        bool scan_operator();
        bool scan_bracket();

        /**
         *
         */
        const char* text_ = 0;

        size_t itr_  = 0;
        size_t len_  = 0;

        std::vector<LexerToken> tokens_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
