#include <functional>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/taxonomy/iterator/preorder.hpp>
#include <genesis/taxonomy/taxon.hpp>
#include <genesis/taxonomy/taxonomy.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/iterator/path.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/color/functions.hpp>
#include <genesis/utils/containers/range.hpp>
#include <genesis/utils/core/algorithm.hpp>
#include <iterator>
#include <map>
#include <memory>
#include <sstream> // __str__
#include <string>
#include <utility>
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

void bind_genesis_utils_color_functions(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::interpolate(const class genesis::utils::Color &, const class genesis::utils::Color &, double) file:genesis/utils/color/functions.hpp line:106
	M("genesis::utils").def("interpolate", (class genesis::utils::Color (*)(const class genesis::utils::Color &, const class genesis::utils::Color &, double)) &genesis::utils::interpolate, "Linearily interpolate between two Color%s.\n\nC++: genesis::utils::interpolate(const class genesis::utils::Color &, const class genesis::utils::Color &, double) --> class genesis::utils::Color", pybind11::arg("color1"), pybind11::arg("color2"), pybind11::arg("fraction"));

	// genesis::utils::gradient(const class std::map<double, class genesis::utils::Color, struct std::less<double>, class std::allocator<struct std::pair<const double, class genesis::utils::Color> > > &, double) file:genesis/utils/color/functions.hpp line:139
	M("genesis::utils").def("gradient", (class genesis::utils::Color (*)(const class std::map<double, class genesis::utils::Color, struct std::less<double>, class std::allocator<struct std::pair<const double, class genesis::utils::Color> > > &, double)) &genesis::utils::gradient, "Returns a Color that is created using a color gradient.\n\n The function takes a color gradient in form of a map from double to Color.\n The keys in the map are offset positions (stops) with specific Color%s assigned to them.\n An arbitrary number of keys in between these boundaries is allowed.\n\n Each key is associated with a Color value, which indicates the color at that position.\n\n Example:\n\n     // Prepare gradient ranges.\n     auto ranges = std::map<double, Color>();\n     ranges[ 0.0 ] = Color( 0.0, 1.0, 0.0 );\n     ranges[ 0.5 ] = Color( 1.0, 1.0, 0.0 );\n     ranges[ 1.0 ] = Color( 1.0, 0.0, 0.0 );\n\n     // Get color at 30% of the green-yellow-red gradient.\n     Color c = gradient( ranges, 0.3 );\n\n This map defines the same gradient that is used for `heat_gradient`. The range boundaries\n do not have to be `[ 0.0, 1.0 ]`. They can be set to any range that is needed to reflect the\n range of possible values.\n\n The second parameter of this function then takes the position at which the gradient is evaluated.\n Values outside of the interval that is defined by the range are set to the closest interval\n border value.\n\n For example, given the range map above, a value of 0.0 gives green; 0.5 gives yellow;\n 1.0 gives red; values in between are interpolated linearily.\n\nC++: genesis::utils::gradient(const class std::map<double, class genesis::utils::Color, struct std::less<double>, class std::allocator<struct std::pair<const double, class genesis::utils::Color> > > &, double) --> class genesis::utils::Color", pybind11::arg("ranges"), pybind11::arg("value"));

	// genesis::utils::heat_gradient(double) file:genesis/utils/color/functions.hpp line:148
	M("genesis::utils").def("heat_gradient", (class genesis::utils::Color (*)(double)) &genesis::utils::heat_gradient, "Return a Color that represents a heat gradient for a given percentage value.\n\n Given a percentage value in the range of `[ 0.0, 1.0] `, the functions represents the heat of\n that value. For 0.0, the heat gradient value is green, for 0.5 yellow and for 1.0 red.\n The values in between are interpolated linearily.\n\nC++: genesis::utils::heat_gradient(double) --> class genesis::utils::Color", pybind11::arg("percentage"));

	// genesis::utils::contains(const class std::vector<std::string, class std::allocator<std::string > > &, const std::string &) file:genesis/utils/core/algorithm.hpp line:74
	M("genesis::utils").def("contains", (bool (*)(const class std::vector<std::string, class std::allocator<std::string > > &, const std::string &)) &genesis::utils::contains<std::vector<std::string, std::allocator<std::string > >,std::string>, "C++: genesis::utils::contains(const class std::vector<std::string, class std::allocator<std::string > > &, const std::string &) --> bool", pybind11::arg("v"), pybind11::arg("x"));

	// genesis::utils::insert_sorted(class std::vector<std::string, class std::allocator<std::string > > &, const std::string &) file:genesis/utils/core/algorithm.hpp line:152
	M("genesis::utils").def("insert_sorted", (class __gnu_cxx::__normal_iterator<std::string *, class std::vector<std::string, class std::allocator<std::string > > > (*)(class std::vector<std::string, class std::allocator<std::string > > &, const std::string &)) &genesis::utils::insert_sorted<std::string>, "C++: genesis::utils::insert_sorted(class std::vector<std::string, class std::allocator<std::string > > &, const std::string &) --> class __gnu_cxx::__normal_iterator<std::string *, class std::vector<std::string, class std::allocator<std::string > > >", pybind11::arg("vec"), pybind11::arg("item"));

	// genesis::utils::stable_sort_indices(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, struct std::less<double>) file:genesis/utils/core/algorithm.hpp line:239
	M("genesis::utils").def("stable_sort_indices", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, struct std::less<double>)) &genesis::utils::stable_sort_indices<__gnu_cxx::__normal_iterator<const double *, std::vector<double, std::allocator<double> > >,std::less<double>>, "C++: genesis::utils::stable_sort_indices(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, struct std::less<double>) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("first"), pybind11::arg("last"), pybind11::arg("comparator"));

	// genesis::utils::stable_sort_indices(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) file:genesis/utils/core/algorithm.hpp line:281
	M("genesis::utils").def("stable_sort_indices", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >)) &genesis::utils::stable_sort_indices<__gnu_cxx::__normal_iterator<const double *, std::vector<double, std::allocator<double> > >>, "C++: genesis::utils::stable_sort_indices(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double, class std::allocator<double> > >) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("first"), pybind11::arg("last"));

	{ // genesis::utils::Range file:genesis/utils/containers/range.hpp line:46
		pybind11::class_<genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>, std::shared_ptr<genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>>> cl(M("genesis::utils"), "Range_genesis_taxonomy_IteratorPreorder_const_genesis_taxonomy_Taxonomy_const_genesis_taxonomy_Taxon_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>(); } ) );
		cl.def( pybind11::init<class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon>, class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon>>(), pybind11::arg("begin"), pybind11::arg("end") );

		cl.def( pybind11::init( [](genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>> const &o){ return new genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>(o); } ) );
		cl.def("assign", (class genesis::utils::Range<class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> > & (genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>::*)(const class genesis::utils::Range<class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> > &)) &genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>::operator=, "C++: genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>::operator=(const class genesis::utils::Range<class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> > &) --> class genesis::utils::Range<class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> > &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("begin", (class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> (genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>::*)()) &genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>::begin, "C++: genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>::begin() --> class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon>");
		cl.def("end", (class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> (genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>::*)()) &genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>::end, "C++: genesis::utils::Range<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>>::end() --> class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon>");

		extend_range(cl);
	}
	{ // genesis::utils::Range file:genesis/utils/containers/range.hpp line:46
		pybind11::class_<genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>, std::shared_ptr<genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>>> cl(M("genesis::utils"), "Range_genesis_taxonomy_IteratorPreorder_genesis_taxonomy_Taxonomy_genesis_taxonomy_Taxon_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>(); } ) );
		cl.def( pybind11::init<class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon>, class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon>>(), pybind11::arg("begin"), pybind11::arg("end") );

		cl.def( pybind11::init( [](genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>> const &o){ return new genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>(o); } ) );
		cl.def("assign", (class genesis::utils::Range<class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> > & (genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>::*)(const class genesis::utils::Range<class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> > &)) &genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>::operator=, "C++: genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>::operator=(const class genesis::utils::Range<class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> > &) --> class genesis::utils::Range<class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> > &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("begin", (class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> (genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>::*)()) &genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>::begin, "C++: genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>::begin() --> class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon>");
		cl.def("end", (class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> (genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>::*)()) &genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>::end, "C++: genesis::utils::Range<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>>::end() --> class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon>");

		extend_range(cl);
	}
	{ // genesis::utils::Range file:genesis/utils/containers/range.hpp line:46
		pybind11::class_<genesis::utils::Range<genesis::tree::IteratorPath<true>>, std::shared_ptr<genesis::utils::Range<genesis::tree::IteratorPath<true>>>> cl(M("genesis::utils"), "Range_genesis_tree_IteratorPath_true_t", "");
		cl.def( pybind11::init( [](){ return new genesis::utils::Range<genesis::tree::IteratorPath<true>>(); } ) );
		cl.def( pybind11::init<class genesis::tree::IteratorPath<true>, class genesis::tree::IteratorPath<true>>(), pybind11::arg("begin"), pybind11::arg("end") );

		cl.def( pybind11::init( [](genesis::utils::Range<genesis::tree::IteratorPath<true>> const &o){ return new genesis::utils::Range<genesis::tree::IteratorPath<true>>(o); } ) );
		cl.def("assign", (class genesis::utils::Range<class genesis::tree::IteratorPath<true> > & (genesis::utils::Range<genesis::tree::IteratorPath<true>>::*)(const class genesis::utils::Range<class genesis::tree::IteratorPath<true> > &)) &genesis::utils::Range<genesis::tree::IteratorPath<true>>::operator=, "C++: genesis::utils::Range<genesis::tree::IteratorPath<true>>::operator=(const class genesis::utils::Range<class genesis::tree::IteratorPath<true> > &) --> class genesis::utils::Range<class genesis::tree::IteratorPath<true> > &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("begin", (class genesis::tree::IteratorPath<true> (genesis::utils::Range<genesis::tree::IteratorPath<true>>::*)()) &genesis::utils::Range<genesis::tree::IteratorPath<true>>::begin, "C++: genesis::utils::Range<genesis::tree::IteratorPath<true>>::begin() --> class genesis::tree::IteratorPath<true>");
		cl.def("end", (class genesis::tree::IteratorPath<true> (genesis::utils::Range<genesis::tree::IteratorPath<true>>::*)()) &genesis::utils::Range<genesis::tree::IteratorPath<true>>::end, "C++: genesis::utils::Range<genesis::tree::IteratorPath<true>>::end() --> class genesis::tree::IteratorPath<true>");

		extend_range(cl);
	}
}
