#include <genesis/util/core/fs.hpp>
#include <genesis/util/core/options.hpp>
#include <genesis/util/threading/thread_pool.hpp>
#include <iterator>
#include <memory>
#include <random>
#include <sstream> // __str__
#include <string>
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

void bind_genesis_util_core_options(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::core::Options file:genesis/util/core/options.hpp line:58
        pybind11::class_<genesis::util::core::Options, genesis::util::core::Options*> cl(
            M( "genesis::util::core" ),
            "Options",
            "Simple Options class for application-wide configuration and settings."
        );

        pybind11::enum_<genesis::util::core::Options::InputReadingThreadPolicy>(
            cl, "InputReadingThreadPolicy", "Decide how to use threads for input reading."
        )
            .value( "kStrict", genesis::util::core::Options::InputReadingThreadPolicy::kStrict )
            .value(
                "kTrivialAsync",
                genesis::util::core::Options::InputReadingThreadPolicy::kTrivialAsync
            )
            .value(
                "kAllAsync", genesis::util::core::Options::InputReadingThreadPolicy::kAllAsync
            );

        // function-signature: class genesis::util::core::Options &
        // genesis::util::core::Options::get()() file:genesis/util/core/options.hpp line:69
        cl.def_static(
            "get",
            ( class genesis::util::core::Options & (*)() ) & genesis::util::core::Options::get,
            "Returns a single instance of this class.\n\nC++: genesis::util::core::Options::get() "
            "--> class genesis::util::core::Options &",
            pybind11::return_value_policy::reference
        );
        // function-signature: unsigned long genesis::util::core::Options::random_seed() const()
        // file:genesis/util/core/options.hpp line:125
        cl.def(
            "random_seed",
            ( unsigned long ( genesis::util::core::Options::* )() const ) &
                genesis::util::core::Options::random_seed,
            "Returns the random seed that was used to initialize the engine.\n\n This is the "
            "global seed that is used as a basis for all thread-local seeding.\n\nC++: "
            "genesis::util::core::Options::random_seed() const --> unsigned long"
        );
        // function-signature: void genesis::util::core::Options::random_seed(const unsigned
        // long)(const unsigned long) file:genesis/util/core/options.hpp line:148
        cl.def(
            "random_seed",
            ( void( genesis::util::core::Options::* )( const unsigned long ) ) &
                genesis::util::core::Options::random_seed,
            "Set a specific global seed for the random engine initialization.\n\n This (re)sets "
            "the global seed for the thread-local random engines. The engine of the thread\n where "
            "this function is called from, as well as any thread spawned after, is reseeded "
            "using\n this seed plus a counter based on the number of spawned threads.\n\n On "
            "startup, the seed is initialized using the current system time, and that exact seed "
            "is\n used for the main thead. The seed can be reset using this function, but this "
            "will only affect\n the thread where the function is called from, as well as any "
            "threads spawned later. If a\n fixed seed is needed, this function hence needs to be "
            "called prior to spawning threads,\n and in particlar, before calling the "
            "init_global_thread_pool() functions. Otherwise,\n this function throws an exception, "
            "in order to avoid potential bugs that would be hard\n to track down.\n\n In a single "
            "threaded environment, this behaves identical to the usual way of (re)seeding\n a "
            "random engine.\n\nC++: genesis::util::core::Options::random_seed(const unsigned long) "
            "--> void",
            pybind11::arg( "seed" )
        );
        // function-signature: unsigned long genesis::util::core::Options::seed_counter() const()
        // file:genesis/util/core/options.hpp line:179
        cl.def(
            "seed_counter",
            ( unsigned long ( genesis::util::core::Options::* )() const ) &
                genesis::util::core::Options::seed_counter,
            "Return the number of seeds used to initialize thread-local random engines.\n\n This "
            "corresponds to the number of threads that have called random_engine().\n\nC++: "
            "genesis::util::core::Options::seed_counter() const --> unsigned long"
        );
        // function-signature: void genesis::util::core::Options::init_global_thread_pool()()
        // file:genesis/util/core/options.hpp line:218
        cl.def(
            "init_global_thread_pool",
            ( void( genesis::util::core::Options::* )() ) &
                genesis::util::core::Options::init_global_thread_pool,
            "Initialize the global thread pool to be used for parallel computations.\n\n This "
            "overload uses the result of guess_number_of_threads() to get the overall number of\n "
            "threads to use, and initializes the pool to use one fewer than that, to account for "
            "the\n main thread also doing work. As our ThreadPool implementation uses what we call "
            "a\n ProactiveFuture, the main thread will start processing tasks from the pool queue "
            "while\n it is waiting for results to get ready. The function also sets the maximum "
            "queue size of\n tasks in the thread pool to 10 times the number of threads, to avoid "
            "infinitely filling\n the task queue when the main thread keeps submitting.\n\n After "
            "calling this function, global_thread_pool() can be used to obtain the global thread\n "
            "pool to enqueue work.\n\nC++: genesis::util::core::Options::init_global_thread_pool() "
            "--> void"
        );
        // function-signature: void genesis::util::core::Options::init_global_thread_pool(unsigned
        // long, unsigned long)(unsigned long, unsigned long) file:genesis/util/core/options.hpp
        // line:245
        cl.def(
            "init_global_thread_pool",
            []( genesis::util::core::Options& o, unsigned long const& a0 ) -> void {
                return o.init_global_thread_pool( a0 );
            },
            "",
            pybind11::arg( "num_threads" )
        );
        cl.def(
            "init_global_thread_pool",
            ( void( genesis::util::core::Options::* )( unsigned long, unsigned long ) ) &
                genesis::util::core::Options::init_global_thread_pool,
            "Initialize the global thread pool to be used for parallel computations.\n\n This "
            "initializes the pool to have one fewer thread as provided by  here.\n Note that the "
            "main thread will also do work, so this is recommended, and keeps\n the  at at least "
            "one fewer than the hardware concurrency (or other upper\n limit of threads, such as "
            "from OpenMP or Slum, that you might want to keep). Use the\n overload "
            "init_global_thread_pool() without any arguments, or call this function with value\n "
            "`0` to determine the number of threads automatically via "
            "guess_number_of_threads().\n\n If  is `1`, we take this to mean that we only want to "
            "use the single main\n thread, and hence the thread pool is initialized with no "
            "threads at all, meaning that\n all enqueued work will instead be processed by the "
            "main thread once it wants to obtain the\n results of tasks. This essentially renders "
            "the thread pool into a lazy evaluating task queue.\n\n After calling this function, "
            "global_thread_pool() can be used to obtain the global thread\n pool to enqueue "
            "work.\n\nC++: genesis::util::core::Options::init_global_thread_pool(unsigned long, "
            "unsigned long) --> void",
            pybind11::arg( "num_threads" ),
            pybind11::arg( "max_queue_size" )
        );
        // function-signature: bool genesis::util::core::Options::has_global_thread_pool() const()
        // file:genesis/util/core/options.hpp line:289
        cl.def(
            "has_global_thread_pool",
            ( bool( genesis::util::core::Options::* )() const ) &
                genesis::util::core::Options::has_global_thread_pool,
            "Return if a global thread pool has been initialized, i.e., if "
            "init_global_thread_pool()\n has already been called.\n\nC++: "
            "genesis::util::core::Options::has_global_thread_pool() const --> bool"
        );
        // function-signature: unsigned long genesis::util::core::Options::global_thread_pool_size()
        // const() file:genesis/util/core/options.hpp line:299
        cl.def(
            "global_thread_pool_size",
            ( unsigned long ( genesis::util::core::Options::* )() const ) &
                genesis::util::core::Options::global_thread_pool_size,
            "Get the number of threads allocatd in the pool, plus one for the main thread.\n\n "
            "This is because with our ProactiveFuture implementaiton, the main thread does work as "
            "well.\n\nC++: genesis::util::core::Options::global_thread_pool_size() const --> "
            "unsigned long"
        );
        // function-signature: void genesis::util::core::Options::input_reading_thread_policy(enum
        // genesis::util::core::Options::InputReadingThreadPolicy)(enum
        // genesis::util::core::Options::InputReadingThreadPolicy)
        // file:genesis/util/core/options.hpp line:352
        cl.def(
            "input_reading_thread_policy",
            ( void( genesis::util::core::Options::* )(
                enum genesis::util::core::Options::InputReadingThreadPolicy
            ) ) &
                genesis::util::core::Options::input_reading_thread_policy,
            "Set the policy for the threading of input source reading.\n\nC++: "
            "genesis::util::core::Options::input_reading_thread_policy(enum "
            "genesis::util::core::Options::InputReadingThreadPolicy) --> void",
            pybind11::arg( "policy" )
        );
        // function-signature: enum genesis::util::core::Options::InputReadingThreadPolicy
        // genesis::util::core::Options::input_reading_thread_policy()()
        // file:genesis/util/core/options.hpp line:360
        cl.def(
            "input_reading_thread_policy",
            ( enum genesis::util::core::Options::
                  InputReadingThreadPolicy( genesis::util::core::Options::* )() ) &
                genesis::util::core::Options::input_reading_thread_policy,
            "Get the policy for the threading of input source reading.\n\nC++: "
            "genesis::util::core::Options::input_reading_thread_policy() --> enum "
            "genesis::util::core::Options::InputReadingThreadPolicy"
        );
        // function-signature: bool genesis::util::core::Options::allow_file_overwriting() const()
        // file:genesis/util/core/options.hpp line:374
        cl.def(
            "allow_file_overwriting",
            ( bool( genesis::util::core::Options::* )() const ) &
                genesis::util::core::Options::allow_file_overwriting,
            "Get whether Genesis is allowed to overwrite files when outputting data.\n\n See "
            "allow_file_overwriting( bool ) for details.\n\nC++: "
            "genesis::util::core::Options::allow_file_overwriting() const --> bool"
        );
        // function-signature: void genesis::util::core::Options::allow_file_overwriting(bool)(bool)
        // file:genesis/util/core/options.hpp line:392
        cl.def(
            "allow_file_overwriting",
            ( void( genesis::util::core::Options::* )( bool ) ) &
                genesis::util::core::Options::allow_file_overwriting,
            "Set whether Genesis is allowed to overwrite files when outputting data.\n\n The "
            "option is `false` by default, which causes Genesis to throw an execption when it "
            "attempts\n to write to a file that is already existing, as long as only the internal "
            "file writing\n functions are used, such as the to_file() function to get a target for "
            "the writer classes.\n We of course cannot control any independent file "
            "operations.\n\n By setting this option to `true`, files are silently overwritten in "
            "case they already\n exist. This has to be activated explicitly in order to avoid "
            "losing files by accident.\n\n \n \n\n\nC++: "
            "genesis::util::core::Options::allow_file_overwriting(bool) --> void",
            pybind11::arg( "value" )
        );
    }
    // genesis::util::core::path_exists(const std::string &) file:genesis/util/core/fs.hpp line:49
    // function-signature: bool genesis::util::core::path_exists(const std::string &)(const
    // std::string &) file:genesis/util/core/fs.hpp line:49
    M( "genesis::util::core" )
        .def(
            "path_exists",
            ( bool ( * )( const std::string& ) ) & genesis::util::core::path_exists,
            "Return whether a path exists, i.e., is a file or directory.\n\nC++: "
            "genesis::util::core::path_exists(const std::string &) --> bool",
            pybind11::arg( "path" )
        );

    // genesis::util::core::is_file(const std::string &) file:genesis/util/core/fs.hpp line:58
    // function-signature: bool genesis::util::core::is_file(const std::string &)(const std::string
    // &) file:genesis/util/core/fs.hpp line:58
    M( "genesis::util::core" )
        .def(
            "is_file",
            ( bool ( * )( const std::string& ) ) & genesis::util::core::is_file,
            "Return true iff the provided path is a (readable) file.\n\n Internally, this function "
            "simply returns the value of file_is_readable(), meaning that we also\n check that the "
            "file can actually be read. That is semantically a bit different from just stating\n "
            "that it is a file... But file system stuff in C++ pre-17 is hard, and this works for "
            "now.\n\nC++: genesis::util::core::is_file(const std::string &) --> bool",
            pybind11::arg( "path" )
        );

    // genesis::util::core::file_exists(const std::string &) file:genesis/util/core/fs.hpp line:65
    // function-signature: bool genesis::util::core::file_exists(const std::string &)(const
    // std::string &) file:genesis/util/core/fs.hpp line:65
    M( "genesis::util::core" )
        .def(
            "file_exists",
            ( bool ( * )( const std::string& ) ) & genesis::util::core::file_exists,
            "Return true iff the file exists (and is in fact a file, and not, e.g., a "
            "directory).\n\n  std::string const& )\n\nC++: genesis::util::core::file_exists(const "
            "std::string &) --> bool",
            pybind11::arg( "filename" )
        );

    // genesis::util::core::file_is_readable(const std::string &) file:genesis/util/core/fs.hpp
    // line:76 function-signature: bool genesis::util::core::file_is_readable(const std::string
    // &)(const std::string &) file:genesis/util/core/fs.hpp line:76
    M( "genesis::util::core" )
        .def(
            "file_is_readable",
            ( bool ( * )( const std::string& ) ) & genesis::util::core::file_is_readable,
            "Return whether a file is readable.\n\n For this, the file has to exist, and be "
            "accessible.\n Another potential error is that too many files are opened already.\n\n "
            "See file_is_readable( std::string const&, std::string& ) for a version of the "
            "function that also\n allows to retrieve the error message in cases where the result "
            "is `false`.\n\nC++: genesis::util::core::file_is_readable(const std::string &) --> "
            "bool",
            pybind11::arg( "filename" )
        );

    // genesis::util::core::file_is_readable(const std::string &, std::string &)
    // file:genesis/util/core/fs.hpp line:84 function-signature: bool
    // genesis::util::core::file_is_readable(const std::string &, std::string &)(const std::string
    // &, std::string &) file:genesis/util/core/fs.hpp line:84
    M( "genesis::util::core" )
        .def(
            "file_is_readable",
            ( bool ( * )( const std::string&, std::string& ) ) &
                genesis::util::core::file_is_readable,
            "Return whether a file is readable, and potentially store the error message.\n\n For "
            "this, the file has to exist, and be accessible.\n Another potential error is that too "
            "many files are opened already.\n\nC++: genesis::util::core::file_is_readable(const "
            "std::string &, std::string &) --> bool",
            pybind11::arg( "filename" ),
            pybind11::arg( "err_str" )
        );

    // genesis::util::core::file_read(const std::string &, bool) file:genesis/util/core/fs.hpp
    // line:94 function-signature: std::string genesis::util::core::file_read(const std::string &,
    // bool)(const std::string &, bool) file:genesis/util/core/fs.hpp line:94
    M( "genesis::util::core" )
        .def(
            "file_read",
            []( const std::string& a0 ) -> std::string {
                return genesis::util::core::file_read( a0 );
            },
            "",
            pybind11::arg( "filename" )
        );
    M( "genesis::util::core" )
        .def(
            "file_read",
            ( std::string( * )( const std::string&, bool ) ) & genesis::util::core::file_read,
            "Return the contents of a file as a string.\n\n If the parameter  is `true` (default), "
            "it is first determined whether the\n file is gzip compressed, and if so, the file is "
            "decompressed when reading.\n\n If the file is not readable, the function throws "
            "`std::runtime_error`.\n\nC++: genesis::util::core::file_read(const std::string &, "
            "bool) --> std::string",
            pybind11::arg( "filename" ),
            pybind11::arg( "detect_compression" )
        );

    // genesis::util::core::file_read_lines(const std::string &, bool) file:genesis/util/core/fs.hpp
    // line:107 function-signature: class std::vector<std::string >
    // genesis::util::core::file_read_lines(const std::string &, bool)(const std::string &, bool)
    // file:genesis/util/core/fs.hpp line:107
    M( "genesis::util::core" )
        .def(
            "file_read_lines",
            []( const std::string& a0 ) -> std::vector<std::string > {
                return genesis::util::core::file_read_lines( a0 );
            },
            "",
            pybind11::arg( "filename" )
        );
    M( "genesis::util::core" )
        .def(
            "file_read_lines",
            ( class std::vector<std::string >( * )( const std::string&, bool ) ) &
                genesis::util::core::file_read_lines,
            "Return the contents of a file as a vector of strings, one entry for each line.\n\n If "
            "the parameter  is `true` (default), it is first determined whether the\n file is gzip "
            "compressed, and if so, the file is decompressed when reading.\n\n If the file is not "
            "readable, the function throws `std::runtime_error`.\n\nC++: "
            "genesis::util::core::file_read_lines(const std::string &, bool) --> class "
            "std::vector<std::string >",
            pybind11::arg( "filename" ),
            pybind11::arg( "detect_compression" )
        );

    // genesis::util::core::file_write(const std::string &, const std::string &, bool)
    // file:genesis/util/core/fs.hpp line:120 function-signature: void
    // genesis::util::core::file_write(const std::string &, const std::string &, bool)(const
    // std::string &, const std::string &, bool) file:genesis/util/core/fs.hpp line:120
    M( "genesis::util::core" )
        .def(
            "file_write",
            []( const std::string& a0, const std::string& a1 ) -> void {
                return genesis::util::core::file_write( a0, a1 );
            },
            "",
            pybind11::arg( "content" ),
            pybind11::arg( "filename" )
        );
    M( "genesis::util::core" )
        .def(
            "file_write",
            ( void ( * )( const std::string&, const std::string&, bool ) ) &
                genesis::util::core::file_write,
            "Write the content of a string to a file.\n\n If the file cannot be written to, the "
            "function throws an exception. Also, by default, if the file\n already exists, an "
            "exception is thrown.\n See \n\n\n\n\nC++: genesis::util::core::file_write(const "
            "std::string &, const std::string &, bool) --> void",
            pybind11::arg( "content" ),
            pybind11::arg( "filename" ),
            pybind11::arg( "create_dirs" )
        );
}
