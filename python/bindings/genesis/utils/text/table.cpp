#include <genesis/utils/text/style.hpp>
#include <genesis/utils/text/table.hpp>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <string>

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

void bind_genesis_utils_text_table(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::utils::text::TableLayout file:genesis/utils/text/table.hpp line:261
        pybind11::class_<
            genesis::utils::text::TableLayout,
            std::shared_ptr<genesis::utils::text::TableLayout>>
            cl( M( "genesis::utils::text" ), "TableLayout", "" );
        // function-signature: genesis::utils::text::TableLayout()
        cl.def( pybind11::init( []() { return new genesis::utils::text::TableLayout(); } ) );
        cl.def_readwrite( "top", &genesis::utils::text::TableLayout::top );
        cl.def_readwrite( "header", &genesis::utils::text::TableLayout::header );
        cl.def_readwrite( "separator", &genesis::utils::text::TableLayout::separator );
        cl.def_readwrite( "row", &genesis::utils::text::TableLayout::row );
        cl.def_readwrite( "bottom", &genesis::utils::text::TableLayout::bottom );
        // function-signature: struct genesis::utils::text::TableLayout::Binder
        // genesis::utils::text::TableLayout::operator()(const class genesis::utils::text::Table
        // &)(const class genesis::utils::text::Table &) file:genesis/utils/text/table.hpp line:315
        cl.def(
            "__call__",
            ( struct genesis::utils::text::TableLayout::
                  Binder( genesis::utils::text::TableLayout::* )( const class genesis::utils::text::
                                                                      Table& ) ) &
                genesis::utils::text::TableLayout::operator(),
            "C++: genesis::utils::text::TableLayout::operator()(const class "
            "genesis::utils::text::Table &) --> struct genesis::utils::text::TableLayout::Binder",
            pybind11::arg( "table" )
        );

        { // genesis::utils::text::TableLayout::Line file:genesis/utils/text/table.hpp line:281
            auto& enclosing_class = cl;
            pybind11::class_<
                genesis::utils::text::TableLayout::Line,
                std::shared_ptr<genesis::utils::text::TableLayout::Line>>
                cl( enclosing_class,
                    "Line",
                    "One line of the TableLayout.\n\n This struct has different roles depending on "
                    "the kind of line:\n\n   * For lines separating parts of the Table (above the "
                    "header, between header and content,\n     and below the content), the "
                    "attribute `enabled` is used when writing to determine\n     whether to write "
                    "this line at all. Also, the `filler` is used to fill the width of the\n     "
                    "columns.\n   * For the header line and all content lines, the `enabled` "
                    "attribute is ignored (as those\n     lines always need to be written). The "
                    "`filler` is also ignored, because instead of it\n     the actual text is "
                    "written.\n %" );
            // function-signature: genesis::utils::text::TableLayout::Line()
            cl.def( pybind11::init( []() { return new genesis::utils::text::TableLayout::Line(); } )
            );
            // function-signature: genesis::utils::text::TableLayout::Line::Line(const struct
            // genesis::utils::text::TableLayout::Line &)(const struct
            // genesis::utils::text::TableLayout::Line &) file:genesis/utils/text/table.hpp line:281
            cl.def( pybind11::init( []( genesis::utils::text::TableLayout::Line const& o ) {
                return new genesis::utils::text::TableLayout::Line( o );
            } ) );
            cl.def_readwrite( "enabled", &genesis::utils::text::TableLayout::Line::enabled );
            cl.def_readwrite(
                "left_border", &genesis::utils::text::TableLayout::Line::left_border
            );
            cl.def_readwrite( "filler", &genesis::utils::text::TableLayout::Line::filler );
            cl.def_readwrite( "separator", &genesis::utils::text::TableLayout::Line::separator );
            cl.def_readwrite(
                "right_border", &genesis::utils::text::TableLayout::Line::right_border
            );
            // function-signature: struct genesis::utils::text::TableLayout::Line &
            // genesis::utils::text::TableLayout::Line::operator=(const struct
            // genesis::utils::text::TableLayout::Line &)(const struct
            // genesis::utils::text::TableLayout::Line &) file:genesis/utils/text/table.hpp line:281
            cl.def(
                "assign",
                ( struct genesis::utils::text::TableLayout::Line &
                  (genesis::utils::text::TableLayout::Line::*)(const struct genesis::utils::text::
                                                                   TableLayout::Line&)) &
                    genesis::utils::text::TableLayout::Line::operator=,
                "C++: genesis::utils::text::TableLayout::Line::operator=(const struct "
                "genesis::utils::text::TableLayout::Line &) --> struct "
                "genesis::utils::text::TableLayout::Line &",
                pybind11::return_value_policy::reference_internal,
                pybind11::arg( "" )
            );
        }

        { // genesis::utils::text::TableLayout::Binder file:genesis/utils/text/table.hpp line:298
            auto& enclosing_class = cl;
            pybind11::class_<
                genesis::utils::text::TableLayout::Binder,
                std::shared_ptr<genesis::utils::text::TableLayout::Binder>>
                cl( enclosing_class,
                    "Binder",
                    "Helper struct to bind a layout to a table.\n\n An std::stream can only take "
                    "one argument at a time. This Binder servers as a single\n argument that "
                    "internally provides both a layout and a table. The `operator<<` of Binder "
                    "is\n overloaded so that the table is outputted to the stream with the layout."
                );
            // function-signature: genesis::utils::text::TableLayout::Binder::Binder(const struct
            // genesis::utils::text::TableLayout &, const class genesis::utils::text::Table &)(const
            // struct genesis::utils::text::TableLayout &, const class genesis::utils::text::Table
            // &) file:genesis/utils/text/table.hpp line:300
            cl.def(
                pybind11::init<
                    const struct genesis::utils::text::TableLayout&,
                    const class genesis::utils::text::Table&>(),
                pybind11::arg( "l" ),
                pybind11::arg( "t" )
            );

            cl.def(
                "__str__",
                []( genesis::utils::text::TableLayout::Binder const& o ) -> std::string {
                    std::ostringstream s;
                    using namespace genesis::utils::text;
                    s << o;
                    return s.str();
                }
            );
        }
    }
    // genesis::utils::text::minimal_layout() file:genesis/utils/text/table.hpp line:333
    // function-signature: struct genesis::utils::text::TableLayout
    // genesis::utils::text::minimal_layout()() file:genesis/utils/text/table.hpp line:333
    M( "genesis::utils::text" )
        .def(
            "minimal_layout",
            ( struct genesis::utils::text::TableLayout( * )() ) &
                genesis::utils::text::minimal_layout,
            "C++: genesis::utils::text::minimal_layout() --> struct "
            "genesis::utils::text::TableLayout"
        );

    // genesis::utils::text::simple_layout(bool) file:genesis/utils/text/table.hpp line:335
    // function-signature: struct genesis::utils::text::TableLayout
    // genesis::utils::text::simple_layout(bool)(bool) file:genesis/utils/text/table.hpp line:335
    M( "genesis::utils::text" )
        .def(
            "simple_layout",
            []() -> genesis::utils::text::TableLayout {
                return genesis::utils::text::simple_layout();
            },
            ""
        );
    M( "genesis::utils::text" )
        .def(
            "simple_layout",
            ( struct genesis::utils::text::TableLayout( * )( bool ) ) &
                genesis::utils::text::simple_layout,
            "C++: genesis::utils::text::simple_layout(bool) --> struct "
            "genesis::utils::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::utils::text::simple_grid(bool) file:genesis/utils/text/table.hpp line:336
    // function-signature: struct genesis::utils::text::TableLayout
    // genesis::utils::text::simple_grid(bool)(bool) file:genesis/utils/text/table.hpp line:336
    M( "genesis::utils::text" )
        .def(
            "simple_grid",
            []() -> genesis::utils::text::TableLayout {
                return genesis::utils::text::simple_grid();
            },
            ""
        );
    M( "genesis::utils::text" )
        .def(
            "simple_grid",
            ( struct genesis::utils::text::TableLayout( * )( bool ) ) &
                genesis::utils::text::simple_grid,
            "C++: genesis::utils::text::simple_grid(bool) --> struct "
            "genesis::utils::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::utils::text::simple_frame(bool) file:genesis/utils/text/table.hpp line:337
    // function-signature: struct genesis::utils::text::TableLayout
    // genesis::utils::text::simple_frame(bool)(bool) file:genesis/utils/text/table.hpp line:337
    M( "genesis::utils::text" )
        .def(
            "simple_frame",
            []() -> genesis::utils::text::TableLayout {
                return genesis::utils::text::simple_frame();
            },
            ""
        );
    M( "genesis::utils::text" )
        .def(
            "simple_frame",
            ( struct genesis::utils::text::TableLayout( * )( bool ) ) &
                genesis::utils::text::simple_frame,
            "C++: genesis::utils::text::simple_frame(bool) --> struct "
            "genesis::utils::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::utils::text::extended_grid(bool) file:genesis/utils/text/table.hpp line:339
    // function-signature: struct genesis::utils::text::TableLayout
    // genesis::utils::text::extended_grid(bool)(bool) file:genesis/utils/text/table.hpp line:339
    M( "genesis::utils::text" )
        .def(
            "extended_grid",
            []() -> genesis::utils::text::TableLayout {
                return genesis::utils::text::extended_grid();
            },
            ""
        );
    M( "genesis::utils::text" )
        .def(
            "extended_grid",
            ( struct genesis::utils::text::TableLayout( * )( bool ) ) &
                genesis::utils::text::extended_grid,
            "C++: genesis::utils::text::extended_grid(bool) --> struct "
            "genesis::utils::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::utils::text::extended_frame(bool) file:genesis/utils/text/table.hpp line:340
    // function-signature: struct genesis::utils::text::TableLayout
    // genesis::utils::text::extended_frame(bool)(bool) file:genesis/utils/text/table.hpp line:340
    M( "genesis::utils::text" )
        .def(
            "extended_frame",
            []() -> genesis::utils::text::TableLayout {
                return genesis::utils::text::extended_frame();
            },
            ""
        );
    M( "genesis::utils::text" )
        .def(
            "extended_frame",
            ( struct genesis::utils::text::TableLayout( * )( bool ) ) &
                genesis::utils::text::extended_frame,
            "C++: genesis::utils::text::extended_frame(bool) --> struct "
            "genesis::utils::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::utils::text::double_grid(bool) file:genesis/utils/text/table.hpp line:342
    // function-signature: struct genesis::utils::text::TableLayout
    // genesis::utils::text::double_grid(bool)(bool) file:genesis/utils/text/table.hpp line:342
    M( "genesis::utils::text" )
        .def(
            "double_grid",
            []() -> genesis::utils::text::TableLayout {
                return genesis::utils::text::double_grid();
            },
            ""
        );
    M( "genesis::utils::text" )
        .def(
            "double_grid",
            ( struct genesis::utils::text::TableLayout( * )( bool ) ) &
                genesis::utils::text::double_grid,
            "C++: genesis::utils::text::double_grid(bool) --> struct "
            "genesis::utils::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::utils::text::double_frame(bool) file:genesis/utils/text/table.hpp line:343
    // function-signature: struct genesis::utils::text::TableLayout
    // genesis::utils::text::double_frame(bool)(bool) file:genesis/utils/text/table.hpp line:343
    M( "genesis::utils::text" )
        .def(
            "double_frame",
            []() -> genesis::utils::text::TableLayout {
                return genesis::utils::text::double_frame();
            },
            ""
        );
    M( "genesis::utils::text" )
        .def(
            "double_frame",
            ( struct genesis::utils::text::TableLayout( * )( bool ) ) &
                genesis::utils::text::double_frame,
            "C++: genesis::utils::text::double_frame(bool) --> struct "
            "genesis::utils::text::TableLayout",
            pybind11::arg( "condensed" )
        );
}
