#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/function/operators.hpp>
#include <genesis/tree/mass_tree/balances.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/tree/tree/subtree.hpp>
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

void bind_genesis_tree_function_operators(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::tree::identical_topology(const class genesis::tree::Tree &, const class genesis::tree::Tree &, bool) file:genesis/tree/function/operators.hpp line:223
	M("genesis::tree").def("identical_topology", [](const class genesis::tree::Tree & a0, const class genesis::tree::Tree & a1) -> bool { return genesis::tree::identical_topology(a0, a1); }, "", pybind11::arg("lhs"), pybind11::arg("rhs"));
	M("genesis::tree").def("identical_topology", (bool (*)(const class genesis::tree::Tree &, const class genesis::tree::Tree &, bool)) &genesis::tree::identical_topology, "Return whether both trees have an identical topology.\n\n The topology is considered identical only if the order of edges is also the same in both trees.\n This means, although two trees might have the same number of leaves and branches, they might\n still be not identical (with respect to this function) when the branches appear in a different\n order or when the root sits at a different node.\n\n Futhermore, if  is set to `true` (default), the function also checks\n whether the indices of nodes and edges are identical at the different nodes and edges of the\n trees. This is important if mulitple (identical) trees are used in a calculation, where the\n indices are used as indices into arrays or the like.\n\nC++: genesis::tree::identical_topology(const class genesis::tree::Tree &, const class genesis::tree::Tree &, bool) --> bool", pybind11::arg("lhs"), pybind11::arg("rhs"), pybind11::arg("identical_indices"));

	// genesis::tree::identical_topology(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, bool) file:genesis/tree/function/operators.hpp line:230
	M("genesis::tree").def("identical_topology", [](const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > & a0) -> bool { return genesis::tree::identical_topology(a0); }, "", pybind11::arg("trees"));
	M("genesis::tree").def("identical_topology", (bool (*)(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, bool)) &genesis::tree::identical_topology, "Return whether all trees have an identical topology.\n\n See identical_topology( Tree const&, Tree const&, bool ) for details.\n\nC++: genesis::tree::identical_topology(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, bool) --> bool", pybind11::arg("trees"), pybind11::arg("identical_indices"));

	// genesis::tree::belongs_to(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &) file:genesis/tree/function/operators.hpp line:239
	M("genesis::tree").def("belongs_to", (bool (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &)) &genesis::tree::belongs_to, "Return whether the TreeNode belongs to the Tree, i.e., whether it is owned by the Tree.\n\nC++: genesis::tree::belongs_to(const class genesis::tree::Tree &, const class genesis::tree::TreeNode &) --> bool", pybind11::arg("tree"), pybind11::arg("node"));

	// genesis::tree::belongs_to(const class genesis::tree::TreeNode &, const class genesis::tree::Tree &) file:genesis/tree/function/operators.hpp line:244
	M("genesis::tree").def("belongs_to", (bool (*)(const class genesis::tree::TreeNode &, const class genesis::tree::Tree &)) &genesis::tree::belongs_to, "Return whether the TreeNode belongs to the Tree, i.e., whether it is owned by the Tree.\n\nC++: genesis::tree::belongs_to(const class genesis::tree::TreeNode &, const class genesis::tree::Tree &) --> bool", pybind11::arg("node"), pybind11::arg("tree"));

	// genesis::tree::belongs_to(const class genesis::tree::Tree &, const class genesis::tree::TreeEdge &) file:genesis/tree/function/operators.hpp line:249
	M("genesis::tree").def("belongs_to", (bool (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeEdge &)) &genesis::tree::belongs_to, "Return whether the TreeEdge belongs to the Tree, i.e., whether it is owned by the Tree.\n\nC++: genesis::tree::belongs_to(const class genesis::tree::Tree &, const class genesis::tree::TreeEdge &) --> bool", pybind11::arg("tree"), pybind11::arg("edge"));

	// genesis::tree::belongs_to(const class genesis::tree::TreeEdge &, const class genesis::tree::Tree &) file:genesis/tree/function/operators.hpp line:254
	M("genesis::tree").def("belongs_to", (bool (*)(const class genesis::tree::TreeEdge &, const class genesis::tree::Tree &)) &genesis::tree::belongs_to, "Return whether the TreeEdge belongs to the Tree, i.e., whether it is owned by the Tree.\n\nC++: genesis::tree::belongs_to(const class genesis::tree::TreeEdge &, const class genesis::tree::Tree &) --> bool", pybind11::arg("edge"), pybind11::arg("tree"));

	// genesis::tree::belongs_to(const class genesis::tree::Tree &, const class genesis::tree::TreeLink &) file:genesis/tree/function/operators.hpp line:259
	M("genesis::tree").def("belongs_to", (bool (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeLink &)) &genesis::tree::belongs_to, "Return whether the TreeLink belongs to the Tree, i.e., whether it is owned by the Tree.\n\nC++: genesis::tree::belongs_to(const class genesis::tree::Tree &, const class genesis::tree::TreeLink &) --> bool", pybind11::arg("tree"), pybind11::arg("link"));

	// genesis::tree::belongs_to(const class genesis::tree::TreeLink &, const class genesis::tree::Tree &) file:genesis/tree/function/operators.hpp line:264
	M("genesis::tree").def("belongs_to", (bool (*)(const class genesis::tree::TreeLink &, const class genesis::tree::Tree &)) &genesis::tree::belongs_to, "Return whether the TreeLink belongs to the Tree, i.e., whether it is owned by the Tree.\n\nC++: genesis::tree::belongs_to(const class genesis::tree::TreeLink &, const class genesis::tree::Tree &) --> bool", pybind11::arg("link"), pybind11::arg("tree"));

	// genesis::tree::belongs_to(const class genesis::tree::Tree &, const class genesis::tree::Subtree &) file:genesis/tree/function/operators.hpp line:269
	M("genesis::tree").def("belongs_to", (bool (*)(const class genesis::tree::Tree &, const class genesis::tree::Subtree &)) &genesis::tree::belongs_to, "Return whether the Subtree belongs to the Tree, i.e., whether its link is owned by the Tree.\n\nC++: genesis::tree::belongs_to(const class genesis::tree::Tree &, const class genesis::tree::Subtree &) --> bool", pybind11::arg("tree"), pybind11::arg("subt"));

	// genesis::tree::belongs_to(const class genesis::tree::Subtree &, const class genesis::tree::Tree &) file:genesis/tree/function/operators.hpp line:274
	M("genesis::tree").def("belongs_to", (bool (*)(const class genesis::tree::Subtree &, const class genesis::tree::Tree &)) &genesis::tree::belongs_to, "Return whether the Subtree belongs to the Tree, i.e., whether its link is owned by the Tree.\n\nC++: genesis::tree::belongs_to(const class genesis::tree::Subtree &, const class genesis::tree::Tree &) --> bool", pybind11::arg("subt"), pybind11::arg("tree"));

	// genesis::tree::edge_between(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &) file:genesis/tree/function/operators.hpp line:279
	M("genesis::tree").def("edge_between", (class genesis::tree::TreeEdge * (*)(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &)) &genesis::tree::edge_between, "Return the TreeEdge between two TreeNode&s, if they are neighbours, or `nullptr` otherwise.\n\nC++: genesis::tree::edge_between(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &) --> class genesis::tree::TreeEdge *", pybind11::return_value_policy::reference_internal, pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::tree::edge_between(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &) file:genesis/tree/function/operators.hpp line:284
	M("genesis::tree").def("edge_between", (const class genesis::tree::TreeEdge * (*)(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &)) &genesis::tree::edge_between, "Return the TreeEdge between two TreeNode&s, if they are neighbours, or `nullptr` otherwise.\n\nC++: genesis::tree::edge_between(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &) --> const class genesis::tree::TreeEdge *", pybind11::return_value_policy::reference_internal, pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::tree::print_info(const class genesis::tree::Tree &) file:genesis/tree/function/operators.hpp line:290
	M("genesis::tree").def("print_info", (std::string (*)(const class genesis::tree::Tree &)) &genesis::tree::print_info, "C++: genesis::tree::print_info(const class genesis::tree::Tree &) --> std::string", pybind11::arg("tree"));

	// genesis::tree::print_info(const class genesis::tree::TreeEdge &) file:genesis/tree/function/operators.hpp line:291
	M("genesis::tree").def("print_info", (std::string (*)(const class genesis::tree::TreeEdge &)) &genesis::tree::print_info, "C++: genesis::tree::print_info(const class genesis::tree::TreeEdge &) --> std::string", pybind11::arg("edge"));

	// genesis::tree::print_info(const class genesis::tree::TreeLink &) file:genesis/tree/function/operators.hpp line:292
	M("genesis::tree").def("print_info", (std::string (*)(const class genesis::tree::TreeLink &)) &genesis::tree::print_info, "C++: genesis::tree::print_info(const class genesis::tree::TreeLink &) --> std::string", pybind11::arg("link"));

	// genesis::tree::print_info(const class genesis::tree::TreeNode &) file:genesis/tree/function/operators.hpp line:293
	M("genesis::tree").def("print_info", (std::string (*)(const class genesis::tree::TreeNode &)) &genesis::tree::print_info, "C++: genesis::tree::print_info(const class genesis::tree::TreeNode &) --> std::string", pybind11::arg("node"));

	// genesis::tree::print_gist(const class genesis::tree::Tree &, long) file:genesis/tree/function/operators.hpp line:295
	M("genesis::tree").def("print_gist", [](const class genesis::tree::Tree & a0) -> std::string { return genesis::tree::print_gist(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("print_gist", (std::string (*)(const class genesis::tree::Tree &, long)) &genesis::tree::print_gist, "C++: genesis::tree::print_gist(const class genesis::tree::Tree &, long) --> std::string", pybind11::arg("tree"), pybind11::arg("items"));

	{ // genesis::tree::BalanceSettings file:genesis/tree/mass_tree/balances.hpp line:76
		pybind11::class_<genesis::tree::BalanceSettings, std::shared_ptr<genesis::tree::BalanceSettings>> cl(M("genesis::tree"), "BalanceSettings", "Settings to calculate balances and the Phylogenetic ILR Transform.\n\n The class stores settings for different parts of the balances calculation:\n\n  * The taxon weights used in soft thresholding are described in Section\n    \"Soft thresholding through weighting taxa\" of [1]. See there for details.\n    We implement the weights `p` as described there, offering different ways to assess the central\n    tendency of counts and to measure the norm of their relative abundances.\n    These two terms (BalanceSettings::tendency and BalanceSettings::norm) are multiplied\n    to get the weight for a taxon.\n    By default, the geometric mean times the euclidean norm are used as weights.\n  * To avoid zero counts in the taxon masses, two ways of adding pseudo-counts are offered:\n    Either a constant is added to all taxon masses, or just to the ones that are zero\n    (BalanceSettings::pseudo_count_summand_all and BalanceSettings::pseudo_count_summand_zeros).\n\n > [1] J. D. Silverman, A. D. Washburne, S. Mukherjee, and L. A. David,\n > \"A phylogenetic transform enhances analysis of compositional microbiota data,\"\n > Elife, vol. 6, p. e21887, Feb. 2017.\n > https://elifesciences.org/articles/21887\n\n \n phylogenetic_ilr_transform().");
		cl.def( pybind11::init( [](){ return new genesis::tree::BalanceSettings(); } ) );
		cl.def( pybind11::init( [](genesis::tree::BalanceSettings const &o){ return new genesis::tree::BalanceSettings(o); } ) );

		pybind11::enum_<genesis::tree::BalanceSettings::WeightTendency>(cl, "WeightTendency", "")
			.value("kNone", genesis::tree::BalanceSettings::WeightTendency::kNone)
			.value("kMedian", genesis::tree::BalanceSettings::WeightTendency::kMedian)
			.value("kArithmeticMean", genesis::tree::BalanceSettings::WeightTendency::kArithmeticMean)
			.value("kGeometricMean", genesis::tree::BalanceSettings::WeightTendency::kGeometricMean);


		pybind11::enum_<genesis::tree::BalanceSettings::WeightNorm>(cl, "WeightNorm", "")
			.value("kNone", genesis::tree::BalanceSettings::WeightNorm::kNone)
			.value("kManhattan", genesis::tree::BalanceSettings::WeightNorm::kManhattan)
			.value("kEuclidean", genesis::tree::BalanceSettings::WeightNorm::kEuclidean)
			.value("kMaximum", genesis::tree::BalanceSettings::WeightNorm::kMaximum)
			.value("kAitchison", genesis::tree::BalanceSettings::WeightNorm::kAitchison);

		cl.def_readwrite("tendency", &genesis::tree::BalanceSettings::tendency);
		cl.def_readwrite("norm", &genesis::tree::BalanceSettings::norm);
		cl.def_readwrite("pseudo_count_summand_all", &genesis::tree::BalanceSettings::pseudo_count_summand_all);
		cl.def_readwrite("pseudo_count_summand_zeros", &genesis::tree::BalanceSettings::pseudo_count_summand_zeros);
	}
	{ // genesis::tree::BalanceData file:genesis/tree/mass_tree/balances.hpp line:170
		pybind11::class_<genesis::tree::BalanceData, std::shared_ptr<genesis::tree::BalanceData>> cl(M("genesis::tree"), "BalanceData", "Data needed to calculate balances.\n\n This data is needed for calculations such as mass_balance(), phylogenetic_ilr_transform(), and\n phylogenetic_factorization(). Use\n \n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::tree::BalanceData(); } ) );
		cl.def( pybind11::init( [](genesis::tree::BalanceData const &o){ return new genesis::tree::BalanceData(o); } ) );
		cl.def_readwrite("tree", &genesis::tree::BalanceData::tree);
		cl.def_readwrite("edge_masses", &genesis::tree::BalanceData::edge_masses);
		cl.def_readwrite("raw_edge_masses", &genesis::tree::BalanceData::raw_edge_masses);
		cl.def_readwrite("taxon_weights", &genesis::tree::BalanceData::taxon_weights);
		cl.def("assign", (struct genesis::tree::BalanceData & (genesis::tree::BalanceData::*)(const struct genesis::tree::BalanceData &)) &genesis::tree::BalanceData::operator=, "C++: genesis::tree::BalanceData::operator=(const struct genesis::tree::BalanceData &) --> struct genesis::tree::BalanceData &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	// genesis::tree::mass_balance_data(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, struct genesis::tree::BalanceSettings) file:genesis/tree/mass_tree/balances.hpp line:248
	M("genesis::tree").def("mass_balance_data", [](const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > & a0) -> genesis::tree::BalanceData { return genesis::tree::mass_balance_data(a0); }, "", pybind11::arg("trees"));
	M("genesis::tree").def("mass_balance_data", (struct genesis::tree::BalanceData (*)(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, struct genesis::tree::BalanceSettings)) &genesis::tree::mass_balance_data, "Calculate the data needed to calculate balances on MassTree%s.\n\n This function takes a set of MassTree%s and BalanceSettings and calculates the data needed\n for calculations such as mass_balance(), phylogenetic_ilr_transform(), and\n phylogenetic_factorization().\n\n The function assumes that the Tree%s are populated with masses along thir branches which represent\n metagenomic sequences being placed there. The masses must not be normalized - there has to be\n at least a mass of ~1.0 per Tree for this function to work. That is each sequence should contribute\n about 1.0 mass to the tree (leaving out some lower masses in case of phylogenetic placement data).\n\nC++: genesis::tree::mass_balance_data(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &, struct genesis::tree::BalanceSettings) --> struct genesis::tree::BalanceData", pybind11::arg("trees"), pybind11::arg("settings"));

	// genesis::tree::mass_balance_data(const class genesis::tree::Tree &, struct genesis::tree::BalanceSettings) file:genesis/tree/mass_tree/balances.hpp line:262
	M("genesis::tree").def("mass_balance_data", [](const class genesis::tree::Tree & a0) -> genesis::tree::BalanceData { return genesis::tree::mass_balance_data(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("mass_balance_data", (struct genesis::tree::BalanceData (*)(const class genesis::tree::Tree &, struct genesis::tree::BalanceSettings)) &genesis::tree::mass_balance_data, "Calculate the data needed to calculate balances of a MassTree.\n\n This is the version for a single MassTree.\n Here, the \n\n\n\n\n\n\nC++: genesis::tree::mass_balance_data(const class genesis::tree::Tree &, struct genesis::tree::BalanceSettings) --> struct genesis::tree::BalanceData", pybind11::arg("tree"), pybind11::arg("settings"));

}
