#include <genesis/util/color/normalization.hpp>
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

// genesis::util::color::ColorNormalization file:genesis/util/color/normalization.hpp line:53
struct PyCallBack_genesis_util_color_ColorNormalization
    : public genesis::util::color::ColorNormalization
{
    using genesis::util::color::ColorNormalization::ColorNormalization;

    double normalize_( double a0 ) const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalization*>( this ), "normalize_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>( a0 );
            if( pybind11::detail::cast_is_temporary_value_reference<double>::value ) {
                static pybind11::detail::override_caster_t<double> caster;
                return pybind11::detail::cast_ref<double>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<double>( std::move( o ) );
        }
        pybind11::pybind11_fail(
            "Tried to call pure virtual function \"ColorNormalization::normalize_\""
        );
    }
    bool is_valid_() const override
    {
        pybind11::gil_scoped_acquire gil;
        pybind11::function overload = pybind11::get_overload(
            static_cast<const genesis::util::color::ColorNormalization*>( this ), "is_valid_"
        );
        if( overload ) {
            auto o = overload.operator()<pybind11::return_value_policy::reference>();
            if( pybind11::detail::cast_is_temporary_value_reference<bool>::value ) {
                static pybind11::detail::override_caster_t<bool> caster;
                return pybind11::detail::cast_ref<bool>( std::move( o ), caster );
            }
            return pybind11::detail::cast_safe<bool>( std::move( o ) );
        }
        pybind11::pybind11_fail(
            "Tried to call pure virtual function \"ColorNormalization::is_valid_\""
        );
    }
};

void bind_genesis_util_color_normalization(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::color::ColorNormalization file:genesis/util/color/normalization.hpp line:53
        pybind11::class_<
            genesis::util::color::ColorNormalization,
            std::shared_ptr<genesis::util::color::ColorNormalization>,
            PyCallBack_genesis_util_color_ColorNormalization>
            cl( M( "genesis::util::color" ),
                "ColorNormalization",
                "Base class for color normalization." );
        // function-signature: genesis::util::color::ColorNormalization::ColorNormalization()()
        // file:genesis/util/color/normalization.hpp line:61
        cl.def( pybind11::init( []() {
            return new PyCallBack_genesis_util_color_ColorNormalization();
        } ) );
        // function-signature: genesis::util::color::ColorNormalization::ColorNormalization(const
        // class genesis::util::color::ColorNormalization &)(const class
        // genesis::util::color::ColorNormalization &) file:genesis/util/color/normalization.hpp
        // line:64
        cl.def( pybind11::init<PyCallBack_genesis_util_color_ColorNormalization const&>() );
        // function-signature: class genesis::util::color::ColorNormalization &
        // genesis::util::color::ColorNormalization::operator=(const class
        // genesis::util::color::ColorNormalization &)(const class
        // genesis::util::color::ColorNormalization &) file:genesis/util/color/normalization.hpp
        // line:67
        cl.def(
            "assign",
            ( class genesis::util::color::ColorNormalization &
              (genesis::util::color::ColorNormalization::*)(const class genesis::util::color::
                                                                ColorNormalization&)) &
                genesis::util::color::ColorNormalization::operator=,
            "C++: genesis::util::color::ColorNormalization::operator=(const class "
            "genesis::util::color::ColorNormalization &) --> class "
            "genesis::util::color::ColorNormalization &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: double genesis::util::color::ColorNormalization::mask_value() const()
        // file:genesis/util/color/normalization.hpp line:84
        cl.def(
            "mask_value",
            ( double( genesis::util::color::ColorNormalization::* )() const ) &
                genesis::util::color::ColorNormalization::mask_value,
            "Mask value that identifies invalid values.\n\n When querying a value that compares "
            "equal to the mask value, the color functions\n return mask_color() instead of a Color "
            "from the palette. This is meant as a simple means\n of filtering and visualizing "
            "invalid values.\n\n The mask value is initialized to `quiet_NaN`, so that it never "
            "compares equal to anything.\n Thus, it is not used by default has to be explicitly "
            "set to a different value.\n\nC++: "
            "genesis::util::color::ColorNormalization::mask_value() const --> double"
        );
        // function-signature: bool genesis::util::color::ColorNormalization::is_valid() const()
        // file:genesis/util/color/normalization.hpp line:92
        cl.def(
            "is_valid",
            ( bool( genesis::util::color::ColorNormalization::* )() const ) &
                genesis::util::color::ColorNormalization::is_valid,
            "Return whether ranges and other values are correct.\n\nC++: "
            "genesis::util::color::ColorNormalization::is_valid() const --> bool"
        );
        // function-signature: class genesis::util::color::ColorNormalization &
        // genesis::util::color::ColorNormalization::mask_value(double)(double)
        // file:genesis/util/color/normalization.hpp line:104
        cl.def(
            "mask_value",
            ( class genesis::util::color::ColorNormalization &
              (genesis::util::color::ColorNormalization::*)(double)) &
                genesis::util::color::ColorNormalization::mask_value,
            "C++: genesis::util::color::ColorNormalization::mask_value(double) --> class "
            "genesis::util::color::ColorNormalization &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: double genesis::util::color::ColorNormalization::operator()(double)
        // const(double) file:genesis/util/color/normalization.hpp line:120
        cl.def(
            "__call__",
            ( double( genesis::util::color::ColorNormalization::* )( double ) const ) &
                genesis::util::color::ColorNormalization::operator(),
            "Normalize a  into range `[ 0.0, 1.0 ]`.\n\n The function first checks whether the "
            "value is finite or mask_value(),\n and returns `quiet_NaN` if so. If not, the "
            "normalization is applied.\n\nC++: "
            "genesis::util::color::ColorNormalization::operator()(double) const --> double",
            pybind11::arg( "value" )
        );
        // function-signature: class std::vector<double>
        // genesis::util::color::ColorNormalization::operator()(const class std::vector<double> &)
        // const(const class std::vector<double> &) file:genesis/util/color/normalization.hpp
        // line:133
        cl.def(
            "__call__",
            ( class std::vector<
                double>( genesis::util::color::ColorNormalization::* )( const class std::vector<
                                                                        double>& ) const ) &
                genesis::util::color::ColorNormalization::operator(),
            "Return the normalized values for a vector of \n\n Simply applies operator() ( double "
            "value ) to all values.\n\nC++: "
            "genesis::util::color::ColorNormalization::operator()(const class std::vector<double> "
            "&) const --> class std::vector<double>",
            pybind11::arg( "values" )
        );
    }
}
