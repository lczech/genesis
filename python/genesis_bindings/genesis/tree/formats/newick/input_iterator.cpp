#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/attribute_tree/tree.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/broker.hpp>
#include <genesis/tree/formats/newick/element.hpp>
#include <genesis/tree/formats/newick/input_iterator.hpp>
#include <genesis/tree/formats/newick/reader.hpp>
#include <genesis/tree/formats/newick/simple_reader.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/formats/newick/simple_writer.hpp>
#include <genesis/tree/formats/newick/writer.hpp>
#include <genesis/tree/printer/table.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/tree/tree_set.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/range.hpp>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/io/input_stream.hpp>
#include <ios>
#include <iterator>
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

// genesis::tree::SimpleNewickNodeData file:genesis/tree/formats/newick/simple_tree.hpp line:99
struct PyCallBack_genesis_tree_SimpleNewickNodeData : public genesis::tree::SimpleNewickNodeData {
	using genesis::tree::SimpleNewickNodeData::SimpleNewickNodeData;

};

// genesis::tree::SimpleNewickEdgeData file:genesis/tree/formats/newick/simple_tree.hpp line:158
struct PyCallBack_genesis_tree_SimpleNewickEdgeData : public genesis::tree::SimpleNewickEdgeData {
	using genesis::tree::SimpleNewickEdgeData::SimpleNewickEdgeData;

};

// genesis::tree::AttributeTreeNodeData file:genesis/tree/attribute_tree/tree.hpp line:92
struct PyCallBack_genesis_tree_AttributeTreeNodeData : public genesis::tree::AttributeTreeNodeData {
	using genesis::tree::AttributeTreeNodeData::AttributeTreeNodeData;

};

// genesis::tree::AttributeTreeEdgeData file:genesis/tree/attribute_tree/tree.hpp line:151
struct PyCallBack_genesis_tree_AttributeTreeEdgeData : public genesis::tree::AttributeTreeEdgeData {
	using genesis::tree::AttributeTreeEdgeData::AttributeTreeEdgeData;

};

void bind_genesis_tree_formats_newick_input_iterator(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::tree::NewickInputIterator file:genesis/tree/formats/newick/input_iterator.hpp line:76
		pybind11::class_<genesis::tree::NewickInputIterator, std::shared_ptr<genesis::tree::NewickInputIterator>> cl(M("genesis::tree"), "NewickInputIterator", "Iterate an input stream and parse it as Newick trees.\n\n This class allows to iterate over an input stream, iterpreting it as Newick trees, and\n yielding one such tree per iteration step. This is useful for processing large files without\n having to keep them fully in memory.\n\n Example:\n\n     for( auto const& t : NewickInputIterator( from_file( \"/path/to/trees.newick\" ))) {\n         std::cout << t.node_count() << \"\";\n     }\n\n Alternatively, the following also works:\n\n     auto it = NewickInputIterator( from_file( \"/path/to/trees.newick\" ));\n     while( it ) {\n         Tree const& t = *it;\n         std::cout << t.node_count() << \"\";\n         ++it;\n     }\n\n Use functions such as utils::from_file() and utils::from_string() to conveniently\n get an input source that can be used here.\n\n See NewickReader for a description of the expected format. In order to change the reading\n behaviour, a NewickReader object can be handed over from which the settings are copied.");
		cl.def( pybind11::init( [](){ return new genesis::tree::NewickInputIterator(); } ) );
		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseInputSource>>(), pybind11::arg("source") );

		cl.def( pybind11::init<class std::shared_ptr<class genesis::utils::BaseInputSource>, const class genesis::tree::NewickReader &>(), pybind11::arg("source"), pybind11::arg("settings") );

		cl.def("__eq__", (bool (genesis::tree::NewickInputIterator::*)(const class genesis::tree::NewickInputIterator &) const) &genesis::tree::NewickInputIterator::operator==, "C++: genesis::tree::NewickInputIterator::operator==(const class genesis::tree::NewickInputIterator &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::tree::NewickInputIterator::*)(const class genesis::tree::NewickInputIterator &) const) &genesis::tree::NewickInputIterator::operator!=, "C++: genesis::tree::NewickInputIterator::operator!=(const class genesis::tree::NewickInputIterator &) const --> bool", pybind11::arg("other"));
		cl.def("dereference", (const class genesis::tree::Tree & (genesis::tree::NewickInputIterator::*)() const) &genesis::tree::NewickInputIterator::operator*, "C++: genesis::tree::NewickInputIterator::operator*() const --> const class genesis::tree::Tree &", pybind11::return_value_policy::reference_internal);
		cl.def("arrow", (const class genesis::tree::Tree * (genesis::tree::NewickInputIterator::*)() const) &genesis::tree::NewickInputIterator::operator->, "C++: genesis::tree::NewickInputIterator::operator->() const --> const class genesis::tree::Tree *", pybind11::return_value_policy::reference_internal);
		cl.def("dereference", (const class genesis::tree::Tree & (genesis::tree::NewickInputIterator::*)() const) &genesis::tree::NewickInputIterator::dereference, "C++: genesis::tree::NewickInputIterator::dereference() const --> const class genesis::tree::Tree &", pybind11::return_value_policy::reference_internal);
		cl.def("begin", (class genesis::tree::NewickInputIterator & (genesis::tree::NewickInputIterator::*)()) &genesis::tree::NewickInputIterator::begin, "Beginning of the iterator.\n\n This is a bit uncommon, as the iterator provides its own begin() and end() functions.\n We do this to allow the easy use case that is explained above.\n\nC++: genesis::tree::NewickInputIterator::begin() --> class genesis::tree::NewickInputIterator &", pybind11::return_value_policy::reference_internal);
		cl.def("end", (class genesis::tree::NewickInputIterator (genesis::tree::NewickInputIterator::*)()) &genesis::tree::NewickInputIterator::end, "End of the iterator.\n\n  begin()\n\nC++: genesis::tree::NewickInputIterator::end() --> class genesis::tree::NewickInputIterator");
		cl.def("pre_increment", (class genesis::tree::NewickInputIterator & (genesis::tree::NewickInputIterator::*)()) &genesis::tree::NewickInputIterator::operator++, "C++: genesis::tree::NewickInputIterator::operator++() --> class genesis::tree::NewickInputIterator &", pybind11::return_value_policy::reference_internal);
		cl.def("increment", (void (genesis::tree::NewickInputIterator::*)()) &genesis::tree::NewickInputIterator::increment, "C++: genesis::tree::NewickInputIterator::increment() --> void");
	}
	{ // genesis::tree::SimpleNewickNodeData file:genesis/tree/formats/newick/simple_tree.hpp line:99
		pybind11::class_<genesis::tree::SimpleNewickNodeData, std::shared_ptr<genesis::tree::SimpleNewickNodeData>, PyCallBack_genesis_tree_SimpleNewickNodeData, genesis::tree::CommonNodeData> cl(M("genesis::tree"), "SimpleNewickNodeData", "Data class for \n\n\n");
		cl.def_readwrite("comments", &genesis::tree::SimpleNewickNodeData::comments);
		cl.def_static("create", (class std::unique_ptr<class genesis::tree::SimpleNewickNodeData, struct std::default_delete<class genesis::tree::SimpleNewickNodeData> > (*)()) &genesis::tree::SimpleNewickNodeData::create, "C++: genesis::tree::SimpleNewickNodeData::create() --> class std::unique_ptr<class genesis::tree::SimpleNewickNodeData, struct std::default_delete<class genesis::tree::SimpleNewickNodeData> >");
	}
	{ // genesis::tree::SimpleNewickEdgeData file:genesis/tree/formats/newick/simple_tree.hpp line:158
		pybind11::class_<genesis::tree::SimpleNewickEdgeData, std::shared_ptr<genesis::tree::SimpleNewickEdgeData>, PyCallBack_genesis_tree_SimpleNewickEdgeData, genesis::tree::CommonEdgeData> cl(M("genesis::tree"), "SimpleNewickEdgeData", "Data class for \n\n\n");
		cl.def_readwrite("values", &genesis::tree::SimpleNewickEdgeData::values);
		cl.def_readwrite("tags", &genesis::tree::SimpleNewickEdgeData::tags);
		cl.def_static("create", (class std::unique_ptr<class genesis::tree::SimpleNewickEdgeData, struct std::default_delete<class genesis::tree::SimpleNewickEdgeData> > (*)()) &genesis::tree::SimpleNewickEdgeData::create, "C++: genesis::tree::SimpleNewickEdgeData::create() --> class std::unique_ptr<class genesis::tree::SimpleNewickEdgeData, struct std::default_delete<class genesis::tree::SimpleNewickEdgeData> >");
	}
	// genesis::tree::convert_common_tree_to_simple_newick_tree(const class genesis::tree::Tree &) file:genesis/tree/formats/newick/simple_tree.hpp line:229
	M("genesis::tree").def("convert_common_tree_to_simple_newick_tree", (class genesis::tree::Tree (*)(const class genesis::tree::Tree &)) &genesis::tree::convert_common_tree_to_simple_newick_tree, "Helper function that takes a CommonTree (or any Tree with Node and Edge data derived\n from it) and turns its data into an SimpleNewickTree, that is, a Tree with\n \n\n\n\n\nC++: genesis::tree::convert_common_tree_to_simple_newick_tree(const class genesis::tree::Tree &) --> class genesis::tree::Tree", pybind11::arg("source"));

	{ // genesis::tree::SimpleNewickTreeNewickReaderPlugin file:genesis/tree/formats/newick/simple_reader.hpp line:48
		pybind11::class_<genesis::tree::SimpleNewickTreeNewickReaderPlugin, std::shared_ptr<genesis::tree::SimpleNewickTreeNewickReaderPlugin>> cl(M("genesis::tree"), "SimpleNewickTreeNewickReaderPlugin", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::SimpleNewickTreeNewickReaderPlugin(); } ) );
		cl.def( pybind11::init( [](genesis::tree::SimpleNewickTreeNewickReaderPlugin const &o){ return new genesis::tree::SimpleNewickTreeNewickReaderPlugin(o); } ) );
		cl.def("assign", (class genesis::tree::SimpleNewickTreeNewickReaderPlugin & (genesis::tree::SimpleNewickTreeNewickReaderPlugin::*)(const class genesis::tree::SimpleNewickTreeNewickReaderPlugin &)) &genesis::tree::SimpleNewickTreeNewickReaderPlugin::operator=, "C++: genesis::tree::SimpleNewickTreeNewickReaderPlugin::operator=(const class genesis::tree::SimpleNewickTreeNewickReaderPlugin &) --> class genesis::tree::SimpleNewickTreeNewickReaderPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("element_to_node", (void (genesis::tree::SimpleNewickTreeNewickReaderPlugin::*)(const struct genesis::tree::NewickBrokerElement &, class genesis::tree::TreeNode &) const) &genesis::tree::SimpleNewickTreeNewickReaderPlugin::element_to_node, "C++: genesis::tree::SimpleNewickTreeNewickReaderPlugin::element_to_node(const struct genesis::tree::NewickBrokerElement &, class genesis::tree::TreeNode &) const --> void", pybind11::arg("element"), pybind11::arg("node"));
		cl.def("element_to_edge", (void (genesis::tree::SimpleNewickTreeNewickReaderPlugin::*)(const struct genesis::tree::NewickBrokerElement &, class genesis::tree::TreeEdge &) const) &genesis::tree::SimpleNewickTreeNewickReaderPlugin::element_to_edge, "C++: genesis::tree::SimpleNewickTreeNewickReaderPlugin::element_to_edge(const struct genesis::tree::NewickBrokerElement &, class genesis::tree::TreeEdge &) const --> void", pybind11::arg("element"), pybind11::arg("edge"));
		cl.def("register_with", (void (genesis::tree::SimpleNewickTreeNewickReaderPlugin::*)(class genesis::tree::NewickReader &) const) &genesis::tree::SimpleNewickTreeNewickReaderPlugin::register_with, "C++: genesis::tree::SimpleNewickTreeNewickReaderPlugin::register_with(class genesis::tree::NewickReader &) const --> void", pybind11::arg("reader"));
	}
	{ // genesis::tree::SimpleNewickTreeNewickReader file:genesis/tree/formats/newick/simple_reader.hpp line:123
		pybind11::class_<genesis::tree::SimpleNewickTreeNewickReader, std::shared_ptr<genesis::tree::SimpleNewickTreeNewickReader>, genesis::tree::NewickReader, genesis::tree::CommonTreeNewickReaderPlugin, genesis::tree::SimpleNewickTreeNewickReaderPlugin> cl(M("genesis::tree"), "SimpleNewickTreeNewickReader", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::SimpleNewickTreeNewickReader(); } ) );
		cl.def( pybind11::init( [](genesis::tree::SimpleNewickTreeNewickReader const &o){ return new genesis::tree::SimpleNewickTreeNewickReader(o); } ) );
		cl.def("assign", (class genesis::tree::SimpleNewickTreeNewickReader & (genesis::tree::SimpleNewickTreeNewickReader::*)(const class genesis::tree::SimpleNewickTreeNewickReader &)) &genesis::tree::SimpleNewickTreeNewickReader::operator=, "C++: genesis::tree::SimpleNewickTreeNewickReader::operator=(const class genesis::tree::SimpleNewickTreeNewickReader &) --> class genesis::tree::SimpleNewickTreeNewickReader &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::tree::SimpleNewickTreeNewickWriterPlugin file:genesis/tree/formats/newick/simple_writer.hpp line:48
		pybind11::class_<genesis::tree::SimpleNewickTreeNewickWriterPlugin, std::shared_ptr<genesis::tree::SimpleNewickTreeNewickWriterPlugin>> cl(M("genesis::tree"), "SimpleNewickTreeNewickWriterPlugin", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::SimpleNewickTreeNewickWriterPlugin(); } ) );
		cl.def( pybind11::init( [](genesis::tree::SimpleNewickTreeNewickWriterPlugin const &o){ return new genesis::tree::SimpleNewickTreeNewickWriterPlugin(o); } ) );
		cl.def("assign", (class genesis::tree::SimpleNewickTreeNewickWriterPlugin & (genesis::tree::SimpleNewickTreeNewickWriterPlugin::*)(const class genesis::tree::SimpleNewickTreeNewickWriterPlugin &)) &genesis::tree::SimpleNewickTreeNewickWriterPlugin::operator=, "C++: genesis::tree::SimpleNewickTreeNewickWriterPlugin::operator=(const class genesis::tree::SimpleNewickTreeNewickWriterPlugin &) --> class genesis::tree::SimpleNewickTreeNewickWriterPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("node_to_element", (void (genesis::tree::SimpleNewickTreeNewickWriterPlugin::*)(const class genesis::tree::TreeNode &, struct genesis::tree::NewickBrokerElement &) const) &genesis::tree::SimpleNewickTreeNewickWriterPlugin::node_to_element, "C++: genesis::tree::SimpleNewickTreeNewickWriterPlugin::node_to_element(const class genesis::tree::TreeNode &, struct genesis::tree::NewickBrokerElement &) const --> void", pybind11::arg("node"), pybind11::arg("element"));
		cl.def("edge_to_element", (void (genesis::tree::SimpleNewickTreeNewickWriterPlugin::*)(const class genesis::tree::TreeEdge &, struct genesis::tree::NewickBrokerElement &) const) &genesis::tree::SimpleNewickTreeNewickWriterPlugin::edge_to_element, "C++: genesis::tree::SimpleNewickTreeNewickWriterPlugin::edge_to_element(const class genesis::tree::TreeEdge &, struct genesis::tree::NewickBrokerElement &) const --> void", pybind11::arg("edge"), pybind11::arg("element"));
		cl.def("register_with", (void (genesis::tree::SimpleNewickTreeNewickWriterPlugin::*)(class genesis::tree::NewickWriter &) const) &genesis::tree::SimpleNewickTreeNewickWriterPlugin::register_with, "C++: genesis::tree::SimpleNewickTreeNewickWriterPlugin::register_with(class genesis::tree::NewickWriter &) const --> void", pybind11::arg("writer"));
	}
	{ // genesis::tree::SimpleNewickTreeNewickWriter file:genesis/tree/formats/newick/simple_writer.hpp line:116
		pybind11::class_<genesis::tree::SimpleNewickTreeNewickWriter, std::shared_ptr<genesis::tree::SimpleNewickTreeNewickWriter>, genesis::tree::NewickWriter, genesis::tree::CommonTreeNewickWriterPlugin, genesis::tree::SimpleNewickTreeNewickWriterPlugin> cl(M("genesis::tree"), "SimpleNewickTreeNewickWriter", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::SimpleNewickTreeNewickWriter(); } ) );
		cl.def( pybind11::init( [](genesis::tree::SimpleNewickTreeNewickWriter const &o){ return new genesis::tree::SimpleNewickTreeNewickWriter(o); } ) );
		cl.def("assign", (class genesis::tree::SimpleNewickTreeNewickWriter & (genesis::tree::SimpleNewickTreeNewickWriter::*)(const class genesis::tree::SimpleNewickTreeNewickWriter &)) &genesis::tree::SimpleNewickTreeNewickWriter::operator=, "C++: genesis::tree::SimpleNewickTreeNewickWriter::operator=(const class genesis::tree::SimpleNewickTreeNewickWriter &) --> class genesis::tree::SimpleNewickTreeNewickWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::tree::PrinterTable file:genesis/tree/printer/table.hpp line:58
		pybind11::class_<genesis::tree::PrinterTable, std::shared_ptr<genesis::tree::PrinterTable>> cl(M("genesis::tree"), "PrinterTable", "Print lists of all nodes, edges and links including their indices and connections\n with each other.");
		cl.def( pybind11::init( [](){ return new genesis::tree::PrinterTable(); } ) );
		cl.def( pybind11::init( [](genesis::tree::PrinterTable const &o){ return new genesis::tree::PrinterTable(o); } ) );
		cl.def("assign", (class genesis::tree::PrinterTable & (genesis::tree::PrinterTable::*)(const class genesis::tree::PrinterTable &)) &genesis::tree::PrinterTable::operator=, "C++: genesis::tree::PrinterTable::operator=(const class genesis::tree::PrinterTable &) --> class genesis::tree::PrinterTable &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("print", (void (genesis::tree::PrinterTable::*)(std::ostream &, const class genesis::tree::Tree &)) &genesis::tree::PrinterTable::print, "C++: genesis::tree::PrinterTable::print(std::ostream &, const class genesis::tree::Tree &) --> void", pybind11::arg("out"), pybind11::arg("tree"));
		cl.def("print", (std::string (genesis::tree::PrinterTable::*)(const class genesis::tree::Tree &)) &genesis::tree::PrinterTable::print, "C++: genesis::tree::PrinterTable::print(const class genesis::tree::Tree &) --> std::string", pybind11::arg("tree"));
	}
	{ // genesis::tree::AttributeTreeNodeData file:genesis/tree/attribute_tree/tree.hpp line:92
		pybind11::class_<genesis::tree::AttributeTreeNodeData, std::shared_ptr<genesis::tree::AttributeTreeNodeData>, PyCallBack_genesis_tree_AttributeTreeNodeData, genesis::tree::CommonNodeData> cl(M("genesis::tree"), "AttributeTreeNodeData", "Data class for \n\n\n");
		cl.def_readwrite("attributes", &genesis::tree::AttributeTreeNodeData::attributes);
		cl.def_static("create", (class std::unique_ptr<class genesis::tree::AttributeTreeNodeData, struct std::default_delete<class genesis::tree::AttributeTreeNodeData> > (*)()) &genesis::tree::AttributeTreeNodeData::create, "C++: genesis::tree::AttributeTreeNodeData::create() --> class std::unique_ptr<class genesis::tree::AttributeTreeNodeData, struct std::default_delete<class genesis::tree::AttributeTreeNodeData> >");
	}
	{ // genesis::tree::AttributeTreeEdgeData file:genesis/tree/attribute_tree/tree.hpp line:151
		pybind11::class_<genesis::tree::AttributeTreeEdgeData, std::shared_ptr<genesis::tree::AttributeTreeEdgeData>, PyCallBack_genesis_tree_AttributeTreeEdgeData, genesis::tree::CommonEdgeData> cl(M("genesis::tree"), "AttributeTreeEdgeData", "Data class for \n\n\n");
		cl.def_readwrite("attributes", &genesis::tree::AttributeTreeEdgeData::attributes);
		cl.def_static("create", (class std::unique_ptr<class genesis::tree::AttributeTreeEdgeData, struct std::default_delete<class genesis::tree::AttributeTreeEdgeData> > (*)()) &genesis::tree::AttributeTreeEdgeData::create, "C++: genesis::tree::AttributeTreeEdgeData::create() --> class std::unique_ptr<class genesis::tree::AttributeTreeEdgeData, struct std::default_delete<class genesis::tree::AttributeTreeEdgeData> >");
	}
}
