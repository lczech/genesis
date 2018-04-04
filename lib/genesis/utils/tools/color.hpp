#ifndef GENESIS_UTILS_TOOLS_COLOR_H_
#define GENESIS_UTILS_TOOLS_COLOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @brief Header of Color class.
 *
 * @file
 * @ingroup utils
 */

#include <string>
#include <utility> // swap

namespace genesis {
namespace utils {

// =================================================================================================
//     Color
// =================================================================================================

/**
 * @brief
 */
class Color
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Default constructor. Sets the color to black.
     */
    Color()
        : r_( 0.0 ), g_( 0.0 ), b_( 0.0 ), a_( 1.0 )
    {}

    /**
     * @brief Constructor for setting the RGB value.
     */
    Color( double r, double g, double b )
        : Color( r, g, b, 1.0 )
    {}

    /**
     * @brief Constructor for setting the RGB value including alpha.
     */
    Color( double r, double g, double b, double a );

    ~Color() = default;

    Color(Color const&) = default;
    Color(Color&&)      = default;

    Color& operator= (Color const&) = default;
    Color& operator= (Color&&)      = default;

    void swap (Color& other)
    {
        using std::swap;
        swap(r_, other.r_);
        swap(g_, other.g_);
        swap(b_, other.b_);
        swap(a_, other.a_);
    }

    // -------------------------------------------------------------------------
    //     Factories
    // -------------------------------------------------------------------------

    static Color from_bytes( unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255 );

    static Color from_hex( std::string const& hex_color, std::string const& prefix = "#" );

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    double r() const
    {
        return r_;
    }

    double g() const
    {
        return g_;
    }

    double b() const
    {
        return b_;
    }

    double a() const
    {
        return a_;
    }

    unsigned char r_byte() const
    {
        return to_byte_( r_ );
    }

    unsigned char g_byte() const
    {
        return to_byte_( g_ );
    }

    unsigned char b_byte() const
    {
        return to_byte_( b_ );
    }

    unsigned char a_byte() const
    {
        return to_byte_( a_ );
    }

    // -------------------------------------------------------------------------
    //     Modificators
    // -------------------------------------------------------------------------

    void r (double value);

    void g (double value);

    void b (double value);

    void a (double value);

    void r_byte (unsigned char value)
    {
        r_ = from_byte_( value );
    }

    void g_byte (unsigned char value)
    {
        g_ = from_byte_( value );
    }

    void b_byte (unsigned char value)
    {
        b_ = from_byte_( value );
    }

    void a_byte (unsigned char value)
    {
        a_ = from_byte_( value );
    }

    // -------------------------------------------------------------------------
    //     Internal Functions
    // -------------------------------------------------------------------------

private:

    static unsigned char to_byte_( double v );
    static double from_byte_( unsigned char v );

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    double r_;
    double g_;
    double b_;
    double a_;

};

// =================================================================================================
//     Basic Operators
// =================================================================================================

inline void swap (Color& lhs, Color& rhs)
{
    lhs.swap(rhs);
}

inline bool operator == (Color const& lhs, Color const& rhs)
{
    return lhs.r() == rhs.r() && lhs.g() == rhs.g() && lhs.b() == rhs.b() && lhs.a() == rhs.a();
}

inline bool operator != (Color const& lhs, Color const& rhs)
{
    return !(lhs == rhs);
}

} // namespace utils
} // namespace genesis

#endif // include guard
