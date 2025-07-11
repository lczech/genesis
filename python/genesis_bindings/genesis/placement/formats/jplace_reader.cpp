#include <genesis/placement/formats/jplace_reader.hpp>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/placement/pquery/name.hpp>
#include <genesis/placement/pquery/placement.hpp>
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
#include <genesis/utils/containers/range.hpp>
#include <genesis/utils/io/base_input_source.hpp>
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

// genesis::placement::PlacementNodeData file:genesis/placement/placement_tree.hpp line:87
struct PyCallBack_genesis_placement_PlacementNodeData : public genesis::placement::PlacementNodeData {
	using genesis::placement::PlacementNodeData::PlacementNodeData;

};

// genesis::placement::PlacementEdgeData file:genesis/placement/placement_tree.hpp line:139
struct PyCallBack_genesis_placement_PlacementEdgeData : public genesis::placement::PlacementEdgeData {
	using genesis::placement::PlacementEdgeData::PlacementEdgeData;

};

void bind_genesis_placement_formats_jplace_reader(std::function< pybind11::module &(std::string const &namespace_) > &M)
{

	extend_helper(M("genesis::placement"));
	{ // genesis::placement::JplaceReader file:genesis/placement/formats/jplace_reader.hpp line:99
		pybind11::class_<genesis::placement::JplaceReader, std::shared_ptr<genesis::placement::JplaceReader>> cl(M("genesis::placement"), "JplaceReader", "Read Jplace data.\n\n This class provides facilities for reading `jplace` files into a Sample or SampleSet.\n Exemplary usage:\n\n     std::string infile = \"path/to/file.jplace\";\n     Sample smp = JplaceReader()\n         .invalid_number_behaviour( InvalidNumberBehaviour::kFix )\n         .read( from_file( infile ));\n\n Using \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::placement::JplaceReader(); } ) );
		cl.def( pybind11::init( [](genesis::placement::JplaceReader const &o){ return new genesis::placement::JplaceReader(o); } ) );

		pybind11::enum_<genesis::placement::JplaceReader::InvalidNumberBehaviour>(cl, "InvalidNumberBehaviour", "Enum to determine the behaviour of the reader in case of invalid numbers.\n\n When reading from `jplace` files, the numerical fields of each placement can contain small\n deviations or inaccuracies. Those are due to numerical issues or floating point number to\n text conversions, which occur when storing the exact values in a textual representation.\n\n Currently, the following checks are offered:\n\n   * `like_weight_ratio < 0.0`\n   * `like_weight_ratio > 1.0`\n   * `pendant_length    < 0.0`\n   * `proximal_length   < 0.0`\n   * `proximal_length   > branch_length`\n\n This enum offers choices to decide how the JplaceReader reacts when facing those errors.\n See invalid_number_behaviour( InvalidNumberBehaviour ) to set the behaviour.")
			.value("kIgnore", genesis::placement::JplaceReader::InvalidNumberBehaviour::kIgnore)
			.value("kLog", genesis::placement::JplaceReader::InvalidNumberBehaviour::kLog)
			.value("kFix", genesis::placement::JplaceReader::InvalidNumberBehaviour::kFix)
			.value("kLogAndFix", genesis::placement::JplaceReader::InvalidNumberBehaviour::kLogAndFix)
			.value("kThrow", genesis::placement::JplaceReader::InvalidNumberBehaviour::kThrow);

		cl.def("assign", (class genesis::placement::JplaceReader & (genesis::placement::JplaceReader::*)(const class genesis::placement::JplaceReader &)) &genesis::placement::JplaceReader::operator=, "C++: genesis::placement::JplaceReader::operator=(const class genesis::placement::JplaceReader &) --> class genesis::placement::JplaceReader &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("read", (class genesis::placement::Sample (genesis::placement::JplaceReader::*)(class std::shared_ptr<class genesis::utils::BaseInputSource>) const) &genesis::placement::JplaceReader::read, "Read from an input source.\n\n Use functions such as utils::from_file() and utils::from_string() to conveniently\n get an input source that can be used here.\n\nC++: genesis::placement::JplaceReader::read(class std::shared_ptr<class genesis::utils::BaseInputSource>) const --> class genesis::placement::Sample", pybind11::arg("source"));
		cl.def("read", (class genesis::placement::SampleSet (genesis::placement::JplaceReader::*)(class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > >) const) &genesis::placement::JplaceReader::read, "Read from multiple sources and parse them as a Jplace documents into a SampleSet.\n\nC++: genesis::placement::JplaceReader::read(class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > >) const --> class genesis::placement::SampleSet", pybind11::arg("sources"));
		cl.def("read", (void (genesis::placement::JplaceReader::*)(class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > >, class genesis::placement::SampleSet &) const) &genesis::placement::JplaceReader::read, "Read from multiple sources and parse them as a Jplace documents into an existing\n SampleSet.\n\n The Sample%s are added to the SampleSet, so that existing Samples in the SampleSet are kept.\n\nC++: genesis::placement::JplaceReader::read(class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > >, class genesis::placement::SampleSet &) const --> void", pybind11::arg("sources"), pybind11::arg("target"));
		cl.def("invalid_number_behaviour", (enum genesis::placement::JplaceReader::InvalidNumberBehaviour (genesis::placement::JplaceReader::*)() const) &genesis::placement::JplaceReader::invalid_number_behaviour, "Return the currenlty set InvalidNumberBehaviour.\n\nC++: genesis::placement::JplaceReader::invalid_number_behaviour() const --> enum genesis::placement::JplaceReader::InvalidNumberBehaviour");
		cl.def("invalid_number_behaviour", (class genesis::placement::JplaceReader & (genesis::placement::JplaceReader::*)(enum genesis::placement::JplaceReader::InvalidNumberBehaviour)) &genesis::placement::JplaceReader::invalid_number_behaviour, "Set the InvalidNumberBehaviour.\n\n This setter controls the InvalidNumberBehaviour of the JplaceReader. The default value is\n InvalidNumberBehaviour::kIgnore. See InvalidNumberBehaviour for the valid options.\n\n The function returns the JplaceReader object to allow for a fluent interface.\n\nC++: genesis::placement::JplaceReader::invalid_number_behaviour(enum genesis::placement::JplaceReader::InvalidNumberBehaviour) --> class genesis::placement::JplaceReader &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));
	}
	{ // genesis::placement::PlacementNodeData file:genesis/placement/placement_tree.hpp line:87
		pybind11::class_<genesis::placement::PlacementNodeData, std::shared_ptr<genesis::placement::PlacementNodeData>, PyCallBack_genesis_placement_PlacementNodeData, genesis::tree::CommonNodeData> cl(M("genesis::placement"), "PlacementNodeData", "Data class for ::PlacementTreeNode%s. Stores a node name.\n\n This class is derived from tree::CommonNodeData and currently adds no functionality on top\n of this base class. See there for more information.\n Also, see ::PlacementTree for information on how and where this class is used.");
		cl.def_static("create", (class std::unique_ptr<class genesis::placement::PlacementNodeData, struct std::default_delete<class genesis::placement::PlacementNodeData> > (*)()) &genesis::placement::PlacementNodeData::create, "C++: genesis::placement::PlacementNodeData::create() --> class std::unique_ptr<class genesis::placement::PlacementNodeData, struct std::default_delete<class genesis::placement::PlacementNodeData> >");
	}
	{ // genesis::placement::PlacementEdgeData file:genesis/placement/placement_tree.hpp line:139
		pybind11::class_<genesis::placement::PlacementEdgeData, std::shared_ptr<genesis::placement::PlacementEdgeData>, PyCallBack_genesis_placement_PlacementEdgeData, genesis::tree::CommonEdgeData> cl(M("genesis::placement"), "PlacementEdgeData", "Data class for PlacementTreeEdge%s. Stores the branch\n length of the edge, and the `edge_num`, as defined in the `jplace` standard.\n\n This class is derived from tree::CommonEdgeData. See there for more information.\n Also, see ::PlacementTree for information on how and where this class is used.");
		cl.def_static("create", (class std::unique_ptr<class genesis::placement::PlacementEdgeData, struct std::default_delete<class genesis::placement::PlacementEdgeData> > (*)()) &genesis::placement::PlacementEdgeData::create, "C++: genesis::placement::PlacementEdgeData::create() --> class std::unique_ptr<class genesis::placement::PlacementEdgeData, struct std::default_delete<class genesis::placement::PlacementEdgeData> >");
		cl.def("edge_num", (int (genesis::placement::PlacementEdgeData::*)() const) &genesis::placement::PlacementEdgeData::edge_num, "Return the `edge_num` of this edge. This value is defined by the `jplace` standard.\n\nC++: genesis::placement::PlacementEdgeData::edge_num() const --> int");
		cl.def("reset_edge_num", (void (genesis::placement::PlacementEdgeData::*)(int)) &genesis::placement::PlacementEdgeData::reset_edge_num, "Force to set the edge_num to a certain value.\n\n The edge_num is usually considered to be constant for the tree. It rarely needs to be set,\n except for when constructing the object. Use with care.\n\nC++: genesis::placement::PlacementEdgeData::reset_edge_num(int) --> void", pybind11::arg("val"));
	}
	{ // genesis::placement::PqueryName file:genesis/placement/pquery/name.hpp line:55
		pybind11::class_<genesis::placement::PqueryName, std::shared_ptr<genesis::placement::PqueryName>> cl(M("genesis::placement"), "PqueryName", "A name of a Pquery and its multiplicity.\n\n This class is modeled after the `jplace` standard, which allows for multiple names for a Pquery.\n This is useful if there are identical sequences in the original data for which the phylogenetic\n placement was carried out. The placements of those sequences can then be treated as one entity,\n i.e., one Pquery, while still maintaining all their identifiers (names).\n\n Furthermore, each such #name can have a #multiplicity, which can be used to store, e.g., the\n number of replicates of the original sequence. It is used as a factor for the weights of\n PqueryPlacement%s in some calculations.");
		cl.def( pybind11::init( [](){ return new genesis::placement::PqueryName(); } ) );
		cl.def( pybind11::init( [](const std::string & a0){ return new genesis::placement::PqueryName(a0); } ), "doc" , pybind11::arg("name"));
		cl.def( pybind11::init<const std::string &, double>(), pybind11::arg("name"), pybind11::arg("multiplicity") );

		cl.def( pybind11::init( [](genesis::placement::PqueryName const &o){ return new genesis::placement::PqueryName(o); } ) );
		cl.def_readwrite("name", &genesis::placement::PqueryName::name);
		cl.def_readwrite("multiplicity", &genesis::placement::PqueryName::multiplicity);
		cl.def("assign", (class genesis::placement::PqueryName & (genesis::placement::PqueryName::*)(const class genesis::placement::PqueryName &)) &genesis::placement::PqueryName::operator=, "C++: genesis::placement::PqueryName::operator=(const class genesis::placement::PqueryName &) --> class genesis::placement::PqueryName &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::placement::PqueryPlacement file:genesis/placement/pquery/placement.hpp line:75
		pybind11::class_<genesis::placement::PqueryPlacement, std::shared_ptr<genesis::placement::PqueryPlacement>> cl(M("genesis::placement"), "PqueryPlacement", "One placement position of a Pquery on a Tree.\n\n This class is modeled after the `jplace` standard, which allows for multiple placement positions\n for a Pquery. Usually, those positions are on different branches of the tree. The property\n values of this class describe one such placement position.\n\n In order to check the position of this placement on the tree, see #proximal_length,\n #pendant_length and edge(). In order to check the likelihood and probability of this placement\n beging placed exaclty where it is, see #likelihood and #like_weight_ratio.");
		cl.def( pybind11::init( [](){ return new genesis::placement::PqueryPlacement(); } ) );
		cl.def( pybind11::init<class genesis::tree::TreeEdge &>(), pybind11::arg("edge") );

		cl.def( pybind11::init( [](genesis::placement::PqueryPlacement const &o){ return new genesis::placement::PqueryPlacement(o); } ) );
		cl.def_readwrite("likelihood", &genesis::placement::PqueryPlacement::likelihood);
		cl.def_readwrite("like_weight_ratio", &genesis::placement::PqueryPlacement::like_weight_ratio);
		cl.def_readwrite("proximal_length", &genesis::placement::PqueryPlacement::proximal_length);
		cl.def_readwrite("pendant_length", &genesis::placement::PqueryPlacement::pendant_length);
		cl.def("assign", (class genesis::placement::PqueryPlacement & (genesis::placement::PqueryPlacement::*)(const class genesis::placement::PqueryPlacement &)) &genesis::placement::PqueryPlacement::operator=, "C++: genesis::placement::PqueryPlacement::operator=(const class genesis::placement::PqueryPlacement &) --> class genesis::placement::PqueryPlacement &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("edge_num", (int (genesis::placement::PqueryPlacement::*)() const) &genesis::placement::PqueryPlacement::edge_num, "Get the `edge_num` where this PqueryPlacement is placed.\n\n This number corresponds to the `edge_num` property as described in the `jplace` standard.\n It is not to be confused with the index of the PlacementTreeEdge.\n\nC++: genesis::placement::PqueryPlacement::edge_num() const --> int");
		cl.def("edge", (class genesis::tree::TreeEdge & (genesis::placement::PqueryPlacement::*)()) &genesis::placement::PqueryPlacement::edge, "Get the PlacementTreeEdge where this PqueryPlacement is placed.\n\nC++: genesis::placement::PqueryPlacement::edge() --> class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal);
		cl.def("reset_edge", (void (genesis::placement::PqueryPlacement::*)(class genesis::tree::TreeEdge &)) &genesis::placement::PqueryPlacement::reset_edge, "Set the PlacementTreeEdge at which this PqueryPlacement is placed.\n\n This should be rarely needed. It is mostly intended for the Readers that populate the data.\n When setting this value, the user is responsible to make sure that the new value is actually\n a PlacementTreeEdge of the PlacementTree that belongs to the Sample where the Pquery of this\n PqueryPlacement is stored.\n\nC++: genesis::placement::PqueryPlacement::reset_edge(class genesis::tree::TreeEdge &) --> void", pybind11::arg("edge"));
	}
	{ // genesis::placement::Pquery file:genesis/placement/pquery.hpp line:82
		pybind11::class_<genesis::placement::Pquery, std::shared_ptr<genesis::placement::Pquery>> cl(M("genesis::placement"), "Pquery", "A pquery holds a set of PqueryPlacement%s and a set of PqueryName%s.\n\n According to the `jplace` standard, a pquery is a container object that represents the possible\n phylogenetic placement positions of a sequence (or set of sequences). Each such position is\n stored as a PqueryPlacement and can be accessed via the functions of this class.\n\n Furthermore, it might be useful to combine the placement positions of several sequences into\n one object. This is for example the case if there are replicate sequences. Thus, a Pquery\n supports to store multiple PqueryName%s, each of them containing an identifying name string and\n a so called \n\n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::placement::Pquery(); } ) );
		cl.def( pybind11::init( [](genesis::placement::Pquery const &o){ return new genesis::placement::Pquery(o); } ) );
		cl.def("assign", (class genesis::placement::Pquery & (genesis::placement::Pquery::*)(const class genesis::placement::Pquery &)) &genesis::placement::Pquery::operator=, "C++: genesis::placement::Pquery::operator=(const class genesis::placement::Pquery &) --> class genesis::placement::Pquery &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("clear", (void (genesis::placement::Pquery::*)()) &genesis::placement::Pquery::clear, "Clear all data stored in this Pquery, i.e., clear all PqueryName%s and all\n PqueryPlacement%s.\n\nC++: genesis::placement::Pquery::clear() --> void");
		cl.def("begin_placements", (class __gnu_cxx::__normal_iterator<class genesis::placement::PqueryPlacement *, class std::vector<class genesis::placement::PqueryPlacement, class std::allocator<class genesis::placement::PqueryPlacement> > > (genesis::placement::Pquery::*)()) &genesis::placement::Pquery::begin_placements, "C++: genesis::placement::Pquery::begin_placements() --> class __gnu_cxx::__normal_iterator<class genesis::placement::PqueryPlacement *, class std::vector<class genesis::placement::PqueryPlacement, class std::allocator<class genesis::placement::PqueryPlacement> > >");
		cl.def("end_placements", (class __gnu_cxx::__normal_iterator<class genesis::placement::PqueryPlacement *, class std::vector<class genesis::placement::PqueryPlacement, class std::allocator<class genesis::placement::PqueryPlacement> > > (genesis::placement::Pquery::*)()) &genesis::placement::Pquery::end_placements, "C++: genesis::placement::Pquery::end_placements() --> class __gnu_cxx::__normal_iterator<class genesis::placement::PqueryPlacement *, class std::vector<class genesis::placement::PqueryPlacement, class std::allocator<class genesis::placement::PqueryPlacement> > >");
		cl.def("expose_placements", (class std::vector<class genesis::placement::PqueryPlacement, class std::allocator<class genesis::placement::PqueryPlacement> > & (genesis::placement::Pquery::*)()) &genesis::placement::Pquery::expose_placements, "C++: genesis::placement::Pquery::expose_placements() --> class std::vector<class genesis::placement::PqueryPlacement, class std::allocator<class genesis::placement::PqueryPlacement> > &", pybind11::return_value_policy::reference_internal);
		cl.def("add_placement", (class genesis::placement::PqueryPlacement & (genesis::placement::Pquery::*)(class genesis::tree::TreeEdge &)) &genesis::placement::Pquery::add_placement, "Create a new PqueryPlacement at a given PlacementTreeEdge, add it to the Pquery and\n return it.\n\n The values of the placement can then be adjusted using the returned object reference.\n\n It is important that the provided edge belongs to the same PlacementTree as the Pquery and\n its containing Sample do. This is up to the user and not checked.\n\n As this function might reallocate the memory where placements are stored, all iterators\n and pointer to PqueryPlacement%s are invalidated.\n\nC++: genesis::placement::Pquery::add_placement(class genesis::tree::TreeEdge &) --> class genesis::placement::PqueryPlacement &", pybind11::return_value_policy::reference_internal, pybind11::arg("edge"));
		cl.def("add_placement", (class genesis::placement::PqueryPlacement & (genesis::placement::Pquery::*)(const class genesis::placement::PqueryPlacement &)) &genesis::placement::Pquery::add_placement, "Create a new PqueryPlacement as a copy of the provided one, add it to the Pquery and\n return it.\n\n As this function might reallocate the memory where placements are stored, all iterators\n and pointer to PqueryPlacement%s are considered to be invalidated.\n\nC++: genesis::placement::Pquery::add_placement(const class genesis::placement::PqueryPlacement &) --> class genesis::placement::PqueryPlacement &", pybind11::return_value_policy::reference_internal, pybind11::arg("val"));
		cl.def("add_placement", (class genesis::placement::PqueryPlacement & (genesis::placement::Pquery::*)(class genesis::tree::TreeEdge &, const class genesis::placement::PqueryPlacement &)) &genesis::placement::Pquery::add_placement, "Create a new PqueryPlacement at a given PlacementTreeEdge, add it to the Pquery and\n return it. The property values of the provided PqueryPlacement are copied.\n\n It is important that the provided edge belongs to the same PlacementTree as the Pquery and\n its containing Sample do. This is up to the user and not checked.\n\n As this function might reallocate the memory where placements are stored, all iterators\n and pointer to PqueryPlacement%s are considered to be invalidated.\n\nC++: genesis::placement::Pquery::add_placement(class genesis::tree::TreeEdge &, const class genesis::placement::PqueryPlacement &) --> class genesis::placement::PqueryPlacement &", pybind11::return_value_policy::reference_internal, pybind11::arg("edge"), pybind11::arg("val"));
		cl.def("placement_size", (unsigned long (genesis::placement::Pquery::*)() const) &genesis::placement::Pquery::placement_size, "Return the number of PqueryPlacement%s stored in this Pquery.\n\nC++: genesis::placement::Pquery::placement_size() const --> unsigned long");
		cl.def("placement_at", (class genesis::placement::PqueryPlacement & (genesis::placement::Pquery::*)(unsigned long)) &genesis::placement::Pquery::placement_at, "Return the PqueryPlacement at a certain index.\n\n The index must be smaller than placement_size(), otherwise this functions throws an exception.\n\nC++: genesis::placement::Pquery::placement_at(unsigned long) --> class genesis::placement::PqueryPlacement &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("remove_placement_at", (void (genesis::placement::Pquery::*)(unsigned long)) &genesis::placement::Pquery::remove_placement_at, "Remove the PqueryPlacement at a certain index position within this Pquery.\n\n As this function moves placements in the container, all iterators and pointer to\n PqueryPlacement%s are considered to be invalidated.\n\nC++: genesis::placement::Pquery::remove_placement_at(unsigned long) --> void", pybind11::arg("index"));
		cl.def("clear_placements", (void (genesis::placement::Pquery::*)()) &genesis::placement::Pquery::clear_placements, "Delete all PqueryPlacement%s of this Pquery.\n\nC++: genesis::placement::Pquery::clear_placements() --> void");
		cl.def("begin_names", (class __gnu_cxx::__normal_iterator<class genesis::placement::PqueryName *, class std::vector<class genesis::placement::PqueryName, class std::allocator<class genesis::placement::PqueryName> > > (genesis::placement::Pquery::*)()) &genesis::placement::Pquery::begin_names, "C++: genesis::placement::Pquery::begin_names() --> class __gnu_cxx::__normal_iterator<class genesis::placement::PqueryName *, class std::vector<class genesis::placement::PqueryName, class std::allocator<class genesis::placement::PqueryName> > >");
		cl.def("end_names", (class __gnu_cxx::__normal_iterator<class genesis::placement::PqueryName *, class std::vector<class genesis::placement::PqueryName, class std::allocator<class genesis::placement::PqueryName> > > (genesis::placement::Pquery::*)()) &genesis::placement::Pquery::end_names, "C++: genesis::placement::Pquery::end_names() --> class __gnu_cxx::__normal_iterator<class genesis::placement::PqueryName *, class std::vector<class genesis::placement::PqueryName, class std::allocator<class genesis::placement::PqueryName> > >");
		cl.def("expose_names", (class std::vector<class genesis::placement::PqueryName, class std::allocator<class genesis::placement::PqueryName> > & (genesis::placement::Pquery::*)()) &genesis::placement::Pquery::expose_names, "C++: genesis::placement::Pquery::expose_names() --> class std::vector<class genesis::placement::PqueryName, class std::allocator<class genesis::placement::PqueryName> > &", pybind11::return_value_policy::reference_internal);
		cl.def("add_name", [](genesis::placement::Pquery &o) -> genesis::placement::PqueryName & { return o.add_name(); }, "", pybind11::return_value_policy::reference_internal);
		cl.def("add_name", [](genesis::placement::Pquery &o, std::string const & a0) -> genesis::placement::PqueryName & { return o.add_name(a0); }, "", pybind11::return_value_policy::reference_internal, pybind11::arg("name"));
		cl.def("add_name", (class genesis::placement::PqueryName & (genesis::placement::Pquery::*)(std::string, double)) &genesis::placement::Pquery::add_name, "Create a new PqueryName using the provided parameters, add it to the Pquery and return it.\n\n As this function might reallocate the memory where names are stored, all iterators\n and pointer to PqueryNames%s are considered to be invalidated.\n\nC++: genesis::placement::Pquery::add_name(std::string, double) --> class genesis::placement::PqueryName &", pybind11::return_value_policy::reference_internal, pybind11::arg("name"), pybind11::arg("multiplicity"));
		cl.def("add_name", (class genesis::placement::PqueryName & (genesis::placement::Pquery::*)(const class genesis::placement::PqueryName &)) &genesis::placement::Pquery::add_name, "Create a new PqueryName as a copy of the provided one, add it to the Pquery and return it.\n\n As this function might reallocate the memory where names are stored, all iterators\n and pointer to PqueryNames%s are considered to be invalidated.\n\nC++: genesis::placement::Pquery::add_name(const class genesis::placement::PqueryName &) --> class genesis::placement::PqueryName &", pybind11::return_value_policy::reference_internal, pybind11::arg("other"));
		cl.def("name_size", (unsigned long (genesis::placement::Pquery::*)() const) &genesis::placement::Pquery::name_size, "Return the number of PqueryName%s stored in this Pquery.\n\nC++: genesis::placement::Pquery::name_size() const --> unsigned long");
		cl.def("name_at", (class genesis::placement::PqueryName & (genesis::placement::Pquery::*)(unsigned long)) &genesis::placement::Pquery::name_at, "Return the PqueryName at a certain index.\n\n The index must be smaller than name_size(), otherwise this functions throws an exception.\n\nC++: genesis::placement::Pquery::name_at(unsigned long) --> class genesis::placement::PqueryName &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("remove_name_at", (void (genesis::placement::Pquery::*)(unsigned long)) &genesis::placement::Pquery::remove_name_at, "Remove the PqueryName at a certain index position within this Pquery.\n\n As this function moves names in the container, all iterators and pointer to PqueryNames%s\n are considered to be invalidated.\n\nC++: genesis::placement::Pquery::remove_name_at(unsigned long) --> void", pybind11::arg("index"));
		cl.def("clear_names", (void (genesis::placement::Pquery::*)()) &genesis::placement::Pquery::clear_names, "Delete all PqueryName%s of this Pquery.\n\nC++: genesis::placement::Pquery::clear_names() --> void");
	}
}
