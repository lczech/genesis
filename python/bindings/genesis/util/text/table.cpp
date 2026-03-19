#include <genesis/util/text/style.hpp>
#include <genesis/util/text/table.hpp>
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

void bind_genesis_util_text_table(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::text::TableLayout file:genesis/util/text/table.hpp line:261
        pybind11::class_<
            genesis::util::text::TableLayout,
            std::shared_ptr<genesis::util::text::TableLayout>>
            cl( M( "genesis::util::text" ), "TableLayout", "" );
        // function-signature: genesis::util::text::TableLayout()
        cl.def( pybind11::init( []() { return new genesis::util::text::TableLayout(); } ) );
        cl.def_readwrite( "top", &genesis::util::text::TableLayout::top );
        cl.def_readwrite( "header", &genesis::util::text::TableLayout::header );
        cl.def_readwrite( "separator", &genesis::util::text::TableLayout::separator );
        cl.def_readwrite( "row", &genesis::util::text::TableLayout::row );
        cl.def_readwrite( "bottom", &genesis::util::text::TableLayout::bottom );
        // function-signature: struct genesis::util::text::TableLayout::Binder
        // genesis::util::text::TableLayout::operator()(const class genesis::util::text::Table
        // &)(const class genesis::util::text::Table &) file:genesis/util/text/table.hpp line:315
        cl.def(
            "__call__",
            ( struct genesis::util::text::TableLayout::
                  Binder( genesis::util::text::TableLayout::* )( const class genesis::util::text::
                                                                     Table& ) ) &
                genesis::util::text::TableLayout::operator(),
            "C++: genesis::util::text::TableLayout::operator()(const class "
            "genesis::util::text::Table &) --> struct genesis::util::text::TableLayout::Binder",
            pybind11::arg( "table" )
        );

        { // genesis::util::text::TableLayout::Line file:genesis/util/text/table.hpp line:281
            auto& enclosing_class = cl;
            pybind11::class_<
                genesis::util::text::TableLayout::Line,
                std::shared_ptr<genesis::util::text::TableLayout::Line>>
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
            // function-signature: genesis::util::text::TableLayout::Line()
            cl.def( pybind11::init( []() { return new genesis::util::text::TableLayout::Line(); } )
            );
            // function-signature: genesis::util::text::TableLayout::Line::Line(const struct
            // genesis::util::text::TableLayout::Line &)(const struct
            // genesis::util::text::TableLayout::Line &) file:genesis/util/text/table.hpp line:281
            cl.def( pybind11::init( []( genesis::util::text::TableLayout::Line const& o ) {
                return new genesis::util::text::TableLayout::Line( o );
            } ) );
            cl.def_readwrite( "enabled", &genesis::util::text::TableLayout::Line::enabled );
            cl.def_readwrite( "left_border", &genesis::util::text::TableLayout::Line::left_border );
            cl.def_readwrite( "filler", &genesis::util::text::TableLayout::Line::filler );
            cl.def_readwrite( "separator", &genesis::util::text::TableLayout::Line::separator );
            cl.def_readwrite(
                "right_border", &genesis::util::text::TableLayout::Line::right_border
            );
            // function-signature: struct genesis::util::text::TableLayout::Line &
            // genesis::util::text::TableLayout::Line::operator=(const struct
            // genesis::util::text::TableLayout::Line &)(const struct
            // genesis::util::text::TableLayout::Line &) file:genesis/util/text/table.hpp line:281
            cl.def(
                "assign",
                ( struct genesis::util::text::TableLayout::Line &
                  (genesis::util::text::TableLayout::Line::*)(const struct genesis::util::text::
                                                                  TableLayout::Line&)) &
                    genesis::util::text::TableLayout::Line::operator=,
                "C++: genesis::util::text::TableLayout::Line::operator=(const struct "
                "genesis::util::text::TableLayout::Line &) --> struct "
                "genesis::util::text::TableLayout::Line &",
                pybind11::return_value_policy::reference_internal,
                pybind11::arg( "" )
            );
        }

        { // genesis::util::text::TableLayout::Binder file:genesis/util/text/table.hpp line:298
            auto& enclosing_class = cl;
            pybind11::class_<
                genesis::util::text::TableLayout::Binder,
                std::shared_ptr<genesis::util::text::TableLayout::Binder>>
                cl( enclosing_class,
                    "Binder",
                    "Helper struct to bind a layout to a table.\n\n An std::stream can only take "
                    "one argument at a time. This Binder servers as a single\n argument that "
                    "internally provides both a layout and a table. The `operator<<` of Binder "
                    "is\n overloaded so that the table is outputted to the stream with the layout."
                );
            // function-signature: genesis::util::text::TableLayout::Binder::Binder(const struct
            // genesis::util::text::TableLayout &, const class genesis::util::text::Table &)(const
            // struct genesis::util::text::TableLayout &, const class genesis::util::text::Table &)
            // file:genesis/util/text/table.hpp line:300
            cl.def(
                pybind11::init<
                    const struct genesis::util::text::TableLayout&,
                    const class genesis::util::text::Table&>(),
                pybind11::arg( "l" ),
                pybind11::arg( "t" )
            );

            cl.def(
                "__str__",
                []( genesis::util::text::TableLayout::Binder const& o ) -> std::string {
                    std::ostringstream s;
                    using namespace genesis::util::text;
                    s << o;
                    return s.str();
                }
            );
        }
    }
    // genesis::util::text::minimal_layout() file:genesis/util/text/table.hpp line:333
    // function-signature: struct genesis::util::text::TableLayout
    // genesis::util::text::minimal_layout()() file:genesis/util/text/table.hpp line:333
    M( "genesis::util::text" )
        .def(
            "minimal_layout",
            ( struct genesis::util::text::TableLayout( * )() ) &
                genesis::util::text::minimal_layout,
            "C++: genesis::util::text::minimal_layout() --> struct genesis::util::text::TableLayout"
        );

    // genesis::util::text::simple_layout(bool) file:genesis/util/text/table.hpp line:335
    // function-signature: struct genesis::util::text::TableLayout
    // genesis::util::text::simple_layout(bool)(bool) file:genesis/util/text/table.hpp line:335
    M( "genesis::util::text" )
        .def(
            "simple_layout",
            []() -> genesis::util::text::TableLayout {
                return genesis::util::text::simple_layout();
            },
            ""
        );
    M( "genesis::util::text" )
        .def(
            "simple_layout",
            ( struct genesis::util::text::TableLayout( * )( bool ) ) &
                genesis::util::text::simple_layout,
            "C++: genesis::util::text::simple_layout(bool) --> struct "
            "genesis::util::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::util::text::simple_grid(bool) file:genesis/util/text/table.hpp line:336
    // function-signature: struct genesis::util::text::TableLayout
    // genesis::util::text::simple_grid(bool)(bool) file:genesis/util/text/table.hpp line:336
    M( "genesis::util::text" )
        .def(
            "simple_grid",
            []() -> genesis::util::text::TableLayout { return genesis::util::text::simple_grid(); },
            ""
        );
    M( "genesis::util::text" )
        .def(
            "simple_grid",
            ( struct genesis::util::text::TableLayout( * )( bool ) ) &
                genesis::util::text::simple_grid,
            "C++: genesis::util::text::simple_grid(bool) --> struct "
            "genesis::util::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::util::text::simple_frame(bool) file:genesis/util/text/table.hpp line:337
    // function-signature: struct genesis::util::text::TableLayout
    // genesis::util::text::simple_frame(bool)(bool) file:genesis/util/text/table.hpp line:337
    M( "genesis::util::text" )
        .def(
            "simple_frame",
            []() -> genesis::util::text::TableLayout {
                return genesis::util::text::simple_frame();
            },
            ""
        );
    M( "genesis::util::text" )
        .def(
            "simple_frame",
            ( struct genesis::util::text::TableLayout( * )( bool ) ) &
                genesis::util::text::simple_frame,
            "C++: genesis::util::text::simple_frame(bool) --> struct "
            "genesis::util::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::util::text::extended_grid(bool) file:genesis/util/text/table.hpp line:339
    // function-signature: struct genesis::util::text::TableLayout
    // genesis::util::text::extended_grid(bool)(bool) file:genesis/util/text/table.hpp line:339
    M( "genesis::util::text" )
        .def(
            "extended_grid",
            []() -> genesis::util::text::TableLayout {
                return genesis::util::text::extended_grid();
            },
            ""
        );
    M( "genesis::util::text" )
        .def(
            "extended_grid",
            ( struct genesis::util::text::TableLayout( * )( bool ) ) &
                genesis::util::text::extended_grid,
            "C++: genesis::util::text::extended_grid(bool) --> struct "
            "genesis::util::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::util::text::extended_frame(bool) file:genesis/util/text/table.hpp line:340
    // function-signature: struct genesis::util::text::TableLayout
    // genesis::util::text::extended_frame(bool)(bool) file:genesis/util/text/table.hpp line:340
    M( "genesis::util::text" )
        .def(
            "extended_frame",
            []() -> genesis::util::text::TableLayout {
                return genesis::util::text::extended_frame();
            },
            ""
        );
    M( "genesis::util::text" )
        .def(
            "extended_frame",
            ( struct genesis::util::text::TableLayout( * )( bool ) ) &
                genesis::util::text::extended_frame,
            "C++: genesis::util::text::extended_frame(bool) --> struct "
            "genesis::util::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::util::text::double_grid(bool) file:genesis/util/text/table.hpp line:342
    // function-signature: struct genesis::util::text::TableLayout
    // genesis::util::text::double_grid(bool)(bool) file:genesis/util/text/table.hpp line:342
    M( "genesis::util::text" )
        .def(
            "double_grid",
            []() -> genesis::util::text::TableLayout { return genesis::util::text::double_grid(); },
            ""
        );
    M( "genesis::util::text" )
        .def(
            "double_grid",
            ( struct genesis::util::text::TableLayout( * )( bool ) ) &
                genesis::util::text::double_grid,
            "C++: genesis::util::text::double_grid(bool) --> struct "
            "genesis::util::text::TableLayout",
            pybind11::arg( "condensed" )
        );

    // genesis::util::text::double_frame(bool) file:genesis/util/text/table.hpp line:343
    // function-signature: struct genesis::util::text::TableLayout
    // genesis::util::text::double_frame(bool)(bool) file:genesis/util/text/table.hpp line:343
    M( "genesis::util::text" )
        .def(
            "double_frame",
            []() -> genesis::util::text::TableLayout {
                return genesis::util::text::double_frame();
            },
            ""
        );
    M( "genesis::util::text" )
        .def(
            "double_frame",
            ( struct genesis::util::text::TableLayout( * )( bool ) ) &
                genesis::util::text::double_frame,
            "C++: genesis::util::text::double_frame(bool) --> struct "
            "genesis::util::text::TableLayout",
            pybind11::arg( "condensed" )
        );
}
