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

    // Color (double r, double g, double b) :
    //     r ( static_cast<unsigned char>(round(std::min(std::max(0.0, r), 1.0) * 255.0 )) ),
    //     g ( static_cast<unsigned char>(round(std::min(std::max(0.0, g), 1.0) * 255.0 )) ),
    //     b ( static_cast<unsigned char>(round(std::min(std::max(0.0, b), 1.0) * 255.0 )) )
    // {}

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
    //     Dump & Debug
    // -------------------------------------------------------------------------

public:

    inline std::string dump()
    {
        return "(" + std::to_string(r_) + ", "
                   + std::to_string(g_) + ", "
                   + std::to_string(b_) + ")";
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

protected:

    unsigned char r_;
    unsigned char g_;
    unsigned char b_;

};

} // namespace genesis

#endif // include guard
