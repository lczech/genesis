#include <functional>
#include <genesis/placement/function/functions.hpp>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/placement/pquery/name.hpp>
#include <genesis/placement/pquery/placement.hpp>
#include <genesis/placement/sample.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/range.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <unordered_set>
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

void bind_genesis_placement_function_functions(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::placement::normalize_weight_ratios(class genesis::placement::Pquery &) file:genesis/placement/function/functions.hpp line:88
	M("genesis::placement").def("normalize_weight_ratios", (void (*)(class genesis::placement::Pquery &)) &genesis::placement::normalize_weight_ratios, "Recalculate the `like_weight_ratio` of the PqueryPlacement&s of a Pquery,\n so that their sum is 1.0, while maintaining their ratio to each other.\n\nC++: genesis::placement::normalize_weight_ratios(class genesis::placement::Pquery &) --> void", pybind11::arg("pquery"));

	// genesis::placement::normalize_weight_ratios(class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:97
	M("genesis::placement").def("normalize_weight_ratios", (void (*)(class genesis::placement::Sample &)) &genesis::placement::normalize_weight_ratios, "Recalculate the `like_weight_ratio` of the PqueryPlacement&s of each Pquery in the Sample,\n so that their sum is 1.0, while maintaining their ratio to each other.\n\n This function simply calls normalize_weight_ratios( Pquery& pquery ) for all Pqueries of\n the Sample.\n\nC++: genesis::placement::normalize_weight_ratios(class genesis::placement::Sample &) --> void", pybind11::arg("smp"));

	// genesis::placement::sort_placements_by_weight(class genesis::placement::Pquery &) file:genesis/placement/function/functions.hpp line:106
	M("genesis::placement").def("sort_placements_by_weight", (void (*)(class genesis::placement::Pquery &)) &genesis::placement::sort_placements_by_weight, "Sort the PqueryPlacement%s of a Pquery by their `like_weight_ratio`, in descending order\n (most likely first).\n\nC++: genesis::placement::sort_placements_by_weight(class genesis::placement::Pquery &) --> void", pybind11::arg("pquery"));

	// genesis::placement::sort_placements_by_weight(class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:112
	M("genesis::placement").def("sort_placements_by_weight", (void (*)(class genesis::placement::Sample &)) &genesis::placement::sort_placements_by_weight, "Sort the PqueryPlacement%s of all \n\n\n\nC++: genesis::placement::sort_placements_by_weight(class genesis::placement::Sample &) --> void", pybind11::arg("smp"));

	// genesis::placement::scale_all_branch_lengths(class genesis::placement::Sample &, double) file:genesis/placement/function/functions.hpp line:121
	M("genesis::placement").def("scale_all_branch_lengths", [](class genesis::placement::Sample & a0) -> void { return genesis::placement::scale_all_branch_lengths(a0); }, "", pybind11::arg("smp"));
	M("genesis::placement").def("scale_all_branch_lengths", (void (*)(class genesis::placement::Sample &, double)) &genesis::placement::scale_all_branch_lengths, "Scale all branch lengths of the Tree and the position of the PqueryPlacement%s by\n a given factor.\n\n This function calls tree::scale_all_branch_lengths() for scaling the tree, and also applies\n the same scaling to the PqueryPlacement::proximal_length.\n\nC++: genesis::placement::scale_all_branch_lengths(class genesis::placement::Sample &, double) --> void", pybind11::arg("smp"), pybind11::arg("factor"));

	// genesis::placement::adjust_branch_lengths(class genesis::placement::Sample &, const class genesis::tree::Tree &) file:genesis/placement/function/functions.hpp line:135
	M("genesis::placement").def("adjust_branch_lengths", (void (*)(class genesis::placement::Sample &, const class genesis::tree::Tree &)) &genesis::placement::adjust_branch_lengths, "Take the branch lengths of the  \n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::adjust_branch_lengths(class genesis::placement::Sample &, const class genesis::tree::Tree &) --> void", pybind11::arg("sample"), pybind11::arg("source"));

	// genesis::placement::filter_min_accumulated_weight(class genesis::placement::Pquery &, double) file:genesis/placement/function/functions.hpp line:149
	M("genesis::placement").def("filter_min_accumulated_weight", [](class genesis::placement::Pquery & a0) -> void { return genesis::placement::filter_min_accumulated_weight(a0); }, "", pybind11::arg("pquery"));
	M("genesis::placement").def("filter_min_accumulated_weight", (void (*)(class genesis::placement::Pquery &, double)) &genesis::placement::filter_min_accumulated_weight, "Remove the PqueryPlacement%s with the lowest `like_weight_ratio`, while keeping the\n accumulated weight (sum of all remaining `like_weight_ratio`s) above a given threshold.\n\n This is a cleaning function to get rid of unlikely placement positions, withouth sacrificing\n too much detail of the overall distribution of weights. The EPA support a similar option, which\n only writes enough of the most likely placement positions to the output to fullfil a threshold.\n\nC++: genesis::placement::filter_min_accumulated_weight(class genesis::placement::Pquery &, double) --> void", pybind11::arg("pquery"), pybind11::arg("threshold"));

	// genesis::placement::filter_min_accumulated_weight(class genesis::placement::Sample &, double) file:genesis/placement/function/functions.hpp line:158
	M("genesis::placement").def("filter_min_accumulated_weight", [](class genesis::placement::Sample & a0) -> void { return genesis::placement::filter_min_accumulated_weight(a0); }, "", pybind11::arg("smp"));
	M("genesis::placement").def("filter_min_accumulated_weight", (void (*)(class genesis::placement::Sample &, double)) &genesis::placement::filter_min_accumulated_weight, "Remove the PqueryPlacement%s with the lowest `like_weight_ratio`, while keeping the\n accumulated weight (sum of all remaining `like_weight_ratio`s) above a given threshold.\n\n This function calls filter_min_accumulated_weight( Pquery& pquery, double threshold ) for all\n Pqueries of the Sample. See this version of the function for more information.\n\nC++: genesis::placement::filter_min_accumulated_weight(class genesis::placement::Sample &, double) --> void", pybind11::arg("smp"), pybind11::arg("threshold"));

	// genesis::placement::filter_n_max_weight_placements(class genesis::placement::Pquery &, unsigned long) file:genesis/placement/function/functions.hpp line:172
	M("genesis::placement").def("filter_n_max_weight_placements", [](class genesis::placement::Pquery & a0) -> void { return genesis::placement::filter_n_max_weight_placements(a0); }, "", pybind11::arg("pquery"));
	M("genesis::placement").def("filter_n_max_weight_placements", (void (*)(class genesis::placement::Pquery &, unsigned long)) &genesis::placement::filter_n_max_weight_placements, "Remove all PqueryPlacement%s but the `n` most likely ones from the Pquery.\n\n Pqueries can contain multiple placements on different branches. For example, the EPA algorithm\n of RAxML outputs up to the 7 most likely positions for placements to the output Jplace file by\n default. The property `like_weight_ratio` weights those placement positions so that the sum over\n all positions (all branches of the tree) per pquery is 1.0.\n\n This function removes all but the `n` most likely placements (the ones which have the highest\n `like_weight_ratio`) from the Pquery. The `like_weight_ratio` of the remaining placements is not\n changed.\n\nC++: genesis::placement::filter_n_max_weight_placements(class genesis::placement::Pquery &, unsigned long) --> void", pybind11::arg("pquery"), pybind11::arg("n"));

	// genesis::placement::filter_n_max_weight_placements(class genesis::placement::Sample &, unsigned long) file:genesis/placement/function/functions.hpp line:181
	M("genesis::placement").def("filter_n_max_weight_placements", [](class genesis::placement::Sample & a0) -> void { return genesis::placement::filter_n_max_weight_placements(a0); }, "", pybind11::arg("smp"));
	M("genesis::placement").def("filter_n_max_weight_placements", (void (*)(class genesis::placement::Sample &, unsigned long)) &genesis::placement::filter_n_max_weight_placements, "Remove all PqueryPlacement%s but the `n` most likely ones from all\n \n\n\n\n\n\n\nC++: genesis::placement::filter_n_max_weight_placements(class genesis::placement::Sample &, unsigned long) --> void", pybind11::arg("smp"), pybind11::arg("n"));

	// genesis::placement::filter_min_weight_threshold(class genesis::placement::Pquery &, double) file:genesis/placement/function/functions.hpp line:186
	M("genesis::placement").def("filter_min_weight_threshold", [](class genesis::placement::Pquery & a0) -> void { return genesis::placement::filter_min_weight_threshold(a0); }, "", pybind11::arg("pquery"));
	M("genesis::placement").def("filter_min_weight_threshold", (void (*)(class genesis::placement::Pquery &, double)) &genesis::placement::filter_min_weight_threshold, "Remove all PqueryPlacement%s that have a `like_weight_ratio` below the given threshold.\n\nC++: genesis::placement::filter_min_weight_threshold(class genesis::placement::Pquery &, double) --> void", pybind11::arg("pquery"), pybind11::arg("threshold"));

	// genesis::placement::filter_min_weight_threshold(class genesis::placement::Sample &, double) file:genesis/placement/function/functions.hpp line:192
	M("genesis::placement").def("filter_min_weight_threshold", [](class genesis::placement::Sample & a0) -> void { return genesis::placement::filter_min_weight_threshold(a0); }, "", pybind11::arg("smp"));
	M("genesis::placement").def("filter_min_weight_threshold", (void (*)(class genesis::placement::Sample &, double)) &genesis::placement::filter_min_weight_threshold, "Remove all PqueryPlacement%s that have a `like_weight_ratio` below the given threshold\n from all \n\n\n\nC++: genesis::placement::filter_min_weight_threshold(class genesis::placement::Sample &, double) --> void", pybind11::arg("smp"), pybind11::arg("threshold"));

	// genesis::placement::filter_min_pendant_length(class genesis::placement::Pquery &, double) file:genesis/placement/function/functions.hpp line:197
	M("genesis::placement").def("filter_min_pendant_length", (void (*)(class genesis::placement::Pquery &, double)) &genesis::placement::filter_min_pendant_length, "Remove all PqueryPlacement%s that have a `pendant_length` below the given threshold.\n\nC++: genesis::placement::filter_min_pendant_length(class genesis::placement::Pquery &, double) --> void", pybind11::arg("pquery"), pybind11::arg("threshold"));

	// genesis::placement::filter_min_pendant_length(class genesis::placement::Sample &, double) file:genesis/placement/function/functions.hpp line:203
	M("genesis::placement").def("filter_min_pendant_length", (void (*)(class genesis::placement::Sample &, double)) &genesis::placement::filter_min_pendant_length, "Remove all PqueryPlacement%s that have a `pendant_length` below the given threshold\n from all \n\n\n\nC++: genesis::placement::filter_min_pendant_length(class genesis::placement::Sample &, double) --> void", pybind11::arg("sample"), pybind11::arg("threshold"));

	// genesis::placement::filter_max_pendant_length(class genesis::placement::Pquery &, double) file:genesis/placement/function/functions.hpp line:208
	M("genesis::placement").def("filter_max_pendant_length", (void (*)(class genesis::placement::Pquery &, double)) &genesis::placement::filter_max_pendant_length, "Remove all PqueryPlacement%s that have a `pendant_length` above the given threshold.\n\nC++: genesis::placement::filter_max_pendant_length(class genesis::placement::Pquery &, double) --> void", pybind11::arg("pquery"), pybind11::arg("threshold"));

	// genesis::placement::filter_max_pendant_length(class genesis::placement::Sample &, double) file:genesis/placement/function/functions.hpp line:214
	M("genesis::placement").def("filter_max_pendant_length", (void (*)(class genesis::placement::Sample &, double)) &genesis::placement::filter_max_pendant_length, "Remove all PqueryPlacement%s that have a `pendant_length` above the given threshold\n from all \n\n\n\nC++: genesis::placement::filter_max_pendant_length(class genesis::placement::Sample &, double) --> void", pybind11::arg("sample"), pybind11::arg("threshold"));

	// genesis::placement::remove_empty_placement_pqueries(class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:224
	M("genesis::placement").def("remove_empty_placement_pqueries", (unsigned long (*)(class genesis::placement::Sample &)) &genesis::placement::remove_empty_placement_pqueries, "Remove all \n\n\n\n\n\n\n\nC++: genesis::placement::remove_empty_placement_pqueries(class genesis::placement::Sample &) --> unsigned long", pybind11::arg("sample"));

	// genesis::placement::filter_pqueries_keeping_names(class genesis::placement::Sample &, const std::string &, bool) file:genesis/placement/function/functions.hpp line:236
	M("genesis::placement").def("filter_pqueries_keeping_names", [](class genesis::placement::Sample & a0, const std::string & a1) -> void { return genesis::placement::filter_pqueries_keeping_names(a0, a1); }, "", pybind11::arg("smp"), pybind11::arg("regex"));
	M("genesis::placement").def("filter_pqueries_keeping_names", (void (*)(class genesis::placement::Sample &, const std::string &, bool)) &genesis::placement::filter_pqueries_keeping_names, "Remove all PqueryName%s which do not match the given \n\n If all names from a given Pquery are removed that way, and if the \n flag is `true` (default), the whole Pquery is removed from the Sample as well.\n\nC++: genesis::placement::filter_pqueries_keeping_names(class genesis::placement::Sample &, const std::string &, bool) --> void", pybind11::arg("smp"), pybind11::arg("regex"), pybind11::arg("remove_empty_name_pqueries"));

	// genesis::placement::filter_pqueries_keeping_names(class genesis::placement::Sample &, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > &, bool) file:genesis/placement/function/functions.hpp line:248
	M("genesis::placement").def("filter_pqueries_keeping_names", [](class genesis::placement::Sample & a0, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > & a1) -> void { return genesis::placement::filter_pqueries_keeping_names(a0, a1); }, "", pybind11::arg("smp"), pybind11::arg("keep_list"));
	M("genesis::placement").def("filter_pqueries_keeping_names", (void (*)(class genesis::placement::Sample &, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > &, bool)) &genesis::placement::filter_pqueries_keeping_names, "Remove all PqueryName%s which do not occur in the given \n\n If all names from a given Pquery are removed that way, and if the \n flag is `true` (default), the whole Pquery is removed from the Sample as well.\n\nC++: genesis::placement::filter_pqueries_keeping_names(class genesis::placement::Sample &, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > &, bool) --> void", pybind11::arg("smp"), pybind11::arg("keep_list"), pybind11::arg("remove_empty_name_pqueries"));

	// genesis::placement::filter_pqueries_removing_names(class genesis::placement::Sample &, const std::string &, bool) file:genesis/placement/function/functions.hpp line:260
	M("genesis::placement").def("filter_pqueries_removing_names", [](class genesis::placement::Sample & a0, const std::string & a1) -> void { return genesis::placement::filter_pqueries_removing_names(a0, a1); }, "", pybind11::arg("smp"), pybind11::arg("regex"));
	M("genesis::placement").def("filter_pqueries_removing_names", (void (*)(class genesis::placement::Sample &, const std::string &, bool)) &genesis::placement::filter_pqueries_removing_names, "Remove all PqueryName%s which match the given \n\n If all names from a given Pquery are removed that way, and if the \n flag is `true` (default), the whole Pquery is removed from the Sample as well.\n\nC++: genesis::placement::filter_pqueries_removing_names(class genesis::placement::Sample &, const std::string &, bool) --> void", pybind11::arg("smp"), pybind11::arg("regex"), pybind11::arg("remove_empty_name_pqueries"));

	// genesis::placement::filter_pqueries_removing_names(class genesis::placement::Sample &, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > &, bool) file:genesis/placement/function/functions.hpp line:272
	M("genesis::placement").def("filter_pqueries_removing_names", [](class genesis::placement::Sample & a0, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > & a1) -> void { return genesis::placement::filter_pqueries_removing_names(a0, a1); }, "", pybind11::arg("smp"), pybind11::arg("remove_list"));
	M("genesis::placement").def("filter_pqueries_removing_names", (void (*)(class genesis::placement::Sample &, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > &, bool)) &genesis::placement::filter_pqueries_removing_names, "Remove all PqueryName%s which occur in the given \n\n If all names from a given Pquery are removed that way, and if the \n flag is `true` (default), the whole Pquery is removed from the Sample as well.\n\nC++: genesis::placement::filter_pqueries_removing_names(class genesis::placement::Sample &, const class std::unordered_set<std::string, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<std::string > > &, bool) --> void", pybind11::arg("smp"), pybind11::arg("remove_list"), pybind11::arg("remove_empty_name_pqueries"));

}
