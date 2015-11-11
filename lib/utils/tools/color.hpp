#ifndef GENESIS_UTILS_TOOLS_COLOR_H_
#define GENESIS_UTILS_TOOLS_COLOR_H_

/**
 * @brief Header of Color class.
 *
 * @file
 * @ingroup utils
 */

#include <string>

namespace genesis {

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

    inline unsigned char r() const
    {
        return r_;
    }

    inline unsigned char g() const
    {
        return g_;
    }

    inline unsigned char b() const
    {
        return b_;
    }

    inline void r (unsigned char value)
    {
        r_ = value;
    }

    inline void g (unsigned char value)
    {
        g_ = value;
    }

    inline void b (unsigned char value)
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

} // namespace genesis

// =================================================================================================
//     Namespace std Extension
// =================================================================================================

namespace std {

template<typename T>
inline void swap (genesis::Color& lhs, genesis::Color& rhs) noexcept
{
    lhs.swap(rhs);
}

} // namespace std

#endif // include guard
