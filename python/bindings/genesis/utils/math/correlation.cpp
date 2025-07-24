#include <genesis/utils/math/correlation.hpp>
#include <genesis/utils/math/distribution.hpp>
#include <genesis/utils/math/moments.hpp>
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

void bind_genesis_utils_math_correlation(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::math::pearson_correlation_coefficient(const class std::vector<double> &, const class std::vector<double> &) file:genesis/utils/math/correlation.hpp line:123
	// function-signature: double genesis::utils::math::pearson_correlation_coefficient(const class std::vector<double> &, const class std::vector<double> &)(const class std::vector<double> &, const class std::vector<double> &) file:genesis/utils/math/correlation.hpp line:123
	M("genesis::utils::math").def("pearson_correlation_coefficient", (double (*)(const class std::vector<double> &, const class std::vector<double> &)) &genesis::utils::math::pearson_correlation_coefficient, "Calculate the Pearson Correlation Coefficient between the entries of two vectors.\n\n  ForwardIteratorA first_a, ForwardIteratorA last_a, ForwardIteratorB first_b, ForwardIteratorB last_b ).\n\nC++: genesis::utils::math::pearson_correlation_coefficient(const class std::vector<double> &, const class std::vector<double> &) --> double", pybind11::arg("vec_a"), pybind11::arg("vec_b"));

	// genesis::utils::math::spearmans_rank_correlation_coefficient(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) file:genesis/utils/math/correlation.hpp line:145
	// function-signature: double genesis::utils::math::spearmans_rank_correlation_coefficient<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >,__gnu_cxx::__normal_iterator<const double *, std::vector<double> >>(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) file:genesis/utils/math/correlation.hpp line:145
	M("genesis::utils::math").def("spearmans_rank_correlation_coefficient", (double (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >)) &genesis::utils::math::spearmans_rank_correlation_coefficient<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >,__gnu_cxx::__normal_iterator<const double *, std::vector<double> >>, "C++: genesis::utils::math::spearmans_rank_correlation_coefficient(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) --> double", pybind11::arg("first_a"), pybind11::arg("last_a"), pybind11::arg("first_b"), pybind11::arg("last_b"));

	// genesis::utils::math::spearmans_rank_correlation_coefficient(const class std::vector<double> &, const class std::vector<double> &) file:genesis/utils/math/correlation.hpp line:166
	// function-signature: double genesis::utils::math::spearmans_rank_correlation_coefficient(const class std::vector<double> &, const class std::vector<double> &)(const class std::vector<double> &, const class std::vector<double> &) file:genesis/utils/math/correlation.hpp line:166
	M("genesis::utils::math").def("spearmans_rank_correlation_coefficient", (double (*)(const class std::vector<double> &, const class std::vector<double> &)) &genesis::utils::math::spearmans_rank_correlation_coefficient, "Calculate Spearman's Rank Correlation Coefficient between the entries of two vectors.\n\n  RandomAccessIteratorA first_a, RandomAccessIteratorA last_a, RandomAccessIteratorB first_b, RandomAccessIteratorB last_b )\n\nC++: genesis::utils::math::spearmans_rank_correlation_coefficient(const class std::vector<double> &, const class std::vector<double> &) --> double", pybind11::arg("vec_a"), pybind11::arg("vec_b"));

	// genesis::utils::math::KendallsTauMethod file:genesis/utils/math/correlation.hpp line:187
	pybind11::enum_<genesis::utils::math::KendallsTauMethod>(M("genesis::utils::math"), "KendallsTauMethod", "Method for computing Kendall's Tau.\n\n See kendalls_tau_correlation_coefficient() for the function that uses this.\n See https://en.wikipedia.org/wiki/Kendall_rank_correlation_coefficient and\n https://docs.scipy.org/doc/scipy-1.12.0/reference/generated/scipy.stats.kendalltau.html#scipy.stats.kendalltau\n for details on the different methods.")
		.value("kTauA", genesis::utils::math::KendallsTauMethod::kTauA)
		.value("kTauB", genesis::utils::math::KendallsTauMethod::kTauB)
		.value("kTauC", genesis::utils::math::KendallsTauMethod::kTauC);

;

	// genesis::utils::math::kendalls_tau_correlation_coefficient(const class std::vector<double> &, const class std::vector<double> &, enum genesis::utils::math::KendallsTauMethod) file:genesis/utils/math/correlation.hpp line:217
	// function-signature: double genesis::utils::math::kendalls_tau_correlation_coefficient(const class std::vector<double> &, const class std::vector<double> &, enum genesis::utils::math::KendallsTauMethod)(const class std::vector<double> &, const class std::vector<double> &, enum genesis::utils::math::KendallsTauMethod) file:genesis/utils/math/correlation.hpp line:217
	M("genesis::utils::math").def("kendalls_tau_correlation_coefficient", [](const class std::vector<double> & a0, const class std::vector<double> & a1) -> double { return genesis::utils::math::kendalls_tau_correlation_coefficient(a0, a1); }, "", pybind11::arg("x"), pybind11::arg("y"));
	M("genesis::utils::math").def("kendalls_tau_correlation_coefficient", (double (*)(const class std::vector<double> &, const class std::vector<double> &, enum genesis::utils::math::KendallsTauMethod)) &genesis::utils::math::kendalls_tau_correlation_coefficient, "Compute Kendall's Tau Correlation Coefficient.\n\n This function computes Kendall's tau, if requested accounting for ties,\n and using Knight's algorithm for speed.\n\n >  Knight, W. (1966). \"A Computer Method for Calculating Kendall's Tau with Ungrouped Data\".\n > Journal of the American Statistical Association. 61 (314): 436–439.\n > doi:10.2307/2282833. JSTOR 2282833.\n\n See kendalls_tau_correlation_coefficient_naive() for the naive, slow version.\n\nC++: genesis::utils::math::kendalls_tau_correlation_coefficient(const class std::vector<double> &, const class std::vector<double> &, enum genesis::utils::math::KendallsTauMethod) --> double", pybind11::arg("x"), pybind11::arg("y"), pybind11::arg("method"));

	// genesis::utils::math::kendalls_tau_correlation_coefficient_naive(const class std::vector<double> &, const class std::vector<double> &, enum genesis::utils::math::KendallsTauMethod) file:genesis/utils/math/correlation.hpp line:247
	// function-signature: double genesis::utils::math::kendalls_tau_correlation_coefficient_naive(const class std::vector<double> &, const class std::vector<double> &, enum genesis::utils::math::KendallsTauMethod)(const class std::vector<double> &, const class std::vector<double> &, enum genesis::utils::math::KendallsTauMethod) file:genesis/utils/math/correlation.hpp line:247
	M("genesis::utils::math").def("kendalls_tau_correlation_coefficient_naive", [](const class std::vector<double> & a0, const class std::vector<double> & a1) -> double { return genesis::utils::math::kendalls_tau_correlation_coefficient_naive(a0, a1); }, "", pybind11::arg("x"), pybind11::arg("y"));
	M("genesis::utils::math").def("kendalls_tau_correlation_coefficient_naive", (double (*)(const class std::vector<double> &, const class std::vector<double> &, enum genesis::utils::math::KendallsTauMethod)) &genesis::utils::math::kendalls_tau_correlation_coefficient_naive, "Compute a simple version of Kendall's Tau Correlation Coefficient.\n\n This version uses a naive nested loop over the elements.\n It is mainly provided for testing the more advanced implementation,\n see kendalls_tau_correlation_coefficient() for details.\n\nC++: genesis::utils::math::kendalls_tau_correlation_coefficient_naive(const class std::vector<double> &, const class std::vector<double> &, enum genesis::utils::math::KendallsTauMethod) --> double", pybind11::arg("x"), pybind11::arg("y"), pybind11::arg("method"));

	// genesis::utils::math::fisher_transformation(double) file:genesis/utils/math/correlation.hpp line:265
	// function-signature: double genesis::utils::math::fisher_transformation(double)(double) file:genesis/utils/math/correlation.hpp line:265
	M("genesis::utils::math").def("fisher_transformation", (double (*)(double)) &genesis::utils::math::fisher_transformation, "Apply Fisher z-transformation to a correlation coefficient.\n\n The coefficient can be calculated with pearson_correlation_coefficient() or\n spearmans_rank_correlation_coefficient() and has to be in range `[ -1.0, 1.0 ]`.\n\n There is also a version of this function for a vector of coefficients.\n\nC++: genesis::utils::math::fisher_transformation(double) --> double", pybind11::arg("correlation_coefficient"));

	// genesis::utils::math::fisher_transformation(const class std::vector<double> &) file:genesis/utils/math/correlation.hpp line:285
	// function-signature: class std::vector<double> genesis::utils::math::fisher_transformation(const class std::vector<double> &)(const class std::vector<double> &) file:genesis/utils/math/correlation.hpp line:285
	M("genesis::utils::math").def("fisher_transformation", (class std::vector<double> (*)(const class std::vector<double> &)) &genesis::utils::math::fisher_transformation, "Apply Fisher z-transformation to a vector of correlation coefficients.\n\n See fisher_transformation( double ) for details.\n\nC++: genesis::utils::math::fisher_transformation(const class std::vector<double> &) --> class std::vector<double>", pybind11::arg("correlation_coefficients"));

	// genesis::utils::math::multinomial_distribution(const class std::vector<unsigned long> &, unsigned long) file:genesis/utils/math/distribution.hpp line:61
	// function-signature: class std::vector<unsigned long> genesis::utils::math::multinomial_distribution(const class std::vector<unsigned long> &, unsigned long)(const class std::vector<unsigned long> &, unsigned long) file:genesis/utils/math/distribution.hpp line:61
	M("genesis::utils::math").def("multinomial_distribution", (class std::vector<unsigned long> (*)(const class std::vector<unsigned long> &, unsigned long)) &genesis::utils::math::multinomial_distribution, "Select a random sample following a multinomial distribution.\n\n This function computes a random sample formed by  trials from an underlying distribution \n The values in  to be non-negative, and are taken as weights of each category; they do not\n need to be normalized to sum to 1. The function models drawing from multiple categories, each\n with probability as given in  with replacement.\n See multivariate_hypergeometric_distribution() for the equivalent distribution to draw\n _without_ replacement instead.\n\n See https://en.wikipedia.org/wiki/Multinomial_distribution for a description of the distribution.\n See https://www.gnu.org/software/gsl/doc/html/randist.html#the-multinomial-distribution for\n details on the distribution and the used algorithm, which follows C.S. Davis, \"The computer\n generation of multinomial random variates\", Comp. Stat. Data Anal. 16 (1993) 205–217.\n\nC++: genesis::utils::math::multinomial_distribution(const class std::vector<unsigned long> &, unsigned long) --> class std::vector<unsigned long>", pybind11::arg("p"), pybind11::arg("n"));

	// genesis::utils::math::multinomial_distribution(const class std::vector<double> &, unsigned long) file:genesis/utils/math/distribution.hpp line:69
	// function-signature: class std::vector<unsigned long> genesis::utils::math::multinomial_distribution(const class std::vector<double> &, unsigned long)(const class std::vector<double> &, unsigned long) file:genesis/utils/math/distribution.hpp line:69
	M("genesis::utils::math").def("multinomial_distribution", (class std::vector<unsigned long> (*)(const class std::vector<double> &, unsigned long)) &genesis::utils::math::multinomial_distribution, "Select a random sample following a multinomial distribution.\n\n This overload accepts a `std::vector<double>`, and checks that all values are finite and\n non-negative. Otherwise the same.\n\nC++: genesis::utils::math::multinomial_distribution(const class std::vector<double> &, unsigned long) --> class std::vector<unsigned long>", pybind11::arg("p"), pybind11::arg("n"));

	// genesis::utils::math::hypergeometric_distribution(unsigned long, unsigned long, unsigned long) file:genesis/utils/math/distribution.hpp line:79
	// function-signature: unsigned long genesis::utils::math::hypergeometric_distribution(unsigned long, unsigned long, unsigned long)(unsigned long, unsigned long, unsigned long) file:genesis/utils/math/distribution.hpp line:79
	M("genesis::utils::math").def("hypergeometric_distribution", (unsigned long (*)(unsigned long, unsigned long, unsigned long)) &genesis::utils::math::hypergeometric_distribution, "Select a random sample from a hypergeometric distribution.\n\n This is the distribution of drawing balls without replacement from an urn with two colors.\n Here,  and  are the numbers of balls of each color, and  is the number of balls\n to sample, and the return value is the number of balls of the first color.\n See https://en.wikipedia.org/wiki/Hypergeometric_distribution\n\nC++: genesis::utils::math::hypergeometric_distribution(unsigned long, unsigned long, unsigned long) --> unsigned long", pybind11::arg("n1"), pybind11::arg("n2"), pybind11::arg("t"));

	// genesis::utils::math::multivariate_hypergeometric_distribution(const class std::vector<unsigned long> &, unsigned long) file:genesis/utils/math/distribution.hpp line:90
	// function-signature: class std::vector<unsigned long> genesis::utils::math::multivariate_hypergeometric_distribution(const class std::vector<unsigned long> &, unsigned long)(const class std::vector<unsigned long> &, unsigned long) file:genesis/utils/math/distribution.hpp line:90
	M("genesis::utils::math").def("multivariate_hypergeometric_distribution", (class std::vector<unsigned long> (*)(const class std::vector<unsigned long> &, unsigned long)) &genesis::utils::math::multivariate_hypergeometric_distribution, "Select a random sample following a multivariate hypergeometric distribution.\n\n This function computes a random sample formed by  trials from an underlying distribution \n The values in  to be non-negative, and are taken as weights of each category; they do not\n need to be normalized to sum to 1. The function models drawing from multiple categories, each\n with weights as given in  without replacement.\n See multinomial_distribution() for the equivalent distribution to draw _with_ replacement instead.\n\nC++: genesis::utils::math::multivariate_hypergeometric_distribution(const class std::vector<unsigned long> &, unsigned long) --> class std::vector<unsigned long>", pybind11::arg("p"), pybind11::arg("n"));

	{ // genesis::utils::math::Moments file:genesis/utils/math/moments.hpp line:60
		pybind11::class_<genesis::utils::math::Moments, std::shared_ptr<genesis::utils::math::Moments>> cl(M("genesis::utils::math"), "Moments", "Compute running mean and variance for an input.\n\n The class allows to keep a running mean and variance for some input, without\n having to know the number of elements beforehand, and in a single pass.\n\n For convenience, it also keeps track of the total count of elements added, their sum,\n and their minimum and maximum.\n\n This class is modelled after Knuth's algorithm in TAOCP vol 2, 3rd edition, page 232.\n See also https://www.johndcook.com/blog/standard_deviation/\n and https://stackoverflow.com/q/2341340");
		// function-signature: genesis::utils::math::Moments::Moments(unsigned long, bool)(unsigned long, bool) file:genesis/utils/math/moments.hpp line:82
		cl.def( pybind11::init( [](){ return new genesis::utils::math::Moments(); } ), "doc" );
		// function-signature: genesis::utils::math::Moments::Moments(unsigned long, bool)(unsigned long, bool) file:genesis/utils/math/moments.hpp line:82
		cl.def( pybind11::init( [](unsigned long const & a0){ return new genesis::utils::math::Moments(a0); } ), "doc" , pybind11::arg("ddof"));
		// function-signature: genesis::utils::math::Moments::Moments(unsigned long, bool)(unsigned long, bool) file:genesis/utils/math/moments.hpp line:82
		cl.def( pybind11::init<unsigned long, bool>(), pybind11::arg("ddof"), pybind11::arg("ignore_nonfinite") );

		// function-signature: genesis::utils::math::Moments::Moments(const class std::vector<double> &, unsigned long, bool)(const class std::vector<double> &, unsigned long, bool) file:genesis/utils/math/moments.hpp line:116
		cl.def( pybind11::init( [](const class std::vector<double> & a0){ return new genesis::utils::math::Moments(a0); } ), "doc" , pybind11::arg("values"));
		// function-signature: genesis::utils::math::Moments::Moments(const class std::vector<double> &, unsigned long, bool)(const class std::vector<double> &, unsigned long, bool) file:genesis/utils/math/moments.hpp line:116
		cl.def( pybind11::init( [](const class std::vector<double> & a0, unsigned long const & a1){ return new genesis::utils::math::Moments(a0, a1); } ), "doc" , pybind11::arg("values"), pybind11::arg("ddof"));
		// function-signature: genesis::utils::math::Moments::Moments(const class std::vector<double> &, unsigned long, bool)(const class std::vector<double> &, unsigned long, bool) file:genesis/utils/math/moments.hpp line:116
		cl.def( pybind11::init<const class std::vector<double> &, unsigned long, bool>(), pybind11::arg("values"), pybind11::arg("ddof"), pybind11::arg("ignore_nonfinite") );

		// function-signature: genesis::utils::math::Moments::Moments(const class genesis::utils::math::Moments &)(const class genesis::utils::math::Moments &) file:genesis/utils/math/moments.hpp line:125
		cl.def( pybind11::init( [](genesis::utils::math::Moments const &o){ return new genesis::utils::math::Moments(o); } ) );
		// function-signature: class genesis::utils::math::Moments & genesis::utils::math::Moments::operator=(const class genesis::utils::math::Moments &)(const class genesis::utils::math::Moments &) file:genesis/utils/math/moments.hpp line:128
		cl.def("assign", (class genesis::utils::math::Moments & (genesis::utils::math::Moments::*)(const class genesis::utils::math::Moments &)) &genesis::utils::math::Moments::operator=, "C++: genesis::utils::math::Moments::operator=(const class genesis::utils::math::Moments &) --> class genesis::utils::math::Moments &", pybind11::return_value_policy::automatic, pybind11::arg(""));
		// function-signature: void genesis::utils::math::Moments::push(double)(double) file:genesis/utils/math/moments.hpp line:135
		cl.def("push", (void (genesis::utils::math::Moments::*)(double)) &genesis::utils::math::Moments::push, "C++: genesis::utils::math::Moments::push(double) --> void", pybind11::arg("val"));
		// function-signature: void genesis::utils::math::Moments::push(const class std::vector<double> &)(const class std::vector<double> &) file:genesis/utils/math/moments.hpp line:170
		cl.def("push", (void (genesis::utils::math::Moments::*)(const class std::vector<double> &)) &genesis::utils::math::Moments::push, "C++: genesis::utils::math::Moments::push(const class std::vector<double> &) --> void", pybind11::arg("values"));
		// function-signature: unsigned long genesis::utils::math::Moments::count() const() file:genesis/utils/math/moments.hpp line:186
		cl.def("count", (unsigned long (genesis::utils::math::Moments::*)() const) &genesis::utils::math::Moments::count, "C++: genesis::utils::math::Moments::count() const --> unsigned long");
		// function-signature: double genesis::utils::math::Moments::mean() const() file:genesis/utils/math/moments.hpp line:191
		cl.def("mean", (double (genesis::utils::math::Moments::*)() const) &genesis::utils::math::Moments::mean, "C++: genesis::utils::math::Moments::mean() const --> double");
		// function-signature: double genesis::utils::math::Moments::variance() const() file:genesis/utils/math/moments.hpp line:196
		cl.def("variance", (double (genesis::utils::math::Moments::*)() const) &genesis::utils::math::Moments::variance, "C++: genesis::utils::math::Moments::variance() const --> double");
		// function-signature: double genesis::utils::math::Moments::stddev() const() file:genesis/utils/math/moments.hpp line:201
		cl.def("stddev", (double (genesis::utils::math::Moments::*)() const) &genesis::utils::math::Moments::stddev, "C++: genesis::utils::math::Moments::stddev() const --> double");
		// function-signature: double genesis::utils::math::Moments::standard_deviation() const() file:genesis/utils/math/moments.hpp line:206
		cl.def("standard_deviation", (double (genesis::utils::math::Moments::*)() const) &genesis::utils::math::Moments::standard_deviation, "C++: genesis::utils::math::Moments::standard_deviation() const --> double");
		// function-signature: double genesis::utils::math::Moments::sum() const() file:genesis/utils/math/moments.hpp line:211
		cl.def("sum", (double (genesis::utils::math::Moments::*)() const) &genesis::utils::math::Moments::sum, "C++: genesis::utils::math::Moments::sum() const --> double");
		// function-signature: double genesis::utils::math::Moments::min() const() file:genesis/utils/math/moments.hpp line:216
		cl.def("min", (double (genesis::utils::math::Moments::*)() const) &genesis::utils::math::Moments::min, "C++: genesis::utils::math::Moments::min() const --> double");
		// function-signature: double genesis::utils::math::Moments::max() const() file:genesis/utils/math/moments.hpp line:221
		cl.def("max", (double (genesis::utils::math::Moments::*)() const) &genesis::utils::math::Moments::max, "C++: genesis::utils::math::Moments::max() const --> double");
	}
}
