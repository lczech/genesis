#include <genesis/placement/function/helper.hpp>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/placement/pquery/name.hpp>
#include <genesis/placement/pquery/placement.hpp>
#include <genesis/placement/pquery/plain.hpp>
#include <genesis/placement/sample.hpp>
#include <genesis/placement/sample_set.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
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

void bind_genesis_placement_pquery_plain(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::placement::PqueryPlain file:genesis/placement/pquery/plain.hpp line:74
		pybind11::class_<genesis::placement::PqueryPlain, std::shared_ptr<genesis::placement::PqueryPlain>> cl(M("genesis::placement"), "PqueryPlain", "Simple POD struct that stores the information of a Pquery in a simple format for\n speeding up some calculations.\n\n This class is used as an alternative representation of Pqueries that does not use pointer and\n thus is faster in certain calculations. It is normally not needed for anything else.");
		cl.def( pybind11::init( [](){ return new genesis::placement::PqueryPlain(); } ) );
		cl.def( pybind11::init( [](genesis::placement::PqueryPlain const &o){ return new genesis::placement::PqueryPlain(o); } ) );
		cl.def_readwrite("index", &genesis::placement::PqueryPlain::index);
		cl.def_readwrite("multiplicity", &genesis::placement::PqueryPlain::multiplicity);
		cl.def_readwrite("placements", &genesis::placement::PqueryPlain::placements);
		cl.def("assign", (struct genesis::placement::PqueryPlain & (genesis::placement::PqueryPlain::*)(const struct genesis::placement::PqueryPlain &)) &genesis::placement::PqueryPlain::operator=, "C++: genesis::placement::PqueryPlain::operator=(const struct genesis::placement::PqueryPlain &) --> struct genesis::placement::PqueryPlain &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	// genesis::placement::pqueries_per_edge(const class genesis::placement::Sample &, bool) file:genesis/placement/function/helper.hpp line:77
	M("genesis::placement").def("pqueries_per_edge", [](const class genesis::placement::Sample & a0) -> std::vector<class std::vector<const class genesis::placement::Pquery *, class std::allocator<const class genesis::placement::Pquery *> >, class std::allocator<class std::vector<const class genesis::placement::Pquery *, class std::allocator<const class genesis::placement::Pquery *> > > > { return genesis::placement::pqueries_per_edge(a0); }, "", pybind11::arg("sample"));
	M("genesis::placement").def("pqueries_per_edge", (class std::vector<class std::vector<const class genesis::placement::Pquery *, class std::allocator<const class genesis::placement::Pquery *> >, class std::allocator<class std::vector<const class genesis::placement::Pquery *, class std::allocator<const class genesis::placement::Pquery *> > > > (*)(const class genesis::placement::Sample &, bool)) &genesis::placement::pqueries_per_edge, "Return a mapping from each edge to the Pqueries on that edge.\n\n If  is `false` (default), each PqueryPlacement of the\n \n\n\n\n\nC++: genesis::placement::pqueries_per_edge(const class genesis::placement::Sample &, bool) --> class std::vector<class std::vector<const class genesis::placement::Pquery *, class std::allocator<const class genesis::placement::Pquery *> >, class std::allocator<class std::vector<const class genesis::placement::Pquery *, class std::allocator<const class genesis::placement::Pquery *> > > >", pybind11::arg("sample"), pybind11::arg("only_max_lwr_placements"));

	// genesis::placement::placements_per_edge(const class genesis::placement::Sample &, bool) file:genesis/placement/function/helper.hpp line:96
	M("genesis::placement").def("placements_per_edge", [](const class genesis::placement::Sample & a0) -> std::vector<class std::vector<const class genesis::placement::PqueryPlacement *, class std::allocator<const class genesis::placement::PqueryPlacement *> >, class std::allocator<class std::vector<const class genesis::placement::PqueryPlacement *, class std::allocator<const class genesis::placement::PqueryPlacement *> > > > { return genesis::placement::placements_per_edge(a0); }, "", pybind11::arg("smp"));
	M("genesis::placement").def("placements_per_edge", (class std::vector<class std::vector<const class genesis::placement::PqueryPlacement *, class std::allocator<const class genesis::placement::PqueryPlacement *> >, class std::allocator<class std::vector<const class genesis::placement::PqueryPlacement *, class std::allocator<const class genesis::placement::PqueryPlacement *> > > > (*)(const class genesis::placement::Sample &, bool)) &genesis::placement::placements_per_edge, "Return a mapping from each PlacementTreeEdge%s to the PqueryPlacement%s that\n are placed on that edge.\n\n The result `vector` is indexed using PlacementTreeEdge::index(). For each entry, it contains\n another `vector` that holds pointers to the PqueryPlacement%s of the Sample.\n\n If the optional parameter `only_max_lwr_placements` is set to `false` (default), each placement\n in the Sample is added, not just the most likely ones. If set to `true`, only the PqueryPlacement\n with the highest \n\n\n\n\n\n\nC++: genesis::placement::placements_per_edge(const class genesis::placement::Sample &, bool) --> class std::vector<class std::vector<const class genesis::placement::PqueryPlacement *, class std::allocator<const class genesis::placement::PqueryPlacement *> >, class std::allocator<class std::vector<const class genesis::placement::PqueryPlacement *, class std::allocator<const class genesis::placement::PqueryPlacement *> > > >", pybind11::arg("smp"), pybind11::arg("only_max_lwr_placements"));

	// genesis::placement::placements_per_edge(const class genesis::placement::Sample &, const class genesis::tree::TreeEdge &) file:genesis/placement/function/helper.hpp line:111
	M("genesis::placement").def("placements_per_edge", (class std::vector<const class genesis::placement::PqueryPlacement *, class std::allocator<const class genesis::placement::PqueryPlacement *> > (*)(const class genesis::placement::Sample &, const class genesis::tree::TreeEdge &)) &genesis::placement::placements_per_edge, "Return a vector of all PqueryPlacement%s that are placed on the given PlacementTreeEdge.\n\n This functions iterates over all placements and collects those that are placed on the given\n edge. In case that this is needed for multiple edges, it is faster to use\n placements_per_edge( Sample ) instead.\n\n The result is invalidated when calling Pquery::add_placement() or other functions that change\n the number of \n\n\n\nC++: genesis::placement::placements_per_edge(const class genesis::placement::Sample &, const class genesis::tree::TreeEdge &) --> class std::vector<const class genesis::placement::PqueryPlacement *, class std::allocator<const class genesis::placement::PqueryPlacement *> >", pybind11::arg("smp"), pybind11::arg("edge"));

	// genesis::placement::placement_count_per_edge(const class genesis::placement::Sample &) file:genesis/placement/function/helper.hpp line:122
	M("genesis::placement").def("placement_count_per_edge", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::placement::Sample &)) &genesis::placement::placement_count_per_edge, "Return a vector that contains the number of PqueryPlacement%s per\n \n\n\n\n\n\nC++: genesis::placement::placement_count_per_edge(const class genesis::placement::Sample &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("sample"));

	// genesis::placement::placement_count_per_edge(const class genesis::placement::SampleSet &) file:genesis/placement/function/helper.hpp line:124
	M("genesis::placement").def("placement_count_per_edge", (class genesis::utils::Matrix<unsigned long> (*)(const class genesis::placement::SampleSet &)) &genesis::placement::placement_count_per_edge, "C++: genesis::placement::placement_count_per_edge(const class genesis::placement::SampleSet &) --> class genesis::utils::Matrix<unsigned long>", pybind11::arg("sample_set"));

	// genesis::placement::plain_queries(const class genesis::placement::Sample &) file:genesis/placement/function/helper.hpp line:139
	M("genesis::placement").def("plain_queries", (class std::vector<struct genesis::placement::PqueryPlain, class std::allocator<struct genesis::placement::PqueryPlain> > (*)(const class genesis::placement::Sample &)) &genesis::placement::plain_queries, "Return a plain representation of all pqueries of this map.\n\n This method produces a whole copy of all pqueries and their placements (though, not their names)\n in a plain POD format. This format is meant for speeding up computations that need access to\n the data a lot - which would require several pointer indirections in the normal representation\n of the data.\n\n This comes of course at the cost of reduced flexibility, as all indices are fixed in the\n plain data structre: changing a value here will not have any effect on the original data or\n even on the values of the pqueries. Thus, most probably this will lead to corruption. Therefore,\n this data structure is meant for reading only.\n\nC++: genesis::placement::plain_queries(const class genesis::placement::Sample &) --> class std::vector<struct genesis::placement::PqueryPlain, class std::allocator<struct genesis::placement::PqueryPlain> >", pybind11::arg("smp"));

	// genesis::placement::rectify_values(class genesis::placement::Sample &) file:genesis/placement/function/helper.hpp line:163
	M("genesis::placement").def("rectify_values", (void (*)(class genesis::placement::Sample &)) &genesis::placement::rectify_values, "Correct invalid values of the PqueryPlacement%s and PqueryName%s as good as possible.\n\n Some values can be slightly outside their valid boundaries, either for numerical reasons, or\n because something went wrong. Often, those can be rectified without too much loss of information.\n\n This function\n\n   * sets negative `like_weight_ratio` to `0.0`\n   * sets `like_weight_ratio > 1.0` to `1.0`\n   * normalizes the `like_weight_ratio` if their sum is `> 1.0`\n   * sets negative `pendant_length` to `0.0`\n   * sets negative `proximal_length` to `0.0`\n   * sets `proximal_length > branch_length` to `branch_length` for its edge.\n   * sets negative `multiplicity` to `0.0` for the name\n\n See rectify_values( SampleSet& ) for a version of this function that works on whole SampleSet%s.\n\nC++: genesis::placement::rectify_values(class genesis::placement::Sample &) --> void", pybind11::arg("sample"));

	// genesis::placement::rectify_values(class genesis::placement::SampleSet &) file:genesis/placement/function/helper.hpp line:171
	M("genesis::placement").def("rectify_values", (void (*)(class genesis::placement::SampleSet &)) &genesis::placement::rectify_values, "Correct invalid values of the PqueryPlacement%s and PqueryName%s as good as possible.\n\n This function calls rectify_values( Sample& ) for all Sample%s in the SampleSet. See there\n for details.\n\nC++: genesis::placement::rectify_values(class genesis::placement::SampleSet &) --> void", pybind11::arg("sset"));

	// genesis::placement::reset_edge_nums(class genesis::tree::Tree &) file:genesis/placement/function/helper.hpp line:183
	M("genesis::placement").def("reset_edge_nums", (void (*)(class genesis::tree::Tree &)) &genesis::placement::reset_edge_nums, "Reset all edge nums of a ::PlacementTree.\n\n The `edge_num` property of the PlacementTreeEdge%s is defined by the `jplace` standard.\n The values have to be assigned increasingly with a postorder traversal of the tree.\n This function resets them so that this is established.\n\n See has_correct_edge_nums() to check whether the edge nums are already correct. This should\n be the case for any valid `jplace` file.\n\nC++: genesis::placement::reset_edge_nums(class genesis::tree::Tree &) --> void", pybind11::arg("tree"));

	// genesis::placement::has_consecutive_edge_nums(const class genesis::tree::Tree &) file:genesis/placement/function/helper.hpp line:201
	M("genesis::placement").def("has_consecutive_edge_nums", (bool (*)(const class genesis::tree::Tree &)) &genesis::placement::has_consecutive_edge_nums, "Verify that the ::PlacementTree has no duplicate\n \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::has_consecutive_edge_nums(const class genesis::tree::Tree &) --> bool", pybind11::arg("tree"));

	// genesis::placement::has_correct_edge_nums(const class genesis::tree::Tree &) file:genesis/placement/function/helper.hpp line:213
	M("genesis::placement").def("has_correct_edge_nums", (bool (*)(const class genesis::tree::Tree &)) &genesis::placement::has_correct_edge_nums, "Verify that the tree has correctly set edge nums.\n\n The `edge_num` property of the PlacementTreeEdge%s is defined by the `jplace` standard.\n The values have to be assigned increasingly with a postorder traversal of the tree.\n This function checks whether this is the case.\n\n See also has_consecutive_edge_nums() for a relaxed version of this function, which might also\n be useful in some cases where the strict correct order according to the standard is not needed.\n\nC++: genesis::placement::has_correct_edge_nums(const class genesis::tree::Tree &) --> bool", pybind11::arg("tree"));

	// genesis::placement::validate(const class genesis::placement::Sample &, bool, bool) file:genesis/placement/function/helper.hpp line:226
	M("genesis::placement").def("validate", [](const class genesis::placement::Sample & a0) -> bool { return genesis::placement::validate(a0); }, "", pybind11::arg("smp"));
	M("genesis::placement").def("validate", [](const class genesis::placement::Sample & a0, bool const & a1) -> bool { return genesis::placement::validate(a0, a1); }, "", pybind11::arg("smp"), pybind11::arg("check_values"));
	M("genesis::placement").def("validate", (bool (*)(const class genesis::placement::Sample &, bool, bool)) &genesis::placement::validate, "Validate the integrity of the pointers, references and data in a Sample object.\n\n Returns true iff everything is set up correctly. In case of inconsistencies, the function stops\n and returns false on the first encountered error.\n\n If `check_values` is set to true, also a check on the validity of numerical values is done, for\n example that the proximal_length is smaller than the corresponding branch_length.\n If additionally `break_on_values` is set, validate() will stop on the first encountered invalid\n value. Otherwise it will report all invalid values to the log stream.\n\nC++: genesis::placement::validate(const class genesis::placement::Sample &, bool, bool) --> bool", pybind11::arg("smp"), pybind11::arg("check_values"), pybind11::arg("break_on_values"));

}
