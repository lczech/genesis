#include <genesis/utils/containers/interval_tree/interval.hpp>
#include <genesis/utils/containers/interval_tree/node.hpp>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/math/matrix.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

void bind_genesis_utils_math_matrix(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::correlation_matrix(const class genesis::utils::Matrix<double> &) file:genesis/utils/math/matrix.hpp line:353
	M("genesis::utils").def("correlation_matrix", (class genesis::utils::Matrix<double> (*)(const class genesis::utils::Matrix<double> &)) &genesis::utils::correlation_matrix, "Calculate the correlation Matrix of a given `data` Matrix.\n\n This is done by standardizing the mean and variance of the given `data` and then calculating the\n sums_of_squares_and_cross_products_matrix().\n\nC++: genesis::utils::correlation_matrix(const class genesis::utils::Matrix<double> &) --> class genesis::utils::Matrix<double>", pybind11::arg("data"));

	// genesis::utils::covariance_matrix(const class genesis::utils::Matrix<double> &) file:genesis/utils/math/matrix.hpp line:361
	M("genesis::utils").def("covariance_matrix", (class genesis::utils::Matrix<double> (*)(const class genesis::utils::Matrix<double> &)) &genesis::utils::covariance_matrix, "Calculate the covariance Matrix of a given `data` Matrix.\n\n This is done by standardizing the mean of the given `data` and then calculating the\n sums_of_squares_and_cross_products_matrix().\n\nC++: genesis::utils::covariance_matrix(const class genesis::utils::Matrix<double> &) --> class genesis::utils::Matrix<double>", pybind11::arg("data"));

	// genesis::utils::sums_of_squares_and_cross_products_matrix(const class genesis::utils::Matrix<double> &) file:genesis/utils/math/matrix.hpp line:366
	M("genesis::utils").def("sums_of_squares_and_cross_products_matrix", (class genesis::utils::Matrix<double> (*)(const class genesis::utils::Matrix<double> &)) &genesis::utils::sums_of_squares_and_cross_products_matrix, "Calculate the Sums of Squares and Cross Products Matrix (SSCP Matrix).\n\nC++: genesis::utils::sums_of_squares_and_cross_products_matrix(const class genesis::utils::Matrix<double> &) --> class genesis::utils::Matrix<double>", pybind11::arg("data"));

	// genesis::utils::matrix_multiplication(const class genesis::utils::Matrix<signed char> &, const class std::vector<double, class std::allocator<double> > &) file:genesis/utils/math/matrix.hpp line:602
	M("genesis::utils").def("matrix_multiplication", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::utils::Matrix<signed char> &, const class std::vector<double, class std::allocator<double> > &)) &genesis::utils::matrix_multiplication<double,signed char,double>, "C++: genesis::utils::matrix_multiplication(const class genesis::utils::Matrix<signed char> &, const class std::vector<double, class std::allocator<double> > &) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("a"), pybind11::arg("b"));

	{ // genesis::utils::EmptyIntervalData file:genesis/utils/containers/interval_tree/interval.hpp line:70
		pybind11::class_<genesis::utils::EmptyIntervalData, std::shared_ptr<genesis::utils::EmptyIntervalData>> cl(M("genesis::utils"), "EmptyIntervalData", "Empty class used as default for Interval data.\n\n An Interval offers to store some data with with it, which is for example useful when working\n with annotated genome regions. By default however, we do not want to store any data,\n and use this class as a dummy,");
		cl.def( pybind11::init( [](){ return new genesis::utils::EmptyIntervalData(); } ) );
	}
	{ // genesis::utils::IntervalLeftOpen file:genesis/utils/containers/interval_tree/interval.hpp line:84
		pybind11::class_<genesis::utils::IntervalLeftOpen, std::shared_ptr<genesis::utils::IntervalLeftOpen>> cl(M("genesis::utils"), "IntervalLeftOpen", "Helper type to define a left open `(]` Interval.");
		cl.def( pybind11::init( [](){ return new genesis::utils::IntervalLeftOpen(); } ) );
	}
	{ // genesis::utils::IntervalRightOpen file:genesis/utils/containers/interval_tree/interval.hpp line:118
		pybind11::class_<genesis::utils::IntervalRightOpen, std::shared_ptr<genesis::utils::IntervalRightOpen>> cl(M("genesis::utils"), "IntervalRightOpen", "Helper type to define a right open `[)` Interval.");
		cl.def( pybind11::init( [](){ return new genesis::utils::IntervalRightOpen(); } ) );
	}
	{ // genesis::utils::IntervalOpen file:genesis/utils/containers/interval_tree/interval.hpp line:152
		pybind11::class_<genesis::utils::IntervalOpen, std::shared_ptr<genesis::utils::IntervalOpen>> cl(M("genesis::utils"), "IntervalOpen", "Helper type to define an open `()` Interval.");
		cl.def( pybind11::init( [](){ return new genesis::utils::IntervalOpen(); } ) );
	}
	{ // genesis::utils::IntervalClosed file:genesis/utils/containers/interval_tree/interval.hpp line:208
		pybind11::class_<genesis::utils::IntervalClosed, std::shared_ptr<genesis::utils::IntervalClosed>> cl(M("genesis::utils"), "IntervalClosed", "Helper type to define a closed `[]` Interval.");
		cl.def( pybind11::init( [](){ return new genesis::utils::IntervalClosed(); } ) );
	}
	// genesis::utils::RedBackColor file:genesis/utils/containers/interval_tree/node.hpp line:65
	pybind11::enum_<genesis::utils::RedBackColor>(M("genesis::utils"), "RedBackColor", "Definition for Red-Black Tree coloring.\n\n Used in IntervalTree and IntervalTreeNode.")
		.value("kInvalid", genesis::utils::RedBackColor::kInvalid)
		.value("kRed", genesis::utils::RedBackColor::kRed)
		.value("kBlack", genesis::utils::RedBackColor::kBlack)
		.value("kKDoubleBlack", genesis::utils::RedBackColor::kKDoubleBlack);

;

}
