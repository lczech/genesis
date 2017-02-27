#ifndef GENESIS_UTILS_FORMATS_SVG_ATTRIBUTES_H_
#define GENESIS_UTILS_FORMATS_SVG_ATTRIBUTES_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include "genesis/utils/core/std.hpp"
#include "genesis/utils/formats/svg/helper.hpp"
#include "genesis/utils/tools/color.hpp"

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
    double opacity;

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
    explicit SvgFill( Color color, double opacity = 1.0 );
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
    double opacity;
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

        Rotate( double angle )
            : a(angle)
        {}

        Rotate( double angle, double cx, double cy )
            : a(angle)
            , cx(cx)
            , cy(cy)
        {}

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

        // -------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------

        double sx = 1.0;
        double sy = 1.0;
    };

    // -------------------------------------------------------------------------
    //     Subclass Skew
    // -------------------------------------------------------------------------

    struct Skew
    {
    public:

        // -------------------------------------------------------------
        //     Constructors and Rule of Five
        // -------------------------------------------------------------

        Skew() = default;

        Skew( double a )
            : ax(a)
            , ay(a)
        {}

        Skew( double ax, double ay )
            : ax(ax)
            , ay(ay)
        {}

        ~Skew() = default;

        Skew( Skew const& ) = default;
        Skew( Skew&& )      = default;

        Skew& operator= ( Skew const& ) = default;
        Skew& operator= ( Skew&& )      = default;

        // -------------------------------------------------------------
        //     Operators and Write
        // -------------------------------------------------------------

        void write( std::ostream& out ) const;

        // -------------------------------------------------------------
        //     Data Members
        // -------------------------------------------------------------

        double ax = 0.0;
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
            : pimpl_( make_unique< Model<T> >( object ))
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

        void write(
            std::ostream& out
        ) const {
            pimpl_->write_( out );
        }

        // -------------------------------------------------------------
        //     Internal Members
        // -------------------------------------------------------------

    private:

        struct Concept
        {
            virtual ~Concept() {}

            virtual void write_(
                std::ostream& out
            ) const = 0;

            virtual std::unique_ptr< Concept > clone() const = 0;
        };

        template< typename T >
        struct Model : Concept
        {
            Model( T const& value )
                : object_( value )
            {}

            void write_(
                std::ostream& out
            ) const {
                object_.write( out );
            }

            std::unique_ptr< Concept > clone() const
            {
                return make_unique< Model<T> >( object_ );
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

    void write( std::ostream& out ) const;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    std::vector<Transformation> transformations;

};

} // namespace utils
} // namespace genesis

#endif // include guard
