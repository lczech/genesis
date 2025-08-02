#include <genesis/util/color/norm_boundary.hpp>
#include <genesis/util/color/norm_diverging.hpp>
#include <genesis/util/color/norm_linear.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <vector>

#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

#ifndef BINDER_PYBIND11_TYPE_CASTER
#    define BINDER_PYBIND11_TYPE_CASTER
PYBIND11_DECLARE_HOLDER_TYPE( T, std::shared_ptr<T>, false )
PYBIND11_DECLARE_HOLDER_TYPE( T, T*, false )
PYBIND11_MAKE_OPAQUE( std::shared_ptr<void> )
#endif

// genesis::util::color::ColorNormalizationBoundary file:genesis/util/color/norm_boundary.hpp
// line:76
struct PyCallBack_genesis_util_color_ColorNormalizationBoundary
    : public genesis::util::color::ColorNormalizationBoundary
{
    using genesis::util::color::ColorNormalizationBoundary::ColorNormalizationBoundary;

    bool is_valid_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationBoundary*>( this ),
            "is_valid_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>();
            if( pybind11::detail::cast_is_temporary_value_reference<bool>::value ) {
                static pybind11::detail::override_caster_t<bool> caster;
                return pybind11::detail::cast_ref<bool>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<bool>( std::move( o ) );
        }
        return ColorNormalizationBoundary::is_valid_();
    }
    void is_valid_or_throw_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationBoundary*>( this ),
            "is_valid_or_throw_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>();
            if( pybind11::detail::cast_is_temporary_value_reference<void>::value ) {
                static pybind11::detail::override_caster_t<void> caster;
                return pybind11::detail::cast_ref<void>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<void>( std::move( o ) );
        }
        return ColorNormalizationBoundary::is_valid_or_throw_();
    }
    double normalize_( double a0 ) const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationBoundary*>( this ),
            "normalize_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>( a0 );
            if( pybind11::detail::cast_is_temporary_value_reference<double>::value ) {
                static pybind11::detail::override_caster_t<double> caster;
                return pybind11::detail::cast_ref<double>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<double>( std::move( o ) );
        }
        return ColorNormalizationBoundary::normalize_( a0 );
    }
};

// genesis::util::color::ColorNormalizationLinear file:genesis/util/color/norm_linear.hpp line:60
struct PyCallBack_genesis_util_color_ColorNormalizationLinear
    : public genesis::util::color::ColorNormalizationLinear
{
    using genesis::util::color::ColorNormalizationLinear::ColorNormalizationLinear;

    bool is_valid_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationLinear*>( this ), "is_valid_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>();
            if( pybind11::detail::cast_is_temporary_value_reference<bool>::value ) {
                static pybind11::detail::override_caster_t<bool> caster;
                return pybind11::detail::cast_ref<bool>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<bool>( std::move( o ) );
        }
        return ColorNormalizationLinear::is_valid_();
    }
    void is_valid_or_throw_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationLinear*>( this ),
            "is_valid_or_throw_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>();
            if( pybind11::detail::cast_is_temporary_value_reference<void>::value ) {
                static pybind11::detail::override_caster_t<void> caster;
                return pybind11::detail::cast_ref<void>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<void>( std::move( o ) );
        }
        return ColorNormalizationLinear::is_valid_or_throw_();
    }
    double normalize_( double a0 ) const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationLinear*>( this ), "normalize_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>( a0 );
            if( pybind11::detail::cast_is_temporary_value_reference<double>::value ) {
                static pybind11::detail::override_caster_t<double> caster;
                return pybind11::detail::cast_ref<double>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<double>( std::move( o ) );
        }
        return ColorNormalizationLinear::normalize_( a0 );
    }
    void update_hook_( double a0, double a1 ) override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationLinear*>( this ),
            "update_hook_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>( a0, a1 );
            if( pybind11::detail::cast_is_temporary_value_reference<void>::value ) {
                static pybind11::detail::override_caster_t<void> caster;
                return pybind11::detail::cast_ref<void>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<void>( std::move( o ) );
        }
        return ColorNormalizationLinear::update_hook_( a0, a1 );
    }
};

// genesis::util::color::ColorNormalizationDiverging file:genesis/util/color/norm_diverging.hpp
// line:72
struct PyCallBack_genesis_util_color_ColorNormalizationDiverging
    : public genesis::util::color::ColorNormalizationDiverging
{
    using genesis::util::color::ColorNormalizationDiverging::ColorNormalizationDiverging;

    bool is_valid_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationDiverging*>( this ),
            "is_valid_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>();
            if( pybind11::detail::cast_is_temporary_value_reference<bool>::value ) {
                static pybind11::detail::override_caster_t<bool> caster;
                return pybind11::detail::cast_ref<bool>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<bool>( std::move( o ) );
        }
        return ColorNormalizationDiverging::is_valid_();
    }
    void is_valid_or_throw_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationDiverging*>( this ),
            "is_valid_or_throw_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>();
            if( pybind11::detail::cast_is_temporary_value_reference<void>::value ) {
                static pybind11::detail::override_caster_t<void> caster;
                return pybind11::detail::cast_ref<void>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<void>( std::move( o ) );
        }
        return ColorNormalizationDiverging::is_valid_or_throw_();
    }
    double normalize_( double a0 ) const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationDiverging*>( this ),
            "normalize_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>( a0 );
            if( pybind11::detail::cast_is_temporary_value_reference<double>::value ) {
                static pybind11::detail::override_caster_t<double> caster;
                return pybind11::detail::cast_ref<double>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<double>( std::move( o ) );
        }
        return ColorNormalizationDiverging::normalize_( a0 );
    }
    void update_hook_( double a0, double a1 ) override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationDiverging*>( this ),
            "update_hook_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>( a0, a1 );
            if( pybind11::detail::cast_is_temporary_value_reference<void>::value ) {
                static pybind11::detail::override_caster_t<void> caster;
                return pybind11::detail::cast_ref<void>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<void>( std::move( o ) );
        }
        return ColorNormalizationDiverging::update_hook_( a0, a1 );
    }
};

void bind_genesis_util_color_norm_boundary(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::color::ColorNormalizationBoundary file:genesis/util/color/norm_boundary.hpp
      // line:76
        pybind11::class_<
            genesis::util::color::ColorNormalizationBoundary,
            std::shared_ptr<genesis::util::color::ColorNormalizationBoundary>,
            PyCallBack_genesis_util_color_ColorNormalizationBoundary,
            genesis::util::color::ColorNormalization>
            cl( M( "genesis::util::color" ),
                "ColorNormalizationBoundary",
                "Color normalization that maps to discrete intervals.\n\n The normalization takes "
                "a set of boundaries, which define the edges of intervals.\n Values falling within "
                "an interval are mapped to a value within `[ 0.0, 1.0 ]`\n that corresponds to the "
                "index of the interval. For example, with boundaries `3.0, 4.0, 5.0, 6.0`,\n we "
                "get three intervals. Valus outside of these boundaries are mapped to `-1.0` or "
                "`2.0`.\n Hence, values are mapped as follows:\n\n     < 3.0        : -1.0\n     [ "
                "3.0, 4.0 ) :  0.0\n     [ 4.0, 5.0 ) :  0.5\n     [ 5.0, 6.0 ] :  1.0\n     > 6.0 "
                "       :  2.0\n\n Thus, this normalization can immediately be used with a "
                "ColorMap.\n Furthermore, the function interval() offers to retrieve the interval "
                "index for a given value.\n For the example above, these would be values between 0 "
                "and 2 for valid values." );
        // function-signature:
        // genesis::util::color::ColorNormalizationBoundary::ColorNormalizationBoundary()()
        // file:genesis/util/color/norm_boundary.hpp line:88
        cl.def( pybind11::init(
            []() { return new genesis::util::color::ColorNormalizationBoundary(); },
            []() { return new PyCallBack_genesis_util_color_ColorNormalizationBoundary(); }
        ) );
        // function-signature:
        // genesis::util::color::ColorNormalizationBoundary::ColorNormalizationBoundary(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/util/color/norm_boundary.hpp line:93
        cl.def( pybind11::init<const class std::vector<double>&>(), pybind11::arg( "boundaries" ) );

        // function-signature:
        // genesis::util::color::ColorNormalizationBoundary::ColorNormalizationBoundary(double,
        // double, unsigned long)(double, double, unsigned long)
        // file:genesis/util/color/norm_boundary.hpp line:99
        cl.def(
            pybind11::init<double, double, unsigned long>(),
            pybind11::arg( "min" ),
            pybind11::arg( "max" ),
            pybind11::arg( "intervals" )
        );

        // function-signature:
        // genesis::util::color::ColorNormalizationBoundary::ColorNormalizationBoundary(const class
        // genesis::util::color::ColorNormalizationBoundary &)(const class
        // genesis::util::color::ColorNormalizationBoundary &)
        // file:genesis/util/color/norm_boundary.hpp line:106
        cl.def( pybind11::init(
            []( PyCallBack_genesis_util_color_ColorNormalizationBoundary const& o ) {
                return new PyCallBack_genesis_util_color_ColorNormalizationBoundary( o );
            }
        ) );
        cl.def( pybind11::init( []( genesis::util::color::ColorNormalizationBoundary const& o ) {
            return new genesis::util::color::ColorNormalizationBoundary( o );
        } ) );
        // function-signature: class genesis::util::color::ColorNormalizationBoundary &
        // genesis::util::color::ColorNormalizationBoundary::operator=(const class
        // genesis::util::color::ColorNormalizationBoundary &)(const class
        // genesis::util::color::ColorNormalizationBoundary &)
        // file:genesis/util/color/norm_boundary.hpp line:109
        cl.def(
            "assign",
            ( class genesis::util::color::ColorNormalizationBoundary &
              (genesis::util::color::
                   ColorNormalizationBoundary::*)(const class genesis::util::color::
                                                      ColorNormalizationBoundary&)) &
                genesis::util::color::ColorNormalizationBoundary::operator=,
            "C++: genesis::util::color::ColorNormalizationBoundary::operator=(const class "
            "genesis::util::color::ColorNormalizationBoundary &) --> class "
            "genesis::util::color::ColorNormalizationBoundary &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: const class std::vector<double> &
        // genesis::util::color::ColorNormalizationBoundary::boundaries() const()
        // file:genesis/util/color/norm_boundary.hpp line:119
        cl.def(
            "boundaries",
            ( const class std::vector<
                double>& (genesis::util::color::ColorNormalizationBoundary::*)() const ) &
                genesis::util::color::ColorNormalizationBoundary::boundaries,
            "Return the boundaries currently set.\n\nC++: "
            "genesis::util::color::ColorNormalizationBoundary::boundaries() const --> const class "
            "std::vector<double> &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: long
        // genesis::util::color::ColorNormalizationBoundary::interval(double) const(double)
        // file:genesis/util/color/norm_boundary.hpp line:127
        cl.def(
            "interval",
            ( long( genesis::util::color::ColorNormalizationBoundary::* )( double ) const ) &
                genesis::util::color::ColorNormalizationBoundary::interval,
            "Return the interval index of a value.\n\nC++: "
            "genesis::util::color::ColorNormalizationBoundary::interval(double) const --> long",
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::color::ColorNormalizationBoundary &
        // genesis::util::color::ColorNormalizationBoundary::boundaries(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/util/color/norm_boundary.hpp line:170
        cl.def(
            "boundaries",
            ( class genesis::util::color::ColorNormalizationBoundary &
              (genesis::util::color::ColorNormalizationBoundary::*)(const class std::vector<
                                                                    double>&)) &
                genesis::util::color::ColorNormalizationBoundary::boundaries,
            "Explicitly set the boundaries to the given  Have to be sorted.\n\nC++: "
            "genesis::util::color::ColorNormalizationBoundary::boundaries(const class "
            "std::vector<double> &) --> class genesis::util::color::ColorNormalizationBoundary &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "values" )
        );
        // function-signature: class genesis::util::color::ColorNormalizationBoundary &
        // genesis::util::color::ColorNormalizationBoundary::scale(double, double, unsigned
        // long)(double, double, unsigned long) file:genesis/util/color/norm_boundary.hpp line:181
        cl.def(
            "scale",
            ( class genesis::util::color::ColorNormalizationBoundary &
              (genesis::util::color::ColorNormalizationBoundary::*)(double, double, unsigned long
              )) &
                genesis::util::color::ColorNormalizationBoundary::scale,
            "Set the boundaries to equal-sizes intervals between  and \n using  many "
            "steps.\n\nC++: genesis::util::color::ColorNormalizationBoundary::scale(double, "
            "double, unsigned long) --> class genesis::util::color::ColorNormalizationBoundary &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "min" ),
            pybind11::arg( "max" ),
            pybind11::arg( "intervals" )
        );
        // function-signature: class genesis::util::color::ColorNormalizationBoundary &
        // genesis::util::color::ColorNormalizationBoundary::autoscale(const class
        // std::vector<double> &, unsigned long)(const class std::vector<double> &, unsigned long)
        // file:genesis/util/color/norm_boundary.hpp line:200
        cl.def(
            "autoscale",
            ( class genesis::util::color::ColorNormalizationBoundary &
              (genesis::util::color::ColorNormalizationBoundary::*)(const class std::vector<
                                                                        double>&,
                                                                    unsigned long)) &
                genesis::util::color::ColorNormalizationBoundary::autoscale,
            "Set the boundaries similar to scale(), but use the given vector of values to "
            "determine\n `min` and `max` first.\n\nC++: "
            "genesis::util::color::ColorNormalizationBoundary::autoscale(const class "
            "std::vector<double> &, unsigned long) --> class "
            "genesis::util::color::ColorNormalizationBoundary &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "values" ),
            pybind11::arg( "intervals" )
        );
    }
    { // genesis::util::color::ColorNormalizationLinear file:genesis/util/color/norm_linear.hpp
      // line:60
        pybind11::class_<
            genesis::util::color::ColorNormalizationLinear,
            std::shared_ptr<genesis::util::color::ColorNormalizationLinear>,
            PyCallBack_genesis_util_color_ColorNormalizationLinear,
            genesis::util::color::ColorNormalization>
            cl( M( "genesis::util::color" ),
                "ColorNormalizationLinear",
                "Default Color normalization, using a sequential linear scaling in the\n range `[ "
                "min, max ]`." );
        // function-signature:
        // genesis::util::color::ColorNormalizationLinear::ColorNormalizationLinear()()
        // file:genesis/util/color/norm_linear.hpp line:72
        cl.def( pybind11::init(
            []() { return new genesis::util::color::ColorNormalizationLinear(); },
            []() { return new PyCallBack_genesis_util_color_ColorNormalizationLinear(); }
        ) );
        // function-signature:
        // genesis::util::color::ColorNormalizationLinear::ColorNormalizationLinear(double,
        // double)(double, double) file:genesis/util/color/norm_linear.hpp line:77
        cl.def( pybind11::init<double, double>(), pybind11::arg( "min" ), pybind11::arg( "max" ) );

        // function-signature:
        // genesis::util::color::ColorNormalizationLinear::ColorNormalizationLinear(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/util/color/norm_linear.hpp line:87
        cl.def( pybind11::init<const class std::vector<double>&>(), pybind11::arg( "values" ) );

        // function-signature:
        // genesis::util::color::ColorNormalizationLinear::ColorNormalizationLinear(const class
        // genesis::util::color::ColorNormalizationLinear &)(const class
        // genesis::util::color::ColorNormalizationLinear &) file:genesis/util/color/norm_linear.hpp
        // line:103
        cl.def( pybind11::init( []( PyCallBack_genesis_util_color_ColorNormalizationLinear const& o
                                ) {
            return new PyCallBack_genesis_util_color_ColorNormalizationLinear( o );
        } ) );
        cl.def( pybind11::init( []( genesis::util::color::ColorNormalizationLinear const& o ) {
            return new genesis::util::color::ColorNormalizationLinear( o );
        } ) );
        // function-signature: class genesis::util::color::ColorNormalizationLinear &
        // genesis::util::color::ColorNormalizationLinear::operator=(const class
        // genesis::util::color::ColorNormalizationLinear &)(const class
        // genesis::util::color::ColorNormalizationLinear &) file:genesis/util/color/norm_linear.hpp
        // line:106
        cl.def(
            "assign",
            ( class genesis::util::color::ColorNormalizationLinear &
              (genesis::util::color::ColorNormalizationLinear::*)(const class genesis::util::color::
                                                                      ColorNormalizationLinear&)) &
                genesis::util::color::ColorNormalizationLinear::operator=,
            "C++: genesis::util::color::ColorNormalizationLinear::operator=(const class "
            "genesis::util::color::ColorNormalizationLinear &) --> class "
            "genesis::util::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: double genesis::util::color::ColorNormalizationLinear::min_value()
        // const() file:genesis/util/color/norm_linear.hpp line:116
        cl.def(
            "min_value",
            ( double( genesis::util::color::ColorNormalizationLinear::* )() const ) &
                genesis::util::color::ColorNormalizationLinear::min_value,
            "Minimum value, that is, where to begin the color scale.\n\nC++: "
            "genesis::util::color::ColorNormalizationLinear::min_value() const --> double"
        );
        // function-signature: double genesis::util::color::ColorNormalizationLinear::max_value()
        // const() file:genesis/util/color/norm_linear.hpp line:124
        cl.def(
            "max_value",
            ( double( genesis::util::color::ColorNormalizationLinear::* )() const ) &
                genesis::util::color::ColorNormalizationLinear::max_value,
            "Minimum value, that is, where to end the color scale.\n\nC++: "
            "genesis::util::color::ColorNormalizationLinear::max_value() const --> double"
        );
        // function-signature: class genesis::util::color::ColorNormalizationLinear &
        // genesis::util::color::ColorNormalizationLinear::scale(double, double)(double, double)
        // file:genesis/util/color/norm_linear.hpp line:136
        cl.def(
            "scale",
            ( class genesis::util::color::ColorNormalizationLinear &
              (genesis::util::color::ColorNormalizationLinear::*)(double, double)) &
                genesis::util::color::ColorNormalizationLinear::scale,
            "C++: genesis::util::color::ColorNormalizationLinear::scale(double, double) --> class "
            "genesis::util::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "min" ),
            pybind11::arg( "max" )
        );
        // function-signature: class genesis::util::color::ColorNormalizationLinear &
        // genesis::util::color::ColorNormalizationLinear::autoscale(const class std::vector<double>
        // &)(const class std::vector<double> &) file:genesis/util/color/norm_linear.hpp line:147
        cl.def(
            "autoscale",
            ( class genesis::util::color::ColorNormalizationLinear &
              (genesis::util::color::ColorNormalizationLinear::*)(const class std::vector<double>&)
            ) & genesis::util::color::ColorNormalizationLinear::autoscale,
            "C++: genesis::util::color::ColorNormalizationLinear::autoscale(const class "
            "std::vector<double> &) --> class genesis::util::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "values" )
        );
        // function-signature: class genesis::util::color::ColorNormalizationLinear &
        // genesis::util::color::ColorNormalizationLinear::autoscale_min(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/util/color/norm_linear.hpp line:155
        cl.def(
            "autoscale_min",
            ( class genesis::util::color::ColorNormalizationLinear &
              (genesis::util::color::ColorNormalizationLinear::*)(const class std::vector<double>&)
            ) & genesis::util::color::ColorNormalizationLinear::autoscale_min,
            "C++: genesis::util::color::ColorNormalizationLinear::autoscale_min(const class "
            "std::vector<double> &) --> class genesis::util::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "values" )
        );
        // function-signature: class genesis::util::color::ColorNormalizationLinear &
        // genesis::util::color::ColorNormalizationLinear::autoscale_max(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/util/color/norm_linear.hpp line:163
        cl.def(
            "autoscale_max",
            ( class genesis::util::color::ColorNormalizationLinear &
              (genesis::util::color::ColorNormalizationLinear::*)(const class std::vector<double>&)
            ) & genesis::util::color::ColorNormalizationLinear::autoscale_max,
            "C++: genesis::util::color::ColorNormalizationLinear::autoscale_max(const class "
            "std::vector<double> &) --> class genesis::util::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "values" )
        );
        // function-signature: class genesis::util::color::ColorNormalizationLinear &
        // genesis::util::color::ColorNormalizationLinear::min_value(double)(double)
        // file:genesis/util/color/norm_linear.hpp line:209
        cl.def(
            "min_value",
            ( class genesis::util::color::ColorNormalizationLinear &
              (genesis::util::color::ColorNormalizationLinear::*)(double)) &
                genesis::util::color::ColorNormalizationLinear::min_value,
            "C++: genesis::util::color::ColorNormalizationLinear::min_value(double) --> class "
            "genesis::util::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::color::ColorNormalizationLinear &
        // genesis::util::color::ColorNormalizationLinear::max_value(double)(double)
        // file:genesis/util/color/norm_linear.hpp line:219
        cl.def(
            "max_value",
            ( class genesis::util::color::ColorNormalizationLinear &
              (genesis::util::color::ColorNormalizationLinear::*)(double)) &
                genesis::util::color::ColorNormalizationLinear::max_value,
            "C++: genesis::util::color::ColorNormalizationLinear::max_value(double) --> class "
            "genesis::util::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
    }
    { // genesis::util::color::ColorNormalizationDiverging
      // file:genesis/util/color/norm_diverging.hpp line:72
        pybind11::class_<
            genesis::util::color::ColorNormalizationDiverging,
            std::shared_ptr<genesis::util::color::ColorNormalizationDiverging>,
            PyCallBack_genesis_util_color_ColorNormalizationDiverging,
            genesis::util::color::ColorNormalizationLinear>
            cl( M( "genesis::util::color" ),
                "ColorNormalizationDiverging",
                "Color normalization for a diverging scale.\n\n The min() and max() values "
                "determine the boundaries of the range of allowed values;\n they map to the first "
                "and last entry of the palette, respectively.\n The mid() value is used as the "
                "\"neutral\" value in the range of allowed values, that is, it\n maps to the "
                "central color of the palette.\n\n In other words, values in `[ min, mid ]` are "
                "mapped to the first half of the palette,\n and values in `[ mid, max ]` to the "
                "second half:\n\n          palette\n        |----|----|\n       /      \\    \\\n  "
                "    /        \\    \\\n      |---------|---|\n     min      mid  max\n\n Values "
                "outside of the allowed range `[ min, max ]`, as well as non-finite values, are\n "
                "treated according to the settings clip_under(), clip_over(), mask_value(), "
                "mask_color(),\n under_color(), over_color()." );
        // function-signature:
        // genesis::util::color::ColorNormalizationDiverging::ColorNormalizationDiverging()()
        // file:genesis/util/color/norm_diverging.hpp line:84
        cl.def( pybind11::init(
            []() { return new genesis::util::color::ColorNormalizationDiverging(); },
            []() { return new PyCallBack_genesis_util_color_ColorNormalizationDiverging(); }
        ) );
        // function-signature:
        // genesis::util::color::ColorNormalizationDiverging::ColorNormalizationDiverging(double,
        // double)(double, double) file:genesis/util/color/norm_diverging.hpp line:93
        cl.def( pybind11::init<double, double>(), pybind11::arg( "min" ), pybind11::arg( "max" ) );

        // function-signature:
        // genesis::util::color::ColorNormalizationDiverging::ColorNormalizationDiverging(double,
        // double, double)(double, double, double) file:genesis/util/color/norm_diverging.hpp
        // line:100
        cl.def(
            pybind11::init<double, double, double>(),
            pybind11::arg( "min" ),
            pybind11::arg( "mid" ),
            pybind11::arg( "max" )
        );

        // function-signature:
        // genesis::util::color::ColorNormalizationDiverging::ColorNormalizationDiverging(const
        // class std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/util/color/norm_diverging.hpp line:111
        cl.def( pybind11::init<const class std::vector<double>&>(), pybind11::arg( "values" ) );

        // function-signature:
        // genesis::util::color::ColorNormalizationDiverging::ColorNormalizationDiverging(const
        // class genesis::util::color::ColorNormalizationDiverging &)(const class
        // genesis::util::color::ColorNormalizationDiverging &)
        // file:genesis/util/color/norm_diverging.hpp line:128
        cl.def( pybind11::init(
            []( PyCallBack_genesis_util_color_ColorNormalizationDiverging const& o ) {
                return new PyCallBack_genesis_util_color_ColorNormalizationDiverging( o );
            }
        ) );
        cl.def( pybind11::init( []( genesis::util::color::ColorNormalizationDiverging const& o ) {
            return new genesis::util::color::ColorNormalizationDiverging( o );
        } ) );
        // function-signature: class genesis::util::color::ColorNormalizationDiverging &
        // genesis::util::color::ColorNormalizationDiverging::operator=(const class
        // genesis::util::color::ColorNormalizationDiverging &)(const class
        // genesis::util::color::ColorNormalizationDiverging &)
        // file:genesis/util/color/norm_diverging.hpp line:131
        cl.def(
            "assign",
            ( class genesis::util::color::ColorNormalizationDiverging &
              (genesis::util::color::
                   ColorNormalizationDiverging::*)(const class genesis::util::color::
                                                       ColorNormalizationDiverging&)) &
                genesis::util::color::ColorNormalizationDiverging::operator=,
            "C++: genesis::util::color::ColorNormalizationDiverging::operator=(const class "
            "genesis::util::color::ColorNormalizationDiverging &) --> class "
            "genesis::util::color::ColorNormalizationDiverging &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: double genesis::util::color::ColorNormalizationDiverging::mid_value()
        // const() file:genesis/util/color/norm_diverging.hpp line:141
        cl.def(
            "mid_value",
            ( double( genesis::util::color::ColorNormalizationDiverging::* )() const ) &
                genesis::util::color::ColorNormalizationDiverging::mid_value,
            "Mid-point value, that is, where the middle value of a diverging_color() is.\n\nC++: "
            "genesis::util::color::ColorNormalizationDiverging::mid_value() const --> double"
        );
        // function-signature: class genesis::util::color::ColorNormalizationDiverging &
        // genesis::util::color::ColorNormalizationDiverging::make_centric(double)(double)
        // file:genesis/util/color/norm_diverging.hpp line:156
        cl.def(
            "make_centric",
            []( genesis::util::color::ColorNormalizationDiverging& o
            ) -> genesis::util::color::ColorNormalizationDiverging& { return o.make_centric(); },
            "",
            pybind11::return_value_policy::reference_internal
        );
        cl.def(
            "make_centric",
            ( class genesis::util::color::ColorNormalizationDiverging &
              (genesis::util::color::ColorNormalizationDiverging::*)(double)) &
                genesis::util::color::ColorNormalizationDiverging::make_centric,
            "Make the range symmetric around a center value.\n\n For example, if autoscale() "
            "yielded `min == -0.89` and `max == 0.95`, then calling this\n function with `center "
            "== 0.0` sets `min == -0.95` and `mid == 0.0`;\n\nC++: "
            "genesis::util::color::ColorNormalizationDiverging::make_centric(double) --> class "
            "genesis::util::color::ColorNormalizationDiverging &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "center" )
        );
        // function-signature: class genesis::util::color::ColorNormalizationDiverging &
        // genesis::util::color::ColorNormalizationDiverging::mid_value(double)(double)
        // file:genesis/util/color/norm_diverging.hpp line:173
        cl.def(
            "mid_value",
            ( class genesis::util::color::ColorNormalizationDiverging &
              (genesis::util::color::ColorNormalizationDiverging::*)(double)) &
                genesis::util::color::ColorNormalizationDiverging::mid_value,
            "C++: genesis::util::color::ColorNormalizationDiverging::mid_value(double) --> class "
            "genesis::util::color::ColorNormalizationDiverging &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
    }
}
