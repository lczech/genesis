#ifndef GENESIS_UTILS_IO_LEXER_ITERATOR_H_
#define GENESIS_UTILS_IO_LEXER_ITERATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/core/logging.hpp"

namespace genesis {
namespace utils {

// =============================================================================
//     Lexer Iterator
// =============================================================================

/**
 * @brief Iterator for the tokens of a Lexer, with the capability of in-time production and
 * consumption of tokens to save memory.
 *
 * This is a drop-in replacement for the default iterator of the token container used by Lexer,
 * but with two extra features: It can consume tokens after they have been processed (see
 * ConsumeWithTail() for more information), and it can produce tokens just before they are beeing
 * accessed (see ProduceWithHead() for more).
 */
class LexerIterator
{
public:

    // -----------------------------------------------------
    //     Constructor and Typedefs
    // -----------------------------------------------------

    typedef std::bidirectional_iterator_tag iterator_category;

    LexerIterator (Lexer* lexer, long position) :
        lexer_    {lexer},
        position_ {position}
    {}

    // -----------------------------------------------------
    //     Member Functions
    // -----------------------------------------------------

    inline Lexer& get_lexer() const
    {
        return *lexer_;
    }

    /**
     * @brief Returns whether there appeared an error while lexing.
     */
    inline bool has_error() const
    {
        return !lexer_->tokens_.empty() && lexer_->tokens_.back().is_error();
    }

    /**
     * @brief Removes all tokens up to (but excluding) the current one.
     *
     * This is a slow operation, thus should not be called in the inner parsing loop. It is meant
     * to save memory when reading and parsing large files.
     */
    inline void consume_head()
    {
        lexer_->tokens_.erase(lexer_->tokens_.begin(), lexer_->tokens_.begin() + position_);
        position_ = 0;
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    inline LexerIterator& operator ++ ()
    {
        // if we are already at the end, do not do anything
        if (position_ < 0) {
            LOG_WARN << "Already reached end of Lexer Tokens.";
            return *this;
        }

        // go to next position, produce token if needed, and set position to end marker when done.
        ++position_;
        while (lexer_->tokens_.size() <= static_cast<size_t> (position_)) {
            if (!lexer_->process_step()) {
                position_ = -1;
                break;
            }
        }

        return *this;
    }

    inline LexerIterator operator ++ (int)
    {
        LexerIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    inline LexerIterator& operator -- ()
    {
        if (position_ == 0) {
            LOG_WARN << "Cannot decrease Lexer Iterator any more.";
        }
        if (position_ > 0) {
            --position_;
        }

        return *this;
    }

    inline LexerIterator operator -- (int)
    {
        LexerIterator tmp = *this;
        --(*this);
        return tmp;
    }

    inline bool operator == (const LexerIterator &other) const
    {
        return other.lexer_ == lexer_ && other.position_ == position_;
    }

    inline bool operator != (const LexerIterator &other) const
    {
        return !(other == *this);
    }

    inline LexerToken* operator -> () const
    {
        return &(lexer_->tokens_[position_]);
    }

protected:

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    Lexer* lexer_;
    long   position_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
