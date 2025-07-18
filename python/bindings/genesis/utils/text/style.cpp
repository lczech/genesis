#include <genesis/utils/text/style.hpp>
#include <genesis/utils/text/table.hpp>
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
#include <string>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>, false)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*, false)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_genesis_utils_text_style(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::text::Style file:genesis/utils/text/style.hpp line:82
		pybind11::class_<genesis::utils::text::Style, std::shared_ptr<genesis::utils::text::Style>> cl(M("genesis::utils::text"), "Style", "Simple text style class for colorized and bold output to a terminal.\n\n This class bundles the following text style properties for output in a terminal:\n\n   * Foreground Color. Set using foreground_color( std::string const& color ).\n     See Style::foreground_colors for the valid color names.\n   * Background Color. Set using background_color( std::string const& color ).\n     See Style::background_colors for the valid color names.\n   * Bold. Set using bold( bool value ).\n\n Those properties can be set using either the respective constructor of this class or using the\n setter functions. Per default, all of them are empty, meaning that no style manupulation is\n done.\n\n In order to generate textual output with those styles, operator()() is used.\n Example:\n\n     Style blue( \"blue\" );\n     blue.bold( true );\n     std::cout << blue( \"some text\" );\n\n Furthermore, the stream operator can be used to get a summary of the properties of a Style\n object:\n\n     LOG_DBG << blue;\n\n will output\n\n     Foreground Color: Blue\n     Background Color:\n     Bold:             true\n\n to the LOG stream.");
		// function-signature: genesis::utils::text::Style::Style()() file:genesis/utils/text/style.hpp line:90
		cl.def( pybind11::init( [](){ return new genesis::utils::text::Style(); } ) );
		// function-signature: genesis::utils::text::Style::Style(const std::string &)(const std::string &) file:genesis/utils/text/style.hpp line:92
		cl.def( pybind11::init<const std::string &>(), pybind11::arg("foreground_color") );

		// function-signature: genesis::utils::text::Style::Style(const char *)(const char *) file:genesis/utils/text/style.hpp line:96
		cl.def( pybind11::init<const char *>(), pybind11::arg("foreground_color") );

		// function-signature: genesis::utils::text::Style::Style(bool)(bool) file:genesis/utils/text/style.hpp line:100
		cl.def( pybind11::init<bool>(), pybind11::arg("bold") );

		// function-signature: genesis::utils::text::Style::Style(const std::string &, bool)(const std::string &, bool) file:genesis/utils/text/style.hpp line:104
		cl.def( pybind11::init<const std::string &, bool>(), pybind11::arg("foreground_color"), pybind11::arg("bold") );

		// function-signature: genesis::utils::text::Style::Style(const std::string &, const std::string &)(const std::string &, const std::string &) file:genesis/utils/text/style.hpp line:109
		cl.def( pybind11::init<const std::string &, const std::string &>(), pybind11::arg("foreground_color"), pybind11::arg("background_color") );

		// function-signature: genesis::utils::text::Style::Style(const std::string &, const char *)(const std::string &, const char *) file:genesis/utils/text/style.hpp line:114
		cl.def( pybind11::init<const std::string &, const char *>(), pybind11::arg("foreground_color"), pybind11::arg("background_color") );

		// function-signature: genesis::utils::text::Style::Style(const std::string &, const std::string &, bool)(const std::string &, const std::string &, bool) file:genesis/utils/text/style.hpp line:119
		cl.def( pybind11::init<const std::string &, const std::string &, bool>(), pybind11::arg("foreground_color"), pybind11::arg("background_color"), pybind11::arg("bold") );

		// function-signature: genesis::utils::text::Style::Style(const class genesis::utils::text::Style &)(const class genesis::utils::text::Style &) file:genesis/utils/text/style.hpp line:127
		cl.def( pybind11::init( [](genesis::utils::text::Style const &o){ return new genesis::utils::text::Style(o); } ) );
		// function-signature: class genesis::utils::text::Style & genesis::utils::text::Style::operator=(const class genesis::utils::text::Style &)(const class genesis::utils::text::Style &) file:genesis/utils/text/style.hpp line:130
		cl.def("assign", (class genesis::utils::text::Style & (genesis::utils::text::Style::*)(const class genesis::utils::text::Style &)) &genesis::utils::text::Style::operator=, "C++: genesis::utils::text::Style::operator=(const class genesis::utils::text::Style &) --> class genesis::utils::text::Style &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		// function-signature: void genesis::utils::text::Style::swap(class genesis::utils::text::Style &)(class genesis::utils::text::Style &) file:genesis/utils/text/style.hpp line:133
		cl.def("swap", (void (genesis::utils::text::Style::*)(class genesis::utils::text::Style &)) &genesis::utils::text::Style::swap, "C++: genesis::utils::text::Style::swap(class genesis::utils::text::Style &) --> void", pybind11::arg("other"));
		// function-signature: class genesis::utils::text::Style & genesis::utils::text::Style::reset()() file:genesis/utils/text/style.hpp line:146
		cl.def("reset", (class genesis::utils::text::Style & (genesis::utils::text::Style::*)()) &genesis::utils::text::Style::reset, "C++: genesis::utils::text::Style::reset() --> class genesis::utils::text::Style &", pybind11::return_value_policy::automatic);
		// function-signature: bool genesis::utils::text::Style::enabled() const() file:genesis/utils/text/style.hpp line:148
		cl.def("enabled", (bool (genesis::utils::text::Style::*)() const) &genesis::utils::text::Style::enabled, "C++: genesis::utils::text::Style::enabled() const --> bool");
		// function-signature: class genesis::utils::text::Style & genesis::utils::text::Style::enabled(bool)(bool) file:genesis/utils/text/style.hpp line:149
		cl.def("enabled", (class genesis::utils::text::Style & (genesis::utils::text::Style::*)(bool)) &genesis::utils::text::Style::enabled, "C++: genesis::utils::text::Style::enabled(bool) --> class genesis::utils::text::Style &", pybind11::return_value_policy::automatic, pybind11::arg("value"));
		// function-signature: bool genesis::utils::text::Style::bold() const() file:genesis/utils/text/style.hpp line:151
		cl.def("bold", (bool (genesis::utils::text::Style::*)() const) &genesis::utils::text::Style::bold, "C++: genesis::utils::text::Style::bold() const --> bool");
		// function-signature: class genesis::utils::text::Style & genesis::utils::text::Style::bold(bool)(bool) file:genesis/utils/text/style.hpp line:152
		cl.def("bold", (class genesis::utils::text::Style & (genesis::utils::text::Style::*)(bool)) &genesis::utils::text::Style::bold, "C++: genesis::utils::text::Style::bold(bool) --> class genesis::utils::text::Style &", pybind11::return_value_policy::automatic, pybind11::arg("value"));
		// function-signature: std::string genesis::utils::text::Style::foreground_color() const() file:genesis/utils/text/style.hpp line:154
		cl.def("foreground_color", (std::string (genesis::utils::text::Style::*)() const) &genesis::utils::text::Style::foreground_color, "C++: genesis::utils::text::Style::foreground_color() const --> std::string");
		// function-signature: class genesis::utils::text::Style & genesis::utils::text::Style::foreground_color(const std::string &)(const std::string &) file:genesis/utils/text/style.hpp line:155
		cl.def("foreground_color", (class genesis::utils::text::Style & (genesis::utils::text::Style::*)(const std::string &)) &genesis::utils::text::Style::foreground_color, "C++: genesis::utils::text::Style::foreground_color(const std::string &) --> class genesis::utils::text::Style &", pybind11::return_value_policy::automatic, pybind11::arg("color"));
		// function-signature: std::string genesis::utils::text::Style::background_color() const() file:genesis/utils/text/style.hpp line:157
		cl.def("background_color", (std::string (genesis::utils::text::Style::*)() const) &genesis::utils::text::Style::background_color, "C++: genesis::utils::text::Style::background_color() const --> std::string");
		// function-signature: class genesis::utils::text::Style & genesis::utils::text::Style::background_color(const std::string &)(const std::string &) file:genesis/utils/text/style.hpp line:158
		cl.def("background_color", (class genesis::utils::text::Style & (genesis::utils::text::Style::*)(const std::string &)) &genesis::utils::text::Style::background_color, "C++: genesis::utils::text::Style::background_color(const std::string &) --> class genesis::utils::text::Style &", pybind11::return_value_policy::automatic, pybind11::arg("color"));
		// function-signature: std::string genesis::utils::text::Style::operator()(const std::string &) const(const std::string &) file:genesis/utils/text/style.hpp line:164
		cl.def("__call__", (std::string (genesis::utils::text::Style::*)(const std::string &) const) &genesis::utils::text::Style::operator(), "C++: genesis::utils::text::Style::operator()(const std::string &) const --> std::string", pybind11::arg("text"));
		// function-signature: std::string genesis::utils::text::Style::to_bash_string(const std::string &) const(const std::string &) file:genesis/utils/text/style.hpp line:166
		cl.def("to_bash_string", (std::string (genesis::utils::text::Style::*)(const std::string &) const) &genesis::utils::text::Style::to_bash_string, "C++: genesis::utils::text::Style::to_bash_string(const std::string &) const --> std::string", pybind11::arg("text"));
		// function-signature: std::string genesis::utils::text::Style::to_python_string(const std::string &) const(const std::string &) file:genesis/utils/text/style.hpp line:167
		cl.def("to_python_string", (std::string (genesis::utils::text::Style::*)(const std::string &) const) &genesis::utils::text::Style::to_python_string, "C++: genesis::utils::text::Style::to_python_string(const std::string &) const --> std::string", pybind11::arg("text"));
		// function-signature: bool genesis::utils::text::Style::is_foreground_color(std::string)(std::string) file:genesis/utils/text/style.hpp line:173
		cl.def_static("is_foreground_color", (bool (*)(std::string)) &genesis::utils::text::Style::is_foreground_color, "C++: genesis::utils::text::Style::is_foreground_color(std::string) --> bool", pybind11::arg("name"));
		// function-signature: bool genesis::utils::text::Style::is_background_color(std::string)(std::string) file:genesis/utils/text/style.hpp line:174
		cl.def_static("is_background_color", (bool (*)(std::string)) &genesis::utils::text::Style::is_background_color, "C++: genesis::utils::text::Style::is_background_color(std::string) --> bool", pybind11::arg("name"));
		// function-signature: std::string genesis::utils::text::Style::get_foreground_color_value(std::string)(std::string) file:genesis/utils/text/style.hpp line:176
		cl.def_static("get_foreground_color_value", (std::string (*)(std::string)) &genesis::utils::text::Style::get_foreground_color_value, "C++: genesis::utils::text::Style::get_foreground_color_value(std::string) --> std::string", pybind11::arg("name"));
		// function-signature: std::string genesis::utils::text::Style::get_background_color_value(std::string)(std::string) file:genesis/utils/text/style.hpp line:177
		cl.def_static("get_background_color_value", (std::string (*)(std::string)) &genesis::utils::text::Style::get_background_color_value, "C++: genesis::utils::text::Style::get_background_color_value(std::string) --> std::string", pybind11::arg("name"));

		cl.def("__str__", [](genesis::utils::text::Style const &o) -> std::string { std::ostringstream s; using namespace genesis::utils::text; s << o; return s.str(); } );
	}
	// genesis::utils::text::swap(class genesis::utils::text::Style &, class genesis::utils::text::Style &) file:genesis/utils/text/style.hpp line:199
	// function-signature: void genesis::utils::text::swap(class genesis::utils::text::Style &, class genesis::utils::text::Style &)(class genesis::utils::text::Style &, class genesis::utils::text::Style &) file:genesis/utils/text/style.hpp line:199
	M("genesis::utils::text").def("swap", (void (*)(class genesis::utils::text::Style &, class genesis::utils::text::Style &)) &genesis::utils::text::swap, "C++: genesis::utils::text::swap(class genesis::utils::text::Style &, class genesis::utils::text::Style &) --> void", pybind11::arg("lhs"), pybind11::arg("rhs"));

	{ // genesis::utils::text::Table file:genesis/utils/text/table.hpp line:62
		pybind11::class_<genesis::utils::text::Table, std::shared_ptr<genesis::utils::text::Table>> cl(M("genesis::utils::text"), "Table", "");
		// function-signature: genesis::utils::text::Table::Table()() file:genesis/utils/text/table.hpp line:185
		cl.def( pybind11::init( [](){ return new genesis::utils::text::Table(); } ) );
		// function-signature: genesis::utils::text::Table::Table(const class genesis::utils::text::Table &)(const class genesis::utils::text::Table &) file:genesis/utils/text/table.hpp line:188
		cl.def( pybind11::init( [](genesis::utils::text::Table const &o){ return new genesis::utils::text::Table(o); } ) );
		// function-signature: class genesis::utils::text::Table & genesis::utils::text::Table::operator=(const class genesis::utils::text::Table &)(const class genesis::utils::text::Table &) file:genesis/utils/text/table.hpp line:191
		cl.def("assign", (class genesis::utils::text::Table & (genesis::utils::text::Table::*)(const class genesis::utils::text::Table &)) &genesis::utils::text::Table::operator=, "C++: genesis::utils::text::Table::operator=(const class genesis::utils::text::Table &) --> class genesis::utils::text::Table &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		// function-signature: unsigned long genesis::utils::text::Table::length() const() file:genesis/utils/text/table.hpp line:203
		cl.def("length", (unsigned long (genesis::utils::text::Table::*)() const) &genesis::utils::text::Table::length, "C++: genesis::utils::text::Table::length() const --> unsigned long");
		// function-signature: void genesis::utils::text::Table::clear()() file:genesis/utils/text/table.hpp line:209
		cl.def("clear", (void (genesis::utils::text::Table::*)()) &genesis::utils::text::Table::clear, "C++: genesis::utils::text::Table::clear() --> void");
		// function-signature: void genesis::utils::text::Table::clear_content()() file:genesis/utils/text/table.hpp line:210
		cl.def("clear_content", (void (genesis::utils::text::Table::*)()) &genesis::utils::text::Table::clear_content, "C++: genesis::utils::text::Table::clear_content() --> void");
		// function-signature: class genesis::utils::text::Table::Column & genesis::utils::text::Table::add_column(std::string, enum genesis::utils::text::Table::Column::Justification)(std::string, enum genesis::utils::text::Table::Column::Justification) file:genesis/utils/text/table.hpp line:212
		cl.def("add_column", [](genesis::utils::text::Table &o) -> genesis::utils::text::Table::Column & { return o.add_column(); }, "", pybind11::return_value_policy::automatic);
		cl.def("add_column", [](genesis::utils::text::Table &o, std::string const & a0) -> genesis::utils::text::Table::Column & { return o.add_column(a0); }, "", pybind11::return_value_policy::automatic, pybind11::arg("label"));
		cl.def("add_column", (class genesis::utils::text::Table::Column & (genesis::utils::text::Table::*)(std::string, enum genesis::utils::text::Table::Column::Justification)) &genesis::utils::text::Table::add_column, "C++: genesis::utils::text::Table::add_column(std::string, enum genesis::utils::text::Table::Column::Justification) --> class genesis::utils::text::Table::Column &", pybind11::return_value_policy::automatic, pybind11::arg("label"), pybind11::arg("justify"));
		// function-signature: class genesis::utils::text::Table & genesis::utils::text::Table::operator<<(std::string)(std::string) file:genesis/utils/text/table.hpp line:217
		cl.def("__lshift__", (class genesis::utils::text::Table & (genesis::utils::text::Table::*)(std::string)) &genesis::utils::text::Table::operator<<, "C++: genesis::utils::text::Table::operator<<(std::string) --> class genesis::utils::text::Table &", pybind11::return_value_policy::automatic, pybind11::arg("value"));
		// function-signature: class genesis::utils::text::Table & genesis::utils::text::Table::append(std::string)(std::string) file:genesis/utils/text/table.hpp line:229
		cl.def("append", (class genesis::utils::text::Table & (genesis::utils::text::Table::*)(std::string)) &genesis::utils::text::Table::append, "C++: genesis::utils::text::Table::append(std::string) --> class genesis::utils::text::Table &", pybind11::return_value_policy::automatic, pybind11::arg("value"));
		// function-signature: class genesis::utils::text::Table & genesis::utils::text::Table::append(const class genesis::utils::text::Style &, std::string)(const class genesis::utils::text::Style &, std::string) file:genesis/utils/text/table.hpp line:230
		cl.def("append", (class genesis::utils::text::Table & (genesis::utils::text::Table::*)(const class genesis::utils::text::Style &, std::string)) &genesis::utils::text::Table::append, "C++: genesis::utils::text::Table::append(const class genesis::utils::text::Style &, std::string) --> class genesis::utils::text::Table &", pybind11::return_value_policy::automatic, pybind11::arg("style"), pybind11::arg("value"));
		// function-signature: class genesis::utils::text::Table & genesis::utils::text::Table::line_break()() file:genesis/utils/text/table.hpp line:232
		cl.def("line_break", (class genesis::utils::text::Table & (genesis::utils::text::Table::*)()) &genesis::utils::text::Table::line_break, "C++: genesis::utils::text::Table::line_break() --> class genesis::utils::text::Table &", pybind11::return_value_policy::automatic);
		// function-signature: std::string genesis::utils::text::Table::to_string() const() file:genesis/utils/text/table.hpp line:241
		cl.def("to_string", (std::string (genesis::utils::text::Table::*)() const) &genesis::utils::text::Table::to_string, "C++: genesis::utils::text::Table::to_string() const --> std::string");
		// function-signature: std::string genesis::utils::text::Table::to_string(const struct genesis::utils::text::TableLayout &) const(const struct genesis::utils::text::TableLayout &) file:genesis/utils/text/table.hpp line:242
		cl.def("to_string", (std::string (genesis::utils::text::Table::*)(const struct genesis::utils::text::TableLayout &) const) &genesis::utils::text::Table::to_string, "C++: genesis::utils::text::Table::to_string(const struct genesis::utils::text::TableLayout &) const --> std::string", pybind11::arg("layout"));

		cl.def("__str__", [](genesis::utils::text::Table const &o) -> std::string { std::ostringstream s; using namespace genesis::utils::text; s << o; return s.str(); } );

		{ // genesis::utils::text::Table::Column file:genesis/utils/text/table.hpp line:74
			auto & enclosing_class = cl;
			pybind11::class_<genesis::utils::text::Table::Column, std::shared_ptr<genesis::utils::text::Table::Column>> cl(enclosing_class, "Column", "");
			// function-signature: genesis::utils::text::Table::Column::Column()() file:genesis/utils/text/table.hpp line:95
			cl.def( pybind11::init( [](){ return new genesis::utils::text::Table::Column(); } ) );
			// function-signature: genesis::utils::text::Table::Column::Column(const std::string &, enum genesis::utils::text::Table::Column::Justification)(const std::string &, enum genesis::utils::text::Table::Column::Justification) file:genesis/utils/text/table.hpp line:97
			cl.def( pybind11::init( [](const std::string & a0){ return new genesis::utils::text::Table::Column(a0); } ), "doc" , pybind11::arg("label"));
			// function-signature: genesis::utils::text::Table::Column::Column(const std::string &, enum genesis::utils::text::Table::Column::Justification)(const std::string &, enum genesis::utils::text::Table::Column::Justification) file:genesis/utils/text/table.hpp line:97
			cl.def( pybind11::init<const std::string &, enum genesis::utils::text::Table::Column::Justification>(), pybind11::arg("label"), pybind11::arg("justify") );

			// function-signature: genesis::utils::text::Table::Column::Column(const class genesis::utils::text::Table::Column &)(const class genesis::utils::text::Table::Column &) file:genesis/utils/text/table.hpp line:105
			cl.def( pybind11::init( [](genesis::utils::text::Table::Column const &o){ return new genesis::utils::text::Table::Column(o); } ) );

			pybind11::enum_<genesis::utils::text::Table::Column::Justification>(cl, "Justification", "")
				.value("kLeft", genesis::utils::text::Table::Column::Justification::kLeft)
				.value("kRight", genesis::utils::text::Table::Column::Justification::kRight)
				.value("kCentered", genesis::utils::text::Table::Column::Justification::kCentered);

			// function-signature: class genesis::utils::text::Table::Column & genesis::utils::text::Table::Column::operator=(const class genesis::utils::text::Table::Column &)(const class genesis::utils::text::Table::Column &) file:genesis/utils/text/table.hpp line:108
			cl.def("assign", (class genesis::utils::text::Table::Column & (genesis::utils::text::Table::Column::*)(const class genesis::utils::text::Table::Column &)) &genesis::utils::text::Table::Column::operator=, "C++: genesis::utils::text::Table::Column::operator=(const class genesis::utils::text::Table::Column &) --> class genesis::utils::text::Table::Column &", pybind11::return_value_policy::automatic, pybind11::arg(""));
			// function-signature: void genesis::utils::text::Table::Column::label(std::string)(std::string) file:genesis/utils/text/table.hpp line:120
			cl.def("label", (void (genesis::utils::text::Table::Column::*)(std::string)) &genesis::utils::text::Table::Column::label, "C++: genesis::utils::text::Table::Column::label(std::string) --> void", pybind11::arg("value"));
			// function-signature: std::string genesis::utils::text::Table::Column::label() const() file:genesis/utils/text/table.hpp line:121
			cl.def("label", (std::string (genesis::utils::text::Table::Column::*)() const) &genesis::utils::text::Table::Column::label, "C++: genesis::utils::text::Table::Column::label() const --> std::string");
			// function-signature: void genesis::utils::text::Table::Column::justify(enum genesis::utils::text::Table::Column::Justification)(enum genesis::utils::text::Table::Column::Justification) file:genesis/utils/text/table.hpp line:123
			cl.def("justify", (void (genesis::utils::text::Table::Column::*)(enum genesis::utils::text::Table::Column::Justification)) &genesis::utils::text::Table::Column::justify, "C++: genesis::utils::text::Table::Column::justify(enum genesis::utils::text::Table::Column::Justification) --> void", pybind11::arg("value"));
			// function-signature: enum genesis::utils::text::Table::Column::Justification genesis::utils::text::Table::Column::justify() const() file:genesis/utils/text/table.hpp line:124
			cl.def("justify", (enum genesis::utils::text::Table::Column::Justification (genesis::utils::text::Table::Column::*)() const) &genesis::utils::text::Table::Column::justify, "C++: genesis::utils::text::Table::Column::justify() const --> enum genesis::utils::text::Table::Column::Justification");
			// function-signature: void genesis::utils::text::Table::Column::width(unsigned long)(unsigned long) file:genesis/utils/text/table.hpp line:126
			cl.def("width", (void (genesis::utils::text::Table::Column::*)(unsigned long)) &genesis::utils::text::Table::Column::width, "C++: genesis::utils::text::Table::Column::width(unsigned long) --> void", pybind11::arg("value"));
			// function-signature: unsigned long genesis::utils::text::Table::Column::width() const() file:genesis/utils/text/table.hpp line:127
			cl.def("width", (unsigned long (genesis::utils::text::Table::Column::*)() const) &genesis::utils::text::Table::Column::width, "C++: genesis::utils::text::Table::Column::width() const --> unsigned long");
			// function-signature: void genesis::utils::text::Table::Column::shrink_width()() file:genesis/utils/text/table.hpp line:129
			cl.def("shrink_width", (void (genesis::utils::text::Table::Column::*)()) &genesis::utils::text::Table::Column::shrink_width, "C++: genesis::utils::text::Table::Column::shrink_width() --> void");
			// function-signature: unsigned long genesis::utils::text::Table::Column::length() const() file:genesis/utils/text/table.hpp line:135
			cl.def("length", (unsigned long (genesis::utils::text::Table::Column::*)() const) &genesis::utils::text::Table::Column::length, "C++: genesis::utils::text::Table::Column::length() const --> unsigned long");
			// function-signature: std::string genesis::utils::text::Table::Column::row(unsigned long) const(unsigned long) file:genesis/utils/text/table.hpp line:137
			cl.def("row", (std::string (genesis::utils::text::Table::Column::*)(unsigned long) const) &genesis::utils::text::Table::Column::row, "C++: genesis::utils::text::Table::Column::row(unsigned long) const --> std::string", pybind11::arg("i"));
			// function-signature: void genesis::utils::text::Table::Column::clear_content()() file:genesis/utils/text/table.hpp line:143
			cl.def("clear_content", (void (genesis::utils::text::Table::Column::*)()) &genesis::utils::text::Table::Column::clear_content, "C++: genesis::utils::text::Table::Column::clear_content() --> void");
			// function-signature: void genesis::utils::text::Table::Column::append(std::string)(std::string) file:genesis/utils/text/table.hpp line:145
			cl.def("append", (void (genesis::utils::text::Table::Column::*)(std::string)) &genesis::utils::text::Table::Column::append, "C++: genesis::utils::text::Table::Column::append(std::string) --> void", pybind11::arg("value"));
			// function-signature: void genesis::utils::text::Table::Column::append(const class genesis::utils::text::Style &, std::string)(const class genesis::utils::text::Style &, std::string) file:genesis/utils/text/table.hpp line:146
			cl.def("append", (void (genesis::utils::text::Table::Column::*)(const class genesis::utils::text::Style &, std::string)) &genesis::utils::text::Table::Column::append, "C++: genesis::utils::text::Table::Column::append(const class genesis::utils::text::Style &, std::string) --> void", pybind11::arg("style"), pybind11::arg("value"));
		}

	}
}
