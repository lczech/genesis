#ifndef GNS_UTILS_LEXER_H_
#define GNS_UTILS_LEXER_H_

/**
 * @brief Provides a basic lexer to analyze a string and split it into tokens.
 *
 * @file
 * @ingroup utils
 */

 #include <string>
 #include <vector>

namespace genesis {
namespace utils {

/** @brief Returns whether a char is a digit. */
inline bool CharIsDigit(const char c)
{
return ('0' <= c) && (c <= '9');
}

/** @brief Returns whether a char is a sign (+-) */
inline bool CharIsSign(const char c)
{
return ('+' == c) || ('-' == c);
}

/** @brief Enum for the different types of lexer tokens. */
enum LexerTokenType {
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

/** @brief Converts a LexerTokenType into its string representation. */
inline std::string LexerTokenTypeToStr(const LexerTokenType t)
{
    switch(t) {
        case kError    : return "Error";
        case kUnknown  : return "Unknown";

        case kWhite    : return "Whitespace";
        case kComment  : return "Comment";
        case kSymbol   : return "Symbol";
        case kNumber   : return "Number";
        case kString   : return "String";
        case kBracket  : return "Bracket";
        case kOperator : return "Operator";
        case kTag      : return "Tag";

        case kEOF      : return "EOF";
        default        : return "Unknown";
    }
}

/**
 * @brief Represents a token that is outputted by the Lexer.
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
 *    It can contain escape charaters using a backslash, where \\n, \\t and \\r
 *    are translated into their whitespace representation using
 *    StringDeescape() when the option Lexer::use_string_escape is set.
 *
 * 4. **Bracket**: Typically of these brackets
 *
 *        ( ) [ ] { } < >
 *
 * 5. **Operator**: An operator, typically out of the set
 *
 *        + - * / < > ? ! ^ = % & | , : ;
 *
 * 6. **Tag**: A character denoting a tag, e.g. `<>` for xml.
 *
 * Furthermore, there are token types marking whitespaces and comments, which
 * are included when the corresponding options are set in the Lexer class.
 *
 * In case of an error while processing the input, an error token is produced
 * which contains the location of the error.
 */
struct LexerToken
{
public:
    /**
     * @brief Constructor that sets the values for this token.
     */
    inline LexerToken
    (
        const LexerTokenType t, const int         l,
        const int            c, const std::string v
    ) :
        type_(t), line_(l), column_(c), value_(v)
    {};

    /**
     * @brief Getter for the LexerTokenType of this token.
     */
    inline LexerTokenType type() const {return type_;};

    /** @brief Getter for the line where this token occured. */
    inline size_t line() const {return line_;};

    /** @brief Getter for the column where this token occured. */
    inline size_t column() const {return column_;};

    /** @brief Getter for the string value of this token. */
    inline std::string value() const {return value_;};

    /**
     * @brief Returns whether this token is a given type of operator.
     *
     * Usage: `token.IsOperator('%')` will return true if this token is
     * of LexerTokenType kOperator and if it is the modulo operator.
     */
    inline bool IsOperator(const char c) const
    {
        return type_ == kOperator && value_[0] == c;
    }

    /** @brief Returns whether this token is a given type of bracket.
     *
     * Usage: `token.IsBracket(')')` will return true if this token is
     * of LexerTokenType kBracket and if it is the closing parenthesis.
     */
    inline bool IsBracket(const char c) const
    {
        return type_ == kBracket && value_[0] == c;
    }

    /**
     * @brief Returns the string representation for the LexerTokenType of
     * this token.
     */
    inline std::string TypeToStr() const
    {
        return LexerTokenTypeToStr(type_);
    }

private:
    const LexerTokenType   type_;
    const int              line_;
    const int              column_;
    const std::string      value_;
};

/**
 * @brief %Lexer class that provides an easy way of tokenizing a string.
 *
 * The usage of this class is as follows:
 *
 *     Lexer l;
 *     l.Analyze("1+2=3");
 *
 * The tokens produced with the Analyze() method are of type LexerToken (see
 * there for a list of the types of tokens) and can be accessed in various ways:
 *
 *   * Using an iterator, see Lexer::TokenIterator
 *   * Using range based loops, see begin()
 *   * Using index based array access, see operator[]()
 *
 * The options Lexer::include_whitespace and Lexer::include_comments are
 * disabled by default, which helps separating the important content from whites
 * and comments. If needed, they can be enabled to include those tokens, too.
 *
 * The options Lexer::glue_sign_to_number, Lexer::trim_quotation_marks and
 * Lexer::use_string_escape are enabled per default, because this is mostly
 * useful for analyzing genetic data.
 */
class Lexer
{
public:
    virtual bool Analyze(const std::string& text);
    bool ValidateBrackets();
    std::string Dump();

    /**
     * @brief Iterator type to access the tokens produces by the lexer.
     *
     * This iterator allows to use a loop like this:
     *
     *     Lexer l;
     *     for (Lexer::TokenIterator t = l.begin(); t != l.end(); ++t) {
     *         std::cout << t->value() << std::endl;
     *     }
     * %
     */
    typedef std::vector<LexerToken>::iterator TokenIterator;

    /**
     * @brief Returns an iterator to the beginning of the token list.
     *
     * This is used for the TokenIterator and also allows to use range based
     * looping over the tokens:
     *
     *     Lexer l;
     *     for (LexerToken t : l) {
     *         std::cout << t.value() << std::endl;
     *     }
     * %
     */
    inline TokenIterator begin()
    {
        return tokens_.begin();
    }

    /**
     * @brief Returns an iterator to the end of the token list.
     */
    inline TokenIterator end()
    {
        return tokens_.end();
    }

    /**
     * @brief Provides index based array acces to the tokens.
     *
     * This also allows to iterate over them using:
     *
     *     Lexer l;
     *     for (size_t i = 0; i < l.size(); ++i) {
     *        LexerToken t = l[i];
     *        std::cout << t.value() << std::endl;
     *     }
     *
     * Caveat: this operator does no boundary check. If you need this check.
     * use at() instead.
     */
    inline LexerToken operator[](const std::size_t index) const
    {
        return tokens_[index];
    }

    /**
     * @brief Provides index based array acces to the tokens, doing a
     * boundary check first.
     *
     * In out of bounds cases, a special EOF token is returned.
     */
    inline LexerToken at(const std::size_t index) const
    {
        if (index < tokens_.size())
            return tokens_[index];
        else
            return LexerToken(LexerTokenType::kEOF, 0, 0, "");
    }

    /**
     * @brief Returns a reference to the first token.
     *
     * Calling this function on an empty() lexer causes undefined behavior.
     */
    inline LexerToken front() const
    {
        return tokens_.front();
    }

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
     * This is usually the case before Analyze was run.
     */
    inline bool empty() const
    {
        return tokens_.empty();
    }

    /** @brief Returns the number of tokens produced during the analysis. */
    inline std::size_t size() const
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
        // use swap to make sure vector is of size 0
        std::vector<LexerToken>().swap(tokens_);
    }

    inline bool HasError() const
    {
        return !tokens_.empty() && tokens_.back().type() == kError;
    }

    /** @brief Determines whether whitespaces are included as tokens. */
    bool include_whitespace = false;

    /** @brief Determines whether comments are included as tokens. */
    bool include_comments = false;

    /*
     * @brief If set, comments are stripped from the text before starting
     * the analysis.
     *
     * This is useful when the specification of the text allows comments to
     * appear anywhere (e.g. Newick trees). If it is not set, comments are
     * only found at the borders between tokens, but not within them
     * (for example, within a number).
     */
    // TODO make this option available
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
     * Strings can be enclosed in 'abc' or "def", see ScanString for more
     * details on that. The value of trim_quotation_marks determines
     * whether those marks are included in the final token or not.
     * Default is to not include them, which makes preprocessing of the
     * string easier.
     */
    bool trim_quotation_marks = true;

    /**
     * @brief Determines whether to call StringDeescape for literal strings.
     *
     * This only affects literal strings, typically enclosed in 'abc' or
     * "def".
     */
    bool use_string_escape = false;

    /**
     * @brief If set, doubled quotation marks in a string are considered
     * as normal quotation marks without ending the string.
     *
     * For example, the character sequecen (including all quotation marks)
     *
     *     "For learning C++, ""Hello World"" is a good start."
     *
     * will be interpreted as a string containing normal quotation marks
     * around `"Hello World"`.
     *
     * The type of quotation marks used here depends on which chars are set
     * to LexerTokenType kString using SetCharType.
     */
    bool use_string_doubled_quotes = false;

protected:
    // TODO find out about virtual inline functions

    bool ScanFromTo (const char* from, const char* to);
    virtual bool ScanUnknown();
    virtual bool ScanWhitespace();
    virtual bool ScanComment();
    virtual bool ScanSymbol();
    virtual bool ScanNumber();
    virtual bool ScanString();
    virtual bool ScanOperator();
    virtual bool ScanBracket();
    virtual bool ScanTag();

    inline void Init (const std::string& text)
    {
        text_ = text.c_str();
        itr_  = 0;
        len_  = text.size();
        line_ = 1;
        tokens_.clear();
    }

    /** @brief Returns the current iterator position. */
    inline size_t GetPosition() const
    {
        return itr_;
    }

    /**
     * @brief Returns the char at the iterator position plus an offset.
     *
     * If the position is not inside the text, a null char is returned.
     *
     * It can be used for a save version to get the current char by using
     * `GetChar(0);` instead of `GetChar();`.
     */
    inline char GetChar(const int offset) const
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
     * it should be used in combination with IsEnd only.
     */
    inline char GetChar() const
    {
        return text_[itr_];
    }

    inline LexerTokenType GetCharType(const char c) const
    {
        // we use char [-128,127] here.
        if (c < 0) {
            return kError;
        } else {
            return start_char_table_[static_cast<unsigned char>(c)];
        }
    }

    /**
     * @brief Returns the token type of the current char.
     *
     * Caveat: The function does not do a boundary check to see if the
     * current char in inside the text. Thus, the function should only be
     * used in combination with IsEnd.
     */
    inline LexerTokenType GetCharType() const
    {
        return GetCharType(GetChar());
    }

    inline void SetCharType (const LexerTokenType type, std::string chars)
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
    inline void NextChar()
    {
        ++itr_;

        // we use the save version of GetChar here, which is equivalant to
        // check for IsEnd. first, CR, then LF. the second condition ensures
        // not to count a CR+LF as two line increases.
        if (GetChar(0) == '\r') {
            ++line_;
        }
        if (GetChar(0) == '\n' && GetChar(-1) != '\r') {
            ++line_;
        }
    }

    /**
     * @brief True if the internal iterator is at the end of the text.
     */
    inline bool IsEnd() const
    {
        return itr_ >= len_;
    }

    /**
     * @brief True if the internal iterator plus some offset is at the
     * end of the text.
     */
    inline bool IsEnd(int offset) const
    {
        return itr_ + offset >= len_;
    }

    /**
     * @brief Extracts a substring of the text betweeen two positions, end
     * excluded.
     */
    inline std::string GetSubstr (size_t start, size_t end) const
    {
        if (start<end) {
            return std::string(text_+start, end-start);
        } else {
            return std::string("");
        }
    }

    /** @brief Create a token and push it to the list. */
    inline void PushToken (
        const LexerTokenType t,
        const size_t start, const std::string value
    ) {
        // find previous new line, we need it to tell the column
        // (first check LF, to handle CR+LF correctly)
        size_t bitr = start;
        while (
            bitr > 0 && (text_[bitr-1] != '\n' || text_[bitr-1] != '\r')
        ) {
            --bitr;
        }

        // make and push token
        LexerToken tok(t, line_, start-bitr, value);
        tokens_.push_back(tok);
    }

    /** @brief Create a token and push it to the list. */
    inline void PushToken (
        const LexerTokenType t, const size_t start, const size_t end
    ) {
        PushToken(t, start, GetSubstr(start, end));
    }

private:
    LexerTokenType start_char_table_[128] = {
        /*      */  kError,     kError,     kError,     kError,
        /*      */  kError,     kError,     kError,     kError,
        /*      */  kError,     kWhite,     kWhite,     kWhite,
        /*      */  kWhite,     kWhite,     kError,     kError,
        /*      */  kError,     kError,     kError,     kError,
        /*      */  kError,     kError,     kError,     kError,
        /*      */  kError,     kError,     kError,     kError,
        /*      */  kError,     kError,     kError,     kError,
        /*  !"# */  kWhite,     kUnknown,   kUnknown,   kUnknown,
        /* $%&' */  kUnknown,   kUnknown,   kUnknown,   kUnknown,
        /* ()*+ */  kUnknown,   kUnknown,   kUnknown,   kUnknown,
        /* ,-./ */  kUnknown,   kUnknown,   kUnknown,   kUnknown,
        /* 0123 */  kNumber,    kNumber,    kNumber,    kNumber,
        /* 4567 */  kNumber,    kNumber,    kNumber,    kNumber,
        /* 89:; */  kNumber,    kNumber,    kUnknown,   kUnknown,
        /* <=>? */  kUnknown,   kUnknown,   kUnknown,   kUnknown,
        /* @ABC */  kUnknown,   kSymbol,    kSymbol,    kSymbol,
        /* DEFG */  kSymbol,    kSymbol,    kSymbol,    kSymbol,
        /* HIJK */  kSymbol,    kSymbol,    kSymbol,    kSymbol,
        /* LMNO */  kSymbol,    kSymbol,    kSymbol,    kSymbol,
        /* PQRS */  kSymbol,    kSymbol,    kSymbol,    kSymbol,
        /* TUVW */  kSymbol,    kSymbol,    kSymbol,    kSymbol,
        /* XYZ[ */  kSymbol,    kSymbol,    kSymbol,    kUnknown,
        /* \]^_ */  kUnknown,   kUnknown,   kUnknown,   kUnknown,
        /* `abc */  kUnknown,   kSymbol,    kSymbol,    kSymbol,
        /* defg */  kSymbol,    kSymbol,    kSymbol,    kSymbol,
        /* hijk */  kSymbol,    kSymbol,    kSymbol,    kSymbol,
        /* lmno */  kSymbol,    kSymbol,    kSymbol,    kSymbol,
        /* pqrs */  kSymbol,    kSymbol,    kSymbol,    kSymbol,
        /* tuvw */  kSymbol,    kSymbol,    kSymbol,    kSymbol,
        /* xyz{ */  kSymbol,    kSymbol,    kSymbol,    kUnknown,
        /* |}~  */  kUnknown,   kUnknown,   kUnknown,   kError
    };

    // Caveat: the following variables are heavily interweaved during a run
    // of Analyze! They have to stay consistent, otherwise the resulting
    // tokens will contain wrong information.

    /** @brief The text that is being analyzed. */
    const char* text_ = "\0";

    /** @brief The current position in the text while analyzing. */
    size_t      itr_  = 0;

    /** @brief The length of the text being analyzed. */
    size_t      len_  = 0;

    /** @brief The current line in the text while analyzing. */
    int         line_ = 1;

    /** @brief The list of tokens resulting from the analysis. */
    std::vector<LexerToken> tokens_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
