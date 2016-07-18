#ifndef GENESIS_UTILS_TOOLS_COLOR_H_
#define GENESIS_UTILS_TOOLS_COLOR_H_

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
 * @brief Header of Color class.
 *
 * @file
 * @ingroup utils
 */

#include <utility>

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
    //     Constructors
    // -------------------------------------------------------------------------

    /**
     * @brief Default constructor. Sets the color to black.
     */
    Color () : r_(0), g_(0), b_(0) {}

    /**
     * @brief Constructor for setting the RGB value.
     */
    Color (unsigned char r, unsigned char g, unsigned char b) : r_(r), g_(g), b_(b) {}

    ~Color() = default;

    Color(Color const&)     = default;
    Color(Color&&) noexcept = default;

    Color& operator= (Color const&)     = default;
    Color& operator= (Color&&) noexcept = default;

    void swap (Color& other) noexcept
    {
        using std::swap;
        swap(r_, other.r_);
        swap(g_, other.g_);
        swap(b_, other.b_);
    }

    // -------------------------------------------------------------------------
    //     Accessors and Modificators
    // -------------------------------------------------------------------------

    unsigned char r() const
    {
        return r_;
    }

    unsigned char g() const
    {
        return g_;
    }

    unsigned char b() const
    {
        return b_;
    }

    void r (unsigned char value)
    {
        r_ = value;
    }

    void g (unsigned char value)
    {
        g_ = value;
    }

    void b (unsigned char value)
    {
        b_ = value;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    unsigned char r_;
    unsigned char g_;
    unsigned char b_;

};

// =================================================================================================
//     Basic Operators
// =================================================================================================

inline void swap (Color& lhs, Color& rhs) noexcept
{
    lhs.swap(rhs);
}

inline bool operator == (Color const& lhs, Color const& rhs)
{
    return lhs.r() == rhs.r() && lhs.g() == rhs.g() && lhs.b() == rhs.b();
}

inline bool operator != (Color const& lhs, Color const& rhs)
{
    return !(lhs == rhs);
}

} // namespace utils
} // namespace genesis

#endif // include guard
