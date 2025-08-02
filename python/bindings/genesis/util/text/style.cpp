#include <genesis/util/text/style.hpp>
#include <genesis/util/text/table.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
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

void bind_genesis_util_text_style(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::text::Style file:genesis/util/text/style.hpp line:82
        pybind11::class_<genesis::util::text::Style, std::shared_ptr<genesis::util::text::Style>>
            cl( M( "genesis::util::text" ),
                "Style",
                "Simple text style class for colorized and bold output to a terminal.\n\n This "
                "class bundles the following text style properties for output in a terminal:\n\n   "
                "* Foreground Color. Set using foreground_color( std::string const& color ).\n     "
                "See Style::foreground_colors for the valid color names.\n   * Background Color. "
                "Set using background_color( std::string const& color ).\n     See "
                "Style::background_colors for the valid color names.\n   * Bold. Set using bold( "
                "bool value ).\n\n Those properties can be set using either the respective "
                "constructor of this class or using the\n setter functions. Per default, all of "
                "them are empty, meaning that no style manupulation is\n done.\n\n In order to "
                "generate textual output with those styles, operator()() is used.\n Example:\n\n   "
                "  Style blue( \"blue\" );\n     blue.bold( true );\n     std::cout << blue( "
                "\"some text\" );\n\n Furthermore, the stream operator can be used to get a "
                "summary of the properties of a Style\n object:\n\n     LOG_DBG << blue;\n\n will "
                "output\n\n     Foreground Color: Blue\n     Background Color:\n     Bold:         "
                "    true\n\n to the LOG stream." );
        // function-signature: genesis::util::text::Style::Style()()
        // file:genesis/util/text/style.hpp line:90
        cl.def( pybind11::init( []() { return new genesis::util::text::Style(); } ) );
        // function-signature: genesis::util::text::Style::Style(const std::string &)(const
        // std::string &) file:genesis/util/text/style.hpp line:92
        cl.def( pybind11::init<const std::string&>(), pybind11::arg( "foreground_color" ) );

        // function-signature: genesis::util::text::Style::Style(const char *)(const char *)
        // file:genesis/util/text/style.hpp line:96
        cl.def( pybind11::init<const char*>(), pybind11::arg( "foreground_color" ) );

        // function-signature: genesis::util::text::Style::Style(bool)(bool)
        // file:genesis/util/text/style.hpp line:100
        cl.def( pybind11::init<bool>(), pybind11::arg( "bold" ) );

        // function-signature: genesis::util::text::Style::Style(const std::string &, bool)(const
        // std::string &, bool) file:genesis/util/text/style.hpp line:104
        cl.def(
            pybind11::init<const std::string&, bool>(),
            pybind11::arg( "foreground_color" ),
            pybind11::arg( "bold" )
        );

        // function-signature: genesis::util::text::Style::Style(const std::string &, const
        // std::string &)(const std::string &, const std::string &) file:genesis/util/text/style.hpp
        // line:109
        cl.def(
            pybind11::init<const std::string&, const std::string&>(),
            pybind11::arg( "foreground_color" ),
            pybind11::arg( "background_color" )
        );

        // function-signature: genesis::util::text::Style::Style(const std::string &, const char
        // *)(const std::string &, const char *) file:genesis/util/text/style.hpp line:114
        cl.def(
            pybind11::init<const std::string&, const char*>(),
            pybind11::arg( "foreground_color" ),
            pybind11::arg( "background_color" )
        );

        // function-signature: genesis::util::text::Style::Style(const std::string &, const
        // std::string &, bool)(const std::string &, const std::string &, bool)
        // file:genesis/util/text/style.hpp line:119
        cl.def(
            pybind11::init<const std::string&, const std::string&, bool>(),
            pybind11::arg( "foreground_color" ),
            pybind11::arg( "background_color" ),
            pybind11::arg( "bold" )
        );

        // function-signature: genesis::util::text::Style::Style(const class
        // genesis::util::text::Style &)(const class genesis::util::text::Style &)
        // file:genesis/util/text/style.hpp line:127
        cl.def( pybind11::init( []( genesis::util::text::Style const& o ) {
            return new genesis::util::text::Style( o );
        } ) );
        // function-signature: class genesis::util::text::Style &
        // genesis::util::text::Style::operator=(const class genesis::util::text::Style &)(const
        // class genesis::util::text::Style &) file:genesis/util/text/style.hpp line:130
        cl.def(
            "assign",
            ( class genesis::util::text::Style &
              (genesis::util::text::Style::*)(const class genesis::util::text::Style&)) &
                genesis::util::text::Style::operator=,
            "C++: genesis::util::text::Style::operator=(const class genesis::util::text::Style &) "
            "--> class genesis::util::text::Style &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: void genesis::util::text::Style::swap(class
        // genesis::util::text::Style &)(class genesis::util::text::Style &)
        // file:genesis/util/text/style.hpp line:133
        cl.def(
            "swap",
            ( void( genesis::util::text::Style::* )( class genesis::util::text::Style& ) ) &
                genesis::util::text::Style::swap,
            "C++: genesis::util::text::Style::swap(class genesis::util::text::Style &) --> void",
            pybind11::arg( "other" )
        );
        // function-signature: class genesis::util::text::Style &
        // genesis::util::text::Style::reset()() file:genesis/util/text/style.hpp line:146
        cl.def(
            "reset",
            ( class genesis::util::text::Style & (genesis::util::text::Style::*)() ) &
                genesis::util::text::Style::reset,
            "C++: genesis::util::text::Style::reset() --> class genesis::util::text::Style &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: bool genesis::util::text::Style::enabled() const()
        // file:genesis/util/text/style.hpp line:148
        cl.def(
            "enabled",
            ( bool( genesis::util::text::Style::* )() const ) & genesis::util::text::Style::enabled,
            "C++: genesis::util::text::Style::enabled() const --> bool"
        );
        // function-signature: class genesis::util::text::Style &
        // genesis::util::text::Style::enabled(bool)(bool) file:genesis/util/text/style.hpp line:149
        cl.def(
            "enabled",
            ( class genesis::util::text::Style & (genesis::util::text::Style::*)(bool)) &
                genesis::util::text::Style::enabled,
            "C++: genesis::util::text::Style::enabled(bool) --> class genesis::util::text::Style &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: bool genesis::util::text::Style::bold() const()
        // file:genesis/util/text/style.hpp line:151
        cl.def(
            "bold",
            ( bool( genesis::util::text::Style::* )() const ) & genesis::util::text::Style::bold,
            "C++: genesis::util::text::Style::bold() const --> bool"
        );
        // function-signature: class genesis::util::text::Style &
        // genesis::util::text::Style::bold(bool)(bool) file:genesis/util/text/style.hpp line:152
        cl.def(
            "bold",
            ( class genesis::util::text::Style & (genesis::util::text::Style::*)(bool)) &
                genesis::util::text::Style::bold,
            "C++: genesis::util::text::Style::bold(bool) --> class genesis::util::text::Style &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: std::string genesis::util::text::Style::foreground_color() const()
        // file:genesis/util/text/style.hpp line:154
        cl.def(
            "foreground_color",
            ( std::string( genesis::util::text::Style::* )() const ) &
                genesis::util::text::Style::foreground_color,
            "C++: genesis::util::text::Style::foreground_color() const --> std::string"
        );
        // function-signature: class genesis::util::text::Style &
        // genesis::util::text::Style::foreground_color(const std::string &)(const std::string &)
        // file:genesis/util/text/style.hpp line:155
        cl.def(
            "foreground_color",
            ( class genesis::util::text::Style &
              (genesis::util::text::Style::*)(const std::string&)) &
                genesis::util::text::Style::foreground_color,
            "C++: genesis::util::text::Style::foreground_color(const std::string &) --> class "
            "genesis::util::text::Style &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "color" )
        );
        // function-signature: std::string genesis::util::text::Style::background_color() const()
        // file:genesis/util/text/style.hpp line:157
        cl.def(
            "background_color",
            ( std::string( genesis::util::text::Style::* )() const ) &
                genesis::util::text::Style::background_color,
            "C++: genesis::util::text::Style::background_color() const --> std::string"
        );
        // function-signature: class genesis::util::text::Style &
        // genesis::util::text::Style::background_color(const std::string &)(const std::string &)
        // file:genesis/util/text/style.hpp line:158
        cl.def(
            "background_color",
            ( class genesis::util::text::Style &
              (genesis::util::text::Style::*)(const std::string&)) &
                genesis::util::text::Style::background_color,
            "C++: genesis::util::text::Style::background_color(const std::string &) --> class "
            "genesis::util::text::Style &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "color" )
        );
        // function-signature: std::string genesis::util::text::Style::operator()(const std::string
        // &) const(const std::string &) file:genesis/util/text/style.hpp line:164
        cl.def(
            "__call__",
            ( std::string( genesis::util::text::Style::* )( const std::string& ) const ) &
                genesis::util::text::Style::operator(),
            "C++: genesis::util::text::Style::operator()(const std::string &) const --> "
            "std::string",
            pybind11::arg( "text" )
        );
        // function-signature: std::string genesis::util::text::Style::to_bash_string(const
        // std::string &) const(const std::string &) file:genesis/util/text/style.hpp line:166
        cl.def(
            "to_bash_string",
            ( std::string( genesis::util::text::Style::* )( const std::string& ) const ) &
                genesis::util::text::Style::to_bash_string,
            "C++: genesis::util::text::Style::to_bash_string(const std::string &) const --> "
            "std::string",
            pybind11::arg( "text" )
        );
        // function-signature: std::string genesis::util::text::Style::to_python_string(const
        // std::string &) const(const std::string &) file:genesis/util/text/style.hpp line:167
        cl.def(
            "to_python_string",
            ( std::string( genesis::util::text::Style::* )( const std::string& ) const ) &
                genesis::util::text::Style::to_python_string,
            "C++: genesis::util::text::Style::to_python_string(const std::string &) const --> "
            "std::string",
            pybind11::arg( "text" )
        );
        // function-signature: bool
        // genesis::util::text::Style::is_foreground_color(std::string)(std::string)
        // file:genesis/util/text/style.hpp line:173
        cl.def_static(
            "is_foreground_color",
            ( bool ( * )( std::string ) ) & genesis::util::text::Style::is_foreground_color,
            "C++: genesis::util::text::Style::is_foreground_color(std::string) --> bool",
            pybind11::arg( "name" )
        );
        // function-signature: bool
        // genesis::util::text::Style::is_background_color(std::string)(std::string)
        // file:genesis/util/text/style.hpp line:174
        cl.def_static(
            "is_background_color",
            ( bool ( * )( std::string ) ) & genesis::util::text::Style::is_background_color,
            "C++: genesis::util::text::Style::is_background_color(std::string) --> bool",
            pybind11::arg( "name" )
        );
        // function-signature: std::string
        // genesis::util::text::Style::get_foreground_color_value(std::string)(std::string)
        // file:genesis/util/text/style.hpp line:176
        cl.def_static(
            "get_foreground_color_value",
            ( std::string( * )( std::string ) ) &
                genesis::util::text::Style::get_foreground_color_value,
            "C++: genesis::util::text::Style::get_foreground_color_value(std::string) --> "
            "std::string",
            pybind11::arg( "name" )
        );
        // function-signature: std::string
        // genesis::util::text::Style::get_background_color_value(std::string)(std::string)
        // file:genesis/util/text/style.hpp line:177
        cl.def_static(
            "get_background_color_value",
            ( std::string( * )( std::string ) ) &
                genesis::util::text::Style::get_background_color_value,
            "C++: genesis::util::text::Style::get_background_color_value(std::string) --> "
            "std::string",
            pybind11::arg( "name" )
        );

        cl.def( "__str__", []( genesis::util::text::Style const& o ) -> std::string {
            std::ostringstream s;
            using namespace genesis::util::text;
            s << o;
            return s.str();
        } );
    }
    // genesis::util::text::swap(class genesis::util::text::Style &, class
    // genesis::util::text::Style &) file:genesis/util/text/style.hpp line:199 function-signature:
    // void genesis::util::text::swap(class genesis::util::text::Style &, class
    // genesis::util::text::Style &)(class genesis::util::text::Style &, class
    // genesis::util::text::Style &) file:genesis/util/text/style.hpp line:199
    M( "genesis::util::text" )
        .def(
            "swap",
            ( void ( * )( class genesis::util::text::Style&, class genesis::util::text::Style& ) ) &
                genesis::util::text::swap,
            "C++: genesis::util::text::swap(class genesis::util::text::Style &, class "
            "genesis::util::text::Style &) --> void",
            pybind11::arg( "lhs" ),
            pybind11::arg( "rhs" )
        );

    { // genesis::util::text::Table file:genesis/util/text/table.hpp line:62
        pybind11::class_<genesis::util::text::Table, std::shared_ptr<genesis::util::text::Table>>
            cl( M( "genesis::util::text" ), "Table", "" );
        // function-signature: genesis::util::text::Table::Table()()
        // file:genesis/util/text/table.hpp line:185
        cl.def( pybind11::init( []() { return new genesis::util::text::Table(); } ) );
        // function-signature: genesis::util::text::Table::Table(const class
        // genesis::util::text::Table &)(const class genesis::util::text::Table &)
        // file:genesis/util/text/table.hpp line:188
        cl.def( pybind11::init( []( genesis::util::text::Table const& o ) {
            return new genesis::util::text::Table( o );
        } ) );
        // function-signature: class genesis::util::text::Table &
        // genesis::util::text::Table::operator=(const class genesis::util::text::Table &)(const
        // class genesis::util::text::Table &) file:genesis/util/text/table.hpp line:191
        cl.def(
            "assign",
            ( class genesis::util::text::Table &
              (genesis::util::text::Table::*)(const class genesis::util::text::Table&)) &
                genesis::util::text::Table::operator=,
            "C++: genesis::util::text::Table::operator=(const class genesis::util::text::Table &) "
            "--> class genesis::util::text::Table &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: unsigned long genesis::util::text::Table::length() const()
        // file:genesis/util/text/table.hpp line:203
        cl.def(
            "length",
            ( unsigned long ( genesis::util::text::Table::* )() const ) &
                genesis::util::text::Table::length,
            "C++: genesis::util::text::Table::length() const --> unsigned long"
        );
        // function-signature: void genesis::util::text::Table::clear()()
        // file:genesis/util/text/table.hpp line:209
        cl.def(
            "clear",
            ( void( genesis::util::text::Table::* )() ) & genesis::util::text::Table::clear,
            "C++: genesis::util::text::Table::clear() --> void"
        );
        // function-signature: void genesis::util::text::Table::clear_content()()
        // file:genesis/util/text/table.hpp line:210
        cl.def(
            "clear_content",
            ( void( genesis::util::text::Table::* )() ) & genesis::util::text::Table::clear_content,
            "C++: genesis::util::text::Table::clear_content() --> void"
        );
        // function-signature: class genesis::util::text::Table::Column &
        // genesis::util::text::Table::add_column(std::string, enum
        // genesis::util::text::Table::Column::Justification)(std::string, enum
        // genesis::util::text::Table::Column::Justification) file:genesis/util/text/table.hpp
        // line:212
        cl.def(
            "add_column",
            []( genesis::util::text::Table& o ) -> genesis::util::text::Table::Column& {
                return o.add_column();
            },
            "",
            pybind11::return_value_policy::reference_internal
        );
        cl.def(
            "add_column",
            []( genesis::util::text::Table& o, std::string const& a0
            ) -> genesis::util::text::Table::Column& { return o.add_column( a0 ); },
            "",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "label" )
        );
        cl.def(
            "add_column",
            ( class genesis::util::text::Table::Column &
              (genesis::util::text::
                   Table::*)( std::string, enum genesis::util::text::Table::Column::Justification )
            ) & genesis::util::text::Table::add_column,
            "C++: genesis::util::text::Table::add_column(std::string, enum "
            "genesis::util::text::Table::Column::Justification) --> class "
            "genesis::util::text::Table::Column &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "label" ),
            pybind11::arg( "justify" )
        );
        // function-signature: class genesis::util::text::Table &
        // genesis::util::text::Table::operator<<(std::string)(std::string)
        // file:genesis/util/text/table.hpp line:217
        cl.def(
            "__lshift__",
            ( class genesis::util::text::Table & (genesis::util::text::Table::*)( std::string ) ) &
                genesis::util::text::Table::operator<<,
            "C++: genesis::util::text::Table::operator<<(std::string) --> class "
            "genesis::util::text::Table &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::text::Table &
        // genesis::util::text::Table::append(std::string)(std::string)
        // file:genesis/util/text/table.hpp line:229
        cl.def(
            "append",
            ( class genesis::util::text::Table & (genesis::util::text::Table::*)( std::string ) ) &
                genesis::util::text::Table::append,
            "C++: genesis::util::text::Table::append(std::string) --> class "
            "genesis::util::text::Table &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::text::Table &
        // genesis::util::text::Table::append(const class genesis::util::text::Style &,
        // std::string)(const class genesis::util::text::Style &, std::string)
        // file:genesis/util/text/table.hpp line:230
        cl.def(
            "append",
            ( class genesis::util::text::Table &
              (genesis::util::text::Table::*)( const class genesis::util::text::Style&,
                                               std::string ) ) &
                genesis::util::text::Table::append,
            "C++: genesis::util::text::Table::append(const class genesis::util::text::Style &, "
            "std::string) --> class genesis::util::text::Table &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "style" ),
            pybind11::arg( "value" )
        );
        // function-signature: class genesis::util::text::Table &
        // genesis::util::text::Table::line_break()() file:genesis/util/text/table.hpp line:232
        cl.def(
            "line_break",
            ( class genesis::util::text::Table & (genesis::util::text::Table::*)() ) &
                genesis::util::text::Table::line_break,
            "C++: genesis::util::text::Table::line_break() --> class genesis::util::text::Table &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: std::string genesis::util::text::Table::to_string() const()
        // file:genesis/util/text/table.hpp line:241
        cl.def(
            "to_string",
            ( std::string( genesis::util::text::Table::* )() const ) &
                genesis::util::text::Table::to_string,
            "C++: genesis::util::text::Table::to_string() const --> std::string"
        );
        // function-signature: std::string genesis::util::text::Table::to_string(const struct
        // genesis::util::text::TableLayout &) const(const struct genesis::util::text::TableLayout
        // &) file:genesis/util/text/table.hpp line:242
        cl.def(
            "to_string",
            ( std::string( genesis::util::text::Table::* )( const struct genesis::util::text::
                                                                TableLayout& ) const ) &
                genesis::util::text::Table::to_string,
            "C++: genesis::util::text::Table::to_string(const struct "
            "genesis::util::text::TableLayout &) const --> std::string",
            pybind11::arg( "layout" )
        );

        cl.def( "__str__", []( genesis::util::text::Table const& o ) -> std::string {
            std::ostringstream s;
            using namespace genesis::util::text;
            s << o;
            return s.str();
        } );

        { // genesis::util::text::Table::Column file:genesis/util/text/table.hpp line:74
            auto& enclosing_class = cl;
            pybind11::class_<
                genesis::util::text::Table::Column,
                std::shared_ptr<genesis::util::text::Table::Column>>
                cl( enclosing_class, "Column", "" );
            // function-signature: genesis::util::text::Table::Column::Column()()
            // file:genesis/util/text/table.hpp line:95
            cl.def( pybind11::init( []() { return new genesis::util::text::Table::Column(); } ) );
            // function-signature: genesis::util::text::Table::Column::Column(const std::string &,
            // enum genesis::util::text::Table::Column::Justification)(const std::string &, enum
            // genesis::util::text::Table::Column::Justification) file:genesis/util/text/table.hpp
            // line:97
            cl.def(
                pybind11::init( []( const std::string& a0 ) {
                    return new genesis::util::text::Table::Column( a0 );
                } ),
                "doc",
                pybind11::arg( "label" )
            );
            // function-signature: genesis::util::text::Table::Column::Column(const std::string &,
            // enum genesis::util::text::Table::Column::Justification)(const std::string &, enum
            // genesis::util::text::Table::Column::Justification) file:genesis/util/text/table.hpp
            // line:97
            cl.def(
                pybind11::init<
                    const std::string&,
                    enum genesis::util::text::Table::Column::Justification>(),
                pybind11::arg( "label" ),
                pybind11::arg( "justify" )
            );

            // function-signature: genesis::util::text::Table::Column::Column(const class
            // genesis::util::text::Table::Column &)(const class genesis::util::text::Table::Column
            // &) file:genesis/util/text/table.hpp line:105
            cl.def( pybind11::init( []( genesis::util::text::Table::Column const& o ) {
                return new genesis::util::text::Table::Column( o );
            } ) );

            pybind11::enum_<genesis::util::text::Table::Column::Justification>(
                cl, "Justification", ""
            )
                .value( "kLeft", genesis::util::text::Table::Column::Justification::kLeft )
                .value( "kRight", genesis::util::text::Table::Column::Justification::kRight )
                .value( "kCentered", genesis::util::text::Table::Column::Justification::kCentered );

            // function-signature: class genesis::util::text::Table::Column &
            // genesis::util::text::Table::Column::operator=(const class
            // genesis::util::text::Table::Column &)(const class genesis::util::text::Table::Column
            // &) file:genesis/util/text/table.hpp line:108
            cl.def(
                "assign",
                ( class genesis::util::text::Table::Column &
                  (genesis::util::text::Table::Column::*)(const class genesis::util::text::Table::
                                                              Column&)) &
                    genesis::util::text::Table::Column::operator=,
                "C++: genesis::util::text::Table::Column::operator=(const class "
                "genesis::util::text::Table::Column &) --> class "
                "genesis::util::text::Table::Column &",
                pybind11::return_value_policy::reference_internal,
                pybind11::arg( "" )
            );
            // function-signature: void
            // genesis::util::text::Table::Column::label(std::string)(std::string)
            // file:genesis/util/text/table.hpp line:120
            cl.def(
                "label",
                ( void( genesis::util::text::Table::Column::* )( std::string ) ) &
                    genesis::util::text::Table::Column::label,
                "C++: genesis::util::text::Table::Column::label(std::string) --> void",
                pybind11::arg( "value" )
            );
            // function-signature: std::string genesis::util::text::Table::Column::label() const()
            // file:genesis/util/text/table.hpp line:121
            cl.def(
                "label",
                ( std::string( genesis::util::text::Table::Column::* )() const ) &
                    genesis::util::text::Table::Column::label,
                "C++: genesis::util::text::Table::Column::label() const --> std::string"
            );
            // function-signature: void genesis::util::text::Table::Column::justify(enum
            // genesis::util::text::Table::Column::Justification)(enum
            // genesis::util::text::Table::Column::Justification) file:genesis/util/text/table.hpp
            // line:123
            cl.def(
                "justify",
                ( void( genesis::util::text::Table::Column::* )(
                    enum genesis::util::text::Table::Column::Justification
                ) ) &
                    genesis::util::text::Table::Column::justify,
                "C++: genesis::util::text::Table::Column::justify(enum "
                "genesis::util::text::Table::Column::Justification) --> void",
                pybind11::arg( "value" )
            );
            // function-signature: enum genesis::util::text::Table::Column::Justification
            // genesis::util::text::Table::Column::justify() const()
            // file:genesis/util/text/table.hpp line:124
            cl.def(
                "justify",
                ( enum genesis::util::text::Table::Column::
                      Justification( genesis::util::text::Table::Column::* )() const ) &
                    genesis::util::text::Table::Column::justify,
                "C++: genesis::util::text::Table::Column::justify() const --> enum "
                "genesis::util::text::Table::Column::Justification"
            );
            // function-signature: void genesis::util::text::Table::Column::width(unsigned
            // long)(unsigned long) file:genesis/util/text/table.hpp line:126
            cl.def(
                "width",
                ( void( genesis::util::text::Table::Column::* )( unsigned long ) ) &
                    genesis::util::text::Table::Column::width,
                "C++: genesis::util::text::Table::Column::width(unsigned long) --> void",
                pybind11::arg( "value" )
            );
            // function-signature: unsigned long genesis::util::text::Table::Column::width() const()
            // file:genesis/util/text/table.hpp line:127
            cl.def(
                "width",
                ( unsigned long ( genesis::util::text::Table::Column::* )() const ) &
                    genesis::util::text::Table::Column::width,
                "C++: genesis::util::text::Table::Column::width() const --> unsigned long"
            );
            // function-signature: void genesis::util::text::Table::Column::shrink_width()()
            // file:genesis/util/text/table.hpp line:129
            cl.def(
                "shrink_width",
                ( void( genesis::util::text::Table::Column::* )() ) &
                    genesis::util::text::Table::Column::shrink_width,
                "C++: genesis::util::text::Table::Column::shrink_width() --> void"
            );
            // function-signature: unsigned long genesis::util::text::Table::Column::length()
            // const() file:genesis/util/text/table.hpp line:135
            cl.def(
                "length",
                ( unsigned long ( genesis::util::text::Table::Column::* )() const ) &
                    genesis::util::text::Table::Column::length,
                "C++: genesis::util::text::Table::Column::length() const --> unsigned long"
            );
            // function-signature: std::string genesis::util::text::Table::Column::row(unsigned
            // long) const(unsigned long) file:genesis/util/text/table.hpp line:137
            cl.def(
                "row",
                ( std::string( genesis::util::text::Table::Column::* )( unsigned long ) const ) &
                    genesis::util::text::Table::Column::row,
                "C++: genesis::util::text::Table::Column::row(unsigned long) const --> std::string",
                pybind11::arg( "i" )
            );
            // function-signature: void genesis::util::text::Table::Column::clear_content()()
            // file:genesis/util/text/table.hpp line:143
            cl.def(
                "clear_content",
                ( void( genesis::util::text::Table::Column::* )() ) &
                    genesis::util::text::Table::Column::clear_content,
                "C++: genesis::util::text::Table::Column::clear_content() --> void"
            );
            // function-signature: void
            // genesis::util::text::Table::Column::append(std::string)(std::string)
            // file:genesis/util/text/table.hpp line:145
            cl.def(
                "append",
                ( void( genesis::util::text::Table::Column::* )( std::string ) ) &
                    genesis::util::text::Table::Column::append,
                "C++: genesis::util::text::Table::Column::append(std::string) --> void",
                pybind11::arg( "value" )
            );
            // function-signature: void genesis::util::text::Table::Column::append(const class
            // genesis::util::text::Style &, std::string)(const class genesis::util::text::Style &,
            // std::string) file:genesis/util/text/table.hpp line:146
            cl.def(
                "append",
                ( void( genesis::util::text::Table::Column::* )(
                    const class genesis::util::text::Style&, std::string
                ) ) &
                    genesis::util::text::Table::Column::append,
                "C++: genesis::util::text::Table::Column::append(const class "
                "genesis::util::text::Style &, std::string) --> void",
                pybind11::arg( "style" ),
                pybind11::arg( "value" )
            );
        }
    }
}
