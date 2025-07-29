#include <genesis/utils/math/compensated_sum.hpp>
#include <genesis/utils/math/compensated_sum_add_ons.hpp>
#include <genesis/utils/math/compensated_sum_instances.hpp>
#include <sstream> // __str__

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

void bind_genesis_utils_math_compensated_sum_instances(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::utils::math::CompensatedSum file:genesis/utils/math/compensated_sum_instances.hpp
      // line:34
        pybind11::class_<
            genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>,
            std::shared_ptr<
                genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>>>
            cl( M( "genesis::utils::math" ),
                "CompensatedSum_genesis_utils_math_KahanSummation_t",
                "" );
        // function-signature:
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::CompensatedSum()()
        // file:genesis/utils/math/compensated_sum.hpp line:117
        cl.def( pybind11::init( []() {
            return new genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>();
        } ) );
        // function-signature:
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::CompensatedSum(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:122
        cl.def( pybind11::init<double>(), pybind11::arg( "value" ) );

        // function-signature:
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::CompensatedSum(const
        // class genesis::utils::math::CompensatedSum<struct genesis::utils::math::KahanSummation>
        // &)(const class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KahanSummation> &) file:genesis/utils/math/compensated_sum.hpp
        // line:153
        cl.def( pybind11::init( []( genesis::utils::math::CompensatedSum<
                                    genesis::utils::math::KahanSummation> const& o ) {
            return new genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>( o
            );
        } ) );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KahanSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::operator=(const
        // class genesis::utils::math::CompensatedSum<struct genesis::utils::math::KahanSummation>
        // &)(const class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KahanSummation> &) file:genesis/utils/math/compensated_sum.hpp
        // line:156
        cl.def(
            "assign",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::KahanSummation> &
              (genesis::utils::math::CompensatedSum<
                  genesis::utils::math::KahanSummation>::*)(const class genesis::utils::math::
                                                                CompensatedSum<
                                                                    struct genesis::utils::math::
                                                                        KahanSummation>&)) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::KahanSummation>::operator=,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::operator=("
            "const class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::KahanSummation> &) --> class "
            "genesis::utils::math::CompensatedSum<struct genesis::utils::math::KahanSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KahanSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::operator+=(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:166
        cl.def(
            "__iadd__",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::KahanSummation> &
              (genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::*)(double
              )) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::KahanSummation>::operator+=,
            "Add a  to the sum.\n\nC++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::operator+="
            "(double) --> class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::KahanSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KahanSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::operator-=(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:177
        cl.def(
            "__isub__",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::KahanSummation> &
              (genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::*)(double
              )) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::KahanSummation>::operator-=,
            "Subtract a  from the sum.\n\n This is identical to addting the negative of the \n     "
            "\n\nC++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::operator-="
            "(double) --> class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::KahanSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KahanSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::operator=(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:189
        cl.def(
            "assign",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::KahanSummation> &
              (genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::*)(double
              )) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::KahanSummation>::operator=,
            "Set the sum to the given \n\n This will also reset the correction term, as we assume "
            "that assining a new value\n is meant to start a new summation.\n\nC++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::operator=("
            "double) --> class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::KahanSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::add(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:212
        cl.def(
            "add",
            ( void( genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::* )(
                double
            ) ) &
                genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::add,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::add("
            "double) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::reset()()
        // file:genesis/utils/math/compensated_sum.hpp line:217
        cl.def(
            "reset",
            ( void( genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::* )(
            ) ) &
                genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::reset,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::reset() "
            "--> void"
        );
        // function-signature: double
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::get() const()
        // file:genesis/utils/math/compensated_sum.hpp line:225
        cl.def(
            "get",
            ( double( genesis::utils::math::CompensatedSum<
                      genesis::utils::math::KahanSummation>::* )() const ) &
                genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::get,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KahanSummation>::get() "
            "const --> double"
        );

        genesis_utils_math_compensated_sum_add_ons( cl );
    }
    { // genesis::utils::math::CompensatedSum file:genesis/utils/math/compensated_sum_instances.hpp
      // line:35
        pybind11::class_<
            genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>,
            std::shared_ptr<
                genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>>>
            cl( M( "genesis::utils::math" ),
                "CompensatedSum_genesis_utils_math_NeumaierSummation_t",
                "" );
        // function-signature:
        // genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::CompensatedSum()()
        // file:genesis/utils/math/compensated_sum.hpp line:117
        cl.def( pybind11::init( []() {
            return new genesis::utils::math::CompensatedSum<
                genesis::utils::math::NeumaierSummation>();
        } ) );
        // function-signature:
        // genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::CompensatedSum(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:122
        cl.def( pybind11::init<double>(), pybind11::arg( "value" ) );

        // function-signature:
        // genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::CompensatedSum(const
        // class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::NeumaierSummation> &)(const class
        // genesis::utils::math::CompensatedSum<struct genesis::utils::math::NeumaierSummation> &)
        // file:genesis/utils/math/compensated_sum.hpp line:153
        cl.def( pybind11::init( []( genesis::utils::math::CompensatedSum<
                                    genesis::utils::math::NeumaierSummation> const& o ) {
            return new genesis::utils::math::CompensatedSum<
                genesis::utils::math::NeumaierSummation>( o );
        } ) );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::NeumaierSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::operator=(const
        // class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::NeumaierSummation> &)(const class
        // genesis::utils::math::CompensatedSum<struct genesis::utils::math::NeumaierSummation> &)
        // file:genesis/utils/math/compensated_sum.hpp line:156
        cl.def(
            "assign",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::NeumaierSummation> &
              (genesis::utils::math::CompensatedSum<
                  genesis::utils::math::NeumaierSummation>::*)(const class genesis::utils::math::
                                                                   CompensatedSum<
                                                                       struct genesis::utils::math::
                                                                           NeumaierSummation>&)) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::NeumaierSummation>::operator=,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::"
            "operator=(const class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::NeumaierSummation> &) --> class "
            "genesis::utils::math::CompensatedSum<struct genesis::utils::math::NeumaierSummation> "
            "&",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::NeumaierSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::operator+=(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:166
        cl.def(
            "__iadd__",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::NeumaierSummation> &
              (genesis::utils::math::CompensatedSum<
                  genesis::utils::math::NeumaierSummation>::*)(double)) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::NeumaierSummation>::operator+=,
            "Add a  to the sum.\n\nC++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::"
            "operator+=(double) --> class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::NeumaierSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::NeumaierSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::operator-=(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:177
        cl.def(
            "__isub__",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::NeumaierSummation> &
              (genesis::utils::math::CompensatedSum<
                  genesis::utils::math::NeumaierSummation>::*)(double)) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::NeumaierSummation>::operator-=,
            "Subtract a  from the sum.\n\n This is identical to addting the negative of the \n     "
            "\n\nC++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::"
            "operator-=(double) --> class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::NeumaierSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::NeumaierSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::operator=(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:189
        cl.def(
            "assign",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::NeumaierSummation> &
              (genesis::utils::math::CompensatedSum<
                  genesis::utils::math::NeumaierSummation>::*)(double)) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::NeumaierSummation>::operator=,
            "Set the sum to the given \n\n This will also reset the correction term, as we assume "
            "that assining a new value\n is meant to start a new summation.\n\nC++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::"
            "operator=(double) --> class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::NeumaierSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::add(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:212
        cl.def(
            "add",
            ( void( genesis::utils::math::CompensatedSum<
                    genesis::utils::math::NeumaierSummation>::* )( double ) ) &
                genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::add,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::add("
            "double) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::reset()()
        // file:genesis/utils/math/compensated_sum.hpp line:217
        cl.def(
            "reset",
            ( void( genesis::utils::math::CompensatedSum<
                    genesis::utils::math::NeumaierSummation>::* )() ) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::NeumaierSummation>::reset,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::reset()"
            " --> void"
        );
        // function-signature: double
        // genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::get()
        // const() file:genesis/utils/math/compensated_sum.hpp line:225
        cl.def(
            "get",
            ( double( genesis::utils::math::CompensatedSum<
                      genesis::utils::math::NeumaierSummation>::* )() const ) &
                genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::get,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::NeumaierSummation>::get() "
            "const --> double"
        );

        genesis_utils_math_compensated_sum_add_ons( cl );
    }
    { // genesis::utils::math::CompensatedSum file:genesis/utils/math/compensated_sum_instances.hpp
      // line:36
        pybind11::class_<
            genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>,
            std::shared_ptr<
                genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>>>
            cl( M( "genesis::utils::math" ),
                "CompensatedSum_genesis_utils_math_KleinSummation_t",
                "" );
        // function-signature:
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::CompensatedSum()()
        // file:genesis/utils/math/compensated_sum.hpp line:117
        cl.def( pybind11::init( []() {
            return new genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>();
        } ) );
        // function-signature:
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::CompensatedSum(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:122
        cl.def( pybind11::init<double>(), pybind11::arg( "value" ) );

        // function-signature:
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::CompensatedSum(const
        // class genesis::utils::math::CompensatedSum<struct genesis::utils::math::KleinSummation>
        // &)(const class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KleinSummation> &) file:genesis/utils/math/compensated_sum.hpp
        // line:153
        cl.def( pybind11::init( []( genesis::utils::math::CompensatedSum<
                                    genesis::utils::math::KleinSummation> const& o ) {
            return new genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>( o
            );
        } ) );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KleinSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::operator=(const
        // class genesis::utils::math::CompensatedSum<struct genesis::utils::math::KleinSummation>
        // &)(const class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KleinSummation> &) file:genesis/utils/math/compensated_sum.hpp
        // line:156
        cl.def(
            "assign",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::KleinSummation> &
              (genesis::utils::math::CompensatedSum<
                  genesis::utils::math::KleinSummation>::*)(const class genesis::utils::math::
                                                                CompensatedSum<
                                                                    struct genesis::utils::math::
                                                                        KleinSummation>&)) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::KleinSummation>::operator=,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::operator=("
            "const class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::KleinSummation> &) --> class "
            "genesis::utils::math::CompensatedSum<struct genesis::utils::math::KleinSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KleinSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::operator+=(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:166
        cl.def(
            "__iadd__",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::KleinSummation> &
              (genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::*)(double
              )) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::KleinSummation>::operator+=,
            "Add a  to the sum.\n\nC++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::operator+="
            "(double) --> class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::KleinSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KleinSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::operator-=(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:177
        cl.def(
            "__isub__",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::KleinSummation> &
              (genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::*)(double
              )) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::KleinSummation>::operator-=,
            "Subtract a  from the sum.\n\n This is identical to addting the negative of the \n     "
            "\n\nC++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::operator-="
            "(double) --> class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::KleinSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::utils::math::CompensatedSum<struct
        // genesis::utils::math::KleinSummation> &
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::operator=(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:189
        cl.def(
            "assign",
            ( class genesis::utils::math::CompensatedSum<
                  struct genesis::utils::math::KleinSummation> &
              (genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::*)(double
              )) &
                genesis::utils::math::CompensatedSum<
                    genesis::utils::math::KleinSummation>::operator=,
            "Set the sum to the given \n\n This will also reset the correction term, as we assume "
            "that assining a new value\n is meant to start a new summation.\n\nC++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::operator=("
            "double) --> class genesis::utils::math::CompensatedSum<struct "
            "genesis::utils::math::KleinSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::add(double)(double)
        // file:genesis/utils/math/compensated_sum.hpp line:212
        cl.def(
            "add",
            ( void( genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::* )(
                double
            ) ) &
                genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::add,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::add("
            "double) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::reset()()
        // file:genesis/utils/math/compensated_sum.hpp line:217
        cl.def(
            "reset",
            ( void( genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::* )(
            ) ) &
                genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::reset,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::reset() "
            "--> void"
        );
        // function-signature: double
        // genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::get() const()
        // file:genesis/utils/math/compensated_sum.hpp line:225
        cl.def(
            "get",
            ( double( genesis::utils::math::CompensatedSum<
                      genesis::utils::math::KleinSummation>::* )() const ) &
                genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::get,
            "C++: "
            "genesis::utils::math::CompensatedSum<genesis::utils::math::KleinSummation>::get() "
            "const --> double"
        );

        genesis_utils_math_compensated_sum_add_ons( cl );
    }
}
