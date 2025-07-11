#include <genesis/placement/formats/newick_reader.hpp>
#include <genesis/placement/function/epca.hpp>
#include <genesis/placement/function/masses.hpp>
#include <genesis/placement/function/operators.hpp>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/placement/pquery/name.hpp>
#include <genesis/placement/pquery/placement.hpp>
#include <genesis/placement/sample.hpp>
#include <genesis/placement/sample_set.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/broker.hpp>
#include <genesis/tree/formats/newick/element.hpp>
#include <genesis/tree/formats/newick/reader.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/tree/tree_set.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/containers/range.hpp>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/input_stream.hpp>
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

void bind_genesis_placement_function_epca(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::placement::epca_imbalance_vector(const class genesis::placement::Sample &, bool) file:genesis/placement/function/epca.hpp line:106
	M("genesis::placement").def("epca_imbalance_vector", [](const class genesis::placement::Sample & a0) -> std::vector<double, class std::allocator<double> > { return genesis::placement::epca_imbalance_vector(a0); }, "", pybind11::arg("sample"));
	M("genesis::placement").def("epca_imbalance_vector", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::placement::Sample &, bool)) &genesis::placement::epca_imbalance_vector, "Calculate the imbalance of placement mass for each \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::epca_imbalance_vector(const class genesis::placement::Sample &, bool) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("sample"), pybind11::arg("normalize"));

	// genesis::placement::epca_imbalance_matrix(const class genesis::placement::SampleSet &, bool, bool) file:genesis/placement/function/epca.hpp line:133
	M("genesis::placement").def("epca_imbalance_matrix", [](const class genesis::placement::SampleSet & a0) -> genesis::utils::Matrix<double> { return genesis::placement::epca_imbalance_matrix(a0); }, "", pybind11::arg("samples"));
	M("genesis::placement").def("epca_imbalance_matrix", [](const class genesis::placement::SampleSet & a0, bool const & a1) -> genesis::utils::Matrix<double> { return genesis::placement::epca_imbalance_matrix(a0, a1); }, "", pybind11::arg("samples"), pybind11::arg("include_leaves"));
	M("genesis::placement").def("epca_imbalance_matrix", (class genesis::utils::Matrix<double> (*)(const class genesis::placement::SampleSet &, bool, bool)) &genesis::placement::epca_imbalance_matrix, "Calculate the imbalance matrix of placment mass for all Sample%s in a SampleSet.\n\n The first step to perform \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::epca_imbalance_matrix(const class genesis::placement::SampleSet &, bool, bool) --> class genesis::utils::Matrix<double>", pybind11::arg("samples"), pybind11::arg("include_leaves"), pybind11::arg("normalize"));

	// genesis::placement::epca_splitify_transform(class genesis::utils::Matrix<double> &, double) file:genesis/placement/function/epca.hpp line:155
	M("genesis::placement").def("epca_splitify_transform", [](class genesis::utils::Matrix<double> & a0) -> void { return genesis::placement::epca_splitify_transform(a0); }, "", pybind11::arg("imbalance_matrix"));
	M("genesis::placement").def("epca_splitify_transform", (void (*)(class genesis::utils::Matrix<double> &, double)) &genesis::placement::epca_splitify_transform, "Perform a component-wise transformation of the imbalance matrix used for epca().\n\n All entries of the Matrix are transformed inplace, using\n\n \n\n\n where the `kappa` (\n) parameter can be any non-negative number. This parameter scales\n between ignoring abundance information (`kappa` = 0), using it linearly (`kappa` = 1), and\n emphasizing it (`kappa` > 1).\n\n \n Matrix to transform inplace.\n \n\n            Scaling value for abundance information. Has to be >= 0.\n\nC++: genesis::placement::epca_splitify_transform(class genesis::utils::Matrix<double> &, double) --> void", pybind11::arg("imbalance_matrix"), pybind11::arg("kappa"));

	// genesis::placement::epca(const class genesis::placement::SampleSet &, double, double, unsigned long) file:genesis/placement/function/epca.hpp line:171
	M("genesis::placement").def("epca", [](const class genesis::placement::SampleSet & a0) -> genesis::placement::EpcaData { return genesis::placement::epca(a0); }, "", pybind11::arg("samples"));
	M("genesis::placement").def("epca", [](const class genesis::placement::SampleSet & a0, double const & a1) -> genesis::placement::EpcaData { return genesis::placement::epca(a0, a1); }, "", pybind11::arg("samples"), pybind11::arg("kappa"));
	M("genesis::placement").def("epca", [](const class genesis::placement::SampleSet & a0, double const & a1, double const & a2) -> genesis::placement::EpcaData { return genesis::placement::epca(a0, a1, a2); }, "", pybind11::arg("samples"), pybind11::arg("kappa"), pybind11::arg("epsilon"));
	M("genesis::placement").def("epca", (struct genesis::placement::EpcaData (*)(const class genesis::placement::SampleSet &, double, double, unsigned long)) &genesis::placement::epca, "Perform EdgePCA on a SampleSet.\n\n The parameters  and  are as described in epca_splitify_transform() and\n epca_filter_constant_columns(), respectively.\n\n The result is returned as a `struct` similar to the one used by utils::pca(),\n but containing an additional vector of the \n\n\n\n\n\nC++: genesis::placement::epca(const class genesis::placement::SampleSet &, double, double, unsigned long) --> struct genesis::placement::EpcaData", pybind11::arg("samples"), pybind11::arg("kappa"), pybind11::arg("epsilon"), pybind11::arg("components"));

	// genesis::placement::total_multiplicity(const class genesis::placement::Pquery &) file:genesis/placement/function/masses.hpp line:51
	M("genesis::placement").def("total_multiplicity", (double (*)(const class genesis::placement::Pquery &)) &genesis::placement::total_multiplicity, "Return the sum of all \n\n\n\nC++: genesis::placement::total_multiplicity(const class genesis::placement::Pquery &) --> double", pybind11::arg("pqry"));

	// genesis::placement::total_multiplicity(const class genesis::placement::Sample &) file:genesis/placement/function/masses.hpp line:57
	M("genesis::placement").def("total_multiplicity", (double (*)(const class genesis::placement::Sample &)) &genesis::placement::total_multiplicity, "Return the sum of all \n\n\n\nC++: genesis::placement::total_multiplicity(const class genesis::placement::Sample &) --> double", pybind11::arg("sample"));

	// genesis::placement::placement_mass_per_edges_with_multiplicities(const class genesis::placement::Sample &) file:genesis/placement/function/masses.hpp line:68
	M("genesis::placement").def("placement_mass_per_edges_with_multiplicities", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::placement::Sample &)) &genesis::placement::placement_mass_per_edges_with_multiplicities, "Return a vector that contains the sum of the masses of the PqueryPlacement%s per\n \n\n\n\n\nC++: genesis::placement::placement_mass_per_edges_with_multiplicities(const class genesis::placement::Sample &) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("sample"));

	// genesis::placement::placement_mass_per_edges_with_multiplicities(const class genesis::placement::SampleSet &) file:genesis/placement/function/masses.hpp line:77
	M("genesis::placement").def("placement_mass_per_edges_with_multiplicities", (class genesis::utils::Matrix<double> (*)(const class genesis::placement::SampleSet &)) &genesis::placement::placement_mass_per_edges_with_multiplicities, "Return a Matrix that contains the placement masses per edge, using the\n \n\n\n\n\n\n\nC++: genesis::placement::placement_mass_per_edges_with_multiplicities(const class genesis::placement::SampleSet &) --> class genesis::utils::Matrix<double>", pybind11::arg("sample_set"));

	// genesis::placement::total_placement_mass_with_multiplicities(const class genesis::placement::Sample &) file:genesis/placement/function/masses.hpp line:92
	M("genesis::placement").def("total_placement_mass_with_multiplicities", (double (*)(const class genesis::placement::Sample &)) &genesis::placement::total_placement_mass_with_multiplicities, "Get the mass of all PqueryPlacement%s of the Sample, using the\n \n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::total_placement_mass_with_multiplicities(const class genesis::placement::Sample &) --> double", pybind11::arg("smp"));

	// genesis::placement::placement_mass_per_edge_without_multiplicities(const class genesis::placement::Sample &) file:genesis/placement/function/masses.hpp line:110
	M("genesis::placement").def("placement_mass_per_edge_without_multiplicities", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::placement::Sample &)) &genesis::placement::placement_mass_per_edge_without_multiplicities, "Return a vector that contains the sum of the masses of the PqueryPlacement%s per\n \n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::placement_mass_per_edge_without_multiplicities(const class genesis::placement::Sample &) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("sample"));

	// genesis::placement::placement_mass_per_edge_without_multiplicities(const class genesis::placement::SampleSet &) file:genesis/placement/function/masses.hpp line:118
	M("genesis::placement").def("placement_mass_per_edge_without_multiplicities", (class genesis::utils::Matrix<double> (*)(const class genesis::placement::SampleSet &)) &genesis::placement::placement_mass_per_edge_without_multiplicities, "Return a Matrix that contains the placement masses per edge.\n\n See placement_mass_per_edge_without_multiplicities( Sample const& ) for details.\n The Matrix is simply a row-wise concatenation of this vector function.\n\nC++: genesis::placement::placement_mass_per_edge_without_multiplicities(const class genesis::placement::SampleSet &) --> class genesis::utils::Matrix<double>", pybind11::arg("sample_set"));

	// genesis::placement::total_placement_mass_without_multiplicities(const class genesis::placement::Sample &) file:genesis/placement/function/masses.hpp line:129
	M("genesis::placement").def("total_placement_mass_without_multiplicities", (double (*)(const class genesis::placement::Sample &)) &genesis::placement::total_placement_mass_without_multiplicities, "Get the summed mass of all PqueryPlacement%s in all \n\n\n\n\n\n\n\n\nC++: genesis::placement::total_placement_mass_without_multiplicities(const class genesis::placement::Sample &) --> double", pybind11::arg("smp"));

	{ // genesis::placement::PlacementTreeNewickReaderPlugin file:genesis/placement/formats/newick_reader.hpp line:56
		pybind11::class_<genesis::placement::PlacementTreeNewickReaderPlugin, std::shared_ptr<genesis::placement::PlacementTreeNewickReaderPlugin>> cl(M("genesis::placement"), "PlacementTreeNewickReaderPlugin", "");
		cl.def( pybind11::init( [](){ return new genesis::placement::PlacementTreeNewickReaderPlugin(); } ) );
		cl.def( pybind11::init( [](genesis::placement::PlacementTreeNewickReaderPlugin const &o){ return new genesis::placement::PlacementTreeNewickReaderPlugin(o); } ) );
		cl.def("assign", (class genesis::placement::PlacementTreeNewickReaderPlugin & (genesis::placement::PlacementTreeNewickReaderPlugin::*)(const class genesis::placement::PlacementTreeNewickReaderPlugin &)) &genesis::placement::PlacementTreeNewickReaderPlugin::operator=, "C++: genesis::placement::PlacementTreeNewickReaderPlugin::operator=(const class genesis::placement::PlacementTreeNewickReaderPlugin &) --> class genesis::placement::PlacementTreeNewickReaderPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("element_to_edge", (void (genesis::placement::PlacementTreeNewickReaderPlugin::*)(const struct genesis::tree::NewickBrokerElement &, class genesis::tree::TreeEdge &) const) &genesis::placement::PlacementTreeNewickReaderPlugin::element_to_edge, "C++: genesis::placement::PlacementTreeNewickReaderPlugin::element_to_edge(const struct genesis::tree::NewickBrokerElement &, class genesis::tree::TreeEdge &) const --> void", pybind11::arg("element"), pybind11::arg("edge"));
		cl.def("finish_reading", (void (genesis::placement::PlacementTreeNewickReaderPlugin::*)(class genesis::tree::Tree &) const) &genesis::placement::PlacementTreeNewickReaderPlugin::finish_reading, "C++: genesis::placement::PlacementTreeNewickReaderPlugin::finish_reading(class genesis::tree::Tree &) const --> void", pybind11::arg("tree"));
		cl.def("register_with", (void (genesis::placement::PlacementTreeNewickReaderPlugin::*)(class genesis::tree::NewickReader &) const) &genesis::placement::PlacementTreeNewickReaderPlugin::register_with, "C++: genesis::placement::PlacementTreeNewickReaderPlugin::register_with(class genesis::tree::NewickReader &) const --> void", pybind11::arg("reader"));
		cl.def("get_edge_num_from_comments", (class genesis::placement::PlacementTreeNewickReaderPlugin & (genesis::placement::PlacementTreeNewickReaderPlugin::*)(bool)) &genesis::placement::PlacementTreeNewickReaderPlugin::get_edge_num_from_comments, "Set whether to use tags or comments of the Newick tree for the edge nums.\n\n In version 1 of the `jplace` standard, the edge num properties of the reference tree\n were noted down as Newick comments in brackets (e.g., `[42]`) instead of the curly braces\n used in later versions of the standard (e.g., `{42}`). By default, we use the newer tags.\n Set this to `true` in order to fall back to the old version using comments instead.\n\nC++: genesis::placement::PlacementTreeNewickReaderPlugin::get_edge_num_from_comments(bool) --> class genesis::placement::PlacementTreeNewickReaderPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("get_edge_num_from_comments", (bool (genesis::placement::PlacementTreeNewickReaderPlugin::*)() const) &genesis::placement::PlacementTreeNewickReaderPlugin::get_edge_num_from_comments, "Get whether to use tags or comments of the Newick tree for the edge nums.\n\n See the setter function for details.\n\nC++: genesis::placement::PlacementTreeNewickReaderPlugin::get_edge_num_from_comments() const --> bool");
	}
	{ // genesis::placement::PlacementTreeNewickReader file:genesis/placement/formats/newick_reader.hpp line:285
		pybind11::class_<genesis::placement::PlacementTreeNewickReader, std::shared_ptr<genesis::placement::PlacementTreeNewickReader>, genesis::tree::NewickReader, genesis::tree::CommonTreeNewickReaderPlugin, genesis::placement::PlacementTreeNewickReaderPlugin> cl(M("genesis::placement"), "PlacementTreeNewickReader", "");
		cl.def( pybind11::init( [](){ return new genesis::placement::PlacementTreeNewickReader(); } ) );
		cl.def( pybind11::init( [](genesis::placement::PlacementTreeNewickReader const &o){ return new genesis::placement::PlacementTreeNewickReader(o); } ) );
		cl.def("assign", (class genesis::placement::PlacementTreeNewickReader & (genesis::placement::PlacementTreeNewickReader::*)(const class genesis::placement::PlacementTreeNewickReader &)) &genesis::placement::PlacementTreeNewickReader::operator=, "C++: genesis::placement::PlacementTreeNewickReader::operator=(const class genesis::placement::PlacementTreeNewickReader &) --> class genesis::placement::PlacementTreeNewickReader &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	// genesis::placement::compatible_trees(const class genesis::tree::Tree &, const class genesis::tree::Tree &) file:genesis/placement/function/operators.hpp line:84
	M("genesis::placement").def("compatible_trees", (bool (*)(const class genesis::tree::Tree &, const class genesis::tree::Tree &)) &genesis::placement::compatible_trees, "Return whether two PlacementTree%s are compatible.\n\n This is the case iff:\n\n   * they have the same topology,\n   * they have the same internal structure (e.g., node indices),\n   * they have the same node names at corresponding nodes,\n   * they have the same edge nums at corresponding edges,\n   * the data types of all nodes and edges are those of a ::PlacementTree\n\n In all other cases, `false` is returned.\n\nC++: genesis::placement::compatible_trees(const class genesis::tree::Tree &, const class genesis::tree::Tree &) --> bool", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::placement::compatible_trees(const class genesis::placement::Sample &, const class genesis::placement::Sample &) file:genesis/placement/function/operators.hpp line:93
	M("genesis::placement").def("compatible_trees", (bool (*)(const class genesis::placement::Sample &, const class genesis::placement::Sample &)) &genesis::placement::compatible_trees, "Return whether the PlacementTree%s of two Sample%s are compatible.\n\n See\n \n\n\n\n\nC++: genesis::placement::compatible_trees(const class genesis::placement::Sample &, const class genesis::placement::Sample &) --> bool", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::placement::convert_common_tree_to_placement_tree(const class genesis::tree::Tree &) file:genesis/placement/function/operators.hpp line:106
	M("genesis::placement").def("convert_common_tree_to_placement_tree", (class genesis::tree::Tree (*)(const class genesis::tree::Tree &)) &genesis::placement::convert_common_tree_to_placement_tree, "Convert a \n\n\n\n\n\n\nC++: genesis::placement::convert_common_tree_to_placement_tree(const class genesis::tree::Tree &) --> class genesis::tree::Tree", pybind11::arg("source_tree"));

}
