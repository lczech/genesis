#include <fstream>
#include <genesis/utils/core/options.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/io/file_output_target.hpp>
#include <genesis/utils/io/gzip_block_ostream.hpp>
#include <genesis/utils/io/gzip_stream.hpp>
#include <genesis/utils/io/output_stream.hpp>
#include <genesis/utils/io/strict_fstream.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
#include <random>
#include <sstream> // __str__
#include <streambuf>
#include <string>
#include <vector>

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

void bind_genesis_utils_io_base_output_target(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::BaseOutputTarget file:genesis/utils/io/base_output_target.hpp line:60
		pybind11::class_<genesis::utils::BaseOutputTarget, std::shared_ptr<genesis::utils::BaseOutputTarget>> cl(M("genesis::utils"), "BaseOutputTarget", "Abstract base class for writing data to an output target.\n\n The class is an interface that allows writing to different targets, and adds a layer of abstraction\n around using simple `std::ostream` functionality. In particular, we want to add some checks,\n naming of the streams, etc. Internally however, the derived classes of this base class use\n `std::ostream`, and make it accessible.\n\n \n FileOutputTarget, GzipOutputTarget, StreamOutputTarget, StringOutputTarget for our derived\n output target classes.\n \n\n to_file(), to_gzip_block_file(), to_stream(), to_string() for helper functions to create\n these classes, to add some syntactic sugar and make it easy to use.");
		cl.def("assign", (class genesis::utils::BaseOutputTarget & (genesis::utils::BaseOutputTarget::*)(const class genesis::utils::BaseOutputTarget &)) &genesis::utils::BaseOutputTarget::operator=, "C++: genesis::utils::BaseOutputTarget::operator=(const class genesis::utils::BaseOutputTarget &) --> class genesis::utils::BaseOutputTarget &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("ostream", (std::ostream & (genesis::utils::BaseOutputTarget::*)()) &genesis::utils::BaseOutputTarget::ostream, "Get the underlying output stream that is used for writing.\n\nC++: genesis::utils::BaseOutputTarget::ostream() --> std::ostream &", pybind11::return_value_policy::reference_internal);
		cl.def("flush", (std::ostream & (genesis::utils::BaseOutputTarget::*)()) &genesis::utils::BaseOutputTarget::flush, "Flush output stream buffer.\n\n Internally, the different output target derived classes use some variant of `std::ostream`\n to do the writing. Hence, the target might need flushing in cases where we want to synchronize\n it while writing, before closing the stream.\n\nC++: genesis::utils::BaseOutputTarget::flush() --> std::ostream &", pybind11::return_value_policy::reference_internal);
		cl.def("target_name", (std::string (genesis::utils::BaseOutputTarget::*)() const) &genesis::utils::BaseOutputTarget::target_name, "Get a name of the output target. This is intended for user output.\n\n This will for example return something like \"output file (/path/to/file.txt)\", so that\n users know what type of output stream it is, and where it streams to.\n\nC++: genesis::utils::BaseOutputTarget::target_name() const --> std::string");
		cl.def("target_string", (std::string (genesis::utils::BaseOutputTarget::*)() const) &genesis::utils::BaseOutputTarget::target_string, "Get a string representing the output target.\n\n This is intended for the writer classes, which for example might want to examine the output\n file name. Hence, this function is meant to return just the file path (for a file target).\n\nC++: genesis::utils::BaseOutputTarget::target_string() const --> std::string");
	}
	{ // genesis::utils::Options file:genesis/utils/core/options.hpp line:54
		pybind11::class_<genesis::utils::Options, genesis::utils::Options*> cl(M("genesis::utils"), "Options", "Simple Options class for application-wide configuration and settings.");
		cl.def_static("get", (class genesis::utils::Options & (*)()) &genesis::utils::Options::get, "Returns a single instance of this class.\n\nC++: genesis::utils::Options::get() --> class genesis::utils::Options &", pybind11::return_value_policy::reference_internal);
		cl.def("command_line", (class std::vector<std::string, class std::allocator<std::string > > (genesis::utils::Options::*)() const) &genesis::utils::Options::command_line, "Returns an array of strings containing the program's command line arguments.\n\nC++: genesis::utils::Options::command_line() const --> class std::vector<std::string, class std::allocator<std::string > >");
		cl.def("command_line_string", (std::string (genesis::utils::Options::*)() const) &genesis::utils::Options::command_line_string, "Returns a string containing the program's command line arguments.\n\nC++: genesis::utils::Options::command_line_string() const --> std::string");
		cl.def("init_global_thread_pool", (void (genesis::utils::Options::*)()) &genesis::utils::Options::init_global_thread_pool, "Initialize the global thread pool to be used for parallel computations.\n\n This overload uses the result of guess_number_of_threads() to get the overall number of\n threads to use, and initializes the pool to use one fewer than that, to account for the\n main thread also doing work. As our ThreadPool implementation uses what we call a\n ProactiveFuture, the main thread will start processing tasks from the pool queue while\n it is waiting for results to get ready.\n\n After calling this function, global_thread_pool() can be used to obtain the global thread\n pool to enqueue work.\n\nC++: genesis::utils::Options::init_global_thread_pool() --> void");
		cl.def("init_global_thread_pool", (void (genesis::utils::Options::*)(unsigned long)) &genesis::utils::Options::init_global_thread_pool, "Initialize the global thread pool to be used for parallel computations.\n\n This initializes the pool to have exactly as many threads as provided by  here.\n Note that the main thread will also do work, so it is recommended to keep the \n at at least one fewer than the hardware concurrency (or other upper limit of threads, such as\n OpenMP or Slum limits, that you might want to keep). Use the overload init_global_thread_pool()\n without any arguments to do this automatically.\n\n If  is `0`, the thread pool is initialized with no threads at all, meaning that\n all enqueued work will instead be processed by the main thread once it wants to obtain the\n results of tasks. This essentially renders the thread pool into a lazy evaluating task queue.\n\n After calling this function, global_thread_pool() can be used to obtain the global thread\n pool to enqueue work.\n\nC++: genesis::utils::Options::init_global_thread_pool(unsigned long) --> void", pybind11::arg("num_threads"));
		cl.def("random_seed", (unsigned long (genesis::utils::Options::*)() const) &genesis::utils::Options::random_seed, "Returns the random seed that was used to initialize the engine.\n\n This is the global seed that is used as a basis for all thread-local seeding.\n\nC++: genesis::utils::Options::random_seed() const --> unsigned long");
		cl.def("random_seed", (void (genesis::utils::Options::*)(const unsigned long)) &genesis::utils::Options::random_seed, "Set a specific global seed for the random engine initialization.\n\n This sets the global seed for the thread-local random engines. Each engine\n is initialized using this seed plus a counter based on the number of spawned threads.\n On startup, the seed is initialized using the current system time. This value can\n be overwritten using this method. This should hence be done prior to spawning threads,\n for instance, before calling the init_global_thread_pool() functions.\n\nC++: genesis::utils::Options::random_seed(const unsigned long) --> void", pybind11::arg("seed"));
		cl.def("random_engine", (class std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647> & (genesis::utils::Options::*)()) &genesis::utils::Options::random_engine, "Returns a thread-local engine for random number generation.\n\n Caveat: The engines are thread-local, and hence thread-safe. However, generally, in a\n multi-threaded setting, the order of execution in not deterministic, and hence results might\n not be reproducible, even when using a fixed seed.\n\nC++: genesis::utils::Options::random_engine() --> class std::linear_congruential_engine<unsigned long, 16807, 0, 2147483647> &", pybind11::return_value_policy::reference_internal);
		cl.def("seed_counter", (unsigned long (genesis::utils::Options::*)() const) &genesis::utils::Options::seed_counter, "Return the number of seeds used to initialize thread-local random engines.\n\n This corresponds to the number of threads that have called random_engine().\n\nC++: genesis::utils::Options::seed_counter() const --> unsigned long");
		cl.def("allow_file_overwriting", (bool (genesis::utils::Options::*)() const) &genesis::utils::Options::allow_file_overwriting, "Get whether Genesis is allowed to overwrite files when outputting data.\n\n See allow_file_overwriting( bool ) for details.\n\nC++: genesis::utils::Options::allow_file_overwriting() const --> bool");
		cl.def("allow_file_overwriting", (void (genesis::utils::Options::*)(bool)) &genesis::utils::Options::allow_file_overwriting, "Set whether Genesis is allowed to overwrite files when outputting data.\n\n The option is `false` by default, which causes Genesis to throw an execption when it attempts\n to write to a file that is already existing.\n\n By setting this option to `true`, files are silently overwritten in case they already\n exist. This has to be activated explicitly in order to avoid losing files by accident.\n\n \n \n\n\nC++: genesis::utils::Options::allow_file_overwriting(bool) --> void", pybind11::arg("value"));
		cl.def("print_object_infos", (void (genesis::utils::Options::*)(bool)) &genesis::utils::Options::print_object_infos, "Set whether an object info one-liner is printed when using the `operator <<` that\n is defined for many classes.\n\n NOTE: So far, this is only implemented for the \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::utils::Options::print_object_infos(bool) --> void", pybind11::arg("value"));
		cl.def("print_object_infos", (bool (genesis::utils::Options::*)() const) &genesis::utils::Options::print_object_infos, "Get whether an object info one-liner is printed when using the `operator <<`.\n\n See print_object_infos( bool ) for details.\n\nC++: genesis::utils::Options::print_object_infos() const --> bool");
		cl.def("print_object_gists", (void (genesis::utils::Options::*)(long)) &genesis::utils::Options::print_object_gists, "Set whether an object gist is printed when using the `operator <<` that is defined\n for many (container) classes.\n\n NOTE: So far, this is only implemented for the \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::utils::Options::print_object_gists(long) --> void", pybind11::arg("value"));
		cl.def("print_object_gists", (long (genesis::utils::Options::*)() const) &genesis::utils::Options::print_object_gists, "Get whether an object gist is printed when using the `operator <<`.\n\n See print_object_gists( long ) for details.\n\nC++: genesis::utils::Options::print_object_gists() const --> long");
		cl.def("info", (std::string (genesis::utils::Options::*)() const) &genesis::utils::Options::info, "Return a list with compile time and run time options with their values.\n\nC++: genesis::utils::Options::info() const --> std::string");
		cl.def("info_compile_time", (std::string (genesis::utils::Options::*)() const) &genesis::utils::Options::info_compile_time, "Return a list of compile time options.\n\nC++: genesis::utils::Options::info_compile_time() const --> std::string");
		cl.def("info_run_time", (std::string (genesis::utils::Options::*)() const) &genesis::utils::Options::info_run_time, "Return a list of run time options.\n\nC++: genesis::utils::Options::info_run_time() const --> std::string");
	}
	// genesis::utils::file_output_stream(const std::string &, class std::basic_ofstream<char> &, enum std::_Ios_Openmode, bool) file:genesis/utils/io/output_stream.hpp line:72
	M("genesis::utils").def("file_output_stream", [](const std::string & a0, class std::basic_ofstream<char> & a1) -> void { return genesis::utils::file_output_stream(a0, a1); }, "", pybind11::arg("file_name"), pybind11::arg("out_stream"));
	M("genesis::utils").def("file_output_stream", [](const std::string & a0, class std::basic_ofstream<char> & a1, enum std::_Ios_Openmode const & a2) -> void { return genesis::utils::file_output_stream(a0, a1, a2); }, "", pybind11::arg("file_name"), pybind11::arg("out_stream"), pybind11::arg("mode"));
	M("genesis::utils").def("file_output_stream", (void (*)(const std::string &, class std::basic_ofstream<char> &, enum std::_Ios_Openmode, bool)) &genesis::utils::file_output_stream, "Helper function to obtain an output stream to a file.\n\n This function is used internally by all file writers. It makes sure that the file is writable,\n and throws an exception otherwise.\n\n Furthermore, the function checks whether the path already exists. If it exists and\n \n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::utils::file_output_stream(const std::string &, class std::basic_ofstream<char> &, enum std::_Ios_Openmode, bool) --> void", pybind11::arg("file_name"), pybind11::arg("out_stream"), pybind11::arg("mode"), pybind11::arg("create_dirs"));

	{ // genesis::utils::FileOutputTarget file:genesis/utils/io/file_output_target.hpp line:53
		pybind11::class_<genesis::utils::FileOutputTarget, std::shared_ptr<genesis::utils::FileOutputTarget>, genesis::utils::BaseOutputTarget> cl(M("genesis::utils"), "FileOutputTarget", "Output target for writing data to a file.\n\n The output file name is provided via the constructor.");
		cl.def( pybind11::init( [](const std::string & a0){ return new genesis::utils::FileOutputTarget(a0); } ), "doc" , pybind11::arg("file_name"));
		cl.def( pybind11::init<const std::string &, enum std::_Ios_Openmode>(), pybind11::arg("file_name"), pybind11::arg("mode") );

	}
	{ // genesis::utils::StrictFStream file:genesis/utils/io/strict_fstream.hpp line:171
		pybind11::class_<genesis::utils::StrictFStream, std::shared_ptr<genesis::utils::StrictFStream>, std::basic_fstream<char,std::char_traits<char>>> cl(M("genesis::utils"), "StrictFStream", "Stream that defines a strict wrapper around std::fstream\n\n The wrapper performs the following steps:\n\n  - Check that the open modes make sense\n  - Check that the call to open() is successful\n  - Check that the opened file is peek-able\n  - Turn on the badbit in the exception mask\n\n This is useful to ensure proper file handling.\n\n The class is based on the strict_fstream::fstream class of the excellent\n [zstr library](https://github.com/mateidavid/zstr) by Matei David; see also our\n \n\n\n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::utils::StrictFStream(); } ) );
		cl.def( pybind11::init( [](const std::string & a0){ return new genesis::utils::StrictFStream(a0); } ), "doc" , pybind11::arg("filename"));
		cl.def( pybind11::init<const std::string &, enum std::_Ios_Openmode>(), pybind11::arg("filename"), pybind11::arg("mode") );

		cl.def("open", [](genesis::utils::StrictFStream &o, const std::string & a0) -> void { return o.open(a0); }, "", pybind11::arg("filename"));
		cl.def("open", (void (genesis::utils::StrictFStream::*)(const std::string &, enum std::_Ios_Openmode)) &genesis::utils::StrictFStream::open, "C++: genesis::utils::StrictFStream::open(const std::string &, enum std::_Ios_Openmode) --> void", pybind11::arg("filename"), pybind11::arg("mode"));
	}
	// genesis::utils::GzipCompressionLevel file:genesis/utils/io/gzip_stream.hpp line:100
	pybind11::enum_<genesis::utils::GzipCompressionLevel>(M("genesis::utils"), "GzipCompressionLevel", "List of possible compression levels used for GzipOStream.\n\n The compression levels are handed over to zlib for compression, which currently allows all values\n between 1 (best speed) and 9 (best compression), with the special case 0 (no compression), as\n well as -1 for the default compression. Currently, the zlib default compression level corresponds\n to level 6, as this is a good compromise between speed and compression\n (it forms the \"elbow\" of the curve), hence we also use this as our default level.\n\n The enum only lists those four special levels. However, we use a fixed enum here (with the\n underlying type `int`), meaning that all values in between 1 and 9 are also allowed to be used.\n Values outside of the range [-1, 9] will lead to an exception being thrown when used in GzipOStream.\n\n \n GzipOStream\n \n\n GzipOFStream")
		.value("kDefaultCompression", genesis::utils::GzipCompressionLevel::kDefaultCompression)
		.value("kNoCompression", genesis::utils::GzipCompressionLevel::kNoCompression)
		.value("kBestSpeed", genesis::utils::GzipCompressionLevel::kBestSpeed)
		.value("kBestCompression", genesis::utils::GzipCompressionLevel::kBestCompression);

;

	{ // genesis::utils::GzipBlockOStream file:genesis/utils/io/gzip_block_ostream.hpp line:89
		pybind11::class_<genesis::utils::GzipBlockOStream, std::shared_ptr<genesis::utils::GzipBlockOStream>, std::ostream> cl(M("genesis::utils"), "GzipBlockOStream", "Output stream that writes blocks of gzip-compressed data to an underlying wrapped stream,\n using parallel compression.\n\n The gzip format specifies that concatenated blocks of gzip-compressed data (including the gzip\n header) are still valid gzip files, and are equivalent to concatenating the decompressed data.\n This is for example used in compressed vcf files (.vcf.gz, Variant Calling Format) to achieve\n random access into compressed data, by maintaining an index table of offsets to the beginning\n of individual compressed blocks.\n\n We here use a similar technique to achieve a compression speedup by using parallel threads\n on different gzip blocks. This gives almost linear speedup, at the cost of ~3% increase in\n resulting file size due to the additional gzip headers of each block. This downside can be\n alleivated by using larger blocks though. By default, we use 64kB blocks.\n\n Exemplary usage:\n\n     // Wrapped output stream to write to. Use binary mode, so that compressed output works.\n     std::ofstream ofile;\n     ofile.open( \"path/to/test.txt.gz\", std::ios_base::binary );\n\n     // Prepare stream\n     GzipBlockOStream ostr( ofile );\n\n     // Write data to stream\n     ostr << \"some data\";\n\n By default, the global thread pool of Options::get().global_thread_pool() is used for compressing\n gzip blocks in parallel. An alternative pool can be provided instead if needed.\n\n Furthermore, note that some file managers might not display the original (uncompressed) file size\n correctly when viewing the resulting gz file, as they might use only the size of one block\n instead of the full resulting uncompressed file size. This should not affect decompression or any\n other downstream processes though. As this class is a stream, we usually do not know beforehand\n how lare the resulting file will be, so there is not much we can do about this.\n\n The class could also be extended in the future to achieve indexing similar to compressed vcf.\n NB: We have not yet tested compatibility with the vcf format, as they might employ additional\n tricks to achieve their goals.");
	}
}
