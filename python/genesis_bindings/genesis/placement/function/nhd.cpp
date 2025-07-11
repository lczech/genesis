#include <genesis/placement/formats/edge_color.hpp>
#include <genesis/placement/formats/newick_writer.hpp>
#include <genesis/placement/formats/serializer.hpp>
#include <genesis/placement/function/epca.hpp>
#include <genesis/placement/function/nhd.hpp>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/placement/pquery/name.hpp>
#include <genesis/placement/pquery/placement.hpp>
#include <genesis/placement/sample.hpp>
#include <genesis/placement/sample_set.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/broker.hpp>
#include <genesis/tree/formats/newick/element.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/formats/newick/writer.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree_set.hpp>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/containers/range.hpp>
#include <genesis/utils/io/base_output_target.hpp>
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

void bind_genesis_placement_function_nhd(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::placement::NodeDistanceHistogram file:genesis/placement/function/nhd.hpp line:75
		pybind11::class_<genesis::placement::NodeDistanceHistogram, std::shared_ptr<genesis::placement::NodeDistanceHistogram>> cl(M("genesis::placement"), "NodeDistanceHistogram", "Simple histogram data structure with equal sized bins.\n\n This struct is used as a fast and light-weight alternative to the proper utils::Histogram class\n for calculating Node Histogram distances.");
		cl.def( pybind11::init( [](){ return new genesis::placement::NodeDistanceHistogram(); } ) );
		cl.def( pybind11::init( [](genesis::placement::NodeDistanceHistogram const &o){ return new genesis::placement::NodeDistanceHistogram(o); } ) );
		cl.def_readwrite("min", &genesis::placement::NodeDistanceHistogram::min);
		cl.def_readwrite("max", &genesis::placement::NodeDistanceHistogram::max);
		cl.def_readwrite("bins", &genesis::placement::NodeDistanceHistogram::bins);
		cl.def("assign", (struct genesis::placement::NodeDistanceHistogram & (genesis::placement::NodeDistanceHistogram::*)(const struct genesis::placement::NodeDistanceHistogram &)) &genesis::placement::NodeDistanceHistogram::operator=, "C++: genesis::placement::NodeDistanceHistogram::operator=(const struct genesis::placement::NodeDistanceHistogram &) --> struct genesis::placement::NodeDistanceHistogram &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::placement::NodeDistanceHistogramSet file:genesis/placement/function/nhd.hpp line:85
		pybind11::class_<genesis::placement::NodeDistanceHistogramSet, std::shared_ptr<genesis::placement::NodeDistanceHistogramSet>> cl(M("genesis::placement"), "NodeDistanceHistogramSet", "Collection of NodeDistanceHistogram%s that describes one Sample.");
		cl.def( pybind11::init( [](){ return new genesis::placement::NodeDistanceHistogramSet(); } ) );
		cl.def( pybind11::init( [](genesis::placement::NodeDistanceHistogramSet const &o){ return new genesis::placement::NodeDistanceHistogramSet(o); } ) );
		cl.def_readwrite("histograms", &genesis::placement::NodeDistanceHistogramSet::histograms);
		cl.def("assign", (struct genesis::placement::NodeDistanceHistogramSet & (genesis::placement::NodeDistanceHistogramSet::*)(const struct genesis::placement::NodeDistanceHistogramSet &)) &genesis::placement::NodeDistanceHistogramSet::operator=, "C++: genesis::placement::NodeDistanceHistogramSet::operator=(const struct genesis::placement::NodeDistanceHistogramSet &) --> struct genesis::placement::NodeDistanceHistogramSet &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	// genesis::placement::node_distance_histogram_set(const class genesis::placement::Sample &, const class genesis::utils::Matrix<double> &, const class genesis::utils::Matrix<signed char> &, const unsigned long) file:genesis/placement/function/nhd.hpp line:102
	M("genesis::placement").def("node_distance_histogram_set", (struct genesis::placement::NodeDistanceHistogramSet (*)(const class genesis::placement::Sample &, const class genesis::utils::Matrix<double> &, const class genesis::utils::Matrix<signed char> &, const unsigned long)) &genesis::placement::node_distance_histogram_set, "Calculate the NodeDistanceHistogramSet representing a single Sample, given the necessary\n matrices of this Sample.\n\n This is a basic function that is mainly used for speedup in applications. The two matrices\n only depend on the tree, but not on the placement data, so for a set of Samples with the same\n tree, they only need to be calculated once.\n\nC++: genesis::placement::node_distance_histogram_set(const class genesis::placement::Sample &, const class genesis::utils::Matrix<double> &, const class genesis::utils::Matrix<signed char> &, const unsigned long) --> struct genesis::placement::NodeDistanceHistogramSet", pybind11::arg("sample"), pybind11::arg("node_distances"), pybind11::arg("node_sides"), pybind11::arg("histogram_bins"));

	// genesis::placement::node_histogram_distance(const struct genesis::placement::NodeDistanceHistogramSet &, const struct genesis::placement::NodeDistanceHistogramSet &) file:genesis/placement/function/nhd.hpp line:112
	M("genesis::placement").def("node_histogram_distance", (double (*)(const struct genesis::placement::NodeDistanceHistogramSet &, const struct genesis::placement::NodeDistanceHistogramSet &)) &genesis::placement::node_histogram_distance, "Given the histogram sets that describe two Sample%s, calculate their distance.\n\nC++: genesis::placement::node_histogram_distance(const struct genesis::placement::NodeDistanceHistogramSet &, const struct genesis::placement::NodeDistanceHistogramSet &) --> double", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::placement::node_histogram_distance(const class std::vector<struct genesis::placement::NodeDistanceHistogramSet, class std::allocator<struct genesis::placement::NodeDistanceHistogramSet> > &) file:genesis/placement/function/nhd.hpp line:121
	M("genesis::placement").def("node_histogram_distance", (class genesis::utils::Matrix<double> (*)(const class std::vector<struct genesis::placement::NodeDistanceHistogramSet, class std::allocator<struct genesis::placement::NodeDistanceHistogramSet> > &)) &genesis::placement::node_histogram_distance, "Given the histogram sets that describe a set of Sample%s, calculate their pairwise\n distance matrix.\n\nC++: genesis::placement::node_histogram_distance(const class std::vector<struct genesis::placement::NodeDistanceHistogramSet, class std::allocator<struct genesis::placement::NodeDistanceHistogramSet> > &) --> class genesis::utils::Matrix<double>", pybind11::arg("histogram_sets"));

	// genesis::placement::node_histogram_distance(const class genesis::placement::Sample &, const class genesis::placement::Sample &, const unsigned long) file:genesis/placement/function/nhd.hpp line:138
	M("genesis::placement").def("node_histogram_distance", [](const class genesis::placement::Sample & a0, const class genesis::placement::Sample & a1) -> double { return genesis::placement::node_histogram_distance(a0, a1); }, "", pybind11::arg("sample_a"), pybind11::arg("sample_b"));
	M("genesis::placement").def("node_histogram_distance", (double (*)(const class genesis::placement::Sample &, const class genesis::placement::Sample &, const unsigned long)) &genesis::placement::node_histogram_distance, "Calculate the Node Histogram Distance of two Sample%s.\n\n The necessary matrices of the Samples are calculated, then their NodeDistanceHistogramSet are\n build, and finally the distance is calculated. Basically, this is a high level\n function that simply chains node_distance_histogram_set() and\n \n\n\n\n\nC++: genesis::placement::node_histogram_distance(const class genesis::placement::Sample &, const class genesis::placement::Sample &, const unsigned long) --> double", pybind11::arg("sample_a"), pybind11::arg("sample_b"), pybind11::arg("histogram_bins"));

	// genesis::placement::node_histogram_distance(const class genesis::placement::SampleSet &, const unsigned long) file:genesis/placement/function/nhd.hpp line:150
	M("genesis::placement").def("node_histogram_distance", [](const class genesis::placement::SampleSet & a0) -> genesis::utils::Matrix<double> { return genesis::placement::node_histogram_distance(a0); }, "", pybind11::arg("sample_set"));
	M("genesis::placement").def("node_histogram_distance", (class genesis::utils::Matrix<double> (*)(const class genesis::placement::SampleSet &, const unsigned long)) &genesis::placement::node_histogram_distance, "Calculate the Node Histogram Distance of every pair of Sample%s in the SampleSet.\n\n This is a high level convenience function that takes a whole SampleSet, calculates\n the necessary matrices, builds the histograms, and calculates their distances.\n\nC++: genesis::placement::node_histogram_distance(const class genesis::placement::SampleSet &, const unsigned long) --> class genesis::utils::Matrix<double>", pybind11::arg("sample_set"), pybind11::arg("histogram_bins"));

	{ // genesis::placement::SampleSerializer file:genesis/placement/formats/serializer.hpp line:54
		pybind11::class_<genesis::placement::SampleSerializer, std::shared_ptr<genesis::placement::SampleSerializer>> cl(M("genesis::placement"), "SampleSerializer", "");
		cl.def( pybind11::init( [](){ return new genesis::placement::SampleSerializer(); } ) );
		cl.def_static("save", (void (*)(const class genesis::placement::Sample &, const std::string &)) &genesis::placement::SampleSerializer::save, "C++: genesis::placement::SampleSerializer::save(const class genesis::placement::Sample &, const std::string &) --> void", pybind11::arg("map"), pybind11::arg("file_name"));
		cl.def_static("load", (class genesis::placement::Sample (*)(const std::string &)) &genesis::placement::SampleSerializer::load, "C++: genesis::placement::SampleSerializer::load(const std::string &) --> class genesis::placement::Sample", pybind11::arg("file_name"));
		cl.def_static("load", (class genesis::placement::SampleSet (*)(const class std::vector<std::string, class std::allocator<std::string > > &)) &genesis::placement::SampleSerializer::load, "C++: genesis::placement::SampleSerializer::load(const class std::vector<std::string, class std::allocator<std::string > > &) --> class genesis::placement::SampleSet", pybind11::arg("file_names"));
		cl.def_static("load", (void (*)(const class std::vector<std::string, class std::allocator<std::string > > &, class genesis::placement::SampleSet &)) &genesis::placement::SampleSerializer::load, "C++: genesis::placement::SampleSerializer::load(const class std::vector<std::string, class std::allocator<std::string > > &, class genesis::placement::SampleSet &) --> void", pybind11::arg("file_names"), pybind11::arg("sample_set"));
	}
	// genesis::placement::placement_color_count_gradient(const class genesis::placement::Sample &, bool) file:genesis/placement/formats/edge_color.hpp line:65
	M("genesis::placement").def("placement_color_count_gradient", [](const class genesis::placement::Sample & a0) -> std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > { return genesis::placement::placement_color_count_gradient(a0); }, "", pybind11::arg("smp"));
	M("genesis::placement").def("placement_color_count_gradient", (class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > (*)(const class genesis::placement::Sample &, bool)) &genesis::placement::placement_color_count_gradient, "C++: genesis::placement::placement_color_count_gradient(const class genesis::placement::Sample &, bool) --> class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> >", pybind11::arg("smp"), pybind11::arg("linear"));

	{ // genesis::placement::PlacementTreeNewickWriterPlugin file:genesis/placement/formats/newick_writer.hpp line:55
		pybind11::class_<genesis::placement::PlacementTreeNewickWriterPlugin, std::shared_ptr<genesis::placement::PlacementTreeNewickWriterPlugin>> cl(M("genesis::placement"), "PlacementTreeNewickWriterPlugin", "");
		cl.def( pybind11::init( [](){ return new genesis::placement::PlacementTreeNewickWriterPlugin(); } ) );
		cl.def( pybind11::init( [](genesis::placement::PlacementTreeNewickWriterPlugin const &o){ return new genesis::placement::PlacementTreeNewickWriterPlugin(o); } ) );
		cl.def("assign", (class genesis::placement::PlacementTreeNewickWriterPlugin & (genesis::placement::PlacementTreeNewickWriterPlugin::*)(const class genesis::placement::PlacementTreeNewickWriterPlugin &)) &genesis::placement::PlacementTreeNewickWriterPlugin::operator=, "C++: genesis::placement::PlacementTreeNewickWriterPlugin::operator=(const class genesis::placement::PlacementTreeNewickWriterPlugin &) --> class genesis::placement::PlacementTreeNewickWriterPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("enable_edge_nums", (bool (genesis::placement::PlacementTreeNewickWriterPlugin::*)() const) &genesis::placement::PlacementTreeNewickWriterPlugin::enable_edge_nums, "C++: genesis::placement::PlacementTreeNewickWriterPlugin::enable_edge_nums() const --> bool");
		cl.def("enable_edge_nums", (void (genesis::placement::PlacementTreeNewickWriterPlugin::*)(bool)) &genesis::placement::PlacementTreeNewickWriterPlugin::enable_edge_nums, "C++: genesis::placement::PlacementTreeNewickWriterPlugin::enable_edge_nums(bool) --> void", pybind11::arg("value"));
		cl.def("enable_placement_counts", (bool (genesis::placement::PlacementTreeNewickWriterPlugin::*)() const) &genesis::placement::PlacementTreeNewickWriterPlugin::enable_placement_counts, "C++: genesis::placement::PlacementTreeNewickWriterPlugin::enable_placement_counts() const --> bool");
		cl.def("enable_placement_counts", (void (genesis::placement::PlacementTreeNewickWriterPlugin::*)(bool)) &genesis::placement::PlacementTreeNewickWriterPlugin::enable_placement_counts, "C++: genesis::placement::PlacementTreeNewickWriterPlugin::enable_placement_counts(bool) --> void", pybind11::arg("value"));
		cl.def("prepare_sample", (void (genesis::placement::PlacementTreeNewickWriterPlugin::*)(const class genesis::placement::Sample &)) &genesis::placement::PlacementTreeNewickWriterPlugin::prepare_sample, "C++: genesis::placement::PlacementTreeNewickWriterPlugin::prepare_sample(const class genesis::placement::Sample &) --> void", pybind11::arg("smp"));
		cl.def("edge_to_element", (void (genesis::placement::PlacementTreeNewickWriterPlugin::*)(const class genesis::tree::TreeEdge &, struct genesis::tree::NewickBrokerElement &) const) &genesis::placement::PlacementTreeNewickWriterPlugin::edge_to_element, "C++: genesis::placement::PlacementTreeNewickWriterPlugin::edge_to_element(const class genesis::tree::TreeEdge &, struct genesis::tree::NewickBrokerElement &) const --> void", pybind11::arg("edge"), pybind11::arg("element"));
		cl.def("register_with", (void (genesis::placement::PlacementTreeNewickWriterPlugin::*)(class genesis::tree::NewickWriter &) const) &genesis::placement::PlacementTreeNewickWriterPlugin::register_with, "C++: genesis::placement::PlacementTreeNewickWriterPlugin::register_with(class genesis::tree::NewickWriter &) const --> void", pybind11::arg("writer"));
	}
	{ // genesis::placement::PlacementTreeNewickWriter file:genesis/placement/formats/newick_writer.hpp line:148
		pybind11::class_<genesis::placement::PlacementTreeNewickWriter, std::shared_ptr<genesis::placement::PlacementTreeNewickWriter>, genesis::tree::NewickWriter, genesis::tree::CommonTreeNewickWriterPlugin, genesis::placement::PlacementTreeNewickWriterPlugin> cl(M("genesis::placement"), "PlacementTreeNewickWriter", "");
		cl.def( pybind11::init( [](){ return new genesis::placement::PlacementTreeNewickWriter(); } ) );
		cl.def( pybind11::init( [](genesis::placement::PlacementTreeNewickWriter const &o){ return new genesis::placement::PlacementTreeNewickWriter(o); } ) );
		cl.def("assign", (class genesis::placement::PlacementTreeNewickWriter & (genesis::placement::PlacementTreeNewickWriter::*)(const class genesis::placement::PlacementTreeNewickWriter &)) &genesis::placement::PlacementTreeNewickWriter::operator=, "C++: genesis::placement::PlacementTreeNewickWriter::operator=(const class genesis::placement::PlacementTreeNewickWriter &) --> class genesis::placement::PlacementTreeNewickWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::placement::EpcaData file:genesis/placement/function/epca.hpp line:76
		pybind11::class_<genesis::placement::EpcaData, std::shared_ptr<genesis::placement::EpcaData>> cl(M("genesis::placement"), "EpcaData", "Helper stucture that collects the output of epca().\n\n It contains the same elements as utils::PcaData, but extends it by a vector of the\n \n\n\n");
		cl.def( pybind11::init( [](){ return new genesis::placement::EpcaData(); } ) );
		cl.def( pybind11::init( [](genesis::placement::EpcaData const &o){ return new genesis::placement::EpcaData(o); } ) );
		cl.def_readwrite("eigenvalues", &genesis::placement::EpcaData::eigenvalues);
		cl.def_readwrite("eigenvectors", &genesis::placement::EpcaData::eigenvectors);
		cl.def_readwrite("projection", &genesis::placement::EpcaData::projection);
		cl.def_readwrite("edge_indices", &genesis::placement::EpcaData::edge_indices);
		cl.def("assign", (struct genesis::placement::EpcaData & (genesis::placement::EpcaData::*)(const struct genesis::placement::EpcaData &)) &genesis::placement::EpcaData::operator=, "C++: genesis::placement::EpcaData::operator=(const struct genesis::placement::EpcaData &) --> struct genesis::placement::EpcaData &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
}
