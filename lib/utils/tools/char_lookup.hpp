#ifndef GENESIS_UTILS_CHAR_LOOKUP_H_
#define GENESIS_UTILS_CHAR_LOOKUP_H_

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

    void set( char c, bool value = true )
    {
        assert(0 <= c);
        table_[c] = value;
    }

    void set_if( std::function<bool ( char )> predicate, bool value = true )
    {
        for (unsigned char c = 0; c < 128; ++c) {
            if (predicate(c)) {
                table_[c] = value;
            }
        }
    }

    void set_selection( std::string const& chars, bool value = true )
    {
        for (char c : chars) {
            assert( 0 <= c);
            table_[static_cast<unsigned char>(c)] = value;
        }
    }

    void set_range( char first, char last, bool value = true )
    {
        for (auto c = first; c <= last; ++c) {
            assert(0 <= c);
            table_[c] = value;
        }
    }

    void set_all( bool value = true )
    {
        for( unsigned char c = 0; c < 128; ++c ) {
            table_[c] = value;
        }
    }

    // -------------------------------------------------------------------------
    //     Getters
    // -------------------------------------------------------------------------

    bool get( char c ) const
    {
        return table_[c];
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
