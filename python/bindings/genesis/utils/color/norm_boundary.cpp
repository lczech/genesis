#include <genesis/utils/color/norm_boundary.hpp>
#include <genesis/utils/color/norm_diverging.hpp>
#include <genesis/utils/color/norm_linear.hpp>
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

// genesis::utils::color::ColorNormalizationBoundary file:genesis/utils/color/norm_boundary.hpp
// line:76
struct PyCallBack_genesis_utils_color_ColorNormalizationBoundary
    : public genesis::utils::color::ColorNormalizationBoundary
{
    using genesis::utils::color::ColorNormalizationBoundary::ColorNormalizationBoundary;

    bool is_valid_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::utils::color::ColorNormalizationBoundary*>( this ),
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
            static_cast<const genesis::utils::color::ColorNormalizationBoundary*>( this ),
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
            static_cast<const genesis::utils::color::ColorNormalizationBoundary*>( this ),
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

// genesis::utils::color::ColorNormalizationLinear file:genesis/utils/color/norm_linear.hpp line:60
struct PyCallBack_genesis_utils_color_ColorNormalizationLinear
    : public genesis::utils::color::ColorNormalizationLinear
{
    using genesis::utils::color::ColorNormalizationLinear::ColorNormalizationLinear;

    bool is_valid_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::utils::color::ColorNormalizationLinear*>( this ), "is_valid_"
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
            static_cast<const genesis::utils::color::ColorNormalizationLinear*>( this ),
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
            static_cast<const genesis::utils::color::ColorNormalizationLinear*>( this ),
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
        return ColorNormalizationLinear::normalize_( a0 );
    }
    void update_hook_( double a0, double a1 ) override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::utils::color::ColorNormalizationLinear*>( this ),
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

// genesis::utils::color::ColorNormalizationDiverging file:genesis/utils/color/norm_diverging.hpp
// line:72
struct PyCallBack_genesis_utils_color_ColorNormalizationDiverging
    : public genesis::utils::color::ColorNormalizationDiverging
{
    using genesis::utils::color::ColorNormalizationDiverging::ColorNormalizationDiverging;

    bool is_valid_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::utils::color::ColorNormalizationDiverging*>( this ),
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
            static_cast<const genesis::utils::color::ColorNormalizationDiverging*>( this ),
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
            static_cast<const genesis::utils::color::ColorNormalizationDiverging*>( this ),
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
            static_cast<const genesis::utils::color::ColorNormalizationDiverging*>( this ),
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

void bind_genesis_utils_color_norm_boundary(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::utils::color::ColorNormalizationBoundary
      // file:genesis/utils/color/norm_boundary.hpp line:76
        pybind11::class_<
            genesis::utils::color::ColorNormalizationBoundary,
            std::shared_ptr<genesis::utils::color::ColorNormalizationBoundary>,
            PyCallBack_genesis_utils_color_ColorNormalizationBoundary,
            genesis::utils::color::ColorNormalization>
            cl( M( "genesis::utils::color" ),
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
        // genesis::utils::color::ColorNormalizationBoundary::ColorNormalizationBoundary()()
        // file:genesis/utils/color/norm_boundary.hpp line:88
        cl.def( pybind11::init(
            []() { return new genesis::utils::color::ColorNormalizationBoundary(); },
            []() { return new PyCallBack_genesis_utils_color_ColorNormalizationBoundary(); }
        ) );
        // function-signature:
        // genesis::utils::color::ColorNormalizationBoundary::ColorNormalizationBoundary(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/utils/color/norm_boundary.hpp line:93
        cl.def( pybind11::init<const class std::vector<double>&>(), pybind11::arg( "boundaries" ) );

        // function-signature:
        // genesis::utils::color::ColorNormalizationBoundary::ColorNormalizationBoundary(double,
        // double, unsigned long)(double, double, unsigned long)
        // file:genesis/utils/color/norm_boundary.hpp line:99
        cl.def(
            pybind11::init<double, double, unsigned long>(),
            pybind11::arg( "min" ),
            pybind11::arg( "max" ),
            pybind11::arg( "intervals" )
        );

        // function-signature:
        // genesis::utils::color::ColorNormalizationBoundary::ColorNormalizationBoundary(const class
        // genesis::utils::color::ColorNormalizationBoundary &)(const class
        // genesis::utils::color::ColorNormalizationBoundary &)
        // file:genesis/utils/color/norm_boundary.hpp line:106
        cl.def( pybind11::init(
            []( PyCallBack_genesis_utils_color_ColorNormalizationBoundary const& o ) {
                return new PyCallBack_genesis_utils_color_ColorNormalizationBoundary( o );
            }
        ) );
        cl.def( pybind11::init( []( genesis::utils::color::ColorNormalizationBoundary const& o ) {
            return new genesis::utils::color::ColorNormalizationBoundary( o );
        } ) );
        // function-signature: class genesis::utils::color::ColorNormalizationBoundary &
        // genesis::utils::color::ColorNormalizationBoundary::operator=(const class
        // genesis::utils::color::ColorNormalizationBoundary &)(const class
        // genesis::utils::color::ColorNormalizationBoundary &)
        // file:genesis/utils/color/norm_boundary.hpp line:109
        cl.def(
            "assign",
            ( class genesis::utils::color::ColorNormalizationBoundary &
              (genesis::utils::color::
                   ColorNormalizationBoundary::*)(const class genesis::utils::color::
                                                      ColorNormalizationBoundary&)) &
                genesis::utils::color::ColorNormalizationBoundary::operator=,
            "C++: genesis::utils::color::ColorNormalizationBoundary::operator=(const class "
            "genesis::utils::color::ColorNormalizationBoundary &) --> class "
            "genesis::utils::color::ColorNormalizationBoundary &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: const class std::vector<double> &
        // genesis::utils::color::ColorNormalizationBoundary::boundaries() const()
        // file:genesis/utils/color/norm_boundary.hpp line:119
        cl.def(
            "boundaries",
            ( const class std::vector<
                double>& (genesis::utils::color::ColorNormalizationBoundary::*)() const ) &
                genesis::utils::color::ColorNormalizationBoundary::boundaries,
            "Return the boundaries currently set.\n\nC++: "
            "genesis::utils::color::ColorNormalizationBoundary::boundaries() const --> const class "
            "std::vector<double> &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: long
        // genesis::utils::color::ColorNormalizationBoundary::interval(double) const(double)
        // file:genesis/utils/color/norm_boundary.hpp line:127
        cl.def(
            "interval",
            ( long( genesis::utils::color::ColorNormalizationBoundary::* )( double ) const ) &
                genesis::utils::color::ColorNormalizationBoundary::interval,
            "Return the interval index of a value.\n\nC++: "
            "genesis::utils::color::ColorNormalizationBoundary::interval(double) const --> long",
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::utils::color::ColorNormalizationBoundary &
        // genesis::utils::color::ColorNormalizationBoundary::boundaries(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/utils/color/norm_boundary.hpp line:170
        cl.def(
            "boundaries",
            ( class genesis::utils::color::ColorNormalizationBoundary &
              (genesis::utils::color::ColorNormalizationBoundary::*)(const class std::vector<
                                                                     double>&)) &
                genesis::utils::color::ColorNormalizationBoundary::boundaries,
            "Explicitly set the boundaries to the given  Have to be sorted.\n\nC++: "
            "genesis::utils::color::ColorNormalizationBoundary::boundaries(const class "
            "std::vector<double> &) --> class genesis::utils::color::ColorNormalizationBoundary &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "values" )
        );
        // function-signature: class genesis::utils::color::ColorNormalizationBoundary &
        // genesis::utils::color::ColorNormalizationBoundary::scale(double, double, unsigned
        // long)(double, double, unsigned long) file:genesis/utils/color/norm_boundary.hpp line:181
        cl.def(
            "scale",
            ( class genesis::utils::color::ColorNormalizationBoundary &
              (genesis::utils::color::ColorNormalizationBoundary::*)(double, double, unsigned long
              )) &
                genesis::utils::color::ColorNormalizationBoundary::scale,
            "Set the boundaries to equal-sizes intervals between  and \n using  many "
            "steps.\n\nC++: genesis::utils::color::ColorNormalizationBoundary::scale(double, "
            "double, unsigned long) --> class genesis::utils::color::ColorNormalizationBoundary &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "min" ),
            pybind11::arg( "max" ),
            pybind11::arg( "intervals" )
        );
        // function-signature: class genesis::utils::color::ColorNormalizationBoundary &
        // genesis::utils::color::ColorNormalizationBoundary::autoscale(const class
        // std::vector<double> &, unsigned long)(const class std::vector<double> &, unsigned long)
        // file:genesis/utils/color/norm_boundary.hpp line:200
        cl.def(
            "autoscale",
            ( class genesis::utils::color::ColorNormalizationBoundary &
              (genesis::utils::color::ColorNormalizationBoundary::*)(const class std::vector<
                                                                         double>&,
                                                                     unsigned long)) &
                genesis::utils::color::ColorNormalizationBoundary::autoscale,
            "Set the boundaries similar to scale(), but use the given vector of values to "
            "determine\n `min` and `max` first.\n\nC++: "
            "genesis::utils::color::ColorNormalizationBoundary::autoscale(const class "
            "std::vector<double> &, unsigned long) --> class "
            "genesis::utils::color::ColorNormalizationBoundary &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "values" ),
            pybind11::arg( "intervals" )
        );
    }
    { // genesis::utils::color::ColorNormalizationLinear file:genesis/utils/color/norm_linear.hpp
      // line:60
        pybind11::class_<
            genesis::utils::color::ColorNormalizationLinear,
            std::shared_ptr<genesis::utils::color::ColorNormalizationLinear>,
            PyCallBack_genesis_utils_color_ColorNormalizationLinear,
            genesis::utils::color::ColorNormalization>
            cl( M( "genesis::utils::color" ),
                "ColorNormalizationLinear",
                "Default Color normalization, using a sequential linear scaling in the\n range `[ "
                "min, max ]`." );
        // function-signature:
        // genesis::utils::color::ColorNormalizationLinear::ColorNormalizationLinear()()
        // file:genesis/utils/color/norm_linear.hpp line:72
        cl.def( pybind11::init(
            []() { return new genesis::utils::color::ColorNormalizationLinear(); },
            []() { return new PyCallBack_genesis_utils_color_ColorNormalizationLinear(); }
        ) );
        // function-signature:
        // genesis::utils::color::ColorNormalizationLinear::ColorNormalizationLinear(double,
        // double)(double, double) file:genesis/utils/color/norm_linear.hpp line:77
        cl.def( pybind11::init<double, double>(), pybind11::arg( "min" ), pybind11::arg( "max" ) );

        // function-signature:
        // genesis::utils::color::ColorNormalizationLinear::ColorNormalizationLinear(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/utils/color/norm_linear.hpp line:87
        cl.def( pybind11::init<const class std::vector<double>&>(), pybind11::arg( "values" ) );

        // function-signature:
        // genesis::utils::color::ColorNormalizationLinear::ColorNormalizationLinear(const class
        // genesis::utils::color::ColorNormalizationLinear &)(const class
        // genesis::utils::color::ColorNormalizationLinear &)
        // file:genesis/utils/color/norm_linear.hpp line:103
        cl.def( pybind11::init( []( PyCallBack_genesis_utils_color_ColorNormalizationLinear const& o
                                ) {
            return new PyCallBack_genesis_utils_color_ColorNormalizationLinear( o );
        } ) );
        cl.def( pybind11::init( []( genesis::utils::color::ColorNormalizationLinear const& o ) {
            return new genesis::utils::color::ColorNormalizationLinear( o );
        } ) );
        // function-signature: class genesis::utils::color::ColorNormalizationLinear &
        // genesis::utils::color::ColorNormalizationLinear::operator=(const class
        // genesis::utils::color::ColorNormalizationLinear &)(const class
        // genesis::utils::color::ColorNormalizationLinear &)
        // file:genesis/utils/color/norm_linear.hpp line:106
        cl.def(
            "assign",
            ( class genesis::utils::color::ColorNormalizationLinear &
              (genesis::utils::color::
                   ColorNormalizationLinear::*)(const class genesis::utils::color::
                                                    ColorNormalizationLinear&)) &
                genesis::utils::color::ColorNormalizationLinear::operator=,
            "C++: genesis::utils::color::ColorNormalizationLinear::operator=(const class "
            "genesis::utils::color::ColorNormalizationLinear &) --> class "
            "genesis::utils::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: double genesis::utils::color::ColorNormalizationLinear::min_value()
        // const() file:genesis/utils/color/norm_linear.hpp line:116
        cl.def(
            "min_value",
            ( double( genesis::utils::color::ColorNormalizationLinear::* )() const ) &
                genesis::utils::color::ColorNormalizationLinear::min_value,
            "Minimum value, that is, where to begin the color scale.\n\nC++: "
            "genesis::utils::color::ColorNormalizationLinear::min_value() const --> double"
        );
        // function-signature: double genesis::utils::color::ColorNormalizationLinear::max_value()
        // const() file:genesis/utils/color/norm_linear.hpp line:124
        cl.def(
            "max_value",
            ( double( genesis::utils::color::ColorNormalizationLinear::* )() const ) &
                genesis::utils::color::ColorNormalizationLinear::max_value,
            "Minimum value, that is, where to end the color scale.\n\nC++: "
            "genesis::utils::color::ColorNormalizationLinear::max_value() const --> double"
        );
        // function-signature: class genesis::utils::color::ColorNormalizationLinear &
        // genesis::utils::color::ColorNormalizationLinear::scale(double, double)(double, double)
        // file:genesis/utils/color/norm_linear.hpp line:136
        cl.def(
            "scale",
            ( class genesis::utils::color::ColorNormalizationLinear &
              (genesis::utils::color::ColorNormalizationLinear::*)(double, double)) &
                genesis::utils::color::ColorNormalizationLinear::scale,
            "C++: genesis::utils::color::ColorNormalizationLinear::scale(double, double) --> class "
            "genesis::utils::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "min" ),
            pybind11::arg( "max" )
        );
        // function-signature: class genesis::utils::color::ColorNormalizationLinear &
        // genesis::utils::color::ColorNormalizationLinear::autoscale(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/utils/color/norm_linear.hpp line:147
        cl.def(
            "autoscale",
            ( class genesis::utils::color::ColorNormalizationLinear &
              (genesis::utils::color::ColorNormalizationLinear::*)(const class std::vector<double>&)
            ) & genesis::utils::color::ColorNormalizationLinear::autoscale,
            "C++: genesis::utils::color::ColorNormalizationLinear::autoscale(const class "
            "std::vector<double> &) --> class genesis::utils::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "values" )
        );
        // function-signature: class genesis::utils::color::ColorNormalizationLinear &
        // genesis::utils::color::ColorNormalizationLinear::autoscale_min(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/utils/color/norm_linear.hpp line:155
        cl.def(
            "autoscale_min",
            ( class genesis::utils::color::ColorNormalizationLinear &
              (genesis::utils::color::ColorNormalizationLinear::*)(const class std::vector<double>&)
            ) & genesis::utils::color::ColorNormalizationLinear::autoscale_min,
            "C++: genesis::utils::color::ColorNormalizationLinear::autoscale_min(const class "
            "std::vector<double> &) --> class genesis::utils::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "values" )
        );
        // function-signature: class genesis::utils::color::ColorNormalizationLinear &
        // genesis::utils::color::ColorNormalizationLinear::autoscale_max(const class
        // std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/utils/color/norm_linear.hpp line:163
        cl.def(
            "autoscale_max",
            ( class genesis::utils::color::ColorNormalizationLinear &
              (genesis::utils::color::ColorNormalizationLinear::*)(const class std::vector<double>&)
            ) & genesis::utils::color::ColorNormalizationLinear::autoscale_max,
            "C++: genesis::utils::color::ColorNormalizationLinear::autoscale_max(const class "
            "std::vector<double> &) --> class genesis::utils::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "values" )
        );
        // function-signature: class genesis::utils::color::ColorNormalizationLinear &
        // genesis::utils::color::ColorNormalizationLinear::min_value(double)(double)
        // file:genesis/utils/color/norm_linear.hpp line:209
        cl.def(
            "min_value",
            ( class genesis::utils::color::ColorNormalizationLinear &
              (genesis::utils::color::ColorNormalizationLinear::*)(double)) &
                genesis::utils::color::ColorNormalizationLinear::min_value,
            "C++: genesis::utils::color::ColorNormalizationLinear::min_value(double) --> class "
            "genesis::utils::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::utils::color::ColorNormalizationLinear &
        // genesis::utils::color::ColorNormalizationLinear::max_value(double)(double)
        // file:genesis/utils/color/norm_linear.hpp line:219
        cl.def(
            "max_value",
            ( class genesis::utils::color::ColorNormalizationLinear &
              (genesis::utils::color::ColorNormalizationLinear::*)(double)) &
                genesis::utils::color::ColorNormalizationLinear::max_value,
            "C++: genesis::utils::color::ColorNormalizationLinear::max_value(double) --> class "
            "genesis::utils::color::ColorNormalizationLinear &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
    }
    { // genesis::utils::color::ColorNormalizationDiverging
      // file:genesis/utils/color/norm_diverging.hpp line:72
        pybind11::class_<
            genesis::utils::color::ColorNormalizationDiverging,
            std::shared_ptr<genesis::utils::color::ColorNormalizationDiverging>,
            PyCallBack_genesis_utils_color_ColorNormalizationDiverging,
            genesis::utils::color::ColorNormalizationLinear>
            cl( M( "genesis::utils::color" ),
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
        // genesis::utils::color::ColorNormalizationDiverging::ColorNormalizationDiverging()()
        // file:genesis/utils/color/norm_diverging.hpp line:84
        cl.def( pybind11::init(
            []() { return new genesis::utils::color::ColorNormalizationDiverging(); },
            []() { return new PyCallBack_genesis_utils_color_ColorNormalizationDiverging(); }
        ) );
        // function-signature:
        // genesis::utils::color::ColorNormalizationDiverging::ColorNormalizationDiverging(double,
        // double)(double, double) file:genesis/utils/color/norm_diverging.hpp line:93
        cl.def( pybind11::init<double, double>(), pybind11::arg( "min" ), pybind11::arg( "max" ) );

        // function-signature:
        // genesis::utils::color::ColorNormalizationDiverging::ColorNormalizationDiverging(double,
        // double, double)(double, double, double) file:genesis/utils/color/norm_diverging.hpp
        // line:100
        cl.def(
            pybind11::init<double, double, double>(),
            pybind11::arg( "min" ),
            pybind11::arg( "mid" ),
            pybind11::arg( "max" )
        );

        // function-signature:
        // genesis::utils::color::ColorNormalizationDiverging::ColorNormalizationDiverging(const
        // class std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/utils/color/norm_diverging.hpp line:111
        cl.def( pybind11::init<const class std::vector<double>&>(), pybind11::arg( "values" ) );

        // function-signature:
        // genesis::utils::color::ColorNormalizationDiverging::ColorNormalizationDiverging(const
        // class genesis::utils::color::ColorNormalizationDiverging &)(const class
        // genesis::utils::color::ColorNormalizationDiverging &)
        // file:genesis/utils/color/norm_diverging.hpp line:128
        cl.def( pybind11::init(
            []( PyCallBack_genesis_utils_color_ColorNormalizationDiverging const& o ) {
                return new PyCallBack_genesis_utils_color_ColorNormalizationDiverging( o );
            }
        ) );
        cl.def( pybind11::init( []( genesis::utils::color::ColorNormalizationDiverging const& o ) {
            return new genesis::utils::color::ColorNormalizationDiverging( o );
        } ) );
        // function-signature: class genesis::utils::color::ColorNormalizationDiverging &
        // genesis::utils::color::ColorNormalizationDiverging::operator=(const class
        // genesis::utils::color::ColorNormalizationDiverging &)(const class
        // genesis::utils::color::ColorNormalizationDiverging &)
        // file:genesis/utils/color/norm_diverging.hpp line:131
        cl.def(
            "assign",
            ( class genesis::utils::color::ColorNormalizationDiverging &
              (genesis::utils::color::
                   ColorNormalizationDiverging::*)(const class genesis::utils::color::
                                                       ColorNormalizationDiverging&)) &
                genesis::utils::color::ColorNormalizationDiverging::operator=,
            "C++: genesis::utils::color::ColorNormalizationDiverging::operator=(const class "
            "genesis::utils::color::ColorNormalizationDiverging &) --> class "
            "genesis::utils::color::ColorNormalizationDiverging &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: double
        // genesis::utils::color::ColorNormalizationDiverging::mid_value() const()
        // file:genesis/utils/color/norm_diverging.hpp line:141
        cl.def(
            "mid_value",
            ( double( genesis::utils::color::ColorNormalizationDiverging::* )() const ) &
                genesis::utils::color::ColorNormalizationDiverging::mid_value,
            "Mid-point value, that is, where the middle value of a diverging_color() is.\n\nC++: "
            "genesis::utils::color::ColorNormalizationDiverging::mid_value() const --> double"
        );
        // function-signature: class genesis::utils::color::ColorNormalizationDiverging &
        // genesis::utils::color::ColorNormalizationDiverging::make_centric(double)(double)
        // file:genesis/utils/color/norm_diverging.hpp line:156
        cl.def(
            "make_centric",
            []( genesis::utils::color::ColorNormalizationDiverging& o
            ) -> genesis::utils::color::ColorNormalizationDiverging& { return o.make_centric(); },
            "",
            pybind11::return_value_policy::reference_internal
        );
        cl.def(
            "make_centric",
            ( class genesis::utils::color::ColorNormalizationDiverging &
              (genesis::utils::color::ColorNormalizationDiverging::*)(double)) &
                genesis::utils::color::ColorNormalizationDiverging::make_centric,
            "Make the range symmetric around a center value.\n\n For example, if autoscale() "
            "yielded `min == -0.89` and `max == 0.95`, then calling this\n function with `center "
            "== 0.0` sets `min == -0.95` and `mid == 0.0`;\n\nC++: "
            "genesis::utils::color::ColorNormalizationDiverging::make_centric(double) --> class "
            "genesis::utils::color::ColorNormalizationDiverging &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "center" )
        );
        // function-signature: class genesis::utils::color::ColorNormalizationDiverging &
        // genesis::utils::color::ColorNormalizationDiverging::mid_value(double)(double)
        // file:genesis/utils/color/norm_diverging.hpp line:173
        cl.def(
            "mid_value",
            ( class genesis::utils::color::ColorNormalizationDiverging &
              (genesis::utils::color::ColorNormalizationDiverging::*)(double)) &
                genesis::utils::color::ColorNormalizationDiverging::mid_value,
            "C++: genesis::utils::color::ColorNormalizationDiverging::mid_value(double) --> class "
            "genesis::utils::color::ColorNormalizationDiverging &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
    }
}
