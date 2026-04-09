#include <genesis/util/color/norm_linear.hpp>
#include <genesis/util/color/norm_logarithmic.hpp>
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

// genesis::util::color::ColorNormalizationLogarithmic file:genesis/util/color/norm_logarithmic.hpp
// line:53
struct PyCallBack_genesis_util_color_ColorNormalizationLogarithmic
    : public genesis::util::color::ColorNormalizationLogarithmic
{
    using genesis::util::color::ColorNormalizationLogarithmic::ColorNormalizationLogarithmic;

    bool is_valid_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationLogarithmic*>( this ),
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
        return ColorNormalizationLogarithmic::is_valid_();
    }
    void is_valid_or_throw_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationLogarithmic*>( this ),
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
        return ColorNormalizationLogarithmic::is_valid_or_throw_();
    }
    double normalize_( double a0 ) const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationLogarithmic*>( this ),
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
        return ColorNormalizationLogarithmic::normalize_( a0 );
    }
    void update_hook_( double a0, double a1 ) override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalizationLogarithmic*>( this ),
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

void bind_genesis_util_color_norm_logarithmic(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::color::ColorNormalizationLogarithmic
      // file:genesis/util/color/norm_logarithmic.hpp line:53
        pybind11::class_<
            genesis::util::color::ColorNormalizationLogarithmic,
            std::shared_ptr<genesis::util::color::ColorNormalizationLogarithmic>,
            PyCallBack_genesis_util_color_ColorNormalizationLogarithmic,
            genesis::util::color::ColorNormalizationLinear>
            cl( M( "genesis::util::color" ),
                "ColorNormalizationLogarithmic",
                "Color normalization for a logarithmic scale." );
        // function-signature:
        // genesis::util::color::ColorNormalizationLogarithmic::ColorNormalizationLogarithmic()()
        // file:genesis/util/color/norm_logarithmic.hpp line:65
        cl.def( pybind11::init(
            []() { return new genesis::util::color::ColorNormalizationLogarithmic(); },
            []() { return new PyCallBack_genesis_util_color_ColorNormalizationLogarithmic(); }
        ) );
        // function-signature:
        // genesis::util::color::ColorNormalizationLogarithmic::ColorNormalizationLogarithmic(double,
        // double)(double, double) file:genesis/util/color/norm_logarithmic.hpp line:72
        cl.def( pybind11::init<double, double>(), pybind11::arg( "min" ), pybind11::arg( "max" ) );

        // function-signature:
        // genesis::util::color::ColorNormalizationLogarithmic::ColorNormalizationLogarithmic(const
        // class std::vector<double> &)(const class std::vector<double> &)
        // file:genesis/util/color/norm_logarithmic.hpp line:80
        cl.def( pybind11::init<const class std::vector<double>&>(), pybind11::arg( "values" ) );

        // function-signature:
        // genesis::util::color::ColorNormalizationLogarithmic::ColorNormalizationLogarithmic(const
        // class genesis::util::color::ColorNormalizationLogarithmic &)(const class
        // genesis::util::color::ColorNormalizationLogarithmic &)
        // file:genesis/util/color/norm_logarithmic.hpp line:97
        cl.def( pybind11::init(
            []( PyCallBack_genesis_util_color_ColorNormalizationLogarithmic const& o ) {
                return new PyCallBack_genesis_util_color_ColorNormalizationLogarithmic( o );
            }
        ) );
        cl.def( pybind11::init( []( genesis::util::color::ColorNormalizationLogarithmic const& o ) {
            return new genesis::util::color::ColorNormalizationLogarithmic( o );
        } ) );
        // function-signature: class genesis::util::color::ColorNormalizationLogarithmic &
        // genesis::util::color::ColorNormalizationLogarithmic::operator=(const class
        // genesis::util::color::ColorNormalizationLogarithmic &)(const class
        // genesis::util::color::ColorNormalizationLogarithmic &)
        // file:genesis/util/color/norm_logarithmic.hpp line:100
        cl.def(
            "assign",
            ( class genesis::util::color::ColorNormalizationLogarithmic &
              (genesis::util::color::
                   ColorNormalizationLogarithmic::*)(const class genesis::util::color::
                                                         ColorNormalizationLogarithmic&)) &
                genesis::util::color::ColorNormalizationLogarithmic::operator=,
            "C++: genesis::util::color::ColorNormalizationLogarithmic::operator=(const class "
            "genesis::util::color::ColorNormalizationLogarithmic &) --> class "
            "genesis::util::color::ColorNormalizationLogarithmic &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: double genesis::util::color::ColorNormalizationLogarithmic::base()
        // const() file:genesis/util/color/norm_logarithmic.hpp line:107
        cl.def(
            "base",
            ( double( genesis::util::color::ColorNormalizationLogarithmic::* )() const ) &
                genesis::util::color::ColorNormalizationLogarithmic::base,
            "C++: genesis::util::color::ColorNormalizationLogarithmic::base() const --> double"
        );
        // function-signature: class genesis::util::color::ColorNormalizationLogarithmic &
        // genesis::util::color::ColorNormalizationLogarithmic::base(double)(double)
        // file:genesis/util/color/norm_logarithmic.hpp line:112
        cl.def(
            "base",
            ( class genesis::util::color::ColorNormalizationLogarithmic &
              (genesis::util::color::ColorNormalizationLogarithmic::*)(double)) &
                genesis::util::color::ColorNormalizationLogarithmic::base,
            "C++: genesis::util::color::ColorNormalizationLogarithmic::base(double) --> class "
            "genesis::util::color::ColorNormalizationLogarithmic &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: bool
        // genesis::util::color::ColorNormalizationLogarithmic::exponential_labels() const()
        // file:genesis/util/color/norm_logarithmic.hpp line:118
        cl.def(
            "exponential_labels",
            ( bool( genesis::util::color::ColorNormalizationLogarithmic::* )() const ) &
                genesis::util::color::ColorNormalizationLogarithmic::exponential_labels,
            "C++: genesis::util::color::ColorNormalizationLogarithmic::exponential_labels() const "
            "--> bool"
        );
        // function-signature: class genesis::util::color::ColorNormalizationLogarithmic &
        // genesis::util::color::ColorNormalizationLogarithmic::exponential_labels(bool)(bool)
        // file:genesis/util/color/norm_logarithmic.hpp line:123
        cl.def(
            "exponential_labels",
            ( class genesis::util::color::ColorNormalizationLogarithmic &
              (genesis::util::color::ColorNormalizationLogarithmic::*)(bool)) &
                genesis::util::color::ColorNormalizationLogarithmic::exponential_labels,
            "C++: genesis::util::color::ColorNormalizationLogarithmic::exponential_labels(bool) "
            "--> class genesis::util::color::ColorNormalizationLogarithmic &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
    }
}
