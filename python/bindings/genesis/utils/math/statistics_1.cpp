#include <genesis/utils/math/statistics.hpp>
#include <iterator>
#include <memory>
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

void bind_genesis_utils_math_statistics_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::math::coefficient_of_variation(const class std::vector<struct genesis::utils::math::MeanStddevPair> &) file:genesis/utils/math/statistics.hpp line:1201
	// function-signature: class std::vector<double> genesis::utils::math::coefficient_of_variation(const class std::vector<struct genesis::utils::math::MeanStddevPair> &)(const class std::vector<struct genesis::utils::math::MeanStddevPair> &) file:genesis/utils/math/statistics.hpp line:1201
	M("genesis::utils::math").def("coefficient_of_variation", (class std::vector<double> (*)(const class std::vector<struct genesis::utils::math::MeanStddevPair> &)) &genesis::utils::math::coefficient_of_variation, "MeanStddevPair const& ms )\n\nC++: genesis::utils::math::coefficient_of_variation(const class std::vector<struct genesis::utils::math::MeanStddevPair> &) --> class std::vector<double>", pybind11::arg("ms"));

	// genesis::utils::math::index_of_dispersion(const struct genesis::utils::math::MeanStddevPair &) file:genesis/utils/math/statistics.hpp line:1219
	// function-signature: double genesis::utils::math::index_of_dispersion(const struct genesis::utils::math::MeanStddevPair &)(const struct genesis::utils::math::MeanStddevPair &) file:genesis/utils/math/statistics.hpp line:1219
	M("genesis::utils::math").def("index_of_dispersion", (double (*)(const struct genesis::utils::math::MeanStddevPair &)) &genesis::utils::math::index_of_dispersion, "Calculate the index of dispersion.\n\n The index of dispersion, also known as the dispersion index, coefficient of dispersion,\n relative variance, variance-to-mean ratio (VMR) or Fano factor, is defined as the ratio of the\n variance to the mean. Variance is the square of the standard deviation.\n See mean_stddev() to calculate those values.\n See https://en.wikipedia.org/wiki/Index_of_dispersion for details.\n\nC++: genesis::utils::math::index_of_dispersion(const struct genesis::utils::math::MeanStddevPair &) --> double", pybind11::arg("ms"));

	// genesis::utils::math::index_of_dispersion(const class std::vector<struct genesis::utils::math::MeanStddevPair> &) file:genesis/utils/math/statistics.hpp line:1227
	// function-signature: class std::vector<double> genesis::utils::math::index_of_dispersion(const class std::vector<struct genesis::utils::math::MeanStddevPair> &)(const class std::vector<struct genesis::utils::math::MeanStddevPair> &) file:genesis/utils/math/statistics.hpp line:1227
	M("genesis::utils::math").def("index_of_dispersion", (class std::vector<double> (*)(const class std::vector<struct genesis::utils::math::MeanStddevPair> &)) &genesis::utils::math::index_of_dispersion, "MeanStddevPair const& ms )\n\nC++: genesis::utils::math::index_of_dispersion(const class std::vector<struct genesis::utils::math::MeanStddevPair> &) --> class std::vector<double>", pybind11::arg("ms"));

}
