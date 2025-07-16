#include <custom/utils/extra_bindings.hpp>
#include <genesis/utils/core/options.hpp>
#include <genesis/utils/threading/thread_pool.hpp>
#include <iterator>
#include <memory>
#include <random>
#include <sstream> // __str__
#include <string>
#include <vector>

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

void bind_genesis_utils_core_options(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::Options file:genesis/utils/core/options.hpp line:57
		pybind11::class_<genesis::utils::Options, genesis::utils::Options*> cl(M("genesis::utils"), "Options", "Simple Options class for application-wide configuration and settings.");

		pybind11::enum_<genesis::utils::Options::InputReadingThreadPolicy>(cl, "InputReadingThreadPolicy", "Decide how to use threads for input reading.")
			.value("kStrict", genesis::utils::Options::InputReadingThreadPolicy::kStrict)
			.value("kTrivialAsync", genesis::utils::Options::InputReadingThreadPolicy::kTrivialAsync)
			.value("kAllAsync", genesis::utils::Options::InputReadingThreadPolicy::kAllAsync);

		// function-signature: class genesis::utils::Options & genesis::utils::Options::get()() file:genesis/utils/core/options.hpp line:68
		cl.def_static("get", (class genesis::utils::Options & (*)()) &genesis::utils::Options::get, "Returns a single instance of this class.\n\nC++: genesis::utils::Options::get() --> class genesis::utils::Options &", pybind11::return_value_policy::reference);
		// function-signature: unsigned long genesis::utils::Options::random_seed() const() file:genesis/utils/core/options.hpp line:124
		cl.def("random_seed", (unsigned long (genesis::utils::Options::*)() const) &genesis::utils::Options::random_seed, "Returns the random seed that was used to initialize the engine.\n\n This is the global seed that is used as a basis for all thread-local seeding.\n\nC++: genesis::utils::Options::random_seed() const --> unsigned long");
		// function-signature: void genesis::utils::Options::random_seed(const unsigned long)(const unsigned long) file:genesis/utils/core/options.hpp line:147
		cl.def("random_seed", (void (genesis::utils::Options::*)(const unsigned long)) &genesis::utils::Options::random_seed, "Set a specific global seed for the random engine initialization.\n\n This (re)sets the global seed for the thread-local random engines. The engine of the thread\n where this function is called from, as well as any thread spawned after, is reseeded using\n this seed plus a counter based on the number of spawned threads.\n\n On startup, the seed is initialized using the current system time, and that exact seed is\n used for the main thead. The seed can be reset using this function, but this will only affect\n the thread where the function is called from, as well as any threads spawned later. If a\n fixed seed is needed, this function hence needs to be called prior to spawning threads,\n and in particlar, before calling the init_global_thread_pool() functions. Otherwise,\n this function throws an exception, in order to avoid potential bugs that would be hard\n to track down.\n\n In a single threaded environment, this behaves identical to the usual way of (re)seeding\n a random engine.\n\nC++: genesis::utils::Options::random_seed(const unsigned long) --> void", pybind11::arg("seed"));
		// function-signature: unsigned long genesis::utils::Options::seed_counter() const() file:genesis/utils/core/options.hpp line:178
		cl.def("seed_counter", (unsigned long (genesis::utils::Options::*)() const) &genesis::utils::Options::seed_counter, "Return the number of seeds used to initialize thread-local random engines.\n\n This corresponds to the number of threads that have called random_engine().\n\nC++: genesis::utils::Options::seed_counter() const --> unsigned long");
		// function-signature: void genesis::utils::Options::init_global_thread_pool()() file:genesis/utils/core/options.hpp line:217
		cl.def("init_global_thread_pool", (void (genesis::utils::Options::*)()) &genesis::utils::Options::init_global_thread_pool, "Initialize the global thread pool to be used for parallel computations.\n\n This overload uses the result of guess_number_of_threads() to get the overall number of\n threads to use, and initializes the pool to use one fewer than that, to account for the\n main thread also doing work. As our ThreadPool implementation uses what we call a\n ProactiveFuture, the main thread will start processing tasks from the pool queue while\n it is waiting for results to get ready. The function also sets the maximum queue size of\n tasks in the thread pool to 10 times the number of threads, to avoid infinitely filling\n the task queue when the main thread keeps submitting.\n\n After calling this function, global_thread_pool() can be used to obtain the global thread\n pool to enqueue work.\n\nC++: genesis::utils::Options::init_global_thread_pool() --> void");
		// function-signature: void genesis::utils::Options::init_global_thread_pool(unsigned long, unsigned long)(unsigned long, unsigned long) file:genesis/utils/core/options.hpp line:244
		cl.def("init_global_thread_pool", [](genesis::utils::Options &o, unsigned long const & a0) -> void { return o.init_global_thread_pool(a0); }, "", pybind11::arg("num_threads"));
		cl.def("init_global_thread_pool", (void (genesis::utils::Options::*)(unsigned long, unsigned long)) &genesis::utils::Options::init_global_thread_pool, "Initialize the global thread pool to be used for parallel computations.\n\n This initializes the pool to have one fewer thread as provided by  here.\n Note that the main thread will also do work, so this is recommended, and keeps\n the  at at least one fewer than the hardware concurrency (or other upper\n limit of threads, such as from OpenMP or Slum, that you might want to keep). Use the\n overload init_global_thread_pool() without any arguments, or call this function with value\n `0` to determine the number of threads automatically via guess_number_of_threads().\n\n If  is `1`, we take this to mean that we only want to use the single main\n thread, and hence the thread pool is initialized with no threads at all, meaning that\n all enqueued work will instead be processed by the main thread once it wants to obtain the\n results of tasks. This essentially renders the thread pool into a lazy evaluating task queue.\n\n After calling this function, global_thread_pool() can be used to obtain the global thread\n pool to enqueue work.\n\nC++: genesis::utils::Options::init_global_thread_pool(unsigned long, unsigned long) --> void", pybind11::arg("num_threads"), pybind11::arg("max_queue_size"));
		// function-signature: bool genesis::utils::Options::has_global_thread_pool() const() file:genesis/utils/core/options.hpp line:288
		cl.def("has_global_thread_pool", (bool (genesis::utils::Options::*)() const) &genesis::utils::Options::has_global_thread_pool, "Return if a global thread pool has been initialized, i.e., if init_global_thread_pool()\n has already been called.\n\nC++: genesis::utils::Options::has_global_thread_pool() const --> bool");
		// function-signature: unsigned long genesis::utils::Options::global_thread_pool_size() const() file:genesis/utils/core/options.hpp line:298
		cl.def("global_thread_pool_size", (unsigned long (genesis::utils::Options::*)() const) &genesis::utils::Options::global_thread_pool_size, "Get the number of threads allocatd in the pool, plus one for the main thread.\n\n This is because with our ProactiveFuture implementaiton, the main thread does work as well.\n\nC++: genesis::utils::Options::global_thread_pool_size() const --> unsigned long");
		// function-signature: void genesis::utils::Options::input_reading_thread_policy(enum genesis::utils::Options::InputReadingThreadPolicy)(enum genesis::utils::Options::InputReadingThreadPolicy) file:genesis/utils/core/options.hpp line:351
		cl.def("input_reading_thread_policy", (void (genesis::utils::Options::*)(enum genesis::utils::Options::InputReadingThreadPolicy)) &genesis::utils::Options::input_reading_thread_policy, "Set the policy for the threading of input source reading.\n\nC++: genesis::utils::Options::input_reading_thread_policy(enum genesis::utils::Options::InputReadingThreadPolicy) --> void", pybind11::arg("policy"));
		// function-signature: enum genesis::utils::Options::InputReadingThreadPolicy genesis::utils::Options::input_reading_thread_policy()() file:genesis/utils/core/options.hpp line:359
		cl.def("input_reading_thread_policy", (enum genesis::utils::Options::InputReadingThreadPolicy (genesis::utils::Options::*)()) &genesis::utils::Options::input_reading_thread_policy, "Get the policy for the threading of input source reading.\n\nC++: genesis::utils::Options::input_reading_thread_policy() --> enum genesis::utils::Options::InputReadingThreadPolicy");
		// function-signature: bool genesis::utils::Options::allow_file_overwriting() const() file:genesis/utils/core/options.hpp line:373
		cl.def("allow_file_overwriting", (bool (genesis::utils::Options::*)() const) &genesis::utils::Options::allow_file_overwriting, "Get whether Genesis is allowed to overwrite files when outputting data.\n\n See allow_file_overwriting( bool ) for details.\n\nC++: genesis::utils::Options::allow_file_overwriting() const --> bool");
		// function-signature: void genesis::utils::Options::allow_file_overwriting(bool)(bool) file:genesis/utils/core/options.hpp line:391
		cl.def("allow_file_overwriting", (void (genesis::utils::Options::*)(bool)) &genesis::utils::Options::allow_file_overwriting, "Set whether Genesis is allowed to overwrite files when outputting data.\n\n The option is `false` by default, which causes Genesis to throw an execption when it attempts\n to write to a file that is already existing, as long as only the internal file writing\n functions are used, such as the to_file() function to get a target for the writer classes.\n We of course cannot control any independent file operations.\n\n By setting this option to `true`, files are silently overwritten in case they already\n exist. This has to be activated explicitly in order to avoid losing files by accident.\n\n \n \n\n\nC++: genesis::utils::Options::allow_file_overwriting(bool) --> void", pybind11::arg("value"));
	}
}
