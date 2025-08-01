#include <genesis/utils/tools/tickmarks.hpp>
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

void bind_genesis_utils_tools_tickmarks(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::utils::Tickmarks file:genesis/utils/tools/tickmarks.hpp line:48
        pybind11::class_<genesis::utils::Tickmarks, std::shared_ptr<genesis::utils::Tickmarks>> cl(
            M( "genesis::utils" ),
            "Tickmarks",
            "Helper class to find \"nice\" tickmark intervals for creating scales and axes."
        );
        // function-signature: genesis::utils::Tickmarks::Tickmarks()()
        // file:genesis/utils/tools/tickmarks.hpp line:71
        cl.def( pybind11::init( []() { return new genesis::utils::Tickmarks(); } ) );
        // function-signature: genesis::utils::Tickmarks::Tickmarks(const struct
        // genesis::utils::Tickmarks &)(const struct genesis::utils::Tickmarks &)
        // file:genesis/utils/tools/tickmarks.hpp line:74
        cl.def( pybind11::init( []( genesis::utils::Tickmarks const& o ) {
            return new genesis::utils::Tickmarks( o );
        } ) );
        cl.def_readwrite( "include_min", &genesis::utils::Tickmarks::include_min );
        cl.def_readwrite( "include_max", &genesis::utils::Tickmarks::include_max );
        cl.def_readwrite( "undershoot_at_min", &genesis::utils::Tickmarks::undershoot_at_min );
        cl.def_readwrite( "overshoot_at_max", &genesis::utils::Tickmarks::overshoot_at_max );
        cl.def_readwrite( "relative_epsilon", &genesis::utils::Tickmarks::relative_epsilon );
        // function-signature: struct genesis::utils::Tickmarks &
        // genesis::utils::Tickmarks::operator=(const struct genesis::utils::Tickmarks &)(const
        // struct genesis::utils::Tickmarks &) file:genesis/utils/tools/tickmarks.hpp line:77
        cl.def(
            "assign",
            ( struct genesis::utils::Tickmarks &
              (genesis::utils::Tickmarks::*)(const struct genesis::utils::Tickmarks&)) &
                genesis::utils::Tickmarks::operator=,
            "C++: genesis::utils::Tickmarks::operator=(const struct genesis::utils::Tickmarks &) "
            "--> struct genesis::utils::Tickmarks &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: double genesis::utils::Tickmarks::step_size(double, unsigned
        // long)(double, unsigned long) file:genesis/utils/tools/tickmarks.hpp line:91
        cl.def_static(
            "step_size",
            ( double ( * )( double, unsigned long ) ) & genesis::utils::Tickmarks::step_size,
            "Calculate a step size that fills the `interval_size` in approximately "
            "`target_steps`\n many steps.\n\n The resulting step size is calculated to be a "
            "\"nice\" size, i.e. fits `[ 1 | 2 | 5 ] * 10^n`.\n Thus, exemplary nice step sizes "
            "are 0.01, 20 and 500.\n\nC++: genesis::utils::Tickmarks::step_size(double, unsigned "
            "long) --> double",
            pybind11::arg( "interval_size" ),
            pybind11::arg( "target_steps" )
        );
        // function-signature: class std::vector<double>
        // genesis::utils::Tickmarks::linear_ticks(double, double, unsigned long)(double, double,
        // unsigned long) file:genesis/utils/tools/tickmarks.hpp line:99
        cl.def(
            "linear_ticks",
            ( class std::vector<double>( genesis::utils::Tickmarks::* )(
                double, double, unsigned long
            ) ) &
                genesis::utils::Tickmarks::linear_ticks,
            "Calculate a set of ticks that linearily span from `min` to `max` in approximately\n "
            "`target_steps` many steps, where each tick sits on a \"nice\" number.\n\n See "
            "step_size() for what is meant by nice numbers.\n\nC++: "
            "genesis::utils::Tickmarks::linear_ticks(double, double, unsigned long) --> class "
            "std::vector<double>",
            pybind11::arg( "min" ),
            pybind11::arg( "max" ),
            pybind11::arg( "target_steps" )
        );
        // function-signature: class std::vector<struct genesis::utils::Tickmarks::LabeledTick>
        // genesis::utils::Tickmarks::linear_labels(double, double, unsigned long)(double, double,
        // unsigned long) file:genesis/utils/tools/tickmarks.hpp line:109
        cl.def(
            "linear_labels",
            ( class std::vector<
                struct genesis::utils::Tickmarks::LabeledTick>( genesis::utils::Tickmarks::* )(
                double, double, unsigned long
            ) ) &
                genesis::utils::Tickmarks::linear_labels,
            "Return a set of labels with relative positions between `min` and `max`, where the\n "
            "labels correspond to the linear_ticks() numbers.\n\n This function is a convenience "
            "wrapper for linear_ticks(), that turns the tick positions\n into relative values, "
            "while keeping the actual tick value as a label. Relative position 0.0\n corresponds "
            "to `min`, relative position 1.0 to `max`.\n\nC++: "
            "genesis::utils::Tickmarks::linear_labels(double, double, unsigned long) --> class "
            "std::vector<struct genesis::utils::Tickmarks::LabeledTick>",
            pybind11::arg( "min" ),
            pybind11::arg( "max" ),
            pybind11::arg( "target_steps" )
        );
        // function-signature: class std::vector<struct genesis::utils::Tickmarks::LabeledTick>
        // genesis::utils::Tickmarks::logarithmic_labels(double, double, double)(double, double,
        // double) file:genesis/utils/tools/tickmarks.hpp line:125
        cl.def(
            "logarithmic_labels",
            []( genesis::utils::Tickmarks & o, double const& a0, double const& a1
            ) -> std::vector<struct genesis::utils::Tickmarks::LabeledTick> {
                return o.logarithmic_labels( a0, a1 );
            },
            "",
            pybind11::arg( "min" ),
            pybind11::arg( "max" )
        );
        cl.def(
            "logarithmic_labels",
            ( class std::vector<
                struct genesis::utils::Tickmarks::LabeledTick>( genesis::utils::Tickmarks::* )(
                double, double, double
            ) ) &
                genesis::utils::Tickmarks::logarithmic_labels,
            "Return a set of labels with relative positions between `min` and `max`, where the\n "
            "labels correspond to logarithmic ticks.\n\n This function is the equivalent of "
            "linear_labels(), but instead uses logarithmic scaling.\n Also, it inserts as many "
            "ticks as are needed to go up to  in powers of \n and thus has no target steps.\n\n "
            "Example: Given `min == 1.0`, `max == 350`, with `overshoot_at_max == false` and\n "
            "`include_max == true`, the resulting vector contains entries with labels `1, 10, 100, "
            "350`.\n\n Remark: There is no logarithmic equivalent for linear_ticks(), as those "
            "would simply be\n powers of  up to `max`.\n\nC++: "
            "genesis::utils::Tickmarks::logarithmic_labels(double, double, double) --> class "
            "std::vector<struct genesis::utils::Tickmarks::LabeledTick>",
            pybind11::arg( "min" ),
            pybind11::arg( "max" ),
            pybind11::arg( "base" )
        );

        { // genesis::utils::Tickmarks::LabeledTick file:genesis/utils/tools/tickmarks.hpp line:56
            auto& enclosing_class = cl;
            pybind11::class_<
                genesis::utils::Tickmarks::LabeledTick,
                std::shared_ptr<genesis::utils::Tickmarks::LabeledTick>>
                cl( enclosing_class, "LabeledTick", "" );
            // function-signature: genesis::utils::Tickmarks::LabeledTick::LabeledTick(double,
            // double)(double, double) file:genesis/utils/tools/tickmarks.hpp line:58
            cl.def(
                pybind11::init<double, double>(), pybind11::arg( "rel_pos" ), pybind11::arg( "lab" )
            );

            cl.def_readwrite(
                "relative_position", &genesis::utils::Tickmarks::LabeledTick::relative_position
            );
            cl.def_readwrite( "label", &genesis::utils::Tickmarks::LabeledTick::label );
        }
    }
}
