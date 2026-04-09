#include <genesis/util/tool/tickmarks.hpp>
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

void bind_genesis_util_tool_tickmarks(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::Tickmarks file:genesis/util/tool/tickmarks.hpp line:48
        pybind11::class_<genesis::util::Tickmarks, std::shared_ptr<genesis::util::Tickmarks>> cl(
            M( "genesis::util" ),
            "Tickmarks",
            "Helper class to find \"nice\" tickmark intervals for creating scales and axes."
        );
        // function-signature: genesis::util::Tickmarks::Tickmarks()()
        // file:genesis/util/tool/tickmarks.hpp line:71
        cl.def( pybind11::init( []() { return new genesis::util::Tickmarks(); } ) );
        // function-signature: genesis::util::Tickmarks::Tickmarks(const struct
        // genesis::util::Tickmarks &)(const struct genesis::util::Tickmarks &)
        // file:genesis/util/tool/tickmarks.hpp line:74
        cl.def( pybind11::init( []( genesis::util::Tickmarks const& o ) {
            return new genesis::util::Tickmarks( o );
        } ) );
        cl.def_readwrite( "include_min", &genesis::util::Tickmarks::include_min );
        cl.def_readwrite( "include_max", &genesis::util::Tickmarks::include_max );
        cl.def_readwrite( "undershoot_at_min", &genesis::util::Tickmarks::undershoot_at_min );
        cl.def_readwrite( "overshoot_at_max", &genesis::util::Tickmarks::overshoot_at_max );
        cl.def_readwrite( "relative_epsilon", &genesis::util::Tickmarks::relative_epsilon );
        // function-signature: struct genesis::util::Tickmarks &
        // genesis::util::Tickmarks::operator=(const struct genesis::util::Tickmarks &)(const struct
        // genesis::util::Tickmarks &) file:genesis/util/tool/tickmarks.hpp line:77
        cl.def(
            "assign",
            ( struct genesis::util::Tickmarks &
              (genesis::util::Tickmarks::*)(const struct genesis::util::Tickmarks&)) &
                genesis::util::Tickmarks::operator=,
            "C++: genesis::util::Tickmarks::operator=(const struct genesis::util::Tickmarks &) --> "
            "struct genesis::util::Tickmarks &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: double genesis::util::Tickmarks::step_size(double, unsigned
        // long)(double, unsigned long) file:genesis/util/tool/tickmarks.hpp line:91
        cl.def_static(
            "step_size",
            ( double ( * )( double, unsigned long ) ) & genesis::util::Tickmarks::step_size,
            "Calculate a step size that fills the `interval_size` in approximately "
            "`target_steps`\n many steps.\n\n The resulting step size is calculated to be a "
            "\"nice\" size, i.e. fits `[ 1 | 2 | 5 ] * 10^n`.\n Thus, exemplary nice step sizes "
            "are 0.01, 20 and 500.\n\nC++: genesis::util::Tickmarks::step_size(double, unsigned "
            "long) --> double",
            pybind11::arg( "interval_size" ),
            pybind11::arg( "target_steps" )
        );
        // function-signature: class std::vector<double>
        // genesis::util::Tickmarks::linear_ticks(double, double, unsigned long)(double, double,
        // unsigned long) file:genesis/util/tool/tickmarks.hpp line:99
        cl.def(
            "linear_ticks",
            ( class std::vector<double>( genesis::util::Tickmarks::* )(
                double, double, unsigned long
            ) ) &
                genesis::util::Tickmarks::linear_ticks,
            "Calculate a set of ticks that linearily span from `min` to `max` in approximately\n "
            "`target_steps` many steps, where each tick sits on a \"nice\" number.\n\n See "
            "step_size() for what is meant by nice numbers.\n\nC++: "
            "genesis::util::Tickmarks::linear_ticks(double, double, unsigned long) --> class "
            "std::vector<double>",
            pybind11::arg( "min" ),
            pybind11::arg( "max" ),
            pybind11::arg( "target_steps" )
        );
        // function-signature: class std::vector<struct genesis::util::Tickmarks::LabeledTick>
        // genesis::util::Tickmarks::linear_labels(double, double, unsigned long)(double, double,
        // unsigned long) file:genesis/util/tool/tickmarks.hpp line:109
        cl.def(
            "linear_labels",
            ( class std::vector<
                struct genesis::util::Tickmarks::LabeledTick>( genesis::util::Tickmarks::* )(
                double, double, unsigned long
            ) ) &
                genesis::util::Tickmarks::linear_labels,
            "Return a set of labels with relative positions between `min` and `max`, where the\n "
            "labels correspond to the linear_ticks() numbers.\n\n This function is a convenience "
            "wrapper for linear_ticks(), that turns the tick positions\n into relative values, "
            "while keeping the actual tick value as a label. Relative position 0.0\n corresponds "
            "to `min`, relative position 1.0 to `max`.\n\nC++: "
            "genesis::util::Tickmarks::linear_labels(double, double, unsigned long) --> class "
            "std::vector<struct genesis::util::Tickmarks::LabeledTick>",
            pybind11::arg( "min" ),
            pybind11::arg( "max" ),
            pybind11::arg( "target_steps" )
        );
        // function-signature: class std::vector<struct genesis::util::Tickmarks::LabeledTick>
        // genesis::util::Tickmarks::logarithmic_labels(double, double, double)(double, double,
        // double) file:genesis/util/tool/tickmarks.hpp line:125
        cl.def(
            "logarithmic_labels",
            []( genesis::util::Tickmarks & o, double const& a0, double const& a1
            ) -> std::vector<struct genesis::util::Tickmarks::LabeledTick> {
                return o.logarithmic_labels( a0, a1 );
            },
            "",
            pybind11::arg( "min" ),
            pybind11::arg( "max" )
        );
        cl.def(
            "logarithmic_labels",
            ( class std::vector<
                struct genesis::util::Tickmarks::LabeledTick>( genesis::util::Tickmarks::* )(
                double, double, double
            ) ) &
                genesis::util::Tickmarks::logarithmic_labels,
            "Return a set of labels with relative positions between `min` and `max`, where the\n "
            "labels correspond to logarithmic ticks.\n\n This function is the equivalent of "
            "linear_labels(), but instead uses logarithmic scaling.\n Also, it inserts as many "
            "ticks as are needed to go up to  in powers of \n and thus has no target steps.\n\n "
            "Example: Given `min == 1.0`, `max == 350`, with `overshoot_at_max == false` and\n "
            "`include_max == true`, the resulting vector contains entries with labels `1, 10, 100, "
            "350`.\n\n Remark: There is no logarithmic equivalent for linear_ticks(), as those "
            "would simply be\n powers of  up to `max`.\n\nC++: "
            "genesis::util::Tickmarks::logarithmic_labels(double, double, double) --> class "
            "std::vector<struct genesis::util::Tickmarks::LabeledTick>",
            pybind11::arg( "min" ),
            pybind11::arg( "max" ),
            pybind11::arg( "base" )
        );

        { // genesis::util::Tickmarks::LabeledTick file:genesis/util/tool/tickmarks.hpp line:56
            auto& enclosing_class = cl;
            pybind11::class_<
                genesis::util::Tickmarks::LabeledTick,
                std::shared_ptr<genesis::util::Tickmarks::LabeledTick>>
                cl( enclosing_class, "LabeledTick", "" );
            // function-signature: genesis::util::Tickmarks::LabeledTick::LabeledTick(double,
            // double)(double, double) file:genesis/util/tool/tickmarks.hpp line:58
            cl.def(
                pybind11::init<double, double>(), pybind11::arg( "rel_pos" ), pybind11::arg( "lab" )
            );

            cl.def_readwrite(
                "relative_position", &genesis::util::Tickmarks::LabeledTick::relative_position
            );
            cl.def_readwrite( "label", &genesis::util::Tickmarks::LabeledTick::label );
        }
    }
}
