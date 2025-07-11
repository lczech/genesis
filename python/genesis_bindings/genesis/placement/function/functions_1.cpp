#include <genesis/placement/function/functions.hpp>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/placement/pquery/name.hpp>
#include <genesis/placement/pquery/placement.hpp>
#include <genesis/placement/pquery/plain.hpp>
#include <genesis/placement/sample.hpp>
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

void bind_genesis_placement_function_functions_1(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::placement::filter_pqueries_intersecting_names(class genesis::placement::Sample &, class genesis::placement::Sample &, bool) file:genesis/placement/function/functions.hpp line:287
	M("genesis::placement").def("filter_pqueries_intersecting_names", [](class genesis::placement::Sample & a0, class genesis::placement::Sample & a1) -> void { return genesis::placement::filter_pqueries_intersecting_names(a0, a1); }, "", pybind11::arg("sample_1"), pybind11::arg("sample_2"));
	M("genesis::placement").def("filter_pqueries_intersecting_names", (void (*)(class genesis::placement::Sample &, class genesis::placement::Sample &, bool)) &genesis::placement::filter_pqueries_intersecting_names, "Remove all PqueryName%s from the two Sample%s that are unique to each of them.\n\n This function builds the intersection of the set of names of both Samples and only keeps those\n PqueryName%s that occur in both sets.\n\n If all names from a given Pquery are removed that way, and if the \n flag is `true` (default), the whole Pquery is removed from the Sample as well.\n\nC++: genesis::placement::filter_pqueries_intersecting_names(class genesis::placement::Sample &, class genesis::placement::Sample &, bool) --> void", pybind11::arg("sample_1"), pybind11::arg("sample_2"), pybind11::arg("remove_empty_name_pqueries"));

	// genesis::placement::filter_pqueries_differing_names(class genesis::placement::Sample &, class genesis::placement::Sample &, bool) file:genesis/placement/function/functions.hpp line:302
	M("genesis::placement").def("filter_pqueries_differing_names", [](class genesis::placement::Sample & a0, class genesis::placement::Sample & a1) -> void { return genesis::placement::filter_pqueries_differing_names(a0, a1); }, "", pybind11::arg("sample_1"), pybind11::arg("sample_2"));
	M("genesis::placement").def("filter_pqueries_differing_names", (void (*)(class genesis::placement::Sample &, class genesis::placement::Sample &, bool)) &genesis::placement::filter_pqueries_differing_names, "Remove all PqueryName%s from the two Sample%s that occur in both of them.\n\n This function builds the intersection of the set of names of both Samples and removes all those\n PqueryName%s that occur in both sets.\n\n If all names from a given Pquery are removed that way, and if the \n flag is `true` (default), the whole Pquery is removed from the Sample as well.\n\nC++: genesis::placement::filter_pqueries_differing_names(class genesis::placement::Sample &, class genesis::placement::Sample &, bool) --> void", pybind11::arg("sample_1"), pybind11::arg("sample_2"), pybind11::arg("remove_empty_name_pqueries"));

	// genesis::placement::remove_empty_name_pqueries(class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:316
	M("genesis::placement").def("remove_empty_name_pqueries", (unsigned long (*)(class genesis::placement::Sample &)) &genesis::placement::remove_empty_name_pqueries, "Remove all \n\n\n\n\n\n\n\nC++: genesis::placement::remove_empty_name_pqueries(class genesis::placement::Sample &) --> unsigned long", pybind11::arg("sample"));

	// genesis::placement::copy_pqueries(const class genesis::placement::Sample &, class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:333
	M("genesis::placement").def("copy_pqueries", (void (*)(const class genesis::placement::Sample &, class genesis::placement::Sample &)) &genesis::placement::copy_pqueries, "Copy all \n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::copy_pqueries(const class genesis::placement::Sample &, class genesis::placement::Sample &) --> void", pybind11::arg("source"), pybind11::arg("target"));

	// genesis::placement::merge_duplicates(class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:346
	M("genesis::placement").def("merge_duplicates", (void (*)(class genesis::placement::Sample &)) &genesis::placement::merge_duplicates, "Look for \n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::merge_duplicates(class genesis::placement::Sample &) --> void", pybind11::arg("smp"));

	// genesis::placement::collect_duplicate_pqueries(class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:362
	M("genesis::placement").def("collect_duplicate_pqueries", (void (*)(class genesis::placement::Sample &)) &genesis::placement::collect_duplicate_pqueries, "Find all \n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::collect_duplicate_pqueries(class genesis::placement::Sample &) --> void", pybind11::arg("smp"));

	// genesis::placement::merge_duplicate_placements(class genesis::placement::Pquery &) file:genesis/placement/function/functions.hpp line:371
	M("genesis::placement").def("merge_duplicate_placements", (void (*)(class genesis::placement::Pquery &)) &genesis::placement::merge_duplicate_placements, "Merge all PqueryPlacement%s of a Pquery that are on the same TreeEdge into one averaged\n PqueryPlacement.\n\n The merging is done via averaging all values of the PqueryPlacement: `likelihood`,\n `like_weight_ratio`, `proximal_length`, `pendant_length` and `parsimony`.\n\nC++: genesis::placement::merge_duplicate_placements(class genesis::placement::Pquery &) --> void", pybind11::arg("pquery"));

	// genesis::placement::merge_duplicate_placements(class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:376
	M("genesis::placement").def("merge_duplicate_placements", (void (*)(class genesis::placement::Sample &)) &genesis::placement::merge_duplicate_placements, "Call merge_duplicate_placements( Pquery& ) for each Pquery of a Sample.\n\nC++: genesis::placement::merge_duplicate_placements(class genesis::placement::Sample &) --> void", pybind11::arg("smp"));

	// genesis::placement::merge_duplicate_names(class genesis::placement::Pquery &) file:genesis/placement/function/functions.hpp line:382
	M("genesis::placement").def("merge_duplicate_names", (void (*)(class genesis::placement::Pquery &)) &genesis::placement::merge_duplicate_names, "Merge all PqueryName%s that have the same `name` property into one, while adding up their\n `multiplicity`.\n\nC++: genesis::placement::merge_duplicate_names(class genesis::placement::Pquery &) --> void", pybind11::arg("pquery"));

	// genesis::placement::merge_duplicate_names(class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:387
	M("genesis::placement").def("merge_duplicate_names", (void (*)(class genesis::placement::Sample &)) &genesis::placement::merge_duplicate_names, "Call `merge_duplicate_names()` for each Pquery of the Sample.\n\nC++: genesis::placement::merge_duplicate_names(class genesis::placement::Sample &) --> void", pybind11::arg("smp"));

	// genesis::placement::total_name_count(const class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:397
	M("genesis::placement").def("total_name_count", (unsigned long (*)(const class genesis::placement::Sample &)) &genesis::placement::total_name_count, "Get the total number of PqueryName%s in all \n\n\n\nC++: genesis::placement::total_name_count(const class genesis::placement::Sample &) --> unsigned long", pybind11::arg("smp"));

	// genesis::placement::total_placement_count(const class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:403
	M("genesis::placement").def("total_placement_count", (unsigned long (*)(const class genesis::placement::Sample &)) &genesis::placement::total_placement_count, "Get the total number of PqueryPlacement%s in all \n\n\n\nC++: genesis::placement::total_placement_count(const class genesis::placement::Sample &) --> unsigned long", pybind11::arg("smp"));

	// genesis::placement::placement_count_max_edge(const class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:409
	M("genesis::placement").def("placement_count_max_edge", (struct std::pair<const class genesis::tree::TreeEdge *, unsigned long> (*)(const class genesis::placement::Sample &)) &genesis::placement::placement_count_max_edge, "Get the number of placements on the edge with the most placements, and a pointer to this\n edge.\n\nC++: genesis::placement::placement_count_max_edge(const class genesis::placement::Sample &) --> struct std::pair<const class genesis::tree::TreeEdge *, unsigned long>", pybind11::arg("smp"));

	// genesis::placement::placement_mass_max_edge(const class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:415
	M("genesis::placement").def("placement_mass_max_edge", (struct std::pair<const class genesis::tree::TreeEdge *, double> (*)(const class genesis::placement::Sample &)) &genesis::placement::placement_mass_max_edge, "Get the summed mass of the placements on the heaviest edge, measured by their\n `like_weight_ratio`, and a pointer to this edge.\n\nC++: genesis::placement::placement_mass_max_edge(const class genesis::placement::Sample &) --> struct std::pair<const class genesis::tree::TreeEdge *, double>", pybind11::arg("smp"));

	// genesis::placement::closest_leaf_weight_distribution(const class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:421
	M("genesis::placement").def("closest_leaf_weight_distribution", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::placement::Sample &)) &genesis::placement::closest_leaf_weight_distribution, "C++: genesis::placement::closest_leaf_weight_distribution(const class genesis::placement::Sample &) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("sample"));

	// genesis::placement::closest_leaf_depth_histogram(const class genesis::placement::Sample &) file:genesis/placement/function/functions.hpp line:448
	M("genesis::placement").def("closest_leaf_depth_histogram", (class std::vector<int, class std::allocator<int> > (*)(const class genesis::placement::Sample &)) &genesis::placement::closest_leaf_depth_histogram, "Return a histogram representing how many placements have which depth with respect to\n their closest leaf node.\n\n The depth between two nodes on a tree is the number of edges between them. Thus, the depth of a\n placement (which sits on an edge of the tree) to a specific node is the number of edges between\n this node and the closer one of the two nodes at the end of the edge where the placement sits.\n\n The closest leaf to a placement is thus the leaf node which has the smallest depth to that\n placement. This function then returns a histogram of how many placements (values of the vector)\n are there that have a specific depth (indices of the vector) to their closest leaf.\n\n Example: A return vector of\n\n     histogram[0] = 2334\n     histogram[1] = 349\n     histogram[2] = 65\n     histogram[3] = 17\n\n means that there are 2334 placements that sit on an edge which leads to a leaf node (thus, the\n depth of one of the nodes of the edge is 0). It has 349 placements that sit on an edge where\n one of its nodes has one neighbour that is a leaf; and so on.\n\n The vector is automatically resized to the needed number of elements.\n\nC++: genesis::placement::closest_leaf_depth_histogram(const class genesis::placement::Sample &) --> class std::vector<int, class std::allocator<int> >", pybind11::arg("smp"));

	// genesis::placement::closest_leaf_distance_histogram(const class genesis::placement::Sample &, const double, const double, const int) file:genesis/placement/function/functions.hpp line:475
	M("genesis::placement").def("closest_leaf_distance_histogram", [](const class genesis::placement::Sample & a0, const double & a1, const double & a2) -> std::vector<int, class std::allocator<int> > { return genesis::placement::closest_leaf_distance_histogram(a0, a1, a2); }, "", pybind11::arg("smp"), pybind11::arg("min"), pybind11::arg("max"));
	M("genesis::placement").def("closest_leaf_distance_histogram", (class std::vector<int, class std::allocator<int> > (*)(const class genesis::placement::Sample &, const double, const double, const int)) &genesis::placement::closest_leaf_distance_histogram, "Returns a histogram counting the number of placements that have a certain distance to\n their closest leaf node, divided into equally large intervals between a min and a max distance.\n\n The distance range between min and max is divided into `bins` many intervals of equal size.\n Then, the distance from each placement to its closest leaf node is calculated and the counter for\n this particular distance inverval in the histogram is incremented.\n\n The distance is measured along the `branch_length` values of the edges, taking the\n `pendant_length` and `proximal_length` of the placements into account. If the distances is\n outside of the interval [min,max], the counter of the first/last bin is incremented respectively.\n\n Example:\n\n     double min      =  0.0;\n     double max      = 20.0;\n     int    bins     = 25;\n     double bin_size = (max - min) / bins;\n     std::vector<int> hist = closest_leaf_distance_histogram (min, max, bins);\n     for (unsigned int bin = 0; bin < hist.size(); ++bin) {\n         LOG_INFO << \"Bin \" << bin << \" [\" << bin * bin_size << \"; \"\n                  << (bin+1) * bin_size << \") has \" << hist[bin] << \" placements.\";\n     }\n %\n\nC++: genesis::placement::closest_leaf_distance_histogram(const class genesis::placement::Sample &, const double, const double, const int) --> class std::vector<int, class std::allocator<int> >", pybind11::arg("smp"), pybind11::arg("min"), pybind11::arg("max"), pybind11::arg("bins"));

	// genesis::placement::closest_leaf_distance_histogram_auto(const class genesis::placement::Sample &, double &, double &, const int) file:genesis/placement/function/functions.hpp line:509
	M("genesis::placement").def("closest_leaf_distance_histogram_auto", [](const class genesis::placement::Sample & a0, double & a1, double & a2) -> std::vector<int, class std::allocator<int> > { return genesis::placement::closest_leaf_distance_histogram_auto(a0, a1, a2); }, "", pybind11::arg("smp"), pybind11::arg("min"), pybind11::arg("max"));
	M("genesis::placement").def("closest_leaf_distance_histogram_auto", (class std::vector<int, class std::allocator<int> > (*)(const class genesis::placement::Sample &, double &, double &, const int)) &genesis::placement::closest_leaf_distance_histogram_auto, "Returns the same type of histogram as closest_leaf_distance_histogram(), but automatically\n determines the needed boundaries.\n\n See closest_leaf_distance_histogram() for general information about what this function does. The\n difference between both functions is that this one first procresses all distances from\n placements to their closest leaf nodes to find out what the shortest and longest are, then sets\n the boundaries of the histogram accordingly. The number of bins is then used to divide this\n range into intervals of equal size.\n\n The boundaries are returned by passing two doubles `min` and `max` to the function by reference.\n The value of `max` will actually contain the result of std::nextafter() called on the longest\n distance; this makes sure that the value itself will be placed in the interval.\n\n Example:\n\n     double min, max;\n     int    bins = 25;\n     std::vector<int> hist = closest_leaf_distance_histogram (min, max, bins);\n     double bin_size = (max - min) / bins;\n     LOG_INFO << \"Histogram boundaries: [\" << min << \",\" << max << \").\";\n     for (unsigned int bin = 0; bin < hist.size(); ++bin) {\n         LOG_INFO << \"Bin \" << bin << \" [\" << bin * bin_size << \"; \"\n                  << (bin+1) * bin_size << \") has \" << hist[bin] << \" placements.\";\n     }\n\n It has a slightly higher time and memory consumption than the non-automatic version\n closest_leaf_distance_histogram(), as it needs to process the values twice in order to find their\n min and max.\n\nC++: genesis::placement::closest_leaf_distance_histogram_auto(const class genesis::placement::Sample &, double &, double &, const int) --> class std::vector<int, class std::allocator<int> >", pybind11::arg("smp"), pybind11::arg("min"), pybind11::arg("max"), pybind11::arg("bins"));

	{ // genesis::placement::PqueryPlacementPlain file:genesis/placement/pquery/plain.hpp line:51
		pybind11::class_<genesis::placement::PqueryPlacementPlain, std::shared_ptr<genesis::placement::PqueryPlacementPlain>> cl(M("genesis::placement"), "PqueryPlacementPlain", "Simple POD struct for a Placement used for speeding up some calculations.\n\n It is not as flexible as the default representation, but its memory footprint is compact, because\n of the lack of pointers. Except for its purpose in some calculations, it is normally not needed\n for anything else.");
		cl.def( pybind11::init( [](){ return new genesis::placement::PqueryPlacementPlain(); } ) );
		cl.def_readwrite("edge_index", &genesis::placement::PqueryPlacementPlain::edge_index);
		cl.def_readwrite("primary_node_index", &genesis::placement::PqueryPlacementPlain::primary_node_index);
		cl.def_readwrite("secondary_node_index", &genesis::placement::PqueryPlacementPlain::secondary_node_index);
		cl.def_readwrite("branch_length", &genesis::placement::PqueryPlacementPlain::branch_length);
		cl.def_readwrite("pendant_length", &genesis::placement::PqueryPlacementPlain::pendant_length);
		cl.def_readwrite("proximal_length", &genesis::placement::PqueryPlacementPlain::proximal_length);
		cl.def_readwrite("like_weight_ratio", &genesis::placement::PqueryPlacementPlain::like_weight_ratio);
	}
}
