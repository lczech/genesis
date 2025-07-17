#include <functional>
#include <genesis/utils/core/info.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

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

void bind_genesis_utils_core_info(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::core::InfoCompiler file:genesis/utils/core/info.hpp line:49
		pybind11::class_<genesis::utils::core::InfoCompiler, std::shared_ptr<genesis::utils::core::InfoCompiler>> cl(M("genesis::utils::core"), "InfoCompiler", "");
		// function-signature: genesis::utils::core::InfoCompiler()
		cl.def( pybind11::init( [](){ return new genesis::utils::core::InfoCompiler(); } ) );
		// function-signature: genesis::utils::core::InfoCompiler::InfoCompiler(const struct genesis::utils::core::InfoCompiler &)(const struct genesis::utils::core::InfoCompiler &) file:genesis/utils/core/info.hpp line:49
		cl.def( pybind11::init( [](genesis::utils::core::InfoCompiler const &o){ return new genesis::utils::core::InfoCompiler(o); } ) );
		cl.def_readwrite("compile_date_time", &genesis::utils::core::InfoCompiler::compile_date_time);
		cl.def_readwrite("is_debug", &genesis::utils::core::InfoCompiler::is_debug);
		cl.def_readwrite("is_release", &genesis::utils::core::InfoCompiler::is_release);
		cl.def_readwrite("build_type", &genesis::utils::core::InfoCompiler::build_type);
		cl.def_readwrite("platform", &genesis::utils::core::InfoCompiler::platform);
		cl.def_readwrite("compiler_family", &genesis::utils::core::InfoCompiler::compiler_family);
		cl.def_readwrite("compiler_version", &genesis::utils::core::InfoCompiler::compiler_version);
		cl.def_readwrite("cpp_standard", &genesis::utils::core::InfoCompiler::cpp_standard);
		cl.def_readwrite("with_openmp", &genesis::utils::core::InfoCompiler::with_openmp);
		cl.def_readwrite("with_avx", &genesis::utils::core::InfoCompiler::with_avx);
		cl.def_readwrite("with_avx2", &genesis::utils::core::InfoCompiler::with_avx2);
		cl.def_readwrite("with_avx512", &genesis::utils::core::InfoCompiler::with_avx512);
		// function-signature: struct genesis::utils::core::InfoCompiler & genesis::utils::core::InfoCompiler::operator=(const struct genesis::utils::core::InfoCompiler &)(const struct genesis::utils::core::InfoCompiler &) file:genesis/utils/core/info.hpp line:49
		cl.def("assign", (struct genesis::utils::core::InfoCompiler & (genesis::utils::core::InfoCompiler::*)(const struct genesis::utils::core::InfoCompiler &)) &genesis::utils::core::InfoCompiler::operator=, "C++: genesis::utils::core::InfoCompiler::operator=(const struct genesis::utils::core::InfoCompiler &) --> struct genesis::utils::core::InfoCompiler &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	// genesis::utils::core::info_get_compiler() file:genesis/utils/core/info.hpp line:134
	// function-signature: const struct genesis::utils::core::InfoCompiler & genesis::utils::core::info_get_compiler()() file:genesis/utils/core/info.hpp line:134
	M("genesis::utils::core").def("info_get_compiler", (const struct genesis::utils::core::InfoCompiler & (*)()) &genesis::utils::core::info_get_compiler, "Return information about compiler settings and flags.\n\nC++: genesis::utils::core::info_get_compiler() --> const struct genesis::utils::core::InfoCompiler &", pybind11::return_value_policy::reference);

	// genesis::utils::core::info_preprocessor_definitions() file:genesis/utils/core/info.hpp line:142
	// function-signature: const class std::unordered_map<std::string, std::string > & genesis::utils::core::info_preprocessor_definitions()() file:genesis/utils/core/info.hpp line:142
	M("genesis::utils::core").def("info_preprocessor_definitions", (const class std::unordered_map<std::string, std::string > & (*)()) &genesis::utils::core::info_preprocessor_definitions, "Return a string map with some relevant preprocessor macros.\n\n This is mostly for debugging, to see how different systems compile the code.\n We only add a set of important macros that we internally have use for as well.\n\nC++: genesis::utils::core::info_preprocessor_definitions() --> const class std::unordered_map<std::string, std::string > &", pybind11::return_value_policy::reference);

	// genesis::utils::core::info_print_compiler() file:genesis/utils/core/info.hpp line:147
	// function-signature: std::string genesis::utils::core::info_print_compiler()() file:genesis/utils/core/info.hpp line:147
	M("genesis::utils::core").def("info_print_compiler", (std::string (*)()) &genesis::utils::core::info_print_compiler, "Print information about compiler settings and flags to a string.\n\nC++: genesis::utils::core::info_print_compiler() --> std::string");

	{ // genesis::utils::core::InfoHardware file:genesis/utils/core/info.hpp line:153
		pybind11::class_<genesis::utils::core::InfoHardware, std::shared_ptr<genesis::utils::core::InfoHardware>> cl(M("genesis::utils::core"), "InfoHardware", "");
		// function-signature: genesis::utils::core::InfoHardware()
		cl.def( pybind11::init( [](){ return new genesis::utils::core::InfoHardware(); } ) );
		// function-signature: genesis::utils::core::InfoHardware::InfoHardware(const struct genesis::utils::core::InfoHardware &)(const struct genesis::utils::core::InfoHardware &) file:genesis/utils/core/info.hpp line:153
		cl.def( pybind11::init( [](genesis::utils::core::InfoHardware const &o){ return new genesis::utils::core::InfoHardware(o); } ) );
		cl.def_readwrite("is_little_endian", &genesis::utils::core::InfoHardware::is_little_endian);
		cl.def_readwrite("total_memory", &genesis::utils::core::InfoHardware::total_memory);
		cl.def_readwrite("vendor_AMD", &genesis::utils::core::InfoHardware::vendor_AMD);
		cl.def_readwrite("vendor_Intel", &genesis::utils::core::InfoHardware::vendor_Intel);
		cl.def_readwrite("vendor_string", &genesis::utils::core::InfoHardware::vendor_string);
		cl.def_readwrite("cpu_model", &genesis::utils::core::InfoHardware::cpu_model);
		cl.def_readwrite("physical_core_count", &genesis::utils::core::InfoHardware::physical_core_count);
		cl.def_readwrite("with_hyperthreading", &genesis::utils::core::InfoHardware::with_hyperthreading);
		cl.def_readwrite("OS_x64", &genesis::utils::core::InfoHardware::OS_x64);
		cl.def_readwrite("OS_AVX", &genesis::utils::core::InfoHardware::OS_AVX);
		cl.def_readwrite("OS_AVX512", &genesis::utils::core::InfoHardware::OS_AVX512);
		cl.def_readwrite("HW_MMX", &genesis::utils::core::InfoHardware::HW_MMX);
		cl.def_readwrite("HW_x64", &genesis::utils::core::InfoHardware::HW_x64);
		cl.def_readwrite("HW_ABM", &genesis::utils::core::InfoHardware::HW_ABM);
		cl.def_readwrite("HW_RDRAND", &genesis::utils::core::InfoHardware::HW_RDRAND);
		cl.def_readwrite("HW_RDSEED", &genesis::utils::core::InfoHardware::HW_RDSEED);
		cl.def_readwrite("HW_BMI1", &genesis::utils::core::InfoHardware::HW_BMI1);
		cl.def_readwrite("HW_BMI2", &genesis::utils::core::InfoHardware::HW_BMI2);
		cl.def_readwrite("HW_ADX", &genesis::utils::core::InfoHardware::HW_ADX);
		cl.def_readwrite("HW_MPX", &genesis::utils::core::InfoHardware::HW_MPX);
		cl.def_readwrite("HW_PREFETCHW", &genesis::utils::core::InfoHardware::HW_PREFETCHW);
		cl.def_readwrite("HW_PREFETCHWT1", &genesis::utils::core::InfoHardware::HW_PREFETCHWT1);
		cl.def_readwrite("HW_RDPID", &genesis::utils::core::InfoHardware::HW_RDPID);
		cl.def_readwrite("HW_SSE", &genesis::utils::core::InfoHardware::HW_SSE);
		cl.def_readwrite("HW_SSE2", &genesis::utils::core::InfoHardware::HW_SSE2);
		cl.def_readwrite("HW_SSE3", &genesis::utils::core::InfoHardware::HW_SSE3);
		cl.def_readwrite("HW_SSSE3", &genesis::utils::core::InfoHardware::HW_SSSE3);
		cl.def_readwrite("HW_SSE41", &genesis::utils::core::InfoHardware::HW_SSE41);
		cl.def_readwrite("HW_SSE42", &genesis::utils::core::InfoHardware::HW_SSE42);
		cl.def_readwrite("HW_SSE4a", &genesis::utils::core::InfoHardware::HW_SSE4a);
		cl.def_readwrite("HW_AES", &genesis::utils::core::InfoHardware::HW_AES);
		cl.def_readwrite("HW_SHA", &genesis::utils::core::InfoHardware::HW_SHA);
		cl.def_readwrite("HW_AVX", &genesis::utils::core::InfoHardware::HW_AVX);
		cl.def_readwrite("HW_XOP", &genesis::utils::core::InfoHardware::HW_XOP);
		cl.def_readwrite("HW_FMA3", &genesis::utils::core::InfoHardware::HW_FMA3);
		cl.def_readwrite("HW_FMA4", &genesis::utils::core::InfoHardware::HW_FMA4);
		cl.def_readwrite("HW_AVX2", &genesis::utils::core::InfoHardware::HW_AVX2);
		cl.def_readwrite("HW_AVX512_F", &genesis::utils::core::InfoHardware::HW_AVX512_F);
		cl.def_readwrite("HW_AVX512_CD", &genesis::utils::core::InfoHardware::HW_AVX512_CD);
		cl.def_readwrite("HW_AVX512_PF", &genesis::utils::core::InfoHardware::HW_AVX512_PF);
		cl.def_readwrite("HW_AVX512_ER", &genesis::utils::core::InfoHardware::HW_AVX512_ER);
		cl.def_readwrite("HW_AVX512_VL", &genesis::utils::core::InfoHardware::HW_AVX512_VL);
		cl.def_readwrite("HW_AVX512_BW", &genesis::utils::core::InfoHardware::HW_AVX512_BW);
		cl.def_readwrite("HW_AVX512_DQ", &genesis::utils::core::InfoHardware::HW_AVX512_DQ);
		cl.def_readwrite("HW_AVX512_IFMA", &genesis::utils::core::InfoHardware::HW_AVX512_IFMA);
		cl.def_readwrite("HW_AVX512_VBMI", &genesis::utils::core::InfoHardware::HW_AVX512_VBMI);
		cl.def_readwrite("HW_AVX512_VPOPCNTDQ", &genesis::utils::core::InfoHardware::HW_AVX512_VPOPCNTDQ);
		cl.def_readwrite("HW_AVX512_4VNNIW", &genesis::utils::core::InfoHardware::HW_AVX512_4VNNIW);
		cl.def_readwrite("HW_AVX512_4FMAPS", &genesis::utils::core::InfoHardware::HW_AVX512_4FMAPS);
		cl.def_readwrite("HW_AVX512_VNNI", &genesis::utils::core::InfoHardware::HW_AVX512_VNNI);
		cl.def_readwrite("HW_AVX512_BF16", &genesis::utils::core::InfoHardware::HW_AVX512_BF16);
		cl.def_readwrite("HW_AVX512_VBMI2", &genesis::utils::core::InfoHardware::HW_AVX512_VBMI2);
		cl.def_readwrite("HW_GFNI", &genesis::utils::core::InfoHardware::HW_GFNI);
		cl.def_readwrite("HW_VAES", &genesis::utils::core::InfoHardware::HW_VAES);
		cl.def_readwrite("HW_AVX512_VPCLMUL", &genesis::utils::core::InfoHardware::HW_AVX512_VPCLMUL);
		cl.def_readwrite("HW_AVX512_BITALG", &genesis::utils::core::InfoHardware::HW_AVX512_BITALG);
		// function-signature: struct genesis::utils::core::InfoHardware & genesis::utils::core::InfoHardware::operator=(const struct genesis::utils::core::InfoHardware &)(const struct genesis::utils::core::InfoHardware &) file:genesis/utils/core/info.hpp line:153
		cl.def("assign", (struct genesis::utils::core::InfoHardware & (genesis::utils::core::InfoHardware::*)(const struct genesis::utils::core::InfoHardware &)) &genesis::utils::core::InfoHardware::operator=, "C++: genesis::utils::core::InfoHardware::operator=(const struct genesis::utils::core::InfoHardware &) --> struct genesis::utils::core::InfoHardware &", pybind11::return_value_policy::automatic, pybind11::arg(""));
	}
	// genesis::utils::core::info_get_hardware() file:genesis/utils/core/info.hpp line:250
	// function-signature: const struct genesis::utils::core::InfoHardware & genesis::utils::core::info_get_hardware()() file:genesis/utils/core/info.hpp line:250
	M("genesis::utils::core").def("info_get_hardware", (const struct genesis::utils::core::InfoHardware & (*)()) &genesis::utils::core::info_get_hardware, "Return information about hardware features.\n\nC++: genesis::utils::core::info_get_hardware() --> const struct genesis::utils::core::InfoHardware &", pybind11::return_value_policy::reference);

	// genesis::utils::core::info_print_hardware(bool) file:genesis/utils/core/info.hpp line:259
	// function-signature: std::string genesis::utils::core::info_print_hardware(bool)(bool) file:genesis/utils/core/info.hpp line:259
	M("genesis::utils::core").def("info_print_hardware", []() -> std::string { return genesis::utils::core::info_print_hardware(); }, "");
	M("genesis::utils::core").def("info_print_hardware", (std::string (*)(bool)) &genesis::utils::core::info_print_hardware, "Print information about hardware features to a string.\n\n This lists hardware features such as memory, CPU model, and potentially CPU features.\n If  is given, each CPU feature that we test for is listed. Otherwise, by default,\n we just print a summary of the features that are important to us, such as AVX.\n\nC++: genesis::utils::core::info_print_hardware(bool) --> std::string", pybind11::arg("full"));

	// genesis::utils::core::info_use_avx() file:genesis/utils/core/info.hpp line:267
	// function-signature: bool genesis::utils::core::info_use_avx()() file:genesis/utils/core/info.hpp line:267
	M("genesis::utils::core").def("info_use_avx", (bool (*)()) &genesis::utils::core::info_use_avx, "Assess if it is safe to use AVX features.\n\n This is the case if the hardware supports AVX, the OS has it activated, and the compiler had\n the respective flag set.\n\nC++: genesis::utils::core::info_use_avx() --> bool");

	// genesis::utils::core::info_use_avx2() file:genesis/utils/core/info.hpp line:275
	// function-signature: bool genesis::utils::core::info_use_avx2()() file:genesis/utils/core/info.hpp line:275
	M("genesis::utils::core").def("info_use_avx2", (bool (*)()) &genesis::utils::core::info_use_avx2, "Assess if it is safe to use AVX2 features.\n\n This is the case if the hardware supports AVX2, the OS has it activated, and the compiler had\n the respective flag set.\n\nC++: genesis::utils::core::info_use_avx2() --> bool");

	// genesis::utils::core::info_use_avx512() file:genesis/utils/core/info.hpp line:283
	// function-signature: bool genesis::utils::core::info_use_avx512()() file:genesis/utils/core/info.hpp line:283
	M("genesis::utils::core").def("info_use_avx512", (bool (*)()) &genesis::utils::core::info_use_avx512, "Assess if it is safe to use AVX512 features.\n\n This is the case if the hardware supports AVX512, the OS has it activated, and the compiler had\n the respective flag set.\n\nC++: genesis::utils::core::info_use_avx512() --> bool");

	// genesis::utils::core::info_physical_core_count() file:genesis/utils/core/info.hpp line:296
	// function-signature: unsigned long genesis::utils::core::info_physical_core_count()() file:genesis/utils/core/info.hpp line:296
	M("genesis::utils::core").def("info_physical_core_count", (unsigned long (*)()) &genesis::utils::core::info_physical_core_count, "Get the number of CPU cores.\n\n This tries to get the physical core count, and if that fails, it uses\n std::thread::hardware_concurrency() along with info_hyperthreads_enabled() to determine\n a best guess at the number of physical cores.\n\nC++: genesis::utils::core::info_physical_core_count() --> unsigned long");

	// genesis::utils::core::info_hyperthreads_enabled() file:genesis/utils/core/info.hpp line:301
	// function-signature: bool genesis::utils::core::info_hyperthreads_enabled()() file:genesis/utils/core/info.hpp line:301
	M("genesis::utils::core").def("info_hyperthreads_enabled", (bool (*)()) &genesis::utils::core::info_hyperthreads_enabled, "Try to get whether hyperthreads are enabled in the current system.\n\nC++: genesis::utils::core::info_hyperthreads_enabled() --> bool");

	// genesis::utils::core::info_number_of_threads_openmp() file:genesis/utils/core/info.hpp line:313
	// function-signature: unsigned long genesis::utils::core::info_number_of_threads_openmp()() file:genesis/utils/core/info.hpp line:313
	M("genesis::utils::core").def("info_number_of_threads_openmp", (unsigned long (*)()) &genesis::utils::core::info_number_of_threads_openmp, "Get the number of threads as indicated by the OpenMP environment.\n\n If genesis was compiled with OpenMP support (that is, if `GENESIS_OPENMP` is defined, which is\n done via the CMake script), we use `omp_get_max_threads()` to get a number of threads from that.\n If however genesis was compiled without OpenMP support, we instead consider the environment\n variable `OMP_NUM_THREADS` as a source for the OpenMP threads.\n\n If unsuccessful, 0 is returned.\n\nC++: genesis::utils::core::info_number_of_threads_openmp() --> unsigned long");

	// genesis::utils::core::info_number_of_threads_slurm() file:genesis/utils/core/info.hpp line:320
	// function-signature: unsigned long genesis::utils::core::info_number_of_threads_slurm()() file:genesis/utils/core/info.hpp line:320
	M("genesis::utils::core").def("info_number_of_threads_slurm", (unsigned long (*)()) &genesis::utils::core::info_number_of_threads_slurm, "Get the number of threads as indicated by the SLURM environment.\n\n This uses the environment variable `SLURM_CPUS_PER_TASK`. If unsuccessful, 0 is returned.\n\nC++: genesis::utils::core::info_number_of_threads_slurm() --> unsigned long");

	// genesis::utils::core::guess_number_of_threads(bool, bool, bool) file:genesis/utils/core/info.hpp line:348
	// function-signature: unsigned long genesis::utils::core::guess_number_of_threads(bool, bool, bool)(bool, bool, bool) file:genesis/utils/core/info.hpp line:348
	M("genesis::utils::core").def("guess_number_of_threads", []() -> unsigned long { return genesis::utils::core::guess_number_of_threads(); }, "");
	M("genesis::utils::core").def("guess_number_of_threads", [](bool const & a0) -> unsigned long { return genesis::utils::core::guess_number_of_threads(a0); }, "", pybind11::arg("use_openmp"));
	M("genesis::utils::core").def("guess_number_of_threads", [](bool const & a0, bool const & a1) -> unsigned long { return genesis::utils::core::guess_number_of_threads(a0, a1); }, "", pybind11::arg("use_openmp"), pybind11::arg("use_slurm"));
	M("genesis::utils::core").def("guess_number_of_threads", (unsigned long (*)(bool, bool, bool)) &genesis::utils::core::guess_number_of_threads, "Make an educated guess on the number of threads to use for multi-threaded functionality.\n\n This function uses multiple sources and ways to try to guess a reasonable number of threads:\n\n  - If  is set, we use the `omp_get_max_threads()` function or the `OMP_NUM_THREADS`\n    environment variable to get a number of threads, using the info_number_of_threads_openmp()\n    function. For details, see there.\n  - If  is set, we get a number of cores from the environment variable\n    `SLURM_CPUS_PER_TASK`.\n  - Lastly, we get the result of `std::thread::hardware_concurrency()` as another hint.\n    If furthermore  is set, and hyperthreads are enabled, we divide that hardware\n    concurrency number by two, in order to account for hyperthreads, resulting in the number\n    of physical cores available on the system (ideally). This avoids core oversubscription that\n    could otherwise be the result of using all threads instead of all physical cores.\n\n If the numbers disagree with each other, we prefer OpenMP over slurm over `hardware_concurrency()`,\n that is, we are going from most specific to least. Furthermore, if the OpenMP based guess yields\n exactly the same number as the hardware concurrency, we also use the  setting,\n as this result usually indicates that OpenMP left at its default, in which case we also want\n to avoid core oversubscription due to hyperthreading.\n\n The function is guaranteed to return a non-zero value, meaning that at least the main thread\n is always accounted for. This is important for Options::get().init_global_thread_pool(), which\n will account for that, and initialize the number of additional threads to one fewer than this.\n\nC++: genesis::utils::core::guess_number_of_threads(bool, bool, bool) --> unsigned long", pybind11::arg("use_openmp"), pybind11::arg("use_slurm"), pybind11::arg("physical_cores"));

}
