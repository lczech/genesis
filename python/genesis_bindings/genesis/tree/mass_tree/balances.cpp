#include <functional>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/mass_tree/balances.hpp>
#include <genesis/tree/mass_tree/functions.hpp>
#include <genesis/tree/mass_tree/phylo_ilr.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/containers/range.hpp>
#include <iterator>
#include <memory>
#include <unordered_set>
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

void bind_genesis_tree_mass_tree_balances(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::tree::mass_balance(const struct genesis::tree::BalanceData &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &) file:genesis/tree/mass_tree/balances.hpp line:275
	M("genesis::tree").def("mass_balance", (class std::vector<double, class std::allocator<double> > (*)(const struct genesis::tree::BalanceData &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &)) &genesis::tree::mass_balance, "Calculate the balance of edge masses between two sets of edges.\n\n The function calculates all balances (for all trees) between the two sets of edges.\n\n \n phylogenetic_ilr_transform( MassTree const& ) for a function that calculates this\n for the subtrees below all nodes in a rooted Tree.\n\nC++: genesis::tree::mass_balance(const struct genesis::tree::BalanceData &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("data"), pybind11::arg("numerator_edge_indices"), pybind11::arg("denominator_edge_indices"));

	// genesis::tree::mass_balance(const struct genesis::tree::BalanceData &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &, unsigned long) file:genesis/tree/mass_tree/balances.hpp line:292
	M("genesis::tree").def("mass_balance", (double (*)(const struct genesis::tree::BalanceData &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &, unsigned long)) &genesis::tree::mass_balance, "Calculate the balance of edge masses between two sets of edges.\n\n The function calculates the balances of a specific tree between the two sets of edges.\n The given  corresponds to the index in the original `vector` of MassTree%s\n used in \n\n\n\n\n\n\n\nC++: genesis::tree::mass_balance(const struct genesis::tree::BalanceData &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &, const class std::unordered_set<unsigned long, struct std::hash<unsigned long>, struct std::equal_to<unsigned long>, class std::allocator<unsigned long> > &, unsigned long) --> double", pybind11::arg("data"), pybind11::arg("numerator_edge_indices"), pybind11::arg("denominator_edge_indices"), pybind11::arg("tree_index"));

	// genesis::tree::mass_tree_merge_trees(const class genesis::tree::Tree &, const class genesis::tree::Tree &, const double, const double) file:genesis/tree/mass_tree/functions.hpp line:77
	M("genesis::tree").def("mass_tree_merge_trees", [](const class genesis::tree::Tree & a0, const class genesis::tree::Tree & a1) -> genesis::tree::Tree { return genesis::tree::mass_tree_merge_trees(a0, a1); }, "", pybind11::arg("lhs"), pybind11::arg("rhs"));
	M("genesis::tree").def("mass_tree_merge_trees", [](const class genesis::tree::Tree & a0, const class genesis::tree::Tree & a1, const double & a2) -> genesis::tree::Tree { return genesis::tree::mass_tree_merge_trees(a0, a1, a2); }, "", pybind11::arg("lhs"), pybind11::arg("rhs"), pybind11::arg("scaler_lhs"));
	M("genesis::tree").def("mass_tree_merge_trees", (class genesis::tree::Tree (*)(const class genesis::tree::Tree &, const class genesis::tree::Tree &, const double, const double)) &genesis::tree::mass_tree_merge_trees, "Merge all masses of two \n\n\n\n\n\n\n\n\nC++: genesis::tree::mass_tree_merge_trees(const class genesis::tree::Tree &, const class genesis::tree::Tree &, const double, const double) --> class genesis::tree::Tree", pybind11::arg("lhs"), pybind11::arg("rhs"), pybind11::arg("scaler_lhs"), pybind11::arg("scaler_rhs"));

	// genesis::tree::mass_tree_merge_trees_inplace(class genesis::tree::Tree &, const class genesis::tree::Tree &, const double, const double) file:genesis/tree/mass_tree/functions.hpp line:94
	M("genesis::tree").def("mass_tree_merge_trees_inplace", [](class genesis::tree::Tree & a0, const class genesis::tree::Tree & a1) -> void { return genesis::tree::mass_tree_merge_trees_inplace(a0, a1); }, "", pybind11::arg("lhs"), pybind11::arg("rhs"));
	M("genesis::tree").def("mass_tree_merge_trees_inplace", [](class genesis::tree::Tree & a0, const class genesis::tree::Tree & a1, const double & a2) -> void { return genesis::tree::mass_tree_merge_trees_inplace(a0, a1, a2); }, "", pybind11::arg("lhs"), pybind11::arg("rhs"), pybind11::arg("scaler_lhs"));
	M("genesis::tree").def("mass_tree_merge_trees_inplace", (void (*)(class genesis::tree::Tree &, const class genesis::tree::Tree &, const double, const double)) &genesis::tree::mass_tree_merge_trees_inplace, "Merge all masses of two \n\n\n\n\n\n\n\n\n\nC++: genesis::tree::mass_tree_merge_trees_inplace(class genesis::tree::Tree &, const class genesis::tree::Tree &, const double, const double) --> void", pybind11::arg("lhs"), pybind11::arg("rhs"), pybind11::arg("scaler_lhs"), pybind11::arg("scaler_rhs"));

	// genesis::tree::mass_tree_clear_masses(class genesis::tree::Tree &) file:genesis/tree/mass_tree/functions.hpp line:104
	M("genesis::tree").def("mass_tree_clear_masses", (void (*)(class genesis::tree::Tree &)) &genesis::tree::mass_tree_clear_masses, "Clear all masses of a ::MassTree, while keeping its topology.\n\nC++: genesis::tree::mass_tree_clear_masses(class genesis::tree::Tree &) --> void", pybind11::arg("tree"));

	// genesis::tree::mass_tree_reverse_signs(class genesis::tree::Tree &) file:genesis/tree/mass_tree/functions.hpp line:109
	M("genesis::tree").def("mass_tree_reverse_signs", (void (*)(class genesis::tree::Tree &)) &genesis::tree::mass_tree_reverse_signs, "Reverse the sign of each mass point on a ::MassTree.\n\nC++: genesis::tree::mass_tree_reverse_signs(class genesis::tree::Tree &) --> void", pybind11::arg("tree"));

	// genesis::tree::mass_tree_scale_masses(class genesis::tree::Tree &, double) file:genesis/tree/mass_tree/functions.hpp line:114
	M("genesis::tree").def("mass_tree_scale_masses", (void (*)(class genesis::tree::Tree &, double)) &genesis::tree::mass_tree_scale_masses, "Scale all masses of a ::MassTree with the multiplicative factor \n \n\nC++: genesis::tree::mass_tree_scale_masses(class genesis::tree::Tree &, double) --> void", pybind11::arg("tree"), pybind11::arg("factor"));

	// genesis::tree::mass_tree_normalize_masses(class genesis::tree::Tree &) file:genesis/tree/mass_tree/functions.hpp line:119
	M("genesis::tree").def("mass_tree_normalize_masses", (void (*)(class genesis::tree::Tree &)) &genesis::tree::mass_tree_normalize_masses, "Scale all masses of a ::MassTree so that they sum up to `1.0`.\n\nC++: genesis::tree::mass_tree_normalize_masses(class genesis::tree::Tree &) --> void", pybind11::arg("tree"));

	// genesis::tree::mass_tree_transform_to_unit_branch_lengths(class genesis::tree::Tree &) file:genesis/tree/mass_tree/functions.hpp line:125
	M("genesis::tree").def("mass_tree_transform_to_unit_branch_lengths", (void (*)(class genesis::tree::Tree &)) &genesis::tree::mass_tree_transform_to_unit_branch_lengths, "Set all branch lengths of a ::MassTree to `1.0`, while keeping the relative position of all\n masses on the branches.\n\nC++: genesis::tree::mass_tree_transform_to_unit_branch_lengths(class genesis::tree::Tree &) --> void", pybind11::arg("tree"));

	// genesis::tree::mass_tree_center_masses_on_branches(class genesis::tree::Tree &) file:genesis/tree/mass_tree/functions.hpp line:135
	M("genesis::tree").def("mass_tree_center_masses_on_branches", (double (*)(class genesis::tree::Tree &)) &genesis::tree::mass_tree_center_masses_on_branches, "Accumulate all masses of a ::MassTree on the centers of their edges.\n\n This function can be used to minimize the data load of a ::MassTree. It is equal to\n mass_tree_binify_masses() when using `number_of_bins == 1`.\n\n Return the work (mass times distance) that was needed to move the masses to the centers.\n\nC++: genesis::tree::mass_tree_center_masses_on_branches(class genesis::tree::Tree &) --> double", pybind11::arg("tree"));

	// genesis::tree::mass_tree_center_masses_on_branches_averaged(class genesis::tree::Tree &) file:genesis/tree/mass_tree/functions.hpp line:146
	M("genesis::tree").def("mass_tree_center_masses_on_branches_averaged", (double (*)(class genesis::tree::Tree &)) &genesis::tree::mass_tree_center_masses_on_branches_averaged, "Accumulate all masses of a ::MassTree at the average mass position per edge.\n\n This function is similar to mass_tree_center_masses_on_branches(), but instead of\n accumulating the masses at the branch center, they are accumulated at their average position\n on the branch.\n\n Return the work (mass times distance) that was needed to move the masses to the centers.\n\nC++: genesis::tree::mass_tree_center_masses_on_branches_averaged(class genesis::tree::Tree &) --> double", pybind11::arg("tree"));

	// genesis::tree::mass_tree_binify_masses(class genesis::tree::Tree &, unsigned long) file:genesis/tree/mass_tree/functions.hpp line:170
	M("genesis::tree").def("mass_tree_binify_masses", (double (*)(class genesis::tree::Tree &, unsigned long)) &genesis::tree::mass_tree_binify_masses, "Accumulate all masses of a ::MassTree into bins on each branch.\n\n Each branch is divided into intervals of equal size, where  is the number of\n those intervals. The mid points of these intervals are then used as bins, to which the masses\n on the branch are moved. Each mass point is moved to its closest bin, so that all mass is\n accumulated at the bins.\n\n This function is useful to reduce the data load of big ::MassTree%s, without affecting the\n accuracy of downstream analyses too much. Using the interval mid points as bins means that masses\n are moved as little as possible.\n\n Example: Given `number_of_bins == 6`, for a branch of length `3.6`, the bins look like this:\n\n     Intervals   0.0   0.6   1.2   1.8   2.4   3.0   3.6\n                 |     |     |     |     |     |     |\n                    ^     ^     ^     ^     ^     ^\n     Bins           0.3   0.9   1.5   2.1   2.7   3.3\n\n The function returns the work (mass times distance) that was needed to move the masses to the\n bins.\n\nC++: genesis::tree::mass_tree_binify_masses(class genesis::tree::Tree &, unsigned long) --> double", pybind11::arg("tree"), pybind11::arg("number_of_bins"));

	// genesis::tree::mass_trees_make_average_branch_lengths(class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &) file:genesis/tree/mass_tree/functions.hpp line:183
	M("genesis::tree").def("mass_trees_make_average_branch_lengths", (void (*)(class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &)) &genesis::tree::mass_trees_make_average_branch_lengths, "Change the branch lengths of all trees to their average, and move the masses accordingly\n in a proportional way.\n\n The function only is reasonable to run if all trees have identical topology, which is however\n not checked explicitly. Use identical_topology() for this.\n\nC++: genesis::tree::mass_trees_make_average_branch_lengths(class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &) --> void", pybind11::arg("mass_trees"));

	// genesis::tree::mass_tree_mass_per_edge(const class genesis::tree::Tree &) file:genesis/tree/mass_tree/functions.hpp line:191
	M("genesis::tree").def("mass_tree_mass_per_edge", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::tree::Tree &)) &genesis::tree::mass_tree_mass_per_edge, "Return a `std::vector` that contains the total \n\n\n\n\n\nC++: genesis::tree::mass_tree_mass_per_edge(const class genesis::tree::Tree &) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("tree"));

	// genesis::tree::mass_tree_mass_per_edge(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &) file:genesis/tree/mass_tree/functions.hpp line:201
	M("genesis::tree").def("mass_tree_mass_per_edge", (class genesis::utils::Matrix<double> (*)(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &)) &genesis::tree::mass_tree_mass_per_edge, "Return the total \n\n\n\n\n\n\n\nC++: genesis::tree::mass_tree_mass_per_edge(const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &) --> class genesis::utils::Matrix<double>", pybind11::arg("mass_trees"));

	// genesis::tree::mass_tree_mass_per_edge_averaged(const class genesis::tree::Tree &) file:genesis/tree/mass_tree/functions.hpp line:211
	M("genesis::tree").def("mass_tree_mass_per_edge_averaged", (class std::vector<struct std::pair<double, double>, class std::allocator<struct std::pair<double, double> > > (*)(const class genesis::tree::Tree &)) &genesis::tree::mass_tree_mass_per_edge_averaged, "Return a `std::vector` that contains the total \n\n\n\n\n\n\n\nC++: genesis::tree::mass_tree_mass_per_edge_averaged(const class genesis::tree::Tree &) --> class std::vector<struct std::pair<double, double>, class std::allocator<struct std::pair<double, double> > >", pybind11::arg("tree"));

	// genesis::tree::mass_tree_sum_of_masses(const class genesis::tree::Tree &) file:genesis/tree/mass_tree/functions.hpp line:223
	M("genesis::tree").def("mass_tree_sum_of_masses", (double (*)(const class genesis::tree::Tree &)) &genesis::tree::mass_tree_sum_of_masses, "Return the total sum of all masses on the ::MassTree.\n\n In order for the earth_movers_distance() algorithm to work properly (and give meaningful\n results), the total mass on the \n\n\n\n\n\n\n\nC++: genesis::tree::mass_tree_sum_of_masses(const class genesis::tree::Tree &) --> double", pybind11::arg("tree"));

	// genesis::tree::mass_tree_validate(const class genesis::tree::Tree &, double) file:genesis/tree/mass_tree/functions.hpp line:246
	M("genesis::tree").def("mass_tree_validate", [](const class genesis::tree::Tree & a0) -> bool { return genesis::tree::mass_tree_validate(a0); }, "", pybind11::arg("tree"));
	M("genesis::tree").def("mass_tree_validate", (bool (*)(const class genesis::tree::Tree &, double)) &genesis::tree::mass_tree_validate, "Validate the data on a ::MassTree.\n\n This function returns true iff the data on the Tree is valid:\n\n  *  The node and edge data types have to be \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::tree::mass_tree_validate(const class genesis::tree::Tree &, double) --> bool", pybind11::arg("tree"), pybind11::arg("valid_total_mass_difference"));

	// genesis::tree::phylogenetic_ilr_transform(const struct genesis::tree::BalanceData &, bool) file:genesis/tree/mass_tree/phylo_ilr.hpp line:73
	M("genesis::tree").def("phylogenetic_ilr_transform", [](const struct genesis::tree::BalanceData & a0) -> genesis::utils::Matrix<double> { return genesis::tree::phylogenetic_ilr_transform(a0); }, "", pybind11::arg("data"));
	M("genesis::tree").def("phylogenetic_ilr_transform", (class genesis::utils::Matrix<double> (*)(const struct genesis::tree::BalanceData &, bool)) &genesis::tree::phylogenetic_ilr_transform, "Calculate the Phylogenetic Isometric Log Ratio transformation.\n\n The balances are calculated per node of the tree, similar to [1]. We however extend this original\n idea by being able to place masses on inner branches as well, instead of just the tips (OTUs).\n The tree has to be bifurcating and rooted. The calculated balances are stored using the node\n indices. Their sign (order of the subtrees) is according to the TreeLink order of each TreeNode:\n The numerator is the first link, the denominator is the second link - unless \n is set to `true`, in which case this is flipped.\n Use sign_matrix() to get the ordering (sign) used for the subtrees.\n\n The function expects  coming from mass_balance_data(), which can be calculated for a\n single tree, or for a set of trees. In the latter case, per-taxon (that is, per-edge) weights\n can also be calculated, see BalanceSettings for details.\n\n > [1] J. D. Silverman, A. D. Washburne, S. Mukherjee, and L. A. David,\n > \"A phylogenetic transform enhances analysis of compositional microbiota data,\"\n > Elife, vol. 6, p. e21887, Feb. 2017.\n > https://elifesciences.org/articles/21887\n\nC++: genesis::tree::phylogenetic_ilr_transform(const struct genesis::tree::BalanceData &, bool) --> class genesis::utils::Matrix<double>", pybind11::arg("data"), pybind11::arg("reverse_signs"));

}
