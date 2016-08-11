#ifndef GENESIS_UTILS_TOOLS_CHAR_LOOKUP_H_
#define GENESIS_UTILS_TOOLS_CHAR_LOOKUP_H_

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
 * @brief Simple lookup table providing a value lookup for each ASCII char (0-127).
 *
 * The class provides a fast mapping from each char to the provided template parameter type.
 */
template< typename T >
class CharLookup
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using value_type = T;
    using key_type   = char;

    static const size_t ArraySize = 128;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Constructor that sets all values to the default value of the template parameter type.
     */
    CharLookup()
        : CharLookup( T{} )
    {}

    /**
     * @brief Constructor that sets all values to a given one.
     */
    CharLookup( T const& init_all )
    {
        for( size_t i = 0; i < ArraySize; ++i ) {
            table_[i] = init_all;
        }
    }

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
     */
    void set_char( char c, T value )
    {
        assert( 0 <= c );
        table_[ static_cast<unsigned char>(c) ] = value;
    }

    /**
     * @brief Set the lookup status for both the upper and lower case of a given char.
     */
    void set_char_upper_lower( char c, T value )
    {
        assert( 0 <= c );
        table_[ static_cast<unsigned char>( toupper(c) )] = value;
        table_[ static_cast<unsigned char>( tolower(c) )] = value;
    }

    /**
     * @brief Set the lookup status for all chars that fulfill a given predicate.
     */
    void set_if( std::function<bool ( char )> predicate, T value )
    {
        for( unsigned char c = 0; c < ArraySize; ++c ) {
            if( predicate(c) ) {
                table_[ c ] = value;
            }
        }
    }

    /**
     * @brief Set the lookup status for all chars that are contained in a given std::string.
     */
    void set_selection( std::string const& chars, T value )
    {
        for( char c : chars ) {
            assert( 0 <= c );
            table_[ static_cast<unsigned char>(c) ] = value;
        }
    }

    /**
     * @brief Set the lookup status for both the upper and lower case of all chars that are
     * contained in a given std::string.
     */
    void set_selection_upper_lower( std::string const& chars, T value )
    {
        for( char c : chars ) {
            assert( 0 <= c );
            table_[ static_cast<unsigned char>( toupper(c) )] = value;
            table_[ static_cast<unsigned char>( tolower(c) )] = value;
        }
    }

    /**
     * @brief Set the lookup status for all chars in an inlcuding range between two chars.
     */
    void set_range( char first, char last, T value )
    {
        for( auto c = first; c <= last; ++c ) {
            assert( 0 <= c );
            table_[ static_cast<unsigned char>(c) ] = value;
        }
    }

    /**
     * @brief Set the lookup status for all chars at once.
     */
    void set_all( T value )
    {
        for( unsigned char c = 0; c < 128; ++c ) {
            table_[ c ] = value;
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
    T operator [] ( char c ) const
    {
        assert( 0 <= c );
        return table_[ static_cast<unsigned char>(c) ];
    }

    /**
     * @brief Return the lookup status for a given char.
     */
    T get( char c ) const
    {
        assert( 0 <= c );
        return table_[ static_cast<unsigned char>(c) ];
    }

    /**
     * @brief Return a `std::string` containg all chars which have lookup status equal to a
     * given value.
     */
    std::string get_chars_equal_to( T comp_value ) const
    {
        std::string ret;
        for( unsigned char c = 0; c < 128; ++c ) {
            if( get(c) == comp_value ) {
                ret += c;
            }
        }
        return ret;
    }

    /**
     * @brief Return whether all chars compare equal to a given value.
     */
    bool all_equal_to( T comp_value ) const
    {
        for( unsigned char c = 0; c < 128; ++c ) {
            if( get(c) != comp_value ) {
                return false;
            }
        }
        return true;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::array< T, ArraySize > table_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
