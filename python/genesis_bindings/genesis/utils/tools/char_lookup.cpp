#include <functional>
#include <genesis/utils/core/logging.hpp>
#include <genesis/utils/core/std.hpp>
#include <genesis/utils/tools/char_lookup.hpp>
#include <ios>
#include <iterator>
#include <memory>
#include <mutex>
#include <ostream>
#include <sstream>
#include <sstream> // __str__
#include <streambuf>
#include <string>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <pybind11/functional.h>
#include <../python/custom_bindings/extensions/utils/matrix.hpp>
#include <genesis/utils/color/functions.hpp>
#include <../python/custom_bindings/extensions/utils/bitvector.hpp>
#include <../python/custom_bindings/extensions/utils/range.hpp>
#include <../python/custom_bindings/extensions/string_target.hpp>
#include <../python/custom_bindings/extensions/sequence/quality.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_set.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_dict.hpp>
#include <../python/custom_bindings/extensions/sequence/fasta_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/fastq_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/reference_genome.hpp>
#include <../python/custom_bindings/extensions/taxonomy/taxopath.hpp>
#include <../python/custom_bindings/extensions/taxonomy/iterator.hpp>
#include <../python/custom_bindings/extensions/tree/tree.hpp>
#include <../python/custom_bindings/extensions/tree/functions_tree.hpp>
#include <../python/custom_bindings/extensions/placement/helper.hpp>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_genesis_utils_tools_char_lookup(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::CharLookup file:genesis/utils/tools/char_lookup.hpp line:54
		pybind11::class_<genesis::utils::CharLookup<unsigned long>, std::shared_ptr<genesis::utils::CharLookup<unsigned long>>> cl(M("genesis::utils"), "CharLookup_unsigned_long_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::CharLookup<unsigned long>(); } ) );
		cl.def( pybind11::init<const unsigned long &>(), pybind11::arg("init_all") );

		cl.def( pybind11::init( [](genesis::utils::CharLookup<unsigned long> const &o){ return new genesis::utils::CharLookup<unsigned long>(o); } ) );
		cl.def("assign", (class genesis::utils::CharLookup<unsigned long> & (genesis::utils::CharLookup<unsigned long>::*)(const class genesis::utils::CharLookup<unsigned long> &)) &genesis::utils::CharLookup<unsigned long>::operator=, "C++: genesis::utils::CharLookup<unsigned long>::operator=(const class genesis::utils::CharLookup<unsigned long> &) --> class genesis::utils::CharLookup<unsigned long> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("set_char", (void (genesis::utils::CharLookup<unsigned long>::*)(char, unsigned long)) &genesis::utils::CharLookup<unsigned long>::set_char, "C++: genesis::utils::CharLookup<unsigned long>::set_char(char, unsigned long) --> void", pybind11::arg("c"), pybind11::arg("value"));
		cl.def("set_char_upper_lower", (void (genesis::utils::CharLookup<unsigned long>::*)(char, unsigned long)) &genesis::utils::CharLookup<unsigned long>::set_char_upper_lower, "C++: genesis::utils::CharLookup<unsigned long>::set_char_upper_lower(char, unsigned long) --> void", pybind11::arg("c"), pybind11::arg("value"));
		cl.def("set_if", (void (genesis::utils::CharLookup<unsigned long>::*)(class std::function<bool (char)>, unsigned long)) &genesis::utils::CharLookup<unsigned long>::set_if, "C++: genesis::utils::CharLookup<unsigned long>::set_if(class std::function<bool (char)>, unsigned long) --> void", pybind11::arg("predicate"), pybind11::arg("value"));
		cl.def("set_selection", (void (genesis::utils::CharLookup<unsigned long>::*)(const std::string &, unsigned long)) &genesis::utils::CharLookup<unsigned long>::set_selection, "C++: genesis::utils::CharLookup<unsigned long>::set_selection(const std::string &, unsigned long) --> void", pybind11::arg("chars"), pybind11::arg("value"));
		cl.def("set_selection_upper_lower", (void (genesis::utils::CharLookup<unsigned long>::*)(const std::string &, unsigned long)) &genesis::utils::CharLookup<unsigned long>::set_selection_upper_lower, "C++: genesis::utils::CharLookup<unsigned long>::set_selection_upper_lower(const std::string &, unsigned long) --> void", pybind11::arg("chars"), pybind11::arg("value"));
		cl.def("set_range", (void (genesis::utils::CharLookup<unsigned long>::*)(char, char, unsigned long)) &genesis::utils::CharLookup<unsigned long>::set_range, "C++: genesis::utils::CharLookup<unsigned long>::set_range(char, char, unsigned long) --> void", pybind11::arg("first"), pybind11::arg("last"), pybind11::arg("value"));
		cl.def("set_all", (void (genesis::utils::CharLookup<unsigned long>::*)(unsigned long)) &genesis::utils::CharLookup<unsigned long>::set_all, "C++: genesis::utils::CharLookup<unsigned long>::set_all(unsigned long) --> void", pybind11::arg("value"));
		cl.def("__getitem__", (unsigned long (genesis::utils::CharLookup<unsigned long>::*)(char) const) &genesis::utils::CharLookup<unsigned long>::operator[], "C++: genesis::utils::CharLookup<unsigned long>::operator[](char) const --> unsigned long", pybind11::arg("c"));
		cl.def("get", (unsigned long (genesis::utils::CharLookup<unsigned long>::*)(char) const) &genesis::utils::CharLookup<unsigned long>::get, "C++: genesis::utils::CharLookup<unsigned long>::get(char) const --> unsigned long", pybind11::arg("c"));
		cl.def("get_chars_equal_to", (std::string (genesis::utils::CharLookup<unsigned long>::*)(unsigned long) const) &genesis::utils::CharLookup<unsigned long>::get_chars_equal_to, "C++: genesis::utils::CharLookup<unsigned long>::get_chars_equal_to(unsigned long) const --> std::string", pybind11::arg("comp_value"));
		cl.def("all_equal_to", (bool (genesis::utils::CharLookup<unsigned long>::*)(unsigned long) const) &genesis::utils::CharLookup<unsigned long>::all_equal_to, "C++: genesis::utils::CharLookup<unsigned long>::all_equal_to(unsigned long) const --> bool", pybind11::arg("comp_value"));
	}
	// genesis::utils::make_unique() file:genesis/utils/core/std.hpp line:82
	M("genesis::utils").def("make_unique", (class std::unique_ptr<class std::mutex, struct std::default_delete<class std::mutex> > (*)()) &genesis::utils::make_unique<std::mutex>, "C++: genesis::utils::make_unique() --> class std::unique_ptr<class std::mutex, struct std::default_delete<class std::mutex> >");

	// genesis::utils::combine_hashes_32(unsigned long, unsigned long) file:genesis/utils/core/std.hpp line:114
	M("genesis::utils").def("combine_hashes_32", (unsigned long (*)(unsigned long, unsigned long)) &genesis::utils::combine_hashes_32, "C++: genesis::utils::combine_hashes_32(unsigned long, unsigned long) --> unsigned long", pybind11::arg("h1"), pybind11::arg("h2"));

	// genesis::utils::combine_hashes_64(unsigned long, unsigned long) file:genesis/utils/core/std.hpp line:119
	M("genesis::utils").def("combine_hashes_64", (unsigned long (*)(unsigned long, unsigned long)) &genesis::utils::combine_hashes_64, "C++: genesis::utils::combine_hashes_64(unsigned long, unsigned long) --> unsigned long", pybind11::arg("h1"), pybind11::arg("h2"));

	// genesis::utils::combine_hashes(unsigned long, unsigned long) file:genesis/utils/core/std.hpp line:127
	M("genesis::utils").def("combine_hashes", (unsigned long (*)(unsigned long, unsigned long)) &genesis::utils::combine_hashes, "Combine two hash values.\n\nC++: genesis::utils::combine_hashes(unsigned long, unsigned long) --> unsigned long", pybind11::arg("h1"), pybind11::arg("h2"));

	// genesis::utils::logging_progress_value(long) file:genesis/utils/core/logging.hpp line:179
	M("genesis::utils").def("logging_progress_value", []() -> long { return genesis::utils::logging_progress_value(); }, "");
	M("genesis::utils").def("logging_progress_value", (long (*)(long)) &genesis::utils::logging_progress_value, "Hack function to make sure that the value arugment in #LOG_PROG is only evaluated once.\n\n Without this function, #LOG_PROG would include two appearances of its variable `value`, which\n means that a statement like\n\n     LOG_PROG(++i, n) << \"of progress.\";\n\n would lead to a double evaluation of the increment statement `++i`. That is not intended, thus\n we need this hack function.\n\nC++: genesis::utils::logging_progress_value(long) --> long", pybind11::arg("value"));

	{ // genesis::utils::LoggingDetails file:genesis/utils/core/logging.hpp line:267
		pybind11::class_<genesis::utils::LoggingDetails, std::shared_ptr<genesis::utils::LoggingDetails>> cl(M("genesis::utils"), "LoggingDetails", "POD stuct containing the settings for which information is included\n with each logging message.\n\n The details are activated via accessing the static variable of the\n Logging class:\n\n     Logging::details.level = true;\n\n All active details are prepended to the actual log message and\n separated by spaces (execpt file and line, they are separated by a\n colon). Their order is fixed.\n\n A message with all details activates looks like this\n\n     0003 2014-10-28 11:40:47 0.001859 0.000103 src/main/main.cc:28 (int main (int argc, char* argv[])) INFO Hello World.\n\n It was the third message being logged in this run of the program, at\n a date and time, 0.001859 sec after the program started and 0.000103\n sec after the last log message. It was called from main.cc line 28 in function main\n and has LoggingLevel Info.");
		cl.def( pybind11::init( [](){ return new genesis::utils::LoggingDetails(); } ) );
		cl.def( pybind11::init<bool, bool, bool, bool, bool, bool, bool, bool, bool>(), pybind11::arg("v_count"), pybind11::arg("v_date"), pybind11::arg("v_time"), pybind11::arg("v_runtime"), pybind11::arg("v_rundiff"), pybind11::arg("v_file"), pybind11::arg("v_line"), pybind11::arg("v_function"), pybind11::arg("v_level") );

		cl.def( pybind11::init( [](genesis::utils::LoggingDetails const &o){ return new genesis::utils::LoggingDetails(o); } ) );
		cl.def_readwrite("count", &genesis::utils::LoggingDetails::count);
		cl.def_readwrite("date", &genesis::utils::LoggingDetails::date);
		cl.def_readwrite("time", &genesis::utils::LoggingDetails::time);
		cl.def_readwrite("runtime", &genesis::utils::LoggingDetails::runtime);
		cl.def_readwrite("rundiff", &genesis::utils::LoggingDetails::rundiff);
		cl.def_readwrite("file", &genesis::utils::LoggingDetails::file);
		cl.def_readwrite("line", &genesis::utils::LoggingDetails::line);
		cl.def_readwrite("function", &genesis::utils::LoggingDetails::function);
		cl.def_readwrite("level", &genesis::utils::LoggingDetails::level);
	}
	{ // genesis::utils::Logging file:genesis/utils/core/logging.hpp line:386
		pybind11::class_<genesis::utils::Logging, std::shared_ptr<genesis::utils::Logging>> cl(M("genesis::utils"), "Logging", "%Logging class with easy and fast usage.\n\n The basic usage of this class is to invoke the macros for the different types\n of log messages and send a stream of messages to them:\n\n     LOG_DBG << \"you are here\";\n     LOG_ERR << \"there was an error: \" << 42;\n\n The provided types of macros are: #LOG_ERR, #LOG_WARN, #LOG_INFO,\n #LOG_DBG, #LOG_DBG1, #LOG_DBG2, #LOG_DBG3, #LOG_DBG4 for all levels of\n logging explained in LoggingLevel.\n\n The details that are logged with each message can be changed via accessing\n Logging::details -- see LoggingDetails for more on that.\n\n In order to use this class, at least one output stream has to be added first\n by invoking either AddOutputStream or AddOutputFile.\n\n The depths of logging can be changed in order to reduce the amount of written\n messages. First, at compile time the macro constant LOG_LEVEL_MAX can be set\n (e.g. in the Makefile) to the highest level that shall be logged. All log\n invoakations with a higher level are never called, and moreover will be\n pruned from the code by most compilers completetly. This makes the log class\n fast -- instead of deleting all log invokations by hand, it is sufficient to\n set the constant to a low level.\n Second, the depths of logging can be changed dynamically at run time by\n setting Logging::max_level to the desired value. Of course, this value\n cannot be higher than LOG_LEVEL_MAX, because those logs are already pruned\n by the compiler.\n\n There are also three more special log types that create a different output than\n the previously mentioned types: #LOG_BOLD, #LOG_TIME and #LOG_PROG. See their\n respective documentation for more information.\n\n The inner working of this class is as follows: Upon invokation via one of the\n macros, an instance is created that stays alive only for the rest of the\n source code line. In this line, the log message is inserted to the buffer\n stream. After the line finishes, the object is automatically destroyed, so\n that its destructor is called. In the destructor, the message is composed and\n logged to all given output streams.\n\n Caveat: Because the macro contains conditions depending on the log level,\n do not use code in a log line that changes the program state.\n Therefore, instead of\n\n     LOG_INFO << \"this is iteration \" << ++i;\n\n use\n\n     ++i;\n     LOG_INFO << \"this is iteration \" << i;\n\n because the former will not work when the max log level is below info level.");
		cl.def( pybind11::init( [](){ return new genesis::utils::Logging(); } ) );
		cl.def( pybind11::init( [](genesis::utils::Logging const &o){ return new genesis::utils::Logging(o); } ) );

		pybind11::enum_<genesis::utils::Logging::LoggingLevel>(cl, "LoggingLevel", pybind11::arithmetic(), "Levels of severity used for logging.\n\n The levels are in ascending order and are used both to signal what\n kind of message is being logged and to provide a threshold for less\n important messages that can be filtered out, for example debug\n messages in the production build of the program. Because some of the\n filtering is already done at compile time, log messages with a level\n higher than #LOG_LEVEL_MAX do not produce any overhead. See also\n Logging class for more on this.")
			.value("kNone", genesis::utils::Logging::kNone)
			.value("kError", genesis::utils::Logging::kError)
			.value("kWarning", genesis::utils::Logging::kWarning)
			.value("kInfo", genesis::utils::Logging::kInfo)
			.value("kProgress", genesis::utils::Logging::kProgress)
			.value("kMessage", genesis::utils::Logging::kMessage)
			.value("kMessage1", genesis::utils::Logging::kMessage1)
			.value("kMessage2", genesis::utils::Logging::kMessage2)
			.value("kMessage3", genesis::utils::Logging::kMessage3)
			.value("kMessage4", genesis::utils::Logging::kMessage4)
			.value("kDebug", genesis::utils::Logging::kDebug)
			.value("kDebug1", genesis::utils::Logging::kDebug1)
			.value("kDebug2", genesis::utils::Logging::kDebug2)
			.value("kDebug3", genesis::utils::Logging::kDebug3)
			.value("kDebug4", genesis::utils::Logging::kDebug4)
			.export_values();

		cl.def("get", (class std::basic_ostringstream<char> & (genesis::utils::Logging::*)(const std::string &, const int, const std::string &, const enum genesis::utils::Logging::LoggingLevel)) &genesis::utils::Logging::get, "C++: genesis::utils::Logging::get(const std::string &, const int, const std::string &, const enum genesis::utils::Logging::LoggingLevel) --> class std::basic_ostringstream<char> &", pybind11::return_value_policy::reference_internal, pybind11::arg("file"), pybind11::arg("line"), pybind11::arg("function"), pybind11::arg("level"));
		cl.def("get", (class std::basic_ostringstream<char> & (genesis::utils::Logging::*)(const std::string &, const int, const std::string &, const enum genesis::utils::Logging::LoggingLevel, const struct genesis::utils::LoggingDetails)) &genesis::utils::Logging::get, "C++: genesis::utils::Logging::get(const std::string &, const int, const std::string &, const enum genesis::utils::Logging::LoggingLevel, const struct genesis::utils::LoggingDetails) --> class std::basic_ostringstream<char> &", pybind11::return_value_policy::reference_internal, pybind11::arg("file"), pybind11::arg("line"), pybind11::arg("function"), pybind11::arg("level"), pybind11::arg("dets"));
		cl.def_static("log_to_stdout", (void (*)()) &genesis::utils::Logging::log_to_stdout, "C++: genesis::utils::Logging::log_to_stdout() --> void");
		cl.def_static("log_to_stream", (void (*)(std::ostream &)) &genesis::utils::Logging::log_to_stream, "C++: genesis::utils::Logging::log_to_stream(std::ostream &) --> void", pybind11::arg("os"));
		cl.def_static("log_to_file", (void (*)(const std::string &)) &genesis::utils::Logging::log_to_file, "C++: genesis::utils::Logging::log_to_file(const std::string &) --> void", pybind11::arg("fn"));
		cl.def_static("clear", (void (*)()) &genesis::utils::Logging::clear, "C++: genesis::utils::Logging::clear() --> void");
		cl.def_static("max_level", (enum genesis::utils::Logging::LoggingLevel (*)()) &genesis::utils::Logging::max_level, "Get the highest log level that is reported. \n\nC++: genesis::utils::Logging::max_level() --> enum genesis::utils::Logging::LoggingLevel");
		cl.def_static("max_level", (void (*)(const enum genesis::utils::Logging::LoggingLevel)) &genesis::utils::Logging::max_level, "C++: genesis::utils::Logging::max_level(const enum genesis::utils::Logging::LoggingLevel) --> void", pybind11::arg("level"));
		cl.def_static("report_percentage", (int (*)()) &genesis::utils::Logging::report_percentage, "Get the current percentage for reporting #LOG_PROG messages. \n\nC++: genesis::utils::Logging::report_percentage() --> int");
		cl.def_static("report_percentage", (void (*)(const int)) &genesis::utils::Logging::report_percentage, "C++: genesis::utils::Logging::report_percentage(const int) --> void", pybind11::arg("percentage"));
		cl.def_static("level_to_string", (std::string (*)(const enum genesis::utils::Logging::LoggingLevel)) &genesis::utils::Logging::level_to_string, "C++: genesis::utils::Logging::level_to_string(const enum genesis::utils::Logging::LoggingLevel) --> std::string", pybind11::arg("level"));
		cl.def_static("log_error", (void (*)(const std::string &)) &genesis::utils::Logging::log_error, "C++: genesis::utils::Logging::log_error(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_warning", (void (*)(const std::string &)) &genesis::utils::Logging::log_warning, "C++: genesis::utils::Logging::log_warning(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_info", (void (*)(const std::string &)) &genesis::utils::Logging::log_info, "C++: genesis::utils::Logging::log_info(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_message", (void (*)(const std::string &)) &genesis::utils::Logging::log_message, "C++: genesis::utils::Logging::log_message(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_message_1", (void (*)(const std::string &)) &genesis::utils::Logging::log_message_1, "C++: genesis::utils::Logging::log_message_1(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_message_2", (void (*)(const std::string &)) &genesis::utils::Logging::log_message_2, "C++: genesis::utils::Logging::log_message_2(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_message_3", (void (*)(const std::string &)) &genesis::utils::Logging::log_message_3, "C++: genesis::utils::Logging::log_message_3(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_message_4", (void (*)(const std::string &)) &genesis::utils::Logging::log_message_4, "C++: genesis::utils::Logging::log_message_4(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_debug", (void (*)(const std::string &)) &genesis::utils::Logging::log_debug, "C++: genesis::utils::Logging::log_debug(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_debug_1", (void (*)(const std::string &)) &genesis::utils::Logging::log_debug_1, "C++: genesis::utils::Logging::log_debug_1(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_debug_2", (void (*)(const std::string &)) &genesis::utils::Logging::log_debug_2, "C++: genesis::utils::Logging::log_debug_2(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_debug_3", (void (*)(const std::string &)) &genesis::utils::Logging::log_debug_3, "C++: genesis::utils::Logging::log_debug_3(const std::string &) --> void", pybind11::arg("msg"));
		cl.def_static("log_debug_4", (void (*)(const std::string &)) &genesis::utils::Logging::log_debug_4, "C++: genesis::utils::Logging::log_debug_4(const std::string &) --> void", pybind11::arg("msg"));
	}
}
