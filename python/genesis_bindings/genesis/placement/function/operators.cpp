#include <genesis/placement/function/operators.hpp>
#include <genesis/placement/function/tree.hpp>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/placement/pquery/name.hpp>
#include <genesis/placement/pquery/placement.hpp>
#include <genesis/placement/sample.hpp>
#include <genesis/placement/sample_set.hpp>
#include <genesis/placement/simulator/distributions.hpp>
#include <genesis/placement/simulator/functions.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/tree/tree_set.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/range.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
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

void bind_genesis_placement_function_operators(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::placement::add_sample_to_mass_tree(const class genesis::placement::Sample &, const double, const double, class genesis::tree::Tree &) file:genesis/placement/function/operators.hpp line:117
	M("genesis::placement").def("add_sample_to_mass_tree", (double (*)(const class genesis::placement::Sample &, const double, const double, class genesis::tree::Tree &)) &genesis::placement::add_sample_to_mass_tree, "Helper function to copy masses from a Sample to a\n \n\n\n\n\n\n\n\n\nC++: genesis::placement::add_sample_to_mass_tree(const class genesis::placement::Sample &, const double, const double, class genesis::tree::Tree &) --> double", pybind11::arg("smp"), pybind11::arg("sign"), pybind11::arg("scaler"), pybind11::arg("target"));

	// genesis::placement::convert_sample_to_mass_tree(const class genesis::placement::Sample &, bool) file:genesis/placement/function/operators.hpp line:127
	M("genesis::placement").def("convert_sample_to_mass_tree", (struct std::pair<class genesis::tree::Tree, double> (*)(const class genesis::placement::Sample &, bool)) &genesis::placement::convert_sample_to_mass_tree, "Convert a Sample to a tree::MassTree.\n\n The function takes all PqueryPlacement%s of the Sample and adds their masses in form of the\n `like_weight_ratio` as mass points on a tree::MassTree.\n\nC++: genesis::placement::convert_sample_to_mass_tree(const class genesis::placement::Sample &, bool) --> struct std::pair<class genesis::tree::Tree, double>", pybind11::arg("sample"), pybind11::arg("normalize"));

	// genesis::placement::convert_sample_set_to_mass_trees(const class genesis::placement::SampleSet &, bool) file:genesis/placement/function/operators.hpp line:133
	M("genesis::placement").def("convert_sample_set_to_mass_trees", (struct std::pair<class genesis::tree::TreeSet, class std::vector<double, class std::allocator<double> > > (*)(const class genesis::placement::SampleSet &, bool)) &genesis::placement::convert_sample_set_to_mass_trees, "Convert all Sample%s in a SampleSet to tree::MassTree%s.\n\nC++: genesis::placement::convert_sample_set_to_mass_trees(const class genesis::placement::SampleSet &, bool) --> struct std::pair<class genesis::tree::TreeSet, class std::vector<double, class std::allocator<double> > >", pybind11::arg("sample_set"), pybind11::arg("normalize"));

	// genesis::placement::print_tree(const class genesis::placement::Sample &) file:genesis/placement/function/operators.hpp line:148
	M("genesis::placement").def("print_tree", (std::string (*)(const class genesis::placement::Sample &)) &genesis::placement::print_tree, "Return a simple view of the Tree of a Sample with information about the\n \n\n\n\nC++: genesis::placement::print_tree(const class genesis::placement::Sample &) --> std::string", pybind11::arg("smp"));

	// genesis::placement::labelled_tree(const class genesis::placement::Sample &, bool, const std::string &) file:genesis/placement/function/tree.hpp line:143
	M("genesis::placement").def("labelled_tree", [](const class genesis::placement::Sample & a0) -> genesis::tree::Tree { return genesis::placement::labelled_tree(a0); }, "", pybind11::arg("sample"));
	M("genesis::placement").def("labelled_tree", [](const class genesis::placement::Sample & a0, bool const & a1) -> genesis::tree::Tree { return genesis::placement::labelled_tree(a0, a1); }, "", pybind11::arg("sample"), pybind11::arg("fully_resolve"));
	M("genesis::placement").def("labelled_tree", (class genesis::tree::Tree (*)(const class genesis::placement::Sample &, bool, const std::string &)) &genesis::placement::labelled_tree, "Produce a \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::labelled_tree(const class genesis::placement::Sample &, bool, const std::string &) --> class genesis::tree::Tree", pybind11::arg("sample"), pybind11::arg("fully_resolve"), pybind11::arg("name_prefix"));

	// genesis::placement::labelled_tree(const class genesis::placement::Sample &, const class genesis::tree::Tree &, bool, const std::string &) file:genesis/placement/function/tree.hpp line:164
	M("genesis::placement").def("labelled_tree", [](const class genesis::placement::Sample & a0, const class genesis::tree::Tree & a1) -> genesis::tree::Tree { return genesis::placement::labelled_tree(a0, a1); }, "", pybind11::arg("sample"), pybind11::arg("tree"));
	M("genesis::placement").def("labelled_tree", [](const class genesis::placement::Sample & a0, const class genesis::tree::Tree & a1, bool const & a2) -> genesis::tree::Tree { return genesis::placement::labelled_tree(a0, a1, a2); }, "", pybind11::arg("sample"), pybind11::arg("tree"), pybind11::arg("fully_resolve"));
	M("genesis::placement").def("labelled_tree", (class genesis::tree::Tree (*)(const class genesis::placement::Sample &, const class genesis::tree::Tree &, bool, const std::string &)) &genesis::placement::labelled_tree, "Produce a \n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::labelled_tree(const class genesis::placement::Sample &, const class genesis::tree::Tree &, bool, const std::string &) --> class genesis::tree::Tree", pybind11::arg("sample"), pybind11::arg("tree"), pybind11::arg("fully_resolve"), pybind11::arg("name_prefix"));

	{ // genesis::placement::SimulatorEdgeDistribution file:genesis/placement/simulator/distributions.hpp line:49
		pybind11::class_<genesis::placement::SimulatorEdgeDistribution, std::shared_ptr<genesis::placement::SimulatorEdgeDistribution>> cl(M("genesis::placement"), "SimulatorEdgeDistribution", "");
		cl.def( pybind11::init( [](){ return new genesis::placement::SimulatorEdgeDistribution(); } ) );
		cl.def( pybind11::init( [](genesis::placement::SimulatorEdgeDistribution const &o){ return new genesis::placement::SimulatorEdgeDistribution(o); } ) );
		cl.def_readwrite("edge_weights", &genesis::placement::SimulatorEdgeDistribution::edge_weights);
		cl.def("assign", (class genesis::placement::SimulatorEdgeDistribution & (genesis::placement::SimulatorEdgeDistribution::*)(const class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::SimulatorEdgeDistribution::operator=, "C++: genesis::placement::SimulatorEdgeDistribution::operator=(const class genesis::placement::SimulatorEdgeDistribution &) --> class genesis::placement::SimulatorEdgeDistribution &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("prepare", (void (genesis::placement::SimulatorEdgeDistribution::*)(const class genesis::placement::Sample &)) &genesis::placement::SimulatorEdgeDistribution::prepare, "Prepare the distribution for usage. Needs to be called before generate().\n\nC++: genesis::placement::SimulatorEdgeDistribution::prepare(const class genesis::placement::Sample &) --> void", pybind11::arg("sample"));
		cl.def("generate", (unsigned long (genesis::placement::SimulatorEdgeDistribution::*)()) &genesis::placement::SimulatorEdgeDistribution::generate, "Return a randomly chosen edge index, according to the distribution.\n\nC++: genesis::placement::SimulatorEdgeDistribution::generate() --> unsigned long");

		cl.def("__str__", [](genesis::placement::SimulatorEdgeDistribution const &o) -> std::string { std::ostringstream s; genesis::placement::operator<<(s, o); return s.str(); } );
	}
	{ // genesis::placement::SimulatorExtraPlacementDistribution file:genesis/placement/simulator/distributions.hpp line:126
		pybind11::class_<genesis::placement::SimulatorExtraPlacementDistribution, std::shared_ptr<genesis::placement::SimulatorExtraPlacementDistribution>> cl(M("genesis::placement"), "SimulatorExtraPlacementDistribution", "Generate a certain number of additional PqueryPlacement%s around a given\n ::PlacementTreeEdge.\n\n This class models both the distribution of the number of generated placements and their\n distribution around the given edge. It is easier to model it this way (both distributions in one\n class), because they have a dependency: Once the number of placements is drawn, they have to be\n distributed along the edges of the tree without repetition according to a distribution of\n distances (path lengths) from the central edge. Doing those two steps in one class is easier.");
		cl.def( pybind11::init( [](){ return new genesis::placement::SimulatorExtraPlacementDistribution(); } ) );
		cl.def( pybind11::init( [](genesis::placement::SimulatorExtraPlacementDistribution const &o){ return new genesis::placement::SimulatorExtraPlacementDistribution(o); } ) );
		cl.def_readwrite("placement_number_weights", &genesis::placement::SimulatorExtraPlacementDistribution::placement_number_weights);
		cl.def_readwrite("placement_path_length_weights", &genesis::placement::SimulatorExtraPlacementDistribution::placement_path_length_weights);
		cl.def("assign", (class genesis::placement::SimulatorExtraPlacementDistribution & (genesis::placement::SimulatorExtraPlacementDistribution::*)(const class genesis::placement::SimulatorExtraPlacementDistribution &)) &genesis::placement::SimulatorExtraPlacementDistribution::operator=, "C++: genesis::placement::SimulatorExtraPlacementDistribution::operator=(const class genesis::placement::SimulatorExtraPlacementDistribution &) --> class genesis::placement::SimulatorExtraPlacementDistribution &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("prepare", (void (genesis::placement::SimulatorExtraPlacementDistribution::*)(const class genesis::placement::Sample &)) &genesis::placement::SimulatorExtraPlacementDistribution::prepare, "C++: genesis::placement::SimulatorExtraPlacementDistribution::prepare(const class genesis::placement::Sample &) --> void", pybind11::arg("sample"));
		cl.def("generate", (class std::vector<unsigned long, class std::allocator<unsigned long> > (genesis::placement::SimulatorExtraPlacementDistribution::*)(const class genesis::tree::TreeEdge &)) &genesis::placement::SimulatorExtraPlacementDistribution::generate, "C++: genesis::placement::SimulatorExtraPlacementDistribution::generate(const class genesis::tree::TreeEdge &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("edge"));
		cl.def("dump_edge_proximities", (std::string (genesis::placement::SimulatorExtraPlacementDistribution::*)() const) &genesis::placement::SimulatorExtraPlacementDistribution::dump_edge_proximities, "C++: genesis::placement::SimulatorExtraPlacementDistribution::dump_edge_proximities() const --> std::string");
		cl.def("edge_proximity_maxima", (class std::vector<unsigned long, class std::allocator<unsigned long> > (genesis::placement::SimulatorExtraPlacementDistribution::*)() const) &genesis::placement::SimulatorExtraPlacementDistribution::edge_proximity_maxima, "C++: genesis::placement::SimulatorExtraPlacementDistribution::edge_proximity_maxima() const --> class std::vector<unsigned long, class std::allocator<unsigned long> >");

		cl.def("__str__", [](genesis::placement::SimulatorExtraPlacementDistribution const &o) -> std::string { std::ostringstream s; genesis::placement::operator<<(s, o); return s.str(); } );

		{ // genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity file:genesis/placement/simulator/distributions.hpp line:134
			auto & enclosing_class = cl;
			pybind11::class_<genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity, std::shared_ptr<genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity>> cl(enclosing_class, "EdgeProximity", "");
			cl.def( pybind11::init( [](){ return new genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity(); } ) );
			cl.def( pybind11::init( [](genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity const &o){ return new genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity(o); } ) );
			cl.def_readwrite("candidates_per_level", &genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity::candidates_per_level);
			cl.def_readwrite("total_candidates", &genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity::total_candidates);
			cl.def("assign", (struct genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity & (genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity::*)(const struct genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity &)) &genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity::operator=, "C++: genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity::operator=(const struct genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity &) --> struct genesis::placement::SimulatorExtraPlacementDistribution::EdgeProximity &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		}

	}
	{ // genesis::placement::SimulatorLikeWeightRatioDistribution file:genesis/placement/simulator/distributions.hpp line:185
		pybind11::class_<genesis::placement::SimulatorLikeWeightRatioDistribution, std::shared_ptr<genesis::placement::SimulatorLikeWeightRatioDistribution>> cl(M("genesis::placement"), "SimulatorLikeWeightRatioDistribution", "");
		cl.def( pybind11::init( [](){ return new genesis::placement::SimulatorLikeWeightRatioDistribution(); } ) );
		cl.def( pybind11::init( [](genesis::placement::SimulatorLikeWeightRatioDistribution const &o){ return new genesis::placement::SimulatorLikeWeightRatioDistribution(o); } ) );
		cl.def_readwrite("intervals", &genesis::placement::SimulatorLikeWeightRatioDistribution::intervals);
		cl.def_readwrite("weights", &genesis::placement::SimulatorLikeWeightRatioDistribution::weights);
		cl.def("assign", (class genesis::placement::SimulatorLikeWeightRatioDistribution & (genesis::placement::SimulatorLikeWeightRatioDistribution::*)(const class genesis::placement::SimulatorLikeWeightRatioDistribution &)) &genesis::placement::SimulatorLikeWeightRatioDistribution::operator=, "C++: genesis::placement::SimulatorLikeWeightRatioDistribution::operator=(const class genesis::placement::SimulatorLikeWeightRatioDistribution &) --> class genesis::placement::SimulatorLikeWeightRatioDistribution &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("prepare", (void (genesis::placement::SimulatorLikeWeightRatioDistribution::*)(const class genesis::placement::Sample &)) &genesis::placement::SimulatorLikeWeightRatioDistribution::prepare, "Prepare the distribution for usage. Needs to be called before generate().\n\nC++: genesis::placement::SimulatorLikeWeightRatioDistribution::prepare(const class genesis::placement::Sample &) --> void", pybind11::arg("sample"));
		cl.def("generate", (double (genesis::placement::SimulatorLikeWeightRatioDistribution::*)()) &genesis::placement::SimulatorLikeWeightRatioDistribution::generate, "Return a randomly chosen like weight ratio.\n\nC++: genesis::placement::SimulatorLikeWeightRatioDistribution::generate() --> double");

		cl.def("__str__", [](genesis::placement::SimulatorLikeWeightRatioDistribution const &o) -> std::string { std::ostringstream s; genesis::placement::operator<<(s, o); return s.str(); } );
	}
	{ // genesis::placement::SimulatorPendantLengthDistribution file:genesis/placement/simulator/distributions.hpp line:237
		pybind11::class_<genesis::placement::SimulatorPendantLengthDistribution, std::shared_ptr<genesis::placement::SimulatorPendantLengthDistribution>> cl(M("genesis::placement"), "SimulatorPendantLengthDistribution", "");
		cl.def( pybind11::init( [](){ return new genesis::placement::SimulatorPendantLengthDistribution(); } ) );
		cl.def( pybind11::init( [](genesis::placement::SimulatorPendantLengthDistribution const &o){ return new genesis::placement::SimulatorPendantLengthDistribution(o); } ) );
		cl.def_readwrite("min", &genesis::placement::SimulatorPendantLengthDistribution::min);
		cl.def_readwrite("max", &genesis::placement::SimulatorPendantLengthDistribution::max);
		cl.def("assign", (class genesis::placement::SimulatorPendantLengthDistribution & (genesis::placement::SimulatorPendantLengthDistribution::*)(const class genesis::placement::SimulatorPendantLengthDistribution &)) &genesis::placement::SimulatorPendantLengthDistribution::operator=, "C++: genesis::placement::SimulatorPendantLengthDistribution::operator=(const class genesis::placement::SimulatorPendantLengthDistribution &) --> class genesis::placement::SimulatorPendantLengthDistribution &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("prepare", (void (genesis::placement::SimulatorPendantLengthDistribution::*)(const class genesis::placement::Sample &)) &genesis::placement::SimulatorPendantLengthDistribution::prepare, "Prepare the distribution for usage. Needs to be called before generate().\n\nC++: genesis::placement::SimulatorPendantLengthDistribution::prepare(const class genesis::placement::Sample &) --> void", pybind11::arg("sample"));
		cl.def("generate", (double (genesis::placement::SimulatorPendantLengthDistribution::*)(const class genesis::tree::TreeEdge &)) &genesis::placement::SimulatorPendantLengthDistribution::generate, "Return a randomly chosen position on an edge.\n\nC++: genesis::placement::SimulatorPendantLengthDistribution::generate(const class genesis::tree::TreeEdge &) --> double", pybind11::arg("edge"));
	}
}
