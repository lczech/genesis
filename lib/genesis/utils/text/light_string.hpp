#ifndef GENESIS_UTILS_TEXT_LIGHT_STRING_H_
#define GENESIS_UTILS_TEXT_LIGHT_STRING_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/std.hpp"

#include <cassert>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    #include <string_view>

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

namespace genesis {
namespace utils {

// =================================================================================================
//     Lightweight String
// =================================================================================================

/**
 * @brief Lightweight string class.
 *
 * Can be used as a replacement for std::string in situations where many small strings need to be
 * used, and the overhead for the three pointers of a standard string is too much. This class
 * instead works similar to a C-string, in that it keeps a single pointer to a null-terminated
 * char sequence. It is however a standard container
 */
class LightString
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    /**
     * @brief  Default constructor creates an empty string ("")
     */
    LightString()
        : data_( std::make_unique<char[]>( 1 ))
    {
        data_[0] = '\0';
    }

    /**
     * @brief Construct from a C-style null-terminated string
     */
    explicit LightString( char const* str )
    {
        if( str ) {
            // Get the length of the string and allocate space (including null terminator)
            size_t len = std::strlen(str);
            data_ = std::make_unique<char[]>(len + 1);
            std::memcpy(data_.get(), str, len + 1);
        } else {
            // If null pointer provided, create an empty string
            data_ = std::make_unique<char[]>(1);
            data_[0] = '\0';
        }
    }

    /**
     * @brief Construct from a std::string
     */
    explicit LightString( std::string const& s )
        : LightString( s.c_str() )
    {}

    #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    /**
     * @brief Constructor from a std::string_view
     */
    explicit LightString( std::string_view sv )
    {
        size_t len = sv.size();
        data_ = std::make_unique<char[]>(len + 1);
        std::memcpy(data_.get(), sv.data(), len);
        data_[len] = '\0';
    }

    #endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    /**
     * @brief Copy constructor - deep copies the internal C-string
     */
    LightString( LightString const& other )
    {
        assert( other.data_ );
        size_t len = std::strlen(other.data_.get());
        data_ = std::make_unique<char[]>(len + 1);
        std::memcpy(data_.get(), other.data_.get(), len + 1);
    }

    /**
     * @brief Move constructor - transfers ownership of the data
     */
    LightString( LightString&& other ) noexcept
        : data_( std::move( other.data_ ))
    {}

    /**
     * @brief Copy assignment operator - deep copies the C-string from the other object
     */
    LightString& operator=( LightString const& other )
    {
        assert( other.data_ );
        if( this != &other ) {
            size_t len = std::strlen(other.data_.get());
            data_ = std::make_unique<char[]>(len + 1);
            std::memcpy(data_.get(), other.data_.get(), len + 1);
        }
        return *this;
    }

    /**
     * @brief Move assignment operator - transfers ownership
     */
    LightString& operator=( LightString&& other ) noexcept
    {
        assert( other.data_ );
        if( this != &other ) {
            data_ = std::move( other.data_ );
        }
        return *this;
    }

    /**
     * @brief Default destructor
     */
    ~LightString() = default;

    /**
     * @brief Efficient swapping of two LightString objects
     */
    void swap( LightString& other ) noexcept
    {
        assert( data_ );
        assert( other.data_ );
        data_.swap(other.data_);
    }

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    /**
     * @brief Returns the length of the string (not including the null terminator)
     */
    size_t size() const
    {
        assert( data_ );
        return std::strlen(data_.get());
    }

    /**
     * @brief Returns true if the string is empty (i.e., its size is zero)
     */
    bool empty() const
    {
        return size() == 0;
    }

    // ---------------------------------------------------------------------
    //     Element Accessors
    // ---------------------------------------------------------------------

    /**
     * @brief Element access operator (non-const and const versions)
     */
    char& operator[](size_t index)
    {
        assert( data_ );
        return data_.get()[index];
    }

    /**
     * @brief Element access operator (non-const and const versions)
     */
    char const& operator[](size_t index) const
    {
        assert( data_ );
        return data_.get()[index];
    }

    /**
     * @brief at() method with bounds checking
     */
    char& at( size_t index )
    {
        assert( data_ );
        if( index >= size() ) {
            throw std::out_of_range("LightString::at(): index out of range");
        }
        return data_.get()[index];
    }

    /**
     * @brief at() method with bounds checking
     */
    char const& at( size_t index ) const
    {
        assert( data_ );
        if( index >= size() ) {
            throw std::out_of_range("LightString::at(): index out of range");
        }
        return data_.get()[index];
    }

    /**
     * @brief Mutable begin iterator for the char content of the string.
     */
    char* begin()
    {
        return data_.get();
    }

    /**
     * @brief Const begin iterator for the char content of the string.
     */
    char const* begin() const
    {
        return data_.get();
    }

    /**
     * @brief Mutable end iterator for the char content of the string.
     */
    char* end()
    {
        return data_.get() + size();
    }

    /**
     * @brief Const end iterator for the char content of the string.
     */
    char const* end() const
    {
        return data_.get() + size();
    }

    // ---------------------------------------------------------------------
    //     Data Accessors
    // ---------------------------------------------------------------------

    /**
     * @brief Returns a pointer to the null-terminated C-string
     */
    char const* c_str() const
    {
        assert( data_ );
        return data_.get();
    }

    /**
     * @brief Returns a pointer to the internal C-string, same as c_str()
     */
    char const* data() const
    {
        assert( data_ );
        return data_.get();
    }

    /**
     * @brief Conversion operator to std::string for convenience
     */
    std::string to_string() const
    {
        assert( data_ );
        return std::string(c_str());
    }

    #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    /**
     * @brief Member function to return a std::string_view representing the string
     */
    std::string_view to_string_view() const
    {
        assert( data_ );
        return std::string_view(data_.get(), size());
    }

    #endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    // ---------------------------------------------------------------------
    //     Comparisons
    // ---------------------------------------------------------------------

    /**
     * @brief Equality comparison based on string content
     */
    bool operator==( LightString const& other ) const
    {
        return std::strcmp(c_str(), other.c_str()) == 0;
    }

    /**
     * @brief Less-than operator for ordering, based on lexicographical order
     */
    bool operator<( LightString const& other ) const
    {
        return std::strcmp(c_str(), other.c_str()) < 0;
    }

    /**
     * @brief Compare two LightString%s.
     *
     * THis behaves like std::string::compare. Returns 0 if equal, negative if *this < other,
     * positive if *this > other.
     */
    int compare( LightString const& other ) const
    {
        return std::strcmp(c_str(), other.c_str());
    }

    // ---------------------------------------------------------------------
    //     Internal Members
    // ---------------------------------------------------------------------

private:

    // Private constructor that takes ownership of an already allocated buffer.
    // The buffer must be null-terminated.
    explicit LightString( std::unique_ptr<char[]> data )
        : data_( std::move( data ))
    {
        assert( data_ );
        assert( data_.get()[ size() ] == '\0' );
    }

    // Friend declaration needed for non-member operator+ to access this constructor.
    friend LightString operator+( LightString const& lhs, LightString const& rhs );

    // ---------------------------------------------------------------------
    //     Member Data
    // ---------------------------------------------------------------------

private:

    // Unique pointer owning a dynamically allocated null-terminated char array
    std::unique_ptr<char[]> data_;

};

// =================================================================================================
//     Operators
// =================================================================================================

// ---------------------------------------------------------------------
//     Stream
// ---------------------------------------------------------------------

/**
 * @brief Overload the stream output operator for LightString.
 */
std::ostream& operator<<( std::ostream& os, LightString const& ls )
{
    os << ls.c_str();
    return os;
}

// ---------------------------------------------------------------------
//     Self-Comparison
// ---------------------------------------------------------------------

// Equality (==) and less than (<) are already implemented as member functions.

/**
 * @brief Define the inequality operator.
 */
inline bool operator!=( LightString const& lhs,  LightString const& rhs )
{
    return !(lhs == rhs);
}

/**
 * @brief Define the greater-than operator.
 */
inline bool operator>( LightString const& lhs,  LightString const& rhs )
{
    return rhs < lhs;
}

/**
 * @brief Define the less-than-or-equal operator.
 */
inline bool operator<=( LightString const& lhs,  LightString const& rhs )
{
    return !(rhs < lhs);
}

/**
 * @brief Define the greater-than-or-equal operator.
 */
inline bool operator>=( LightString const& lhs,  LightString const& rhs )
{
    return !(lhs < rhs);
}

// ---------------------------------------------------------------------
//     Comparison with std::string
// ---------------------------------------------------------------------

/**
 * @brief Equality comparisons between LightString and std::string
 */
inline bool operator==( LightString const& ls, std::string const& s )
{
    return std::strcmp( ls.c_str(), s.c_str() ) == 0;
}

/**
 * @brief Equality comparisons between LightString and std::string
 */
inline bool operator==( std::string const& s, LightString const& ls)
{
    return std::strcmp( s.c_str(), ls.c_str() ) == 0;
}

/**
 * @brief Inequality comparisons between LightString and std::string
 */
inline bool operator!=( LightString const& ls, std::string const& s )
{
    return !(ls == s);
}

/**
 * @brief Inequality comparisons between LightString and std::string
 */
inline bool operator!=( std::string const& s, LightString const& ls )
{
    return !(s == ls);
}

/**
 * @brief Less-than comparisons between LightString and std::string
 */
inline bool operator<( LightString const& ls, std::string const& s)
{
    return std::strcmp( ls.c_str(), s.c_str() ) < 0;
}

/**
 * @brief Less-than comparisons between LightString and std::string
 */
inline bool operator<( std::string const& s, LightString const& ls)
{
    return std::strcmp( s.c_str(), ls.c_str() ) < 0;
}

/**
 * @brief Greater-than comparisons between LightString and std::string
 */
inline bool operator>( LightString const& ls, std::string const& s )
{
    return s < ls;
}

/**
 * @brief Greater-than comparisons between LightString and std::string
 */
inline bool operator>( std::string const& s, LightString const& ls )
{
    return ls < s;
}

/**
 * @brief Less-than-or-equal comparisons between LightString and std::string
 */
inline bool operator<=( LightString const& ls, std::string const& s )
{
    return !(s < ls);
}

/**
 * @brief Less-than-or-equal comparisons between LightString and std::string
 */
inline bool operator<=( std::string const& s, LightString const& ls )
{
    return !(ls < s);
}

/**
 * @brief Greater-than-or-equal comparisons between LightString and std::string
 */
inline bool operator>=( LightString const& ls, std::string const& s )
{
    return !(ls < s);
}

/**
 * @brief Greater-than-or-equal comparisons between LightString and std::string
 */
inline bool operator>=( std::string const& s, LightString const& ls )
{
    return !(s < ls);
}

// ---------------------------------------------------------------------
//     Comparison with std::string_view
// ---------------------------------------------------------------------

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

/**
 * @brief Equality comparisons between LightString and std::string_view
 */
inline bool operator==( LightString const& ls, std::string_view sv )
{
    return ls.to_string_view() == sv;
}

/**
 * @brief Equality comparisons between LightString and std::string_view
 */
inline bool operator==( std::string_view sv, LightString const& ls )
{
    return sv == ls.to_string_view();
}

/**
 * @brief Inequality comparisons between LightString and std::string_view
 */
inline bool operator!=( LightString const& ls, std::string_view sv )
{
    return !(ls == sv);
}

/**
 * @brief Inequality comparisons between LightString and std::string_view
 */
inline bool operator!=( std::string_view sv, LightString const& ls )
{
    return !(sv == ls);
}

/**
 * @brief Less-than comparisons between LightString and std::string_view
 */
inline bool operator<( LightString const& ls, std::string_view sv )
{
    return ls.to_string_view() < sv;
}

/**
 * @brief Less-than comparisons between LightString and std::string_view
 */
inline bool operator<( std::string_view sv, LightString const& ls )
{
    return sv < ls.to_string_view();
}

/**
 * @brief Greater-than comparisons between LightString and std::string_view
 */
inline bool operator>( LightString const& ls, std::string_view sv )
{
    return sv < ls;
}

/**
 * @brief Greater-than comparisons between LightString and std::string_view
 */
inline bool operator>( std::string_view sv, LightString const& ls )
{
    return ls < sv;
}

/**
 * @brief Less-than-or-equal comparisons between LightString and std::string_view
 */
inline bool operator<=( LightString const& ls, std::string_view sv )
{
    return !(sv < ls);
}

/**
 * @brief Less-than-or-equal comparisons between LightString and std::string_view
 */
inline bool operator<=( std::string_view sv, LightString const& ls )
{
    return !(ls < sv);
}

/**
 * @brief Greater-than-or-equal comparisons between LightString and std::string_view
 */
inline bool operator>=( LightString const& ls, std::string_view sv )
{
    return !(ls < sv);
}

/**
 * @brief Greater-than-or-equal comparisons between LightString and std::string_view
 */
inline bool operator>=( std::string_view sv, LightString const& ls )
{
    return !(sv < ls);
}

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

// ---------------------------------------------------------------------
//     Comparison with C-style strings ( char const*)
// ---------------------------------------------------------------------

/**
 * @brief Equality comparisons between LightString and `char const*`
 */
inline bool operator==( LightString const& ls, char const* s )
{
    return std::strcmp( ls.c_str(), s ) == 0;
}

/**
 * @brief Equality comparisons between LightString and `char const*`
 */
inline bool operator==( char const* s, LightString const& ls )
{
    return std::strcmp( s, ls.c_str() ) == 0;
}

/**
 * @brief Inequality comparisons between LightString and `char const*`
 */
inline bool operator!=( LightString const& ls, char const* s )
{
    return !(ls == s);
}

/**
 * @brief Inequality comparisons between LightString and `char const*`
 */
inline bool operator!=( char const* s, LightString const& ls )
{
    return !(s == ls);
}

/**
 * @brief Less-than comparisons between LightString and `char const*`
 */
inline bool operator<( LightString const& ls, char const* s )
{
    return std::strcmp( ls.c_str(), s ) < 0;
}

/**
 * @brief Less-than comparisons between LightString and `char const*`
 */
inline bool operator<( char const* s, LightString const& ls )
{
    return std::strcmp( s, ls.c_str() ) < 0;
}

/**
 * @brief Greater-than comparisons between LightString and `char const*`
 */
inline bool operator>( LightString const& ls, char const* s )
{
    return s < ls;
}

/**
 * @brief Greater-than comparisons between LightString and `char const*`
 */
inline bool operator>( char const* s, LightString const& ls )
{
    return ls < s;
}

/**
 * @brief Less-than-or-equal comparisons between LightString and `char const*`
 */
inline bool operator<=( LightString const& ls, char const* s )
{
    return !(s < ls);
}

/**
 * @brief Less-than-or-equal comparisons between LightString and `char const*`
 */
inline bool operator<=( char const* s, LightString const& ls )
{
    return !(ls < s);
}

/**
 * @brief Greater-than-or-equal comparisons between LightString and `char const*`
 */
inline bool operator>=( LightString const& ls, char const* s )
{
    return !(ls < s);
}

/**
 * @brief Greater-than-or-equal comparisons between LightString and `char const*`
 */
inline bool operator>=( char const* s, LightString const& ls )
{
    return !(s < ls);
}

// ---------------------------------------------------------------------
//     Concat LightString with other string types
// ---------------------------------------------------------------------

/**
 * @brief Concatenate two LightString%s into a new LightString
 */
inline LightString operator+( LightString const& lhs, LightString const& rhs )
{
    // Allocate memory for the concatenated string (including null terminator),
    // and copy over the data of both.
    size_t new_size = lhs.size() + rhs.size();
    auto buffer = std::make_unique<char[]>(new_size + 1);
    std::memcpy(buffer.get(), lhs.c_str(), lhs.size());
    std::memcpy(buffer.get() + lhs.size(), rhs.c_str(), rhs.size());
    buffer[new_size] = '\0';

    // Use the private constructor to move the buffer into a new LightString instance.
    return LightString(std::move(buffer));
}

/**
 * @brief Concatenation of LightString with a C-string on the left
 */
inline LightString operator+( char const* lhs, LightString const& rhs )
{
    return LightString(lhs) + rhs;
}

/**
 * @brief Concatenation of LightString with a C-string on the right
 */
inline LightString operator+( LightString const& lhs,  char const* rhs )
{
    return lhs + LightString(rhs);
}

/**
 * @brief Concatenation of LightString with std::string on the left, returning a std::string
 */
inline std::string operator+( std::string const& lhs, LightString const& rhs )
{
    return lhs + std::string(rhs.c_str());
}

/**
 * @brief Concatenation of LightString with std::string on the right, returning a std::string
 */
inline std::string operator+( LightString const& lhs, std::string const& rhs )
{
    return std::string(lhs.c_str()) + rhs;
}

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

/**
 * @brief Concatenation of LightString with std::string_view on the left
 */
inline LightString operator+( std::string_view lhs, LightString const& rhs )
{
    return LightString( std::string(lhs)) + rhs;
}

/**
 * @brief Concatenation of LightString with std::string_view on the right
 */
inline LightString operator+( LightString const& lhs, std::string_view rhs )
{
    return lhs + LightString( std::string(rhs));
}

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

} // namespace utils
} // namespace genesis

// ---------------------------------------------------------------------
//     Hash
// ---------------------------------------------------------------------

namespace std
{

#if GENESIS_CPP_STD >= GENESIS_CPP_STD_17

    /**
     * @brief Provide a hash specialization for LightString
     */
    template<>
    struct hash<::genesis::utils::LightString>
    {
        inline std::size_t operator()( ::genesis::utils::LightString const& ls ) const noexcept
        {
            // Construct a non-owning string_view from the LightString data without copying,
            // and use std::hash<std::string_view> which is typically equivalent to std::hash<std::string>
            return std::hash<std::string_view>{}( std::string_view( ls.c_str(), ls.size() ));
        }
    };

#else

    /**
     * @brief Provide a hash specialization for LightString
     *
     * Caveat: This internally constructs a string, in order to get compatible hash values
     * to a normal `std::string`. If you need performance, compile with C++17 or later instead,
     * such that we can use a std::string_view here for performance.
     */
    template<>
    struct hash<::genesis::utils::LightString>
    {
        std::size_t operator()( ::genesis::utils::LightString const& ls ) const
        {
            // Use std::hash<std::string> on the converted std::string.
            // This is a simple solution and works well if hashing performance is not critical.
            return std::hash<std::string>()(ls.to_string());
        }
    };

#endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

} // namespace std

#endif // include guard
