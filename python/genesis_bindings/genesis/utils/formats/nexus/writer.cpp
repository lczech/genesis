#include <genesis/utils/formats/nexus/block.hpp>
#include <genesis/utils/formats/nexus/document.hpp>
#include <genesis/utils/formats/nexus/writer.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/math/ranking.hpp>
#include <genesis/utils/math/statistics.hpp>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
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

void bind_genesis_utils_formats_nexus_writer(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::utils::NexusWriter file:genesis/utils/formats/nexus/writer.hpp line:55
		pybind11::class_<genesis::utils::NexusWriter, std::shared_ptr<genesis::utils::NexusWriter>> cl(M("genesis::utils"), "NexusWriter", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::NexusWriter(); } ) );
		cl.def( pybind11::init( [](genesis::utils::NexusWriter const &o){ return new genesis::utils::NexusWriter(o); } ) );
		cl.def("assign", (class genesis::utils::NexusWriter & (genesis::utils::NexusWriter::*)(const class genesis::utils::NexusWriter &)) &genesis::utils::NexusWriter::operator=, "C++: genesis::utils::NexusWriter::operator=(const class genesis::utils::NexusWriter &) --> class genesis::utils::NexusWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("write", (void (genesis::utils::NexusWriter::*)(const class genesis::utils::NexusDocument &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const) &genesis::utils::NexusWriter::write, "Write a NexusDocument to an output target, using the Nexus format.\n\n See the output target convenience functions utils::to_file(), utils::to_stream(), and\n utils::to_string() for examples of how to obtain a suitable output target.\n\nC++: genesis::utils::NexusWriter::write(const class genesis::utils::NexusDocument &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const --> void", pybind11::arg("document"), pybind11::arg("target"));
		cl.def("to_string", (std::string (genesis::utils::NexusWriter::*)(const class genesis::utils::NexusDocument &) const) &genesis::utils::NexusWriter::to_string, "Directly return a NexusDocument as a string in Nexus format.\n\n This might be useful in some cases.\n\nC++: genesis::utils::NexusWriter::to_string(const class genesis::utils::NexusDocument &) const --> std::string", pybind11::arg("document"));
	}
	{ // genesis::utils::NexusDocument file:genesis/utils/formats/nexus/document.hpp line:54
		pybind11::class_<genesis::utils::NexusDocument, std::shared_ptr<genesis::utils::NexusDocument>> cl(M("genesis::utils"), "NexusDocument", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::NexusDocument(); } ) );
		cl.def("has_block", (bool (genesis::utils::NexusDocument::*)(std::string) const) &genesis::utils::NexusDocument::has_block, "C++: genesis::utils::NexusDocument::has_block(std::string) const --> bool", pybind11::arg("block_name"));
		cl.def("get_block", (class genesis::utils::NexusBlock * (genesis::utils::NexusDocument::*)(std::string)) &genesis::utils::NexusDocument::get_block, "C++: genesis::utils::NexusDocument::get_block(std::string) --> class genesis::utils::NexusBlock *", pybind11::return_value_policy::reference_internal, pybind11::arg("block_name"));
		cl.def("begin", (class __gnu_cxx::__normal_iterator<const class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> > *, class std::vector<class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> >, class std::allocator<class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> > > > > (genesis::utils::NexusDocument::*)() const) &genesis::utils::NexusDocument::begin, "C++: genesis::utils::NexusDocument::begin() const --> class __gnu_cxx::__normal_iterator<const class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> > *, class std::vector<class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> >, class std::allocator<class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> > > > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<const class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> > *, class std::vector<class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> >, class std::allocator<class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> > > > > (genesis::utils::NexusDocument::*)() const) &genesis::utils::NexusDocument::end, "C++: genesis::utils::NexusDocument::end() const --> class __gnu_cxx::__normal_iterator<const class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> > *, class std::vector<class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> >, class std::allocator<class std::unique_ptr<class genesis::utils::NexusBlock, struct std::default_delete<class genesis::utils::NexusBlock> > > > >");
	}
	// genesis::utils::ranking_standard(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) file:genesis/utils/math/ranking.hpp line:61
	M("genesis::utils").def("ranking_standard", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >)) &genesis::utils::ranking_standard<__gnu_cxx::__normal_iterator<const double *, std::vector<double, std::allocator<double> > >>, "C++: genesis::utils::ranking_standard(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("first"), pybind11::arg("last"));

	// genesis::utils::ranking_standard(const class std::vector<double, class std::allocator<double> > &) file:genesis/utils/math/ranking.hpp line:93
	M("genesis::utils").def("ranking_standard", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class std::vector<double, class std::allocator<double> > &)) &genesis::utils::ranking_standard, "ranking_standard( RandomAccessIterator first, RandomAccessIterator last )\n\nC++: genesis::utils::ranking_standard(const class std::vector<double, class std::allocator<double> > &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("vec"));

	// genesis::utils::ranking_modified(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) file:genesis/utils/math/ranking.hpp line:112
	M("genesis::utils").def("ranking_modified", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >)) &genesis::utils::ranking_modified<__gnu_cxx::__normal_iterator<const double *, std::vector<double, std::allocator<double> > >>, "C++: genesis::utils::ranking_modified(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("first"), pybind11::arg("last"));

	// genesis::utils::ranking_modified(const class std::vector<double, class std::allocator<double> > &) file:genesis/utils/math/ranking.hpp line:151
	M("genesis::utils").def("ranking_modified", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class std::vector<double, class std::allocator<double> > &)) &genesis::utils::ranking_modified, "ranking_modified( RandomAccessIterator first, RandomAccessIterator last )\n\nC++: genesis::utils::ranking_modified(const class std::vector<double, class std::allocator<double> > &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("vec"));

	// genesis::utils::ranking_dense(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) file:genesis/utils/math/ranking.hpp line:169
	M("genesis::utils").def("ranking_dense", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >)) &genesis::utils::ranking_dense<__gnu_cxx::__normal_iterator<const double *, std::vector<double, std::allocator<double> > >>, "C++: genesis::utils::ranking_dense(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("first"), pybind11::arg("last"));

	// genesis::utils::ranking_dense(const class std::vector<double, class std::allocator<double> > &) file:genesis/utils/math/ranking.hpp line:201
	M("genesis::utils").def("ranking_dense", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class std::vector<double, class std::allocator<double> > &)) &genesis::utils::ranking_dense, "ranking_dense( RandomAccessIterator first, RandomAccessIterator last )\n\nC++: genesis::utils::ranking_dense(const class std::vector<double, class std::allocator<double> > &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("vec"));

	// genesis::utils::ranking_ordinal(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) file:genesis/utils/math/ranking.hpp line:219
	M("genesis::utils").def("ranking_ordinal", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >)) &genesis::utils::ranking_ordinal<__gnu_cxx::__normal_iterator<const double *, std::vector<double, std::allocator<double> > >>, "C++: genesis::utils::ranking_ordinal(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("first"), pybind11::arg("last"));

	// genesis::utils::ranking_ordinal(const class std::vector<double, class std::allocator<double> > &) file:genesis/utils/math/ranking.hpp line:242
	M("genesis::utils").def("ranking_ordinal", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class std::vector<double, class std::allocator<double> > &)) &genesis::utils::ranking_ordinal, "ranking_ordinal( RandomAccessIterator first, RandomAccessIterator last )\n\nC++: genesis::utils::ranking_ordinal(const class std::vector<double, class std::allocator<double> > &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("vec"));

	// genesis::utils::ranking_fractional(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) file:genesis/utils/math/ranking.hpp line:262
	M("genesis::utils").def("ranking_fractional", (class std::vector<double, class std::allocator<double> > (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >)) &genesis::utils::ranking_fractional<__gnu_cxx::__normal_iterator<const double *, std::vector<double, std::allocator<double> > >>, "C++: genesis::utils::ranking_fractional(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("first"), pybind11::arg("last"));

	// genesis::utils::ranking_fractional(const class std::vector<double, class std::allocator<double> > &) file:genesis/utils/math/ranking.hpp line:321
	M("genesis::utils").def("ranking_fractional", (class std::vector<double, class std::allocator<double> > (*)(const class std::vector<double, class std::allocator<double> > &)) &genesis::utils::ranking_fractional, "ranking_fractional( RandomAccessIterator first, RandomAccessIterator last )\n\nC++: genesis::utils::ranking_fractional(const class std::vector<double, class std::allocator<double> > &) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("vec"));

	{ // genesis::utils::MinMaxPair file:genesis/utils/math/statistics.hpp line:105
		pybind11::class_<genesis::utils::MinMaxPair<double>, std::shared_ptr<genesis::utils::MinMaxPair<double>>> cl(M("genesis::utils"), "MinMaxPair_double_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::MinMaxPair<double>(); } ) );
		cl.def( pybind11::init( [](genesis::utils::MinMaxPair<double> const &o){ return new genesis::utils::MinMaxPair<double>(o); } ) );
		cl.def_readwrite("min", &genesis::utils::MinMaxPair<double>::min);
		cl.def_readwrite("max", &genesis::utils::MinMaxPair<double>::max);
	}
	{ // genesis::utils::MeanStddevPair file:genesis/utils/math/statistics.hpp line:117
		pybind11::class_<genesis::utils::MeanStddevPair, std::shared_ptr<genesis::utils::MeanStddevPair>> cl(M("genesis::utils"), "MeanStddevPair", "Store a mean and a standard deviation value.\n\n This notation is simply more readable than the std default of using a `pair<T, T>` for such\n types.");
		cl.def( pybind11::init( [](){ return new genesis::utils::MeanStddevPair(); } ) );
		cl.def( pybind11::init( [](genesis::utils::MeanStddevPair const &o){ return new genesis::utils::MeanStddevPair(o); } ) );
		cl.def_readwrite("mean", &genesis::utils::MeanStddevPair::mean);
		cl.def_readwrite("stddev", &genesis::utils::MeanStddevPair::stddev);
	}
	{ // genesis::utils::Quartiles file:genesis/utils/math/statistics.hpp line:127
		pybind11::class_<genesis::utils::Quartiles, std::shared_ptr<genesis::utils::Quartiles>> cl(M("genesis::utils"), "Quartiles", "Store the values of quartiles: `q0 == min`, `q1 == 25%`, `q2 == 50%`, `q3 == 75%`,\n `q4 == max`.");
		cl.def( pybind11::init( [](){ return new genesis::utils::Quartiles(); } ) );
		cl.def( pybind11::init( [](genesis::utils::Quartiles const &o){ return new genesis::utils::Quartiles(o); } ) );
		cl.def_readwrite("q0", &genesis::utils::Quartiles::q0);
		cl.def_readwrite("q1", &genesis::utils::Quartiles::q1);
		cl.def_readwrite("q2", &genesis::utils::Quartiles::q2);
		cl.def_readwrite("q3", &genesis::utils::Quartiles::q3);
		cl.def_readwrite("q4", &genesis::utils::Quartiles::q4);
	}
	// genesis::utils::closure(class __gnu_cxx::__normal_iterator<double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<double *, class std::vector<double, class std::allocator<double> > >) file:genesis/utils/math/statistics.hpp line:288
	M("genesis::utils").def("closure", (void (*)(class __gnu_cxx::__normal_iterator<double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<double *, class std::vector<double, class std::allocator<double> > >)) &genesis::utils::closure<__gnu_cxx::__normal_iterator<double *, std::vector<double, std::allocator<double> > >>, "C++: genesis::utils::closure(class __gnu_cxx::__normal_iterator<double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<double *, class std::vector<double, class std::allocator<double> > >) --> void", pybind11::arg("first"), pybind11::arg("last"));

}
