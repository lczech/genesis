#include <genesis/util/math/compensated_sum.hpp>
#include <genesis/util/math/compensated_sum_add_ons.hpp>
#include <genesis/util/math/compensated_sum_instances.hpp>
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

void bind_genesis_util_math_compensated_sum_instances(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::math::CompensatedSum file:genesis/util/math/compensated_sum_instances.hpp
      // line:34
        pybind11::class_<
            genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>,
            std::shared_ptr<
                genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>>>
            cl( M( "genesis::util::math" ), "KahanSum", "" );
        // function-signature:
        // genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::CompensatedSum()()
        // file:genesis/util/math/compensated_sum.hpp line:117
        cl.def( pybind11::init( []() {
            return new genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>();
        } ) );
        // function-signature:
        // genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::CompensatedSum(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:122
        cl.def( pybind11::init<double>(), pybind11::arg( "value" ) );

        // function-signature:
        // genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::CompensatedSum(const
        // class genesis::util::math::CompensatedSum<struct genesis::util::math::KahanSummation>
        // &)(const class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KahanSummation> &) file:genesis/util/math/compensated_sum.hpp
        // line:153
        cl.def( pybind11::init(
            []( genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation> const& o
            ) {
                return new genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>(
                    o
                );
            }
        ) );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KahanSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator=(const
        // class genesis::util::math::CompensatedSum<struct genesis::util::math::KahanSummation>
        // &)(const class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KahanSummation> &) file:genesis/util/math/compensated_sum.hpp
        // line:156
        cl.def(
            "assign",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::KahanSummation> &
              (genesis::util::math::CompensatedSum<
                  genesis::util::math::KahanSummation>::*)(const class genesis::util::math::
                                                               CompensatedSum<
                                                                   struct genesis::util::math::
                                                                       KahanSummation>&)) &
                genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator=,
            "C++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator=("
            "const class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::KahanSummation> &) --> class "
            "genesis::util::math::CompensatedSum<struct genesis::util::math::KahanSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KahanSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator+=(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:166
        cl.def(
            "__iadd__",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::KahanSummation> &
              (genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::*)(double
              )) &
                genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator+=
            ,
            "Add a  to the sum.\n\nC++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator+=("
            "double) --> class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::KahanSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KahanSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator-=(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:177
        cl.def(
            "__isub__",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::KahanSummation> &
              (genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::*)(double
              )) &
                genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator-=
            ,
            "Subtract a  from the sum.\n\n This is identical to addting the negative of the \n     "
            "\n\nC++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator-=("
            "double) --> class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::KahanSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KahanSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator=(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:189
        cl.def(
            "assign",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::KahanSummation> &
              (genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::*)(double
              )) &
                genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator=,
            "Set the sum to the given \n\n This will also reset the correction term, as we assume "
            "that assining a new value\n is meant to start a new summation.\n\nC++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::operator=("
            "double) --> class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::KahanSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::add(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:212
        cl.def(
            "add",
            ( void( genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::* )(
                double
            ) ) &
                genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::add,
            "C++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::add(double) "
            "--> void",
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::reset()()
        // file:genesis/util/math/compensated_sum.hpp line:217
        cl.def(
            "reset",
            ( void( genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::* )()
            ) & genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::reset,
            "C++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::reset() --> "
            "void"
        );
        // function-signature: double
        // genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::get() const()
        // file:genesis/util/math/compensated_sum.hpp line:225
        cl.def(
            "get",
            ( double( genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::* )(
            ) const ) &
                genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::get,
            "C++: genesis::util::math::CompensatedSum<genesis::util::math::KahanSummation>::get() "
            "const --> double"
        );

        genesis_util_math_compensated_sum_add_ons( cl );
    }
    { // genesis::util::math::CompensatedSum file:genesis/util/math/compensated_sum_instances.hpp
      // line:35
        pybind11::class_<
            genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>,
            std::shared_ptr<
                genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>>>
            cl( M( "genesis::util::math" ), "NeumaierSum", "" );
        // function-signature:
        // genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::CompensatedSum()()
        // file:genesis/util/math/compensated_sum.hpp line:117
        cl.def( pybind11::init( []() {
            return new genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>(
            );
        } ) );
        // function-signature:
        // genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::CompensatedSum(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:122
        cl.def( pybind11::init<double>(), pybind11::arg( "value" ) );

        // function-signature:
        // genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::CompensatedSum(const
        // class genesis::util::math::CompensatedSum<struct genesis::util::math::NeumaierSummation>
        // &)(const class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::NeumaierSummation> &) file:genesis/util/math/compensated_sum.hpp
        // line:153
        cl.def( pybind11::init( []( genesis::util::math::CompensatedSum<
                                    genesis::util::math::NeumaierSummation> const& o ) {
            return new genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>(
                o
            );
        } ) );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::NeumaierSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::operator=(const
        // class genesis::util::math::CompensatedSum<struct genesis::util::math::NeumaierSummation>
        // &)(const class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::NeumaierSummation> &) file:genesis/util/math/compensated_sum.hpp
        // line:156
        cl.def(
            "assign",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::NeumaierSummation> &
              (genesis::util::math::CompensatedSum<
                  genesis::util::math::NeumaierSummation>::*)(const class genesis::util::math::
                                                                  CompensatedSum<
                                                                      struct genesis::util::math::
                                                                          NeumaierSummation>&)) &
                genesis::util::math::CompensatedSum<
                    genesis::util::math::NeumaierSummation>::operator=,
            "C++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::operator="
            "(const class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::NeumaierSummation> &) --> class "
            "genesis::util::math::CompensatedSum<struct genesis::util::math::NeumaierSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::NeumaierSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::operator+=(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:166
        cl.def(
            "__iadd__",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::NeumaierSummation> &
              (genesis::util::math::CompensatedSum<
                  genesis::util::math::NeumaierSummation>::*)(double)) &
                genesis::util::math::CompensatedSum<
                    genesis::util::math::NeumaierSummation>::operator+=,
            "Add a  to the sum.\n\nC++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::operator+"
            "=(double) --> class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::NeumaierSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::NeumaierSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::operator-=(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:177
        cl.def(
            "__isub__",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::NeumaierSummation> &
              (genesis::util::math::CompensatedSum<
                  genesis::util::math::NeumaierSummation>::*)(double)) &
                genesis::util::math::CompensatedSum<
                    genesis::util::math::NeumaierSummation>::operator-=,
            "Subtract a  from the sum.\n\n This is identical to addting the negative of the \n     "
            "\n\nC++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::operator-"
            "=(double) --> class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::NeumaierSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::NeumaierSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::operator=(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:189
        cl.def(
            "assign",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::NeumaierSummation> &
              (genesis::util::math::CompensatedSum<
                  genesis::util::math::NeumaierSummation>::*)(double)) &
                genesis::util::math::CompensatedSum<
                    genesis::util::math::NeumaierSummation>::operator=,
            "Set the sum to the given \n\n This will also reset the correction term, as we assume "
            "that assining a new value\n is meant to start a new summation.\n\nC++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::operator="
            "(double) --> class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::NeumaierSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::add(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:212
        cl.def(
            "add",
            ( void( genesis::util::math::CompensatedSum<
                    genesis::util::math::NeumaierSummation>::* )( double ) ) &
                genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::add,
            "C++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::add("
            "double) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::reset()()
        // file:genesis/util/math/compensated_sum.hpp line:217
        cl.def(
            "reset",
            ( void( genesis::util::math::CompensatedSum<
                    genesis::util::math::NeumaierSummation>::* )() ) &
                genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::reset,
            "C++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::reset() "
            "--> void"
        );
        // function-signature: double
        // genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::get()
        // const() file:genesis/util/math/compensated_sum.hpp line:225
        cl.def(
            "get",
            ( double( genesis::util::math::CompensatedSum<
                      genesis::util::math::NeumaierSummation>::* )() const ) &
                genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::get,
            "C++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::NeumaierSummation>::get() "
            "const --> double"
        );

        genesis_util_math_compensated_sum_add_ons( cl );
    }
    { // genesis::util::math::CompensatedSum file:genesis/util/math/compensated_sum_instances.hpp
      // line:36
        pybind11::class_<
            genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>,
            std::shared_ptr<
                genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>>>
            cl( M( "genesis::util::math" ), "KleinSum", "" );
        // function-signature:
        // genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::CompensatedSum()()
        // file:genesis/util/math/compensated_sum.hpp line:117
        cl.def( pybind11::init( []() {
            return new genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>();
        } ) );
        // function-signature:
        // genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::CompensatedSum(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:122
        cl.def( pybind11::init<double>(), pybind11::arg( "value" ) );

        // function-signature:
        // genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::CompensatedSum(const
        // class genesis::util::math::CompensatedSum<struct genesis::util::math::KleinSummation>
        // &)(const class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KleinSummation> &) file:genesis/util/math/compensated_sum.hpp
        // line:153
        cl.def( pybind11::init(
            []( genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation> const& o
            ) {
                return new genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>(
                    o
                );
            }
        ) );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KleinSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator=(const
        // class genesis::util::math::CompensatedSum<struct genesis::util::math::KleinSummation>
        // &)(const class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KleinSummation> &) file:genesis/util/math/compensated_sum.hpp
        // line:156
        cl.def(
            "assign",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::KleinSummation> &
              (genesis::util::math::CompensatedSum<
                  genesis::util::math::KleinSummation>::*)(const class genesis::util::math::
                                                               CompensatedSum<
                                                                   struct genesis::util::math::
                                                                       KleinSummation>&)) &
                genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator=,
            "C++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator=("
            "const class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::KleinSummation> &) --> class "
            "genesis::util::math::CompensatedSum<struct genesis::util::math::KleinSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KleinSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator+=(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:166
        cl.def(
            "__iadd__",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::KleinSummation> &
              (genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::*)(double
              )) &
                genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator+=
            ,
            "Add a  to the sum.\n\nC++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator+=("
            "double) --> class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::KleinSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KleinSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator-=(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:177
        cl.def(
            "__isub__",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::KleinSummation> &
              (genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::*)(double
              )) &
                genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator-=
            ,
            "Subtract a  from the sum.\n\n This is identical to addting the negative of the \n     "
            "\n\nC++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator-=("
            "double) --> class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::KleinSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::math::CompensatedSum<struct
        // genesis::util::math::KleinSummation> &
        // genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator=(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:189
        cl.def(
            "assign",
            ( class genesis::util::math::CompensatedSum<
                  struct genesis::util::math::KleinSummation> &
              (genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::*)(double
              )) &
                genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator=,
            "Set the sum to the given \n\n This will also reset the correction term, as we assume "
            "that assining a new value\n is meant to start a new summation.\n\nC++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::operator=("
            "double) --> class genesis::util::math::CompensatedSum<struct "
            "genesis::util::math::KleinSummation> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::add(double)(double)
        // file:genesis/util/math/compensated_sum.hpp line:212
        cl.def(
            "add",
            ( void( genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::* )(
                double
            ) ) &
                genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::add,
            "C++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::add(double) "
            "--> void",
            pybind11::arg( "value" )
        );
        // function-signature: void
        // genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::reset()()
        // file:genesis/util/math/compensated_sum.hpp line:217
        cl.def(
            "reset",
            ( void( genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::* )()
            ) & genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::reset,
            "C++: "
            "genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::reset() --> "
            "void"
        );
        // function-signature: double
        // genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::get() const()
        // file:genesis/util/math/compensated_sum.hpp line:225
        cl.def(
            "get",
            ( double( genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::* )(
            ) const ) &
                genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::get,
            "C++: genesis::util::math::CompensatedSum<genesis::util::math::KleinSummation>::get() "
            "const --> double"
        );

        genesis_util_math_compensated_sum_add_ons( cl );
    }
}
