#ifndef GENESIS_UTILS_FORMATS_SVG_ATTRIBUTES_H_
#define GENESIS_UTILS_FORMATS_SVG_ATTRIBUTES_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/color/color.hpp"

#include <iosfwd>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Svg Stroke
// =================================================================================================

struct SvgStroke
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = SvgStroke;

    enum class Type
    {
        kColor,
        kGradient,
        kNone,
        kOmit
    };

    enum class LineCap
    {
        kOmit,
        kButt,
        kSquare,
        kRound
    };

    enum class LineJoin
    {
        kOmit,
        kMiter,
        kRound,
        kBevel
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    explicit SvgStroke( Type type = Type::kColor );
    explicit SvgStroke( Color color, double width = 1.0 );
    explicit SvgStroke( std::string gradient_id );

    ~SvgStroke() = default;

    SvgStroke( SvgStroke const& ) = default;
    SvgStroke( SvgStroke&& )      = default;

    SvgStroke& operator= ( SvgStroke const& ) = default;
    SvgStroke& operator= ( SvgStroke&& )      = default;

    // -------------------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------------------

    void write( std::ostream& out ) const;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    Type type;

    std::string gradient_id;

    Color  color;

    double      width;
    std::string width_unit;

    LineCap  line_cap;
    LineJoin line_join;
    double   miterlimit;

    std::vector< double > dash_array;
    double                dash_offset;

};

// =================================================================================================
//     Svg Fill
// =================================================================================================

struct SvgFill
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = SvgFill;

    enum class Type
    {
        kColor,
        kGradient,
        kNone,
        kOmit
    };

    enum class Rule
    {
        kNone,
        kNonZero,
        kEvenOdd
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    explicit SvgFill( Type type = Type::kColor );
    explicit SvgFill( Color color );
    explicit SvgFill( std::string gradient_id );

    ~SvgFill() = default;

    SvgFill( SvgFill const& ) = default;
    SvgFill( SvgFill&& )      = default;

    SvgFill& operator= ( SvgFill const& ) = default;
    SvgFill& operator= ( SvgFill&& )      = default;

    // -------------------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------------------

    void write( std::ostream& out ) const;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    Type type;

    std::string gradient_id;

    Color  color;
    Rule   rule;

};

// =================================================================================================
//     Svg Font
// =================================================================================================

struct SvgFont
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = SvgFont;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    explicit SvgFont( double size = 10, std::string const& family = "Verdana" );

    ~SvgFont() = default;

    SvgFont( SvgFont const& ) = default;
    SvgFont( SvgFont&& )      = default;

    SvgFont& operator= ( SvgFont const& ) = default;
    SvgFont& operator= ( SvgFont&& )      = default;

    // -------------------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------------------

    void write( std::ostream& out ) const;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    double      size;
    std::string family;

};

// =================================================================================================
//     Svg Transformation
// =================================================================================================

struct SvgTransform
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using self_type = SvgTransform;

    // -------------------------------------------------------------------------
    //     Subclass Translate
    // -------------------------------------------------------------------------

    struct Translate
    {
    public:

        // -------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------

        Translate() = default;

        Translate( double tx, double ty )
            : tx(tx)
            , ty(ty)
        {}

        Translate( SvgPoint offset )
            : Translate( offset.x, offset.y )
        {}

        ~Translate() = default;

        Translate( Translate const& ) = default;
        Translate( Translate&& )      = default;

        Translate& operator= ( Translate const& ) = default;
        Translate& operator= ( Translate&& )      = default;

        // -------------------------------------------------------------
        //     Operators and Write
        // -------------------------------------------------------------

        void write( std::ostream& out ) const;
        SvgPoint apply( SvgPoint const& p ) const;

        // -------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------

        double tx = 0.0;
        double ty = 0.0;
    };

    // -------------------------------------------------------------------------
    //     Subclass Rotate
    // -------------------------------------------------------------------------

    struct Rotate
    {
    public:

        // -------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------

        Rotate() = default;

        /**
         * @brief Rotation, in degrees.
         */
        Rotate( double angle )
            : a(angle)
        {}

        /**
         * @brief Rotation, in degrees, around a given offset point.
         */
        Rotate( double angle, double cx, double cy )
            : a(angle)
            , cx(cx)
            , cy(cy)
        {}

        /**
         * @brief Rotation, in degrees, around a given offset point.
         */
        Rotate( double angle, SvgPoint offset )
            : Rotate( angle, offset.x, offset.y )
        {}

        ~Rotate() = default;

        Rotate( Rotate const& ) = default;
        Rotate( Rotate&& )      = default;

        Rotate& operator= ( Rotate const& ) = default;
        Rotate& operator= ( Rotate&& )      = default;

        // -------------------------------------------------------------
        //     Operators and Write
        // -------------------------------------------------------------

        void write( std::ostream& out ) const;
        SvgPoint apply( SvgPoint const& p ) const;

        // -------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------

        double a  = 0.0;
        double cx = 0.0;
        double cy = 0.0;
    };

    // -------------------------------------------------------------------------
    //     Subclass Scale
    // -------------------------------------------------------------------------

    struct Scale
    {
    public:

        // -------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------

        Scale() = default;

        Scale( double sxy )
            : sx(sxy)
            , sy(sxy)
        {}

        Scale( double sx, double sy )
            : sx(sx)
            , sy(sy)
        {}

        ~Scale() = default;

        Scale( Scale const& ) = default;
        Scale( Scale&& )      = default;

        Scale& operator= ( Scale const& ) = default;
        Scale& operator= ( Scale&& )      = default;

        // -------------------------------------------------------------
        //     Operators and Write
        // -------------------------------------------------------------

        void write( std::ostream& out ) const;
        SvgPoint apply( SvgPoint const& p ) const;

        // -------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------

        double sx = 1.0;
        double sy = 1.0;
    };

    // -------------------------------------------------------------------------
    //     Subclass SkewX
    // -------------------------------------------------------------------------

    struct SkewX
    {
    public:

        // -------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------

        SkewX() = default;

        /**
         * @brief SkewX an object, with an angle given in degrees.
         */
        SkewX( double a )
            : ax(a)
        {}

        ~SkewX() = default;

        SkewX( SkewX const& ) = default;
        SkewX( SkewX&& )      = default;

        SkewX& operator= ( SkewX const& ) = default;
        SkewX& operator= ( SkewX&& )      = default;

        // -------------------------------------------------------------
        //     Operators and Write
        // -------------------------------------------------------------

        void write( std::ostream& out ) const;
        SvgPoint apply( SvgPoint const& p ) const;

        // -------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------

        double ax = 0.0;
    };

    // -------------------------------------------------------------------------
    //     Subclass SkewY
    // -------------------------------------------------------------------------

    struct SkewY
    {
    public:

        // -------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------

        SkewY() = default;

        /**
         * @brief SkewY an object, with an angle given in degrees.
         */
        SkewY( double a )
            : ay(a)
        {}

        ~SkewY() = default;

        SkewY( SkewY const& ) = default;
        SkewY( SkewY&& )      = default;

        SkewY& operator= ( SkewY const& ) = default;
        SkewY& operator= ( SkewY&& )      = default;

        // -------------------------------------------------------------
        //     Operators and Write
        // -------------------------------------------------------------

        void write( std::ostream& out ) const;
        SvgPoint apply( SvgPoint const& p ) const;

        // -------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------

        double ay = 0.0;
    };

    // -------------------------------------------------------------------------
    //     Subclass Matrix
    // -------------------------------------------------------------------------

    struct Matrix
    {
    public:

        // -------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------

        Matrix() = default;

        Matrix( double a, double b, double c, double d, double e, double f )
            : a(a)
            , b(b)
            , c(c)
            , d(d)
            , e(e)
            , f(f)
        {}

        ~Matrix() = default;

        Matrix( Matrix const& ) = default;
        Matrix( Matrix&& )      = default;

        Matrix& operator= ( Matrix const& ) = default;
        Matrix& operator= ( Matrix&& )      = default;

        // -------------------------------------------------------------
        //     Operators and Write
        // -------------------------------------------------------------

        void write( std::ostream& out ) const;
        SvgPoint apply( SvgPoint const& p ) const;

        // -------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------

        double a = 1.0;
        double b = 0.0;
        double c = 0.0;
        double d = 1.0;
        double e = 0.0;
        double f = 0.0;
    };

    // -------------------------------------------------------------------------
    //     Transformation
    // -------------------------------------------------------------------------

    /**
     * @brief Internal helper class used as an abstraction to be able to store Transformations
     * without need for inheritance and virtual functions within the different transformation types.
     */
    class Transformation
    {
    public:

        // -------------------------------------------------------------
        //     Typedefs and Enums
        // -------------------------------------------------------------

        using self_type = Transformation;

        // -------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------

        template< typename T >
        Transformation( T const& object )
            // Although we are in utils namespace here, we specify the namespace full,
            // in order to avoid ambiguous overload when compiled with C++17.
            : pimpl_( genesis::utils::make_unique< Model<T> >( object ))
        {}

        Transformation( Transformation const& other )
            : pimpl_( other.pimpl_->clone() )
        {}

        Transformation( Transformation&& )      = default;

        Transformation& operator=( Transformation other )
        {
            std::swap( pimpl_, other.pimpl_ );
            return *this;
        }

        ~Transformation() = default;

        // -------------------------------------------------------------
        //     Members
        // -------------------------------------------------------------

        SvgPoint apply( SvgPoint const& p ) const
        {
            return pimpl_->apply_( p );
        }

        void write( std::ostream& out ) const
        {
            pimpl_->write_( out );
        }

        // -------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------

    private:

        struct Concept
        {
            virtual ~Concept() {}

            virtual SvgPoint apply_( SvgPoint const& p ) const = 0;
            virtual void write_( std::ostream& out ) const = 0;

            virtual std::unique_ptr< Concept > clone() const = 0;
        };

        template< typename T >
        struct Model : Concept
        {
            Model( T const& value )
                : object_( value )
            {}

            SvgPoint apply_( SvgPoint const& p ) const override
            {
                return object_.apply( p );
            }

            void write_( std::ostream& out ) const override
            {
                object_.write( out );
            }

            std::unique_ptr< Concept > clone() const override
            {
                // Although we are in utils namespace here, we specify the namespace full,
                // in order to avoid ambiguous overload when compiled with C++17.
                return genesis::utils::make_unique< Model<T> >( object_ );
            }

            T object_;
        };

        std::unique_ptr< Concept > pimpl_;

    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SvgTransform() = default;
    ~SvgTransform() = default;

    SvgTransform( SvgTransform const& ) = default;
    SvgTransform( SvgTransform&& )      = default;

    SvgTransform& operator= ( SvgTransform const& ) = default;
    SvgTransform& operator= ( SvgTransform&& )      = default;

    // -------------------------------------------------------------------------
    //     Drawing Function
    // -------------------------------------------------------------------------

    void append( Transformation&& t );
    void append( Transformation const& t );

    /**
     * @brief Apply all transformations to a point, and return the new transformed coordinate.
     */
    SvgPoint apply( SvgPoint const& p ) const;

    /**
     * @brief Apply all transformations to a box, and return the new transformed coordinates.
     *
     * As our implmentation is not a full svg renderer, we cheat here and just transform the
     * corners of the box. That means, with rotations etc, there could be parts of an object
     * where this model fails us. Note that we transform all four corners, and hence compute
     * a box that is too large under rotations.
     *
     * Still, this is good enough for our puposes of determining the rough outlines of objects,
     * so that we can scale the document around them.
     */
    SvgBox apply( SvgBox const& b ) const;

    void write( std::ostream& out ) const;

    void clear()
    {
        transformations.clear();
    }

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    std::vector<Transformation> transformations;

};

} // namespace utils
} // namespace genesis

#endif // include guard
