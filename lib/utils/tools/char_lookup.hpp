#ifndef GENESIS_UTILS_TOOLS_CHAR_LOOKUP_H_
#define GENESIS_UTILS_TOOLS_CHAR_LOOKUP_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */


#include <array>
#include <assert.h>
#include <functional>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Chars Lookup Table
// =================================================================================================

/**
 * @brief Simple lookup table providing a binary state for each char.
 *
 * This char lookup table class is mostly useful for checking valid chars when reading data.
 * By default (after using the default constructor), the lookup status of all chars is `false`.
 */
class CharLookup
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    CharLookup()
        : table_()
    {}

    ~CharLookup() = default;

    CharLookup(CharLookup const&) = default;
    CharLookup(CharLookup&&)      = default;

    CharLookup& operator= (CharLookup const&) = default;
    CharLookup& operator= (CharLookup&&)      = default;

    // -------------------------------------------------------------------------
    //     Setters
    // -------------------------------------------------------------------------

    /**
     * @brief Set the lookup status for a given char.
     *
     * The set value is `true` by default.
     */
    void set_char( char c, bool value = true )
    {
        assert(0 <= c);
        table_[c] = value;
    }

    /**
     * @brief Set the lookup status for all chars that fulfill a given predicate.
     *
     * The set value is `true` by default.
     */
    void set_if( std::function<bool ( char )> predicate, bool value = true )
    {
        for (unsigned char c = 0; c < 128; ++c) {
            if (predicate(c)) {
                table_[c] = value;
            }
        }
    }

    /**
     * @brief Set the lookup status for all chars that are contained in a given std::string.
     *
     * The set value is `true` by default.
     */
    void set_selection( std::string const& chars, bool value = true )
    {
        for (char c : chars) {
            assert( 0 <= c);
            table_[static_cast<unsigned char>(c)] = value;
        }
    }

    /**
     * @brief Set the lookup status for all chars in an inlcuding range between two chars.
     *
     * The set value is `true` by default.
     */
    void set_range( char first, char last, bool value = true )
    {
        for (auto c = first; c <= last; ++c) {
            assert(0 <= c);
            table_[c] = value;
        }
    }

    /**
     * @brief Set the lookup status for all chars at once.
     *
     * The set value is `true` by default.
     */
    void set_all( bool value = true )
    {
        for( unsigned char c = 0; c < 128; ++c ) {
            table_[c] = value;
        }
    }

    // -------------------------------------------------------------------------
    //     Getters
    // -------------------------------------------------------------------------

    /**
     * @brief Return the lookup status for a given char.
     *
     * We only provide the const getter version of this operator in order to avoid accidentally
     * setting the value. Use one of the `set...()` functions explicitily for this.
     */
    bool operator [] ( char c ) const
    {
        return table_[c];
    }

    /**
     * @brief Return the lookup status for a given char.
     */
    bool get( char c ) const
    {
        return table_[c];
    }

    /**
     * @brief Return a `std::string` containg all chars which have lookup status `true`.
     */
    std::string get_selection() const
    {
        std::string ret;
        for( size_t c = 0; c < 128; ++c ) {
            if( get(c) ) {
                ret += c;
            }
        }
        return ret;
    }

    /**
     * @brief Return whether all chars are set to `true`.
     */
    bool all_set() const
    {
        for( size_t c = 0; c < 128; ++c ) {
            if( !get(c) ) {
                return false;
            }
        }
        return true;
    }


    /**
     * @brief Return whether all chars are set to `false`.
     */
    bool all_unset() const
    {
        for( size_t c = 0; c < 128; ++c ) {
            if( get(c) ) {
                return false;
            }
        }
        return true;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::array<bool, 128> table_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
