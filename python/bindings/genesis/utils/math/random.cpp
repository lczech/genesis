#include <genesis/utils/math/random.hpp>
#include <genesis/utils/math/statistics.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

void bind_genesis_utils_math_random(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::math::permuted_congruential_generator() file:genesis/utils/math/random.hpp line:52
	// function-signature: unsigned int genesis::utils::math::permuted_congruential_generator()() file:genesis/utils/math/random.hpp line:52
	M("genesis::utils::math").def("permuted_congruential_generator", (unsigned int (*)()) &genesis::utils::math::permuted_congruential_generator, "Fast random number generator for 32bit integers.\n\n See https://en.wikipedia.org/wiki/Permuted_congruential_generator for details.\n See permuted_congruential_generator_init() to set the seed.\n\nC++: genesis::utils::math::permuted_congruential_generator() --> unsigned int");

	// genesis::utils::math::permuted_congruential_generator(unsigned int) file:genesis/utils/math/random.hpp line:59
	// function-signature: unsigned int genesis::utils::math::permuted_congruential_generator(unsigned int)(unsigned int) file:genesis/utils/math/random.hpp line:59
	M("genesis::utils::math").def("permuted_congruential_generator", (unsigned int (*)(unsigned int)) &genesis::utils::math::permuted_congruential_generator, "Fast random number generator for 32bit integers, for numbers in the range `[0, max]`.\n\n Note that the interval is closed, i.e.,  is inclusive.\n\nC++: genesis::utils::math::permuted_congruential_generator(unsigned int) --> unsigned int", pybind11::arg("max"));

	// genesis::utils::math::permuted_congruential_generator(unsigned int, unsigned int) file:genesis/utils/math/random.hpp line:66
	// function-signature: unsigned int genesis::utils::math::permuted_congruential_generator(unsigned int, unsigned int)(unsigned int, unsigned int) file:genesis/utils/math/random.hpp line:66
	M("genesis::utils::math").def("permuted_congruential_generator", (unsigned int (*)(unsigned int, unsigned int)) &genesis::utils::math::permuted_congruential_generator, "Fast random number generator for 32bit integers, for numbers in the range `[min, max]`.\n\n Note that the interval is closed, i.e.,  is inclusive.\n\nC++: genesis::utils::math::permuted_congruential_generator(unsigned int, unsigned int) --> unsigned int", pybind11::arg("min"), pybind11::arg("max"));

	// genesis::utils::math::permuted_congruential_generator_bool() file:genesis/utils/math/random.hpp line:71
	// function-signature: bool genesis::utils::math::permuted_congruential_generator_bool()() file:genesis/utils/math/random.hpp line:71
	M("genesis::utils::math").def("permuted_congruential_generator_bool", (bool (*)()) &genesis::utils::math::permuted_congruential_generator_bool, "Fast random number generator for 32bit integers, for bool with 0.5 probability.\n\nC++: genesis::utils::math::permuted_congruential_generator_bool() --> bool");

	// genesis::utils::math::permuted_congruential_generator_bool(unsigned int) file:genesis/utils/math/random.hpp line:80
	// function-signature: bool genesis::utils::math::permuted_congruential_generator_bool(unsigned int)(unsigned int) file:genesis/utils/math/random.hpp line:80
	M("genesis::utils::math").def("permuted_congruential_generator_bool", (bool (*)(unsigned int)) &genesis::utils::math::permuted_congruential_generator_bool, "Fast random number generator for 32bit integers, for bool with a given chance.\n\n This is returning a random bool, with a 1 in  possibility to be `true`.\n For instance, calling the function with `chance_one_in == 5` means a 1 in 5 chance,\n or probability of 0.2 for a `true` result.\n\nC++: genesis::utils::math::permuted_congruential_generator_bool(unsigned int) --> bool", pybind11::arg("chance_one_in"));

	// genesis::utils::math::permuted_congruential_generator_init(unsigned long) file:genesis/utils/math/random.hpp line:85
	// function-signature: void genesis::utils::math::permuted_congruential_generator_init(unsigned long)(unsigned long) file:genesis/utils/math/random.hpp line:85
	M("genesis::utils::math").def("permuted_congruential_generator_init", (void (*)(unsigned long)) &genesis::utils::math::permuted_congruential_generator_init, "Set the seed for permuted_congruential_generator().\n\nC++: genesis::utils::math::permuted_congruential_generator_init(unsigned long) --> void", pybind11::arg("seed"));

	// genesis::utils::math::select_without_replacement(unsigned long, unsigned long) file:genesis/utils/math/random.hpp line:102
	// function-signature: class std::vector<unsigned long> genesis::utils::math::select_without_replacement(unsigned long, unsigned long)(unsigned long, unsigned long) file:genesis/utils/math/random.hpp line:102
	M("genesis::utils::math").def("select_without_replacement", (class std::vector<unsigned long> (*)(unsigned long, unsigned long)) &genesis::utils::math::select_without_replacement, "Select  many unique numbers out of the range `[ 0, n )`.\n\n In other words, select  numbers out of the sequence of  numbers, without replacement.\n\n The implementation is based on Algorithm 3.4.2S of Knuth's book Seminumeric Algorithms.\n\n \n Desired sample size.\n \n\n Total population size.\n \n\n  List of  unique numbers out of the range `[ 0, n )`.\n\nC++: genesis::utils::math::select_without_replacement(unsigned long, unsigned long) --> class std::vector<unsigned long>", pybind11::arg("k"), pybind11::arg("n"));

	{ // genesis::utils::math::MinMaxPair file:genesis/utils/math/statistics.hpp line:109
		pybind11::class_<genesis::utils::math::MinMaxPair<double>, std::shared_ptr<genesis::utils::math::MinMaxPair<double>>> cl(M("genesis::utils::math"), "MinMaxPair_double_t", "");
		// function-signature: genesis::utils::math::MinMaxPair<double>::MinMaxPair()() file:genesis/utils/math/statistics.hpp line:109
		cl.def( pybind11::init( [](){ return new genesis::utils::math::MinMaxPair<double>(); } ) );
		// function-signature: genesis::utils::math::MinMaxPair<double>::MinMaxPair(const struct genesis::utils::math::MinMaxPair<double> &)(const struct genesis::utils::math::MinMaxPair<double> &) file:genesis/utils/math/statistics.hpp line:109
		cl.def( pybind11::init( [](genesis::utils::math::MinMaxPair<double> const &o){ return new genesis::utils::math::MinMaxPair<double>(o); } ) );
		cl.def_readwrite("min", &genesis::utils::math::MinMaxPair<double>::min);
		cl.def_readwrite("max", &genesis::utils::math::MinMaxPair<double>::max);
	}
	{ // genesis::utils::math::MeanStddevPair file:genesis/utils/math/statistics.hpp line:121
		pybind11::class_<genesis::utils::math::MeanStddevPair, std::shared_ptr<genesis::utils::math::MeanStddevPair>> cl(M("genesis::utils::math"), "MeanStddevPair", "Store a mean and a standard deviation value.\n\n This notation is simply more readable than the std default of using a `pair<T, T>` for such\n types.");
		// function-signature: genesis::utils::math::MeanStddevPair::MeanStddevPair()() file:genesis/utils/math/statistics.hpp line:121
		cl.def( pybind11::init( [](){ return new genesis::utils::math::MeanStddevPair(); } ) );
		// function-signature: genesis::utils::math::MeanStddevPair::MeanStddevPair(const struct genesis::utils::math::MeanStddevPair &)(const struct genesis::utils::math::MeanStddevPair &) file:genesis/utils/math/statistics.hpp line:121
		cl.def( pybind11::init( [](genesis::utils::math::MeanStddevPair const &o){ return new genesis::utils::math::MeanStddevPair(o); } ) );
		cl.def_readwrite("mean", &genesis::utils::math::MeanStddevPair::mean);
		cl.def_readwrite("stddev", &genesis::utils::math::MeanStddevPair::stddev);
	}
	// genesis::utils::math::closure(class __gnu_cxx::__normal_iterator<double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<double *, class std::vector<double> >) file:genesis/utils/math/statistics.hpp line:296
	// function-signature: void genesis::utils::math::closure<__gnu_cxx::__normal_iterator<double *, std::vector<double> >>(class __gnu_cxx::__normal_iterator<double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<double *, class std::vector<double> >)(class __gnu_cxx::__normal_iterator<double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<double *, class std::vector<double> >) file:genesis/utils/math/statistics.hpp line:296
	M("genesis::utils::math").def("closure", (void (*)(class __gnu_cxx::__normal_iterator<double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<double *, class std::vector<double> >)) &genesis::utils::math::closure<__gnu_cxx::__normal_iterator<double *, std::vector<double> >>, "C++: genesis::utils::math::closure(class __gnu_cxx::__normal_iterator<double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<double *, class std::vector<double> >) --> void", pybind11::arg("first"), pybind11::arg("last"));

	// genesis::utils::math::closure(class std::vector<double> &) file:genesis/utils/math/statistics.hpp line:338
	// function-signature: void genesis::utils::math::closure(class std::vector<double> &)(class std::vector<double> &) file:genesis/utils/math/statistics.hpp line:338
	M("genesis::utils::math").def("closure", (void (*)(class std::vector<double> &)) &genesis::utils::math::closure, "Calculate the closure of a `std::vector` of `double` elements.\n\n \n closure( ForwardIterator first, ForwardIterator last ) for details.\n\nC++: genesis::utils::math::closure(class std::vector<double> &) --> void", pybind11::arg("vec"));

	// genesis::utils::math::mean_stddev(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, double) file:genesis/utils/math/statistics.hpp line:422
	// function-signature: struct genesis::utils::math::MeanStddevPair genesis::utils::math::mean_stddev<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >>(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, double)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, double) file:genesis/utils/math/statistics.hpp line:422
	M("genesis::utils::math").def("mean_stddev", [](class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> > const & a0, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> > const & a1) -> genesis::utils::math::MeanStddevPair { return genesis::utils::math::mean_stddev(a0, a1); }, "", pybind11::arg("first"), pybind11::arg("last"));
	M("genesis::utils::math").def("mean_stddev", (struct genesis::utils::math::MeanStddevPair (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, double)) &genesis::utils::math::mean_stddev<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >>, "C++: genesis::utils::math::mean_stddev(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, double) --> struct genesis::utils::math::MeanStddevPair", pybind11::arg("first"), pybind11::arg("last"), pybind11::arg("epsilon"));

	// genesis::utils::math::mean_stddev(const class std::vector<double> &, double) file:genesis/utils/math/statistics.hpp line:477
	// function-signature: struct genesis::utils::math::MeanStddevPair genesis::utils::math::mean_stddev(const class std::vector<double> &, double)(const class std::vector<double> &, double) file:genesis/utils/math/statistics.hpp line:477
	M("genesis::utils::math").def("mean_stddev", [](const class std::vector<double> & a0) -> genesis::utils::math::MeanStddevPair { return genesis::utils::math::mean_stddev(a0); }, "", pybind11::arg("vec"));
	M("genesis::utils::math").def("mean_stddev", (struct genesis::utils::math::MeanStddevPair (*)(const class std::vector<double> &, double)) &genesis::utils::math::mean_stddev, "Calculate the mean and standard deviation of a `std::vector` of `double` elements.\n\n \n mean_stddev( ForwardIterator first, ForwardIterator last, double epsilon ) for details.\n \n\n arithmetic_mean() for a function that only calculates the mean, and thus saves the effort\n of a second iteration over the range.\n\nC++: genesis::utils::math::mean_stddev(const class std::vector<double> &, double) --> struct genesis::utils::math::MeanStddevPair", pybind11::arg("vec"), pybind11::arg("epsilon"));

	// genesis::utils::math::arithmetic_mean(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) file:genesis/utils/math/statistics.hpp line:502
	// function-signature: double genesis::utils::math::arithmetic_mean<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >>(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) file:genesis/utils/math/statistics.hpp line:502
	M("genesis::utils::math").def("arithmetic_mean", (double (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >)) &genesis::utils::math::arithmetic_mean<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >>, "C++: genesis::utils::math::arithmetic_mean(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) --> double", pybind11::arg("first"), pybind11::arg("last"));

	// genesis::utils::math::arithmetic_mean(const class std::vector<double> &) file:genesis/utils/math/statistics.hpp line:537
	// function-signature: double genesis::utils::math::arithmetic_mean(const class std::vector<double> &)(const class std::vector<double> &) file:genesis/utils/math/statistics.hpp line:537
	M("genesis::utils::math").def("arithmetic_mean", (double (*)(const class std::vector<double> &)) &genesis::utils::math::arithmetic_mean, "Calculate the arithmetic mean of a `std::vector` of `double` elements.\n\n \n arithmetic_mean( ForwardIterator first, ForwardIterator last ) for details.\n \n\n mean_stddev() for a function that simultaneously calculates the standard deviation.\n \n\n geometric_mean() for a function that calculates the geometric mean, and\n \n\n harmonic_mean() for a function that calculates the harmonic mean.\n\nC++: genesis::utils::math::arithmetic_mean(const class std::vector<double> &) --> double", pybind11::arg("vec"));

	// genesis::utils::math::weighted_arithmetic_mean(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) file:genesis/utils/math/statistics.hpp line:560
	// function-signature: double genesis::utils::math::weighted_arithmetic_mean<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >>(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) file:genesis/utils/math/statistics.hpp line:560
	M("genesis::utils::math").def("weighted_arithmetic_mean", (double (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >)) &genesis::utils::math::weighted_arithmetic_mean<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >>, "C++: genesis::utils::math::weighted_arithmetic_mean(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) --> double", pybind11::arg("first_value"), pybind11::arg("last_value"), pybind11::arg("first_weight"), pybind11::arg("last_weight"));

}
