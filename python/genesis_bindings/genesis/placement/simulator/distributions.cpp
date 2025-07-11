#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/placement/pquery/name.hpp>
#include <genesis/placement/pquery/placement.hpp>
#include <genesis/placement/sample.hpp>
#include <genesis/placement/simulator/distributions.hpp>
#include <genesis/placement/simulator/functions.hpp>
#include <genesis/placement/simulator/simulator.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/range.hpp>
#include <iterator>
#include <memory>
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

void bind_genesis_placement_simulator_distributions(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::placement::SimulatorProximalLengthDistribution file:genesis/placement/simulator/distributions.hpp line:296
		pybind11::class_<genesis::placement::SimulatorProximalLengthDistribution, std::shared_ptr<genesis::placement::SimulatorProximalLengthDistribution>> cl(M("genesis::placement"), "SimulatorProximalLengthDistribution", "");
		cl.def( pybind11::init( [](){ return new genesis::placement::SimulatorProximalLengthDistribution(); } ) );
		cl.def( pybind11::init( [](genesis::placement::SimulatorProximalLengthDistribution const &o){ return new genesis::placement::SimulatorProximalLengthDistribution(o); } ) );
		cl.def("assign", (class genesis::placement::SimulatorProximalLengthDistribution & (genesis::placement::SimulatorProximalLengthDistribution::*)(const class genesis::placement::SimulatorProximalLengthDistribution &)) &genesis::placement::SimulatorProximalLengthDistribution::operator=, "C++: genesis::placement::SimulatorProximalLengthDistribution::operator=(const class genesis::placement::SimulatorProximalLengthDistribution &) --> class genesis::placement::SimulatorProximalLengthDistribution &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("prepare", (void (genesis::placement::SimulatorProximalLengthDistribution::*)(const class genesis::placement::Sample &)) &genesis::placement::SimulatorProximalLengthDistribution::prepare, "Prepare the distribution for usage. Needs to be called before generate().\n\nC++: genesis::placement::SimulatorProximalLengthDistribution::prepare(const class genesis::placement::Sample &) --> void", pybind11::arg("sample"));
		cl.def("generate", (double (genesis::placement::SimulatorProximalLengthDistribution::*)(const class genesis::tree::TreeEdge &)) &genesis::placement::SimulatorProximalLengthDistribution::generate, "Return a randomly chosen position on an edge.\n\nC++: genesis::placement::SimulatorProximalLengthDistribution::generate(const class genesis::tree::TreeEdge &) --> double", pybind11::arg("edge"));
	}
	// genesis::placement::set_uniform_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) file:genesis/placement/simulator/functions.hpp line:65
	M("genesis::placement").def("set_uniform_weights", (void (*)(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::set_uniform_weights, "C++: genesis::placement::set_uniform_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) --> void", pybind11::arg("sample"), pybind11::arg("edge_distrib"));

	// genesis::placement::set_uniform_weights(unsigned long, class genesis::placement::SimulatorEdgeDistribution &) file:genesis/placement/simulator/functions.hpp line:66
	M("genesis::placement").def("set_uniform_weights", (void (*)(unsigned long, class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::set_uniform_weights, "C++: genesis::placement::set_uniform_weights(unsigned long, class genesis::placement::SimulatorEdgeDistribution &) --> void", pybind11::arg("edge_count"), pybind11::arg("edge_distrib"));

	// genesis::placement::set_random_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) file:genesis/placement/simulator/functions.hpp line:68
	M("genesis::placement").def("set_random_weights", (void (*)(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::set_random_weights, "C++: genesis::placement::set_random_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) --> void", pybind11::arg("sample"), pybind11::arg("edge_distrib"));

	// genesis::placement::set_random_weights(unsigned long, class genesis::placement::SimulatorEdgeDistribution &) file:genesis/placement/simulator/functions.hpp line:69
	M("genesis::placement").def("set_random_weights", (void (*)(unsigned long, class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::set_random_weights, "C++: genesis::placement::set_random_weights(unsigned long, class genesis::placement::SimulatorEdgeDistribution &) --> void", pybind11::arg("edge_count"), pybind11::arg("edge_distrib"));

	// genesis::placement::set_random_edges(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) file:genesis/placement/simulator/functions.hpp line:71
	M("genesis::placement").def("set_random_edges", (void (*)(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::set_random_edges, "C++: genesis::placement::set_random_edges(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) --> void", pybind11::arg("sample"), pybind11::arg("edge_distrib"));

	// genesis::placement::set_random_edges(unsigned long, class genesis::placement::SimulatorEdgeDistribution &) file:genesis/placement/simulator/functions.hpp line:72
	M("genesis::placement").def("set_random_edges", (void (*)(unsigned long, class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::set_random_edges, "C++: genesis::placement::set_random_edges(unsigned long, class genesis::placement::SimulatorEdgeDistribution &) --> void", pybind11::arg("edge_count"), pybind11::arg("edge_distrib"));

	// genesis::placement::set_depths_distributed_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) file:genesis/placement/simulator/functions.hpp line:74
	M("genesis::placement").def("set_depths_distributed_weights", (void (*)(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::set_depths_distributed_weights, "C++: genesis::placement::set_depths_distributed_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) --> void", pybind11::arg("sample"), pybind11::arg("edge_distrib"));

	// genesis::placement::set_depths_distributed_weights(const class genesis::placement::Sample &, const class std::vector<double, class std::allocator<double> > &, class genesis::placement::SimulatorEdgeDistribution &) file:genesis/placement/simulator/functions.hpp line:79
	M("genesis::placement").def("set_depths_distributed_weights", (void (*)(const class genesis::placement::Sample &, const class std::vector<double, class std::allocator<double> > &, class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::set_depths_distributed_weights, "C++: genesis::placement::set_depths_distributed_weights(const class genesis::placement::Sample &, const class std::vector<double, class std::allocator<double> > &, class genesis::placement::SimulatorEdgeDistribution &) --> void", pybind11::arg("sample"), pybind11::arg("depth_weights"), pybind11::arg("edge_distrib"));

	// genesis::placement::set_random_subtree_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) file:genesis/placement/simulator/functions.hpp line:85
	M("genesis::placement").def("set_random_subtree_weights", (unsigned long (*)(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::set_random_subtree_weights, "C++: genesis::placement::set_random_subtree_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) --> unsigned long", pybind11::arg("sample"), pybind11::arg("edge_distrib"));

	// genesis::placement::set_subtree_weights(const class genesis::placement::Sample &, unsigned long, class genesis::placement::SimulatorEdgeDistribution &) file:genesis/placement/simulator/functions.hpp line:90
	M("genesis::placement").def("set_subtree_weights", (void (*)(const class genesis::placement::Sample &, unsigned long, class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::set_subtree_weights, "C++: genesis::placement::set_subtree_weights(const class genesis::placement::Sample &, unsigned long, class genesis::placement::SimulatorEdgeDistribution &) --> void", pybind11::arg("sample"), pybind11::arg("link_index"), pybind11::arg("edge_distrib"));

	// genesis::placement::learn_per_edge_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) file:genesis/placement/simulator/functions.hpp line:96
	M("genesis::placement").def("learn_per_edge_weights", (void (*)(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &)) &genesis::placement::learn_per_edge_weights, "C++: genesis::placement::learn_per_edge_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorEdgeDistribution &) --> void", pybind11::arg("sample"), pybind11::arg("edge_distrib"));

	// genesis::placement::learn_placement_number_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorExtraPlacementDistribution &) file:genesis/placement/simulator/functions.hpp line:102
	M("genesis::placement").def("learn_placement_number_weights", (void (*)(const class genesis::placement::Sample &, class genesis::placement::SimulatorExtraPlacementDistribution &)) &genesis::placement::learn_placement_number_weights, "C++: genesis::placement::learn_placement_number_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorExtraPlacementDistribution &) --> void", pybind11::arg("sample"), pybind11::arg("p_distib"));

	// genesis::placement::learn_placement_path_length_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorExtraPlacementDistribution &) file:genesis/placement/simulator/functions.hpp line:107
	M("genesis::placement").def("learn_placement_path_length_weights", (void (*)(const class genesis::placement::Sample &, class genesis::placement::SimulatorExtraPlacementDistribution &)) &genesis::placement::learn_placement_path_length_weights, "C++: genesis::placement::learn_placement_path_length_weights(const class genesis::placement::Sample &, class genesis::placement::SimulatorExtraPlacementDistribution &) --> void", pybind11::arg("sample"), pybind11::arg("p_distib"));

	// genesis::placement::learn_like_weight_ratio_distribution(const class genesis::placement::Sample &, class genesis::placement::SimulatorLikeWeightRatioDistribution &, unsigned long) file:genesis/placement/simulator/functions.hpp line:116
	M("genesis::placement").def("learn_like_weight_ratio_distribution", [](const class genesis::placement::Sample & a0, class genesis::placement::SimulatorLikeWeightRatioDistribution & a1) -> void { return genesis::placement::learn_like_weight_ratio_distribution(a0, a1); }, "", pybind11::arg("sample"), pybind11::arg("lwr_distib"));
	M("genesis::placement").def("learn_like_weight_ratio_distribution", (void (*)(const class genesis::placement::Sample &, class genesis::placement::SimulatorLikeWeightRatioDistribution &, unsigned long)) &genesis::placement::learn_like_weight_ratio_distribution, "C++: genesis::placement::learn_like_weight_ratio_distribution(const class genesis::placement::Sample &, class genesis::placement::SimulatorLikeWeightRatioDistribution &, unsigned long) --> void", pybind11::arg("sample"), pybind11::arg("lwr_distib"), pybind11::arg("number_of_intervals"));

	{ // genesis::placement::Simulator file:genesis/placement/simulator/simulator.hpp line:48
		pybind11::class_<genesis::placement::Simulator, std::shared_ptr<genesis::placement::Simulator>> cl(M("genesis::placement"), "Simulator", "Simulate \n");
		cl.def( pybind11::init( [](){ return new genesis::placement::Simulator(); } ) );
		cl.def( pybind11::init( [](genesis::placement::Simulator const &o){ return new genesis::placement::Simulator(o); } ) );
		cl.def("assign", (class genesis::placement::Simulator & (genesis::placement::Simulator::*)(const class genesis::placement::Simulator &)) &genesis::placement::Simulator::operator=, "C++: genesis::placement::Simulator::operator=(const class genesis::placement::Simulator &) --> class genesis::placement::Simulator &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("generate", (void (genesis::placement::Simulator::*)(class genesis::placement::Sample &, unsigned long)) &genesis::placement::Simulator::generate, "C++: genesis::placement::Simulator::generate(class genesis::placement::Sample &, unsigned long) --> void", pybind11::arg("sample"), pybind11::arg("n"));
		cl.def("edge_distribution", (class genesis::placement::SimulatorEdgeDistribution & (genesis::placement::Simulator::*)()) &genesis::placement::Simulator::edge_distribution, "C++: genesis::placement::Simulator::edge_distribution() --> class genesis::placement::SimulatorEdgeDistribution &", pybind11::return_value_policy::reference_internal);
		cl.def("extra_placement_distribution", (class genesis::placement::SimulatorExtraPlacementDistribution & (genesis::placement::Simulator::*)()) &genesis::placement::Simulator::extra_placement_distribution, "C++: genesis::placement::Simulator::extra_placement_distribution() --> class genesis::placement::SimulatorExtraPlacementDistribution &", pybind11::return_value_policy::reference_internal);
		cl.def("like_weight_ratio_distribution", (class genesis::placement::SimulatorLikeWeightRatioDistribution & (genesis::placement::Simulator::*)()) &genesis::placement::Simulator::like_weight_ratio_distribution, "C++: genesis::placement::Simulator::like_weight_ratio_distribution() --> class genesis::placement::SimulatorLikeWeightRatioDistribution &", pybind11::return_value_policy::reference_internal);
		cl.def("proximal_length_distribution", (class genesis::placement::SimulatorProximalLengthDistribution & (genesis::placement::Simulator::*)()) &genesis::placement::Simulator::proximal_length_distribution, "C++: genesis::placement::Simulator::proximal_length_distribution() --> class genesis::placement::SimulatorProximalLengthDistribution &", pybind11::return_value_policy::reference_internal);
		cl.def("pendant_length_distribution", (class genesis::placement::SimulatorPendantLengthDistribution & (genesis::placement::Simulator::*)()) &genesis::placement::Simulator::pendant_length_distribution, "C++: genesis::placement::Simulator::pendant_length_distribution() --> class genesis::placement::SimulatorPendantLengthDistribution &", pybind11::return_value_policy::reference_internal);
	}
}
